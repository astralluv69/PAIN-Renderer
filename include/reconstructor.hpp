#pragma once

#include <stb_image.h>
#include <stb_image_resize2.h>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include <stdexcept>

#include "vbo.h"

struct CameraView {
    glm::mat4 view_proj;
    std::vector<glm::vec4> target_rgba;
};

struct CameraViewSetup {
    const char* filepath;
    float azimuth_deg;
    float elevation_deg;
};

class Reconstructor {
    public:
        static void GenerateIcosphere (int subdivisions, float radius, std::vector<ObjVertex>& out_positions, std::vector<GLuint>& out_indices) {
            const float t = (1.0f + std::sqrt(5.0f)) / 2.0f;

            std::vector<glm::vec3> pos = {
                {-1,  t,  0}, { 1,  t,  0}, {-1, -t,  0}, { 1, -t,  0},
                { 0, -1,  t}, { 0,  1,  t}, { 0, -1, -t}, { 0,  1, -t},
                { t,  0, -1}, { t,  0,  1}, {-t,  0, -1}, {-t,  0,  1}
            };

            for (glm::vec3& p : pos) {
                p = glm::normalize(p);
            }

            std::vector<glm::uvec3> faces = {
                {0, 11, 5},  {0, 5, 1},   {0, 1, 7},   {0, 7, 10},  {0, 10, 11},
                {1, 5, 9},   {5, 11, 4},  {11, 10, 2}, {10, 7, 6},  {7, 1, 8},
                {3, 9, 4},   {3, 4, 2},   {3, 2, 6},   {3, 6, 8},   {3, 8, 9},
                {4, 9, 5},   {2, 4, 11},  {6, 2, 10},  {8, 6, 7},   {9, 8, 1}
            };

            for (size_t s = 0; s < subdivisions; ++s) {
                std::map<std::pair<GLuint, GLuint>, GLuint> mid_cache;
                std::vector<glm::uvec3> new_faces;
                new_faces.reserve(faces.size() * 4);

                auto get_mid = [&](GLuint idx1, GLuint idx2) -> GLuint {
                    auto key = std::make_pair(std::min(idx1, idx2), std::max(idx1, idx2));
                    auto idxt = mid_cache.find(key);

                    if (idxt != mid_cache.end()) {
                        return idxt -> second;
                    }

                    glm::vec3 mid = glm::normalize((pos[idx1] + pos[idx2]) * 0.5f);
                    uint32_t idx = static_cast<uint32_t>(pos.size());
                    pos.push_back(mid);
                    mid_cache[key] = idx;
                    return idx;
                };

                for (const glm::uvec3& f : faces) {
                    GLuint edge1 = get_mid(f.x, f.y);
                    GLuint edge2 = get_mid(f.y, f.z);
                    GLuint edge3 = get_mid(f.z, f.x);

                    new_faces.push_back({f.x, edge1, edge3});
                    new_faces.push_back({f.y, edge2, edge1});
                    new_faces.push_back({f.z, edge3, edge2});
                    new_faces.push_back({edge1, edge2, edge3});
                }

                faces = std::move(new_faces);
            }

            out_positions.resize(pos.size());
            for (size_t i = 0; i < pos.size(); ++i) {
                out_positions[i].position = pos[i] * radius;
            }

            out_indices.clear();
            out_indices.reserve(faces.size() * 3);
            for (const glm::vec3& f : faces) {
                out_indices.push_back(f.x);
                out_indices.push_back(f.y);
                out_indices.push_back(f.z);
            }
        }

        static glm::mat4 GenerateViewProj (
            float azimuth_deg,
            float elevation_deg = 0.0f,
            float fov_deg = 45.0f,
            float aspect = 1.0f
        ) {
            float azimuth_rad = glm::radians(azimuth_deg);
            float elevation_rad = glm::radians(elevation_deg);

            glm::vec3 eye(
                std::cos(elevation_rad) * std::sin(azimuth_rad),
                std::sin(elevation_rad),
                std::cos(elevation_rad) * std::cos(azimuth_rad)
            );

            glm::vec3 world_up = (std::abs(elevation_deg) > 85.0f)
                ? glm::vec3(0.0f, 0.0f, -1.0f) : glm::vec3(0.0f, 1.0f, 0.0f);

            glm::mat4 view = glm::lookAt(eye, glm::vec3(0.0f), world_up);
            glm::mat4 proj = glm::perspective(glm::radians(fov_deg), aspect, 0.1f, 100.0f);

            proj[0][0] *= 3.0f;
            proj[1][1] *= 3.0f;

            return proj * view;
        }

