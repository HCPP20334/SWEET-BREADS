#pragma once
#include <GL/GL.h>
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif

#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif

#pragma warning(push)
#pragma warning(disable : 4996)
#include "stb_image_write.h"
#pragma warning(pop)
#include <iostream>
#include <string>
#include <chrono>
typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int* attribList);
#define WGL_CONTEXT_MAJOR_VERSION_ARB             0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB             0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB              0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB          0x00000001
#define WGL_CONTEXT_FLAGS_ARB                     0x2094
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
HGLRC CreateGL(HDC hDC, int major, int minor)
{
    HGLRC hRC_temp = wglCreateContext(hDC);
    if (!hRC_temp) return NULL;

    if (!wglMakeCurrent(hDC, hRC_temp))
    {
        wglDeleteContext(hRC_temp);
        return NULL;
    }
    wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

    if (!wglCreateContextAttribsARB)
    {
        wglMakeCurrent(NULL, NULL);
        return hRC_temp;
    }
    const int attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, major,
        WGL_CONTEXT_MINOR_VERSION_ARB, minor,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };

    HGLRC hRC_final = wglCreateContextAttribsARB(hDC, 0, attribs);
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC_temp);
    std::cout << "(ogl) loaded" << major << "." << minor << "\n";
    return hRC_final;
}
#include <vector>
#include <thread>
#include <random>
#include <format>
#include <atomic>
#include <fstream>
#include <mutex>
#include "logger.hpp"
struct random {
    std::random_device rd;
    std::mt19937_64 rdmt;
    template <typename T>
    T generate(T value_max, T value_min = 0)
    {
        rdmt.seed(rd());
        if constexpr (std::is_floating_point_v<T>) {
            std::uniform_real_distribution<T> dist(0, value_max);
            return dist(rdmt);
        }
        else {
            std::uniform_int_distribution<T> dist(0, value_max);
            return dist(rdmt);
        }
    }
};
auto rnd = std::make_unique<random>();
struct colorU32 {
    uint32_t r, g, b, a;
    colorU32() : r(0), g(0), b(0), a(0) {}
    colorU32(uint32_t _r, uint32_t _g, uint32_t _b, uint32_t _a) : r(_r), g(_g), b(_b), a(_a) {}
    uint32_t get() const {
        return (((uint32_t)(this->a) << 24) | ((uint32_t)(this->b) << 16) | ((uint32_t)(this->g) << 8) | ((uint32_t)(this->r) << 0));
    }
    // get convert ColorU32 RGBA to ImVec4
    ImVec4 getIV4() const {
        colorU32 colorGet = { this->r,this->g,this->b,this->a };
        return ImVec4(
            static_cast<float>(this->r / 255),
            static_cast<float>(this->g / 255),
            static_cast<float>(this->b / 255),
            static_cast<float>(this->a / 255)

        );
    }
    std::string HEX() const {
        return (std::format("{:02X}{:02X}{:02X}{:02X}", this->r, this->g, this->b, this->a));
    }
    // Debug Features to check colors
    std::string toStringView() const {
        return std::format("U32RGBA({},{},{},{})->0x{}", this->r, this->g, this->b, this->a, colorU32(this->r, this->g, this->b, this->a).get());
    }


};
// convert to ImVec4 color to ColorU32 | Work Correctly
uint32_t ToU32(ImVec4 color) {
    ImVec4 col = { color.x * 255 ,color.y * 255,color.z * 255,color.w * 255 };
    return (((uint32_t)(col.w) << 24) | ((uint32_t)(col.z) << 16) | ((uint32_t)(col.y) << 8) | ((uint32_t)(col.x) << 0));
}
struct u32Vec2 {
    uint32_t w, h;
    u32Vec2() : w(0), h(0) {}
    u32Vec2(uint32_t _w, uint32_t _h) : w(_w), h(_h) {}
};
struct GLM {
    int iVtxOffset = 64000;
    uint64_t u64Pixels = 0;
    void Pen(int draw_x, int draw_y, colorU32 u32Color);
    void toU32Buffer(uint8_t* buffer_std);
    std::vector<uint32_t>pixel_buffer;
    int64_t i64BrushSize = 20;
    GLuint textureID;
    int width_texture = 0, height_texture = 0;
    // Init Texture
   