        static std::vector<glm::vec4> LoadImage (
            const char* filepath,
            int target_w,
            int target_h,
            float color_tol = 0.09f,
            float brightness_weight = 0.2f
        ) {
            int original_w = 0, original_h = 0, channels = 0;
            unsigned char* raw_img = stbi_load(
                filepath,
                &original_w,
                &original_h,
                &channels,
                STBI_rgb_alpha
            );

            if (!raw_img) {
                throw std::runtime_error("Failed to load image: " + std::string(filepath));
            }

            const float inv_255  = 1.0f / 255.0f;
            std::vector<glm::vec4> img(original_w * original_h);
            for (size_t i = 0; i < original_w * original_h; ++i) {
                img[i] = glm::vec4(
                    raw_img[i * 4 + 0],
                    raw_img[i * 4 + 1],
                    raw_img[i * 4 + 2],
                    0.0f
                );
            }
            stbi_image_free(raw_img);

            int x_min = original_w / 3;
            int x_max = original_w * 2 / 3;
            int y_min = original_h / 3;
            int y_max = original_h * 2 / 3;

            std::vector<glm::vec3> border_samples;
            int step_x = std::max(1, (x_max - x_min) / 8);
            int step_y = std::max(1, (y_max - y_min) / 8);

            for (int x = x_min; x <= x_max; x += step_x) {
                border_samples.push_back(glm::vec3(img[y_min * original_w + x]));
                border_samples.push_back(glm::vec3(img[y_max * original_w + x]));
            }

            for (int y = y_min; y <= y_max; y += step_y) {
                border_samples.push_back(glm::vec3(img[y * original_h + x_min]));
                border_samples.push_back(glm::vec3(img[y * original_h + x_max]));
            }

            std::vector<uint8_t> visited(original_w * original_h, 0);
            std::vector<int> queue;
            queue.reserve((x_max - x_min + 1) * (y_max - y_min + 1));

            auto try_push = [&](int x, int y){
                if (x < x_min || x > x_max || y < y_min || y > y_max) return;

                int idx = y * original_w + x;
                if (visited[idx]) return;

                float min_dist = 1e9f;
                glm::vec3 pix_rbg = glm::vec3(img[idx]);
                for (const glm::vec3& sample : border_samples) {
                    min_dist = std::min(
                        min_dist,
                        ShadowInvariantDistance(pix_rbg, sample, brightness_weight)
                    );
                }

                if (min_dist <= color_tol) {
                    visited[idx] = 1;
                    queue.push_back(idx);
                }
            };

            for (int x = x_min; x <= x_max; ++x) {
                try_push(x, y_min);
                try_push(x, y_max);
            }

            for (int y = y_min; y <= y_max; ++y) {
                try_push(x_min, y);
                try_push(x_max, y);
            }

            size_t head = 0;
            while (head < queue.size()) {
                int current = queue[head++];
                int current_x = current % original_w;
                int current_y = current / original_w;

                try_push(current_x + 1, current_y);
                try_push(current_x - 1, current_y);
                try_push(current_x, current_y + 1);
                try_push(current_x, current_y - 1);
            }

            for (int y = y_min; y <= y_max; ++y) {
                for (int x = x_min; x <= x_max; ++x) {
                    int idx = y * original_w + x;
                    if (!visited[idx]) {
                        img[idx].a = 1.0f;
                    }
                }
            }

            std::vector<float> smooth_alpha(original_w * original_h, 0.0f);
            for (int y = std::max(1, y_min - 1); y <= std::min(original_h - 2, y_max + 1); ++y) {
                for (int x = std::max(1, x_min - 1); x <= std::min(original_w - 2, x_max + 1); ++x) {
                    float sum = 0.0f;
                    for (int dy = -1; dy <= 1; ++dy) {
                        for (int dx = -1; dx <= 1; ++dx) {
                            sum += img[(y + dy) * original_w + (x + dx)].a;
                        }
                    }
                    smooth_alpha[y * original_w + x] = sum / 9.0f;
                }
            }
            for (int i = 0; i < original_w * original_h; ++i) {
                img[i].a = smooth_alpha[i];
            }

            int src_x0 = x_min;
            int src_y0 = y_min;
            int src_w = std::max(1, x_max - x_min);
            int src_h = std::max(1, y_max - y_min);

            std::vector<glm::vec4> out_rgba(target_w * target_h);
            for (int y = 0; y < target_h; ++y) {
                float v = src_y0 + ((static_cast<float>(target_h - 1 - y) + 0.5f) / target_h) * src_h - 0.5f;
                int y0 = std::clamp(static_cast<int>(std::floor(v)), 0, original_h - 1);
                int y1 = std::min(y0 + 1, original_h - 1);
                float fy = std::clamp(v - y0, 0.0f, 1.0f);

                for (int x = 0; x < target_w; ++x) {
                    float u = src_x0 + ((static_cast<float>(x) + 0.5f) / target_w) * src_w - 0.5f;
                    int x0 = std::clamp(static_cast<int>(std::floor(u)), 0, original_w - 1);
                    int x1 = std::min(x0 + 1, original_w - 1);
                    float fx = std::clamp(u - x0, 0.0f, 1.0f);

                    glm::vec4 c00 = img[y0 * original_w + x0];
                    glm::vec4 c10 = img[y0 * original_w + x1];
                    glm::vec4 c01 = img[y1 * original_w + x0];
                    glm::vec4 c11 = img[y1 * original_w + x1];

                    glm::vec4 c = glm::mix(glm::mix(c00, c10, fx), glm::mix(c01, c11, fx), fy);
                    out_rgba[y * target_w + x] = glm::vec4(c.r * c.a, c.g * c.a, c.b * c.a, c.a);
                }
            }

            return out_rgba;
        }

        void Optimize (
            std::vector<ObjVertex>& vertices,
            const std::vector<GLuint>& indices,
            const std::vector<CameraView>& views,
            int image_width,
            int image_height,
            int iterations = 800,
            float lr_position = 0.005f,
            float lr_color = 0.015f,
            float edge_weight = 0.02f,
            float convergence_tol = 1e-6f
        ) {
            if (vertices.empty() || indices.empty() || views.empty()) {
                throw std::invalid_argument("Reconstructor::Optimize received empty buffers or views.");
            }

            Init(vertices.size(), image_width, image_height);

            float prev_loss = 1e9f;

            for (int iter = 0; iter < iterations; ++iter) {
                float progress = static_cast<float>(iter) / static_cast<float>(std::max(1, iterations - 1));

                float sigma = glm::mix(3.0e-3f, 3.5e-4f, progress);

                float lr_scale = std::max(0.1f, 0.5f * (1.0f * std::cos(3.14159265f * progress)));

                float loss = Step(
                    vertices,
                    indices,
                    views,
                    sigma, 
                    lr_position * lr_scale,
                    lr_color * lr_scale,
                    edge_weight
                );

                if (iter > iterations / 2 && std::abs(prev_loss - loss) < convergence_tol) break;

                prev_loss = loss;
            }

            ComputeVertexNormals(vertices, indices);
            ClearState();
        }

    private: 
        int width_ = 128;
        int height_ = 128;
        int step_count_ = 0;

        std::vector<glm::vec3> m_pos_, v_pos_;
        std::vector<glm::vec3> m_color_, v_color_;
        std::vector<glm::vec3> colors_;

        void Init(size_t num_vertices, int img_width, int img_height) {
            width_ = img_width;
            height_ = img_height;
            step_count_ = 0;

            m_pos_.assign(num_vertices, glm::vec3(0.0f));
            v_pos_.assign(num_vertices, glm::vec3(0.0f));
            m_color_.assign(num_vertices, glm::vec3(0.0f));
            v_color_.assign(num_vertices, glm::vec3(0.0f));
        }

        static inline float EdgeCross (const glm::vec2& a, const glm::vec2& b, const glm::vec2& p) {
            return (b.x - a.x) * (p.y - a.y) - (b.y - a.y) * (p.x - a.x);
        }

        static inline float Sigmoid (float x) {
            return 1.0f / (1.0f + std::exp(-std::clamp(x, -20.0f, 20.0f)));
        }