    void InitTexture() {
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_texture, height_texture, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, pixel_buffer.data());
    }
   
    void getCoordPixel(uint32_t x, uint32_t y) {
        std::string buf = std::format("coord_pixelX:{}\ncoord_pixelY:{}", x, y);
        std::ofstream coordImage("coord.wdt");
        if (coordImage.is_open()) {
            coordImage.write(buf.c_str(), buf.size());
            coordImage.close();
        }
    }
    void printImage() {
        std::string suffix = "coord_pixelX:", suffix1 = "coord_pixelY:";
        std::ifstream coordImage("coord.wdt", std::ios::binary);
        std::string buffer_out;
        int x_out = 0, y_out = 0;
        coordImage.seekg(0, std::ios::end);
        size_t size = coordImage.tellg();
        coordImage.seekg(0, std::ios::beg);
        coordImage.read(buffer_out.data(), size);

        if (buffer_out.rfind(suffix, 0) == 0) {
            x_out = stoi(buffer_out.substr(suffix.length()));
            SetPixel(x_out, y_out, colorU32(255, 255, 255, 244).get());
        }
        if (buffer_out.rfind(suffix1, 0) == 0) {
            y_out = stoi(buffer_out.substr(suffix1.length()));
            SetPixel(x_out, y_out, colorU32(255, 255, 255, 244).get());
        }
        coordImage.close();
    }
    uint64_t xor64(int t) {
        uint64_t seed = (t + __rdtsc());
        seed ^= seed << 16;
        seed ^= seed << 8;
        seed ^= seed >> 4;
        return seed;
    }
   
    void noise(uint32_t x, uint32_t y) {
        const uint32_t num_threads = std::jthread::hardware_concurrency();
        std::vector<std::jthread> threads;
        uint32_t stripe_height = y / num_threads;

        for (uint32_t t = 0; t < num_threads; t++) {
            uint32_t startY = t * stripe_height;
            uint32_t endY = (t == num_threads - 1) ? y : startY + stripe_height;

            threads.emplace_back([=, this] {
                for (uint32_t fill_y = startY; fill_y < endY; fill_y++) {
                    for (uint32_t fill_x = 0; fill_x < x; fill_x++) {
                        uint32_t random_alpha = (uint32_t)xor64(t) %(155, 100);
                        uint32_t color_rand[] = {
                            10,15,
                            13,18,
                            19,
                            22,25,25
                        };
                        std::atomic<int> rd = 0;
                        rd.fetch_add(1, std::memory_order_relaxed);
                        if (rd.load() > std::size(color_rand)) {
                            rd = 0;
                        }
                        SetPixel(fill_x, fill_y, colorU32{ color_rand[rd], color_rand[rd], color_rand[rd], random_alpha }.get());
                    }
                }
                });
        }
    }
    bool init = false;
    void Overlay(int x, int y) {


        ImGui::SetCursorPos({ 0,0 });
        ImGui::Image(int64_t((void*)textureID), ImVec2(x, y));


    }
    void SetRawPixelDataToFile(const std::string& filename) {
        if (pixel_buffer.empty()) return;
        std::ofstream outFile(filename, std::ios::binary);
        std::cout << "Saving..  " << filename << std::endl;
        outFile.write(reinterpret_cast<const char*>(pixel_buffer.data()),
            pixel_buffer.size() * sizeof(uint32_t));
        outFile.close();
        std::cout << "Raw data saved to " << filename << std::endl;
        ImGui::GetIO().DeltaTime;
    }
    void SetSize(int w, int h) {
        width_texture = w;
        height_texture = h;
        pixel_buffer.resize(w * h);
        std::fill(pixel_buffer.begin(), pixel_buffer.end(), 255);
        std::cout << " (PahomEngine::GLM) func: SetSize(int w, int h) :-> set pixel_buffer size " << width_texture << " x " << height_texture << std::endl;

    }
    // Add Pixel to Pixel Buffer
    void SetPixel(int x, int y, uint32_t color) {
        if (x >= 0 && x < width_texture && y >= 0 && y < height_texture) {
            pixel_buffer[y * width_texture + x] = color;
            // getCoordPixel(x, y);
            // std::cout << " (PahomEngine::GLM) func: SetSize(int w, int h) :-> set pixel_buffer size " << width_texture << " x " << height_texture << std::endl;
          
        }
    }
    // segment code to not usuly
   /* void copyPixel(GLuint textureImage) {
        textureID = textureImage;
    }
    void GenTextureToPixelBuffer(void* pixel_buffer,GLuint* textureOut) {
        GLuint texture = 0;
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexSubImage2D(
            GL_TEXTURE_2D,
            0,
            0, 0,
            width_texture, height_texture,
            GL_RGBA, GL_UNSIGNED_BYTE,
            pixel_buffer
        );
        glBindTexture(GL_TEXTURE_2D, 0);
        *textureOut = texture;
    }
    void SwapRawPixel(uint8_t* val, uint32_t *val2) {
        *val2 = reinterpret_cast<uint32_t>(reinterpret_cast<void*>(val));
    }*/

    void UpdateTextureV2(uint8_t* data,GLuint& tdata, int& w, int& h) {
        if (textureID == 0) return;
        glBindTexture(GL_TEXTURE_2D, tdata);
        glTexSubImage2D(
            GL_TEXTURE_2D,
            0,
            0, 0,
            w, h,
            GL_RGBA, GL_UNSIGNED_BYTE,
            data
        );
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    // Update Texture: needed to Update Pixels to texture
    void InitTextureV2(const uint8_t* data, GLuint& tdata, int* w, int* h) {
        if (tdata != 0) {
            glDeleteTextures(1, &tdata);
        }

        glGenTextures(1, &tdata);
        glBindTexture(GL_TEXTURE_2D, tdata);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        int targetWidth = (w != nullptr) ? *w : 0;
        int targetHeight = (h != nullptr) ? *h : 0;
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, targetWidth, targetHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    void UpdateTexture() {
        if (textureID == 0) return;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexSubImage2D(
            GL_TEXTURE_2D,
            0,
            0, 0,
            width_texture, height_texture,
            GL_RGBA, GL_UNSIGNED_BYTE,
            pixel_buffer.data()
        );
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    std::atomic<int> atrand() {
        return rand();
    }
    void FillTexture(uint32_t data, u32Vec2 size) {
        uint32_t u32Allocate = 0;
        if (size.w < 0 && size.h < 0) {
            u32Allocate = (512 * 512) + 4;
        }
        else {
            u32Allocate = (size.w * size.h) + 4;
        }
        pixel_buffer.resize(u32Allocate);
        std::fill(pixel_buffer.begin(), pixel_buffer.end(), data);
        UpdateTexture();
    }
    //
    std::atomic<double> g_dElapsed = 0.0;

    // unit test #0 fill random multicolor square/ Worked! Paint Fill Random Pixel to Set Size
    void fillSqware(int size_x, int size_y, bool isEnableMultiThreads = false, uint32_t tmax = 0) {
        if (!isEnableMultiThreads) {
            auto start_time = std::chrono::high_resolution_clock::now();

            for (int x = 0; x < size_x; x++) {
                for (int y = 0; y < size_y; y++) {
                    SetPixel(x, y, colorU32(rand() % 255, rand() % 255, rand() % 255, rand() % 255).get());
                }
            }

            auto end_time = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> elapsed = end_time - start_time;

            std::cout << std::format("   (PahomEngine::GLM) => FillSqware() [Single] time: {:.3f} ms\n", elapsed.count());
        }
        else {
            uint32_t num_threads = tmax < 1 ? std::jthread::hardware_concurrency() : tmax;
            size_t total_pixels = (size_t)size_x * size_y;
            size_t chunk_size = total_pixels / num_threads;
            auto start_time = std::chrono::high_resolution_clock::now();
            {
                std::vector<std::jthread> tCPUThreads;
                tCPUThreads.reserve(num_threads);

                for (uint32_t t = 0; t < num_threads; ++t) {
                    size_t start_index = t * chunk_size;
                    size_t end_index = (t == num_threads - 1) ? total_pixels : (t + 1) * chunk_size;

                    tCPUThreads.emplace_back([=] {
                        uint32_t state = static_cast<uint32_t>(t + time(nullptr));
                        auto xorshift32 = [&state]() {
                            state ^= state << 13;
                            state ^= state >> 17;
                            state ^= state << 5;
                            return state;
                            };

                        for (size_t i = start_index; i < end_index; ++i) {
                            uint32_t r = xorshift32();
                            SetPixel(i % size_x, i / size_x, r | 0xFF000000);
                        }
                        });
                }
            } 
            auto end_time = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> elapsed = end_time - start_time;

            g_dElapsed.store(elapsed.count(), std::memory_order_relaxed);
            std::cout << std::format("   (PahomEngine::GLM) => FillSqware() [Multi, {} threads] time: {:.3f} ms\n", num_threads, elapsed.count());
        }
    }
    bool bSelectorTextureIsOpen = false;

#define GLM_ARRAYSIZE(_ARR) (static_cast<int>(sizeof(_ARR) / sizeof(*(_ARR)))) 
#define i64Texture(tex) (reinterpret_cast<int64_t>(reinterpret_cast<void*>(tex)))
#define color_null colorU32(0,0,0);
    Logger Ln;
    void swapTextures(GLuint in) {
        Ln.send(1, "GLM::swapTextures() -> swap {:p} to {:p}", reinterpret_cast<void*>(&in), reinterpret_cast<void*>(&textureID));
        textureID = in;
        Ln.send(1, "GLM::swapTextures() -> swaped  {:p} to {:p}", reinterpret_cast<void*>(&in), reinterpret_cast<void*>(&textureID));
    }
    void savePng(std::string filename) {

        stbi_write_png(filename.c_str(), width_texture, height_texture, 4, pixel_buffer.data(), width_texture * 4);
        std::cout << " (PahomEngine::GLM) func: savePng(std::string_view filename,int x,int y)-> saved to " << filename << " size: " << width_texture << "x" << height_texture << std::endl;
    }
    void SelectTextureToSwapUI(GLuint* arrayTextures, size_t maxTextures, uint8_t** u8ptr_buffer) {
        int nLineImage = 0;
        static GLuint glTextureSwap;
        if (ImGui::BeginPopup("SelectToTexture", ImGuiWindowFlags_AlwaysAutoResize)) {
            bSelectorTextureIsOpen = true;
            if (GLM_ARRAYSIZE(arrayTextures) > 0) {
                for (int getTextureList = 0; getTextureList < maxTextures; getTextureList++) {
                    /* ImGui::PushID(getTextureList);
                     if (ImGui::Button("Copy")) {
                         toU32Buffer(u8ptr_buffer[getTextureList]);
                     }
                     ImGui::PopID();*/
                    ImGui::SameLine();
                    ImGui::Image(i64Texture(arrayTextures[getTextureList]), ImVec2(128, 128));
                    if (ImGui::IsItemClicked()) {
                        arrayTextures[getTextureList] = textureID;
                        bSelectorTextureIsOpen = false;
                        ImGui::CloseCurrentPopup();
                    }
                    nLineImage++;
                    if (nLineImage > 300) {
                        ImGui::SameLine();
                        nLineImage = 0;
                    }
                }

            }
            ImGui::EndPopup();
        }
    }
    void ImPosX(float x) {
        ImGui::SetCursorPosX(x);
    }
    void pushImageToPaintBuffer(const uint8_t* data, size_t pixels) {
        const uint32_t* buffer_start = reinterpret_cast<const uint32_t*>(data);
        const uint32_t* buffer_end = buffer_start + pixels;
        pixel_buffer.assign(buffer_start, buffer_end);
    }



};
void GLM::toU32Buffer(uint8_t* buffer_std) {
    std::copy(reinterpret_cast<uint32_t*>(reinterpret_cast<void*>(buffer_std)), reinterpret_cast<uint32_t*>(reinterpret_cast<void*>(buffer_std)) + pixel_buffer.size(), pixel_buffer.data());
}
void GLM::Pen(int draw_x, int draw_y, colorU32 u32Color) {
    int brush_size = 20;
    int half_brush = brush_size / 2;
    for (int x = draw_x - half_brush; x < draw_x + half_brush; x++) {
        for (int y = draw_y - half_brush; y < draw_y + half_brush; y++) {
            if (x >= 0 && x < width_texture &&
                y >= 0 && y < height_texture)
            {
                SetPixel(x, y, u32Color.get());
            }
        }
    }
}

GLM glpx;
bool bStopBench = false;
void FillBenchCPU(int iSizeX, int iSizeY, uint32_t tmax) {
    ImGui::Begin("Fill OGL Test", NULL);
    ImGui::SetWindowSize({ 512,512 });
    ImGui::SetCursorPos({ 0,0 });
    ImGui::Image((int64_t)(void*)glpx.textureID, { 512,512 });
    glpx.UpdateTexture();
    ImGui::End();
}