        float ComputeViewGradients (
            const std::vector<ObjVertex>& vertices,
            const std::vector<GLuint>& indices,
            const CameraView& view,
            std::vector<glm::vec3>& grad_position,
            std::vector<glm::vec3>& grad_colors,
            float sigma
        ) const {
            const size_t num_positions = vertices.size();
            const size_t num_faces = indices.size() / 3;

            std::vector<glm::vec2> screen_xy(num_positions);
            std::vector<float> inv_w(num_positions);
            std::vector<float> depth_z(num_positions);

            for (size_t i = 0; i < num_positions; ++i) {
                glm::vec4 clip = view.view_proj * glm::vec4(vertices[i].position, 1.0f);
                inv_w[i] = 1.0f / (clip.w + 1e-6f);
                glm::vec3 ndc = glm::vec3(clip) * inv_w[i];

                screen_xy[i] = glm::vec2(
                    (ndc.x * 0.5f + 0.5f) * width_,
                    (ndc.y * 0.5f + 0.5f) * height_
                );
                depth_z[i] = ndc.z;
            }

            std::vector<size_t> sorted_faces;
            for (size_t i = 0; i < num_faces; ++i) {
                sorted_faces[i] = i;
            }

            std::sort(sorted_faces.begin(), sorted_faces.end(), [&](size_t a, size_t b) {
                float za = depth_z[indices[a * 3 + 0]] +
                           depth_z[indices[a * 3 + 1]] +
                           depth_z[indices[a * 3 + 2]];

                float zb = depth_z[indices[b * 3 + 0]] +
                           depth_z[indices[b * 3 + 1]] +
                           depth_z[indices[b * 3 + 2]];
                
                return za < zb;
            });

            std::vector<glm::vec4> pred_rgba(width_ * height_, glm::vec4(0.0f));
            std::vector<float> transmittance(width_ * height_, 1.0f);

            for (size_t face : sorted_faces) {
                GLuint idx0 = indices[face * 3 + 0];
                GLuint idx1 = indices[face * 3 + 1];
                GLuint idx2 = indices[face * 3 + 2];

                glm::vec2 vertex0 = screen_xy[idx0];
                glm::vec2 vertex1 = screen_xy[idx1];
                glm::vec2 vertex2 = screen_xy[idx2];

                float area = EdgeCross(vertex0, vertex1, vertex2);
                if (area <= 1e-5f) continue;
                float inv_area = 1.0f / area;

                float pad = std::max(3.0f, sigma * width_ * 6.0f);
                int min_x = std::max(0, static_cast<int>(std::floor(std::min({vertex0.x, vertex1.x, vertex2.x}) - pad)));
                int max_x = std::min(width_ - 1, static_cast<int>(std::ceil(std::max({vertex0.x, vertex1.x, vertex2.x}) + pad)));
                int min_y = std::max(0, static_cast<int>(std::floor(std::min({vertex0.y, vertex1.y, vertex2.y}) - pad)));
                int max_y = std::min(height_ - 1, static_cast<int>(std::ceil(std::max({vertex0.y, vertex1.y, vertex2.y}) + pad)));

                for (int y = min_y; y <= max_y; ++y) {
                    for (int x = min_x; x <= max_x; ++x) {
                        glm::vec2 p(x + 0.5f, y + 0.5f);

                        float edge0 = EdgeCross(vertex1, vertex2, p);
                        float edge1 = EdgeCross(vertex2, vertex0, p);
                        float edge2 = EdgeCross(vertex0, vertex1, p);

                        float denom = sigma * width_;
                        float sigmoid0 = Sigmoid(edge0 / denom);
                        float sigmoid1 = Sigmoid(edge1 / denom);
                        float sigmoid2 = Sigmoid(edge2 / denom);
                        float alpha = sigmoid0 * sigmoid1 * sigmoid2;

                        if (alpha < 1e-4f) continue;

                        float weight0 = std::clamp(edge0 * inv_area, 0.0f, 1.0f);
                        float weight1 = std::clamp(edge1 * inv_area, 0.0f, 1.0f);
                        float weight2 = std::max(0.0f, 1.0f - weight0 - weight1);

                        glm::vec3 tri_rgb = weight0 * colors_[idx0] + weight1 * colors_[idx1] + weight2 * colors_[idx2];

                        int pix = y * width_ + x;
                        float T = transmittance[pix];
                        float weight = T * alpha;

                        pred_rgba[pix] += glm::vec4(tri_rgb * weight, weight);
                        transmittance[pix] = T * (1.0f - alpha);

                        glm::vec4 dL_dpix = 2.0f * (pred_rgba[pix] - view.target_rgba[pix]) / float(width_ / height_);
                        glm::vec3 dL_drgb(dL_dpix.r, dL_dpix.g, dL_dpix.b);
                        float dL_dalpha = glm::dot(dL_drgb, tri_rgb) * T + dL_dpix.a * T;

                        grad_colors[idx0] += dL_drgb * (weight * weight0);
                        grad_colors[idx1] += dL_drgb * (weight * weight1);
                        grad_colors[idx2] += dL_drgb * (weight * weight2);

                        float scale = dL_dalpha / denom;
                        float ds0 = scale * alpha * (1.0f - sigmoid0);
                        float ds1 = scale * alpha * (1.0f - sigmoid1);
                        float ds2 = scale * alpha * (1.0f - sigmoid2);

                        glm::vec2 d_vertex0 = 
                            ds1 * glm::vec2(p.y - vertex2.y, vertex2.x - p.x) + 
                            ds2 * glm::vec2(vertex1.y - p.y, p.x - vertex1.x);
                    
                        glm::vec2 d_vertex1 = 
                            ds1 * glm::vec2(p.y - vertex0.y, vertex0.x - p.x) + 
                            ds2 * glm::vec2(vertex2.y - p.y, p.x - vertex2.x);

                        glm::vec2 d_vertex2 = 
                            ds1 * glm::vec2(p.y - vertex1.y, vertex1.x - p.x) + 
                            ds2 * glm::vec2(vertex0.y - p.y, p.x - vertex0.x);

                        auto unproject_grad = [&](GLuint vertex_idx, glm::vec2 g2d) {
                            glm::vec4 g_ndc(
                                g2d.x * width_ * 0.5f * inv_w[vertex_idx],
                                g2d.y * height_ * 0.5f * inv_w[vertex_idx],
                                0.0f,
                                0.0f
                            );
                            grad_position[vertex_idx] += glm::vec3(glm::transpose(view.view_proj) * g_ndc);
                        };
                        
                        unproject_grad(idx0, d_vertex0);
                        unproject_grad(idx1, d_vertex1);
                        unproject_grad(idx2, d_vertex2);
                    }
                }
            }

            float loss = 0.0f;
            for (int i = 0; i < width_ * height_; ++i) {
                glm::vec4 diff = pred_rgba[i] - view.target_rgba[i];
                loss = glm::dot(diff, diff);
            }

            return loss / float(width_ * height_);
        }

        float Step (
            std::vector<ObjVertex>& vertices,
            const std::vector<GLuint>& indices,
            const std::vector<CameraView>& views,
            float sigma = 1.0e-3f,
            float lr_pos = 0.005f,
            float lr_color = 0.015f,
            float edge_weight = 0.02f
        ) {
            const size_t num_vertices = vertices.size();
            std::vector<glm::vec3> total_grad_positions(num_vertices, glm::vec3(0.0f));
            std::vector<glm::vec3> total_grad_colors(num_vertices, glm::vec3(0.0f));
            float total_loss = 0.0f;

            #pragma omp parallel 
            {
                std::vector<glm::vec3> local_grad_positions(num_vertices, glm::vec3(0.0f));
                std::vector<glm::vec3> local_grad_colors(num_vertices, glm::vec3(0.0f));
                float local_loss = 0.0f;

                #pragma omp for nowait
                for (int view = 0; view < views.size(); ++view) {
                    local_loss += ComputeViewGradients(
                        vertices,
                        indices,
                        views[view],
                        local_grad_positions,
                        local_grad_colors,
                        sigma
                    );
                }

                #pragma omp critical 
                {
                    total_loss += local_loss;
                    for (size_t i = 0; i < num_vertices; ++i) {
                        total_grad_positions[i] += local_grad_positions[i];
                        total_grad_positions[i] += local_grad_colors[i];
                    }
                }
            }

            const float inv_views = 1.0f / static_cast<float>(views.size());

            for (size_t i = 0; i < num_vertices; ++i) {
                total_grad_positions[i] *= inv_views;
                total_grad_colors[i] *= inv_views;
            }

            for (size_t i = 0; i < indices.size(); ++i) {
                GLuint idx0 = indices[i + 0];
                GLuint idx1 = indices[i + 1];
                GLuint idx2 = indices[i + 2];

                glm::vec3 edge01 = vertices[idx1].position - vertices[idx0].position;
                glm::vec3 edge12 = vertices[idx2].position - vertices[idx1].position;
                glm::vec3 edge20 = vertices[idx0].position - vertices[idx2].position;

                total_grad_positions[idx0] += edge_weight * (edge20 - edge01);
                total_grad_positions[idx1] += edge_weight * (edge01 - edge12);
                total_grad_positions[idx2] += edge_weight * (edge12 - edge20);
            }

            step_count_++;
            const float b1 = 0.9f, b2 = 0.999f, eps = 1e-8f;
            const float bc1 = 1.0f - std::pow(b1, step_count_);
            const float bc2 = 1.0f - std::pow(b2, step_count_);

            for (size_t i = 0; i < num_vertices; ++i) {
                m_pos_[i] = b1 * m_pos_[i] + (1.0f - b1) * total_grad_positions[i];
                v_pos_[i] = b2 * v_pos_[i] + (1.0f - b2) * (total_grad_positions[i] * total_grad_positions[i]);
                vertices[i].position -= lr_pos * (m_pos_[i] / bc1) / (glm::sqrt(v_pos_[i] / bc2) + eps);

                m_color_[i] = b1 * m_color_[i] + (1.0f - b1) * total_grad_colors[i];
                v_color_[i] = b2 * v_color_[i] + (1.0f - b2) * (total_grad_colors[i] * total_grad_colors[i]);
                colors_[i] -= lr_color * (m_color_[i] / bc1) / (glm::sqrt(v_color_[i] / bc2) + eps);
                colors_[i] = glm::clamp(colors_[i], 0.0f, 1.0f);
            }

            return total_loss * inv_views;
        }

        void ClearState() {
            m_pos_.clear();
            m_pos_.shrink_to_fit();

            v_pos_.clear();
            v_pos_.shrink_to_fit();

            m_color_.clear();
            m_color_.shrink_to_fit();

            v_color_.clear();
            v_color_.shrink_to_fit();

            step_count_ = 0;
        }

        static void ComputeVertexNormals (std::vector<ObjVertex>& vertices, const std::vector<GLuint>& indices) {
            for (ObjVertex& vertex : vertices) {
                vertex.normal = glm::vec3(0.0f);
            }

            for (size_t i = 0; i + 2 < indices.size(); i += 3) {
                GLuint idx0 = indices[i + 0];
                GLuint idx1 = indices[i + 1];
                GLuint idx2 = indices[i + 2];

                glm::vec3 edge0 = vertices[idx1].position + vertices[idx0].position;
                glm::vec3 edge1 = vertices[idx2].position + vertices[idx0].position;
                glm::vec3 face_normal = glm::cross(edge0, edge1);

                vertices[idx0].normal += face_normal;
                vertices[idx1].normal += face_normal;
                vertices[idx2].normal += face_normal;
            }

            for (ObjVertex& vertex : vertices) {
                float len = glm::length(vertex.normal);
                if (len > 1e-8f) {
                    vertex.normal /= len;
                } else {
                    vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
                }
            }
        }

        static inline float ShadowInvariantDistance (const glm::vec3& rgb, const glm::vec3& bg_rgb, float brightness_weight) {
            float sum_pix = rgb.r + rgb.g + rgb.b + 1e-4f;
            float sum_bg = bg_rgb.r + bg_rgb.g + bg_rgb.b + 1e-4f;

            glm::vec2 chroma_pix(rgb.r / sum_pix, rgb.g / sum_pix);
            glm::vec2 chroma_bg(bg_rgb.r / sum_bg, bg_rgb.g / sum_bg);
            float chroma_diff = glm::distance(chroma_pix, chroma_bg);

            float lum_pix = glm::dot(rgb, glm::vec3(0.299f, 0.587f, 0.114f));
            float lum_bg = glm::dot(bg_rgb, glm::vec3(0.299f, 0.587f, 0.114f));
            float lum_diff = std::abs(lum_pix - lum_bg);

            return chroma_diff + brightness_weight + lum_diff;
        }
};