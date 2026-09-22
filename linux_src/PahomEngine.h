#define CRT_NO_WARNINGS 1
#include "imgui.h"
#include "imgui_impl_opengl2.h"
#include "imgui_impl_sdl2.h" // Заменено с imgui_impl_win32.h на SDL2 бэкенд ImGui
#include <SDL.h>
#include <SDL_opengl.h>
#include <string>
#include <iostream>
#include <fstream>
#include <string>
#include "imgui_stdlib.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "imgui_internal.h"
#include <filesystem>

#include <random>
#include <thread>
#include <mutex>

#define PAHOM_ENGINE_ID (int64_t)0x17

#pragma comment(lib,"Audio.lib")
typedef void (APIENTRY* PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
typedef void (APIENTRY* PFNGLGETBUFFERPARAMETERIVPROC)(GLenum target, GLenum pname, GLint* params);

// typedefs
int64_t fstack(void* reg, std::string param_name) {
    std::cout << param_name << "=" << reinterpret_cast<int64_t>(reg) << " stack=" << reg;
    return reinterpret_cast<int64_t>(reg);
}
std::string str_stack(void* reg, const std::string& param_name) {
    const char* str = static_cast<const char*>(reg);
    std::cout << param_name << "=" << reinterpret_cast<uintptr_t>(reg)
        << " stack=" << reg << std::endl;
    return std::string(str);
}

namespace ImGui {
    bool CustomToggle(const char* label, bool* v) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        ImGui::PushID(label); ImGui::TextColored((*v ? ImVec4(1.0f, 1.0f, 1.0f, 1.00f) : ImVec4(0.65f, 0.65f, 0.65f, 1.00f)), label);
        ImGui::SameLine();
        float height = ImGui::GetFrameHeight();
        float width = height * 1.55f;
        ImGuiStyle& style = ImGui::GetStyle();

        ImVec2 p = ImGui::GetCursorScreenPos();

        ImU32 color_bg_on = ImGui::GetColorU32(ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImU32 color_bg_off = ImGui::GetColorU32(ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImU32 color_knob = *v ? ImGui::GetColorU32(ImVec4(0.34f, 0.06f, 0.98f, 1.00f)) :
            ImGui::GetColorU32(ImVec4(0.36f, 0.36f, 0.36f, 1.0f));
        ImU32 border_color = ImGui::GetColorU32(ImGuiCol_Border);

        ImGui::InvisibleButton(label, ImVec2(width, height));
        bool clicked = ImGui::IsItemClicked();
        if (clicked) *v = !(*v);

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), *v ? color_bg_on : color_bg_off, height);

        if (style.FrameBorderSize > 0.0f) {
            draw_list->AddRect(
                p,
                ImVec2(p.x + width, p.y + height),
                border_color,
                height * 0.5f,
                0,
                style.FrameBorderSize
            );
        }

        float base_radius = *v ? 10 : 8;
        ImVec2 knob_pos = *v ?
            ImVec2(p.x + width - height * 0.5f, p.y + height * 0.5f) :
            ImVec2(p.x + height * 0.5f, p.y + height * 0.5f);

        if (*v) {
            const int glow_layers = 3;
            for (int i = 0; i < glow_layers; i++) {
                float alpha = 0.3f * (1.0f - (float)i / glow_layers);
                float radius = base_radius + 2.0f * (i + 1);
                draw_list->AddCircle(
                    knob_pos,
                    radius,
                    ImGui::GetColorU32(ImVec4(0.34f, 0.06f, 0.98f, alpha)),
                    0,
                    2.0f
                );
            }
        }

        draw_list->AddCircleFilled(knob_pos, base_radius, color_knob);

        ImGui::PopID();
        return clicked;
    }
    bool Spinner(const char* label, float radius, int thickness, const ImU32& color) {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);

        ImVec2 pos = window->DC.CursorPos;
        ImVec2 size((radius) * 2, (radius + style.FramePadding.y) * 2);

        const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
        ItemSize(bb, style.FramePadding.y);
        if (!ItemAdd(bb, id))
            return false;

        window->DrawList->PathClear();

        int num_segments = 30;
        int start = abs(ImSin(g.Time * 1.8f) * (num_segments - 5));

        const float a_min = IM_PI * 2.0f * ((float)start) / (float)num_segments;
        const float a_max = IM_PI * 2.0f * ((float)num_segments - 3) / (float)num_segments;

        const ImVec2 centre = ImVec2(pos.x + radius, pos.y + radius + style.FramePadding.y);

        for (int i = 0; i < num_segments; i++) {
            const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
            window->DrawList->PathLineTo(ImVec2(centre.x + ImCos(a + g.Time * 8) * radius,
                centre.y + ImSin(a + g.Time * 8) * radius));
        }

        window->DrawList->PathStroke(color, false, thickness);
        return true;
    }
    bool InputInt64(const char* label, int64_t* v, int64_t step, int64_t step_fast, ImGuiInputTextFlags flags) {
        const char* format = (flags & ImGuiInputTextFlags_CharsHexadecimal) ? "%08X" : "%d";
        return InputScalar(label, ImGuiDataType_S64, (void*)v, (void*)(step > 0 ? &step : NULL), (void*)(step_fast > 0 ? &step_fast : NULL), format, flags);
    }
    bool SliderInt64(const char* label, int64_t* v, int64_t v_min, int64_t v_max, const char* format, ImGuiSliderFlags flags) {
        return SliderScalar(label, ImGuiDataType_S64, v, &v_min, &v_max, format, flags);
    }
}

ImVec4 RGBAtoIV4(int r, int g, int b, int a) {
    float newr = (float)r / 255.0f;
    float newg = (float)g / 255.0f;
    float newb = (float)b / 255.0f;
    float newa = (float)a;
    return ImVec4(newr, newg, newb, newa);
}

class CImage {
public:
    float x[2] = { 250, 230 };
    float y[2] = { 250, 230 };
    int fIctx = 0;
    int fIcty = 0;
    GLuint gl_buffer;
    unsigned char CICharBuffer;
    bool CreateImg();
    bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height, unsigned char* imgBuffer);
    ImVec2 ResizeImage(uint64_t fCArrayFloat);
    uint64_t InitCImage(std::string png_file);
    bool CreateImage(int64_t w, int64_t h);
    std::string getAspectRatio(int x, int y);
    int64_t GetImageSize(int width, int height);
    int64_t GetVRAMSize(GLuint vbo);
    int64_t GetFileSize(const std::string& filename);
};

std::string CImage::getAspectRatio(int x, int y) {
    double ratio = (double)x / y;
    struct { double val; std::string name; } list[] = {
        {4.0 / 3.0, "4:3"},
        {16.0 / 9.0, "16:9"},
        {21.0 / 9.0, "21:9"},
        {3.0 / 2.0, "3:2"},
        {1.0, "1:1"},
        {5.0 / 4.0, "5:4"},
        {32.0 / 9.0, "32:9"},
        {8.0 / 5.0, "16:10"}
    };
    for (auto& a : list)
        if (std::fabs(ratio - a.val) < 0.01)
            return a.name;
    return std::to_string(ratio);
}

ImVec2 CImage::ResizeImage(uint64_t fCArrayFloat) {
    fIctx = (int)x[fCArrayFloat];
    fIcty = (int)y[fCArrayFloat];
    return ImVec2((float)fIctx, (float)fIcty);
}

bool CImage::CreateImage(int64_t w, int64_t h) {
    ImGui::Image((ImTextureID)(uintptr_t)gl_buffer, ImVec2((float)w, (float)h));
    return true;
}

uint64_t CImage::InitCImage(std::string png_file) {
    LoadTextureFromFile((png_file).c_str(), &gl_buffer, &fIctx, &fIcty, &CICharBuffer);
    return 3;
}

int64_t CImage::GetImageSize(int width, int height) {
    return static_cast<int64_t>(width) * height * 4;
}

int64_t CImage::GetFileSize(const std::string& filename) {
    std::ifstream ImageStream(filename, std::ios::binary | std::ios::ate);
    if (!ImageStream.is_open()) {
        std::cerr << "[CImage::GetFileSize] Failed to open file: " << filename << std::endl;
        return 0;
    }
    std::streampos fileSize = ImageStream.tellg();
    ImageStream.close();
    return fileSize;
}

bool CImage::LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height, unsigned char* imgBuffer) {
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (!image_data) {
        return false;
    }
    else {
        *imgBuffer = *image_data;
        GLuint image_texture;
        glGenTextures(1, &image_texture);
        glBindTexture(GL_TEXTURE_2D, image_texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
        stbi_image_free(image_data);

        *out_texture = image_texture;
        *out_width = image_width;
        *out_height = image_height;

        glBindTexture(GL_TEXTURE_2D, 0);
        return true;
    }
}

struct KurlikAUDIO {
    std::string audiolist[6] = {
        "assets/audio/kurlik.wav",
        "assets/audio/voda.wav",
        "assets/audio/krik.wav",
        "assets/audio/aaa.wav",
        "assets/audio/mrrobot.wav",
        "assets/audio/33.wav"
    };
    void play(int64_t i);
    void play2(int64_t i);
    int64_t idx = 0;
    float masterVolume = 0.02f;
    void pause();
    std::mutex audio_d;
    std::thread audio_t;
    bool audio_stop = false;

    void aud() {
        audio_stop = false;
        audio_t = std::thread([=, this] {
            std::lock_guard<std::mutex> a(audio_d);
            // Заменено WinAPI Beep на SDL_Delay или кроссплатформенный аналог логики
            SDL_Delay(2);
            SDL_Delay(2);
            SDL_Delay(3);
            audio_stop = true;
        });
        if (!audio_stop) {
            audio_t.join();
        }
        else {
            audio_t.detach();
        }
    }
};

void KurlikAUDIO::play(int64_t i) {
    aud();
    std::cout << "(audio) audio off\n";
}
void KurlikAUDIO::play2(int64_t i) {
    aud();
    std::cout << "(audio) audio off\n";
}
void KurlikAUDIO::pause() {
    aud();
    std::cout << "(audio) audio off\n";
}

struct STRINGSDATA {
    void log(std::string text, std::string moduleName) {
        std::cout << "[PahomEngine::" << moduleName << text << std::endl;
    }
    std::string PAHOM_ENGINE =
        " Engine b0.23. \n";
};

#define LOGO_IMAGE    0
#define BACK_IMAGE    1
#define BREAD_IMAGE   2
#define PAHOM_IMAGE   3
#define PAHOM2_IMAGE  4
#define PANEL_IMAGE   5

struct ASSETSDATA {
    std::string asset[6] = {
        "assets/logo.png",
        "assets/back.jpg",
        "assets/bread.png",
        "assets/pahom.png",
        "assets/pahom2.png",
        "assets/panel.png"
    };

    bool validFiles(std::string file) {
        std::ifstream fileAssets(file);
        if (fileAssets.is_open()) {
            fileAssets.close();
            return true;
        }
        return false;
    }
};

struct IMAGEDATA {
    GLuint  TextureArray[256];
    uint8_t TextureBufferArray[256];
    int TextureX[256];
    int TextureY[256];
};

struct KEYMAPDATA {
    int8_t u8FORWARD = SDL_SCANCODE_A;
    int8_t u8BACK = SDL_SCANCODE_D;
    int8_t u8RESET = SDL_SCANCODE_R;
    int8_t u8SPACE = SDL_SCANCODE_SPACE; // Заменено на SDL сканкод/символ
    int8_t u8ButtonLeft = 'L';
    int8_t u8ButtonRight = 'R';
    int64_t kbDelay = 1;
    int64_t vMaxDelay = 1;

    void reMap(uint8_t key, uint8_t newBind) {
        key = newBind;
    }
};

struct MEMORYDATA {
    int64_t i64MemoryTotal = 0;
    int64_t i64MemoryFree  = 0;
    int64_t i64MemoryUsed  = 0;

    void getData(int64_t* mem, int64_t idx_data) {
        // Кроссплатформенная заглушка вместо GlobalMemoryStatus под WinAPI
        *mem = 16; // Пример возвращаемого значения в ГБ
    }

    std::string MemoryInfo() {
        getData(&i64MemoryTotal, 1);
        getData(&i64MemoryFree, 2);
        getData(&i64MemoryUsed, 0);
        std::string mem_str =
            "Total:   " + std::to_string(i64MemoryTotal) + " GB"
            "\nUsed :   " + std::to_string(i64MemoryUsed)  + " GB"
            "\nFree :   " + std::to_string(i64MemoryFree)  + " GB";
        return mem_str;
    }
};

auto ptrMemory = std::make_unique<MEMORYDATA>();

struct GameEvent {
    std::string TextBufferStr;
    ImVec4 col;
    bool isTextHidden = false;
    int64_t i64WindowSize[2] = { 800, 600 };

    void TextBuffer() {
        ImVec2 f2TextCenterPos = {
           (i64WindowSize[0] - ImGui::CalcTextSize(TextBufferStr.c_str()).x) / 2,
           (i64WindowSize[1] - ImGui::CalcTextSize(TextBufferStr.c_str()).y) / 2,
        };
        ImGui::SetCursorPos(f2TextCenterPos);
        if (!isTextHidden) {
            ImGui::TextColored(col, "%s", TextBufferStr.c_str());
        }
    }
    void Text(ImVec4 c, std::string text) {
        TextBufferStr += "\n" + text;
        setColorText(c);
    }
    void setColorText(ImVec4 c) {
        col = c;
    }
    void clearEvent() {
        TextBufferStr.clear();
    }
};

struct EXCEPTIONS {
    bool ErrorTextures = false;
    std::string sLastError;
    void* pLastStack = nullptr;
    void Write(std::string t, void* pErrorSegment) {
        ErrorTextures = true;
        sLastError += t;
        pLastStack = pErrorSegment;
    }
};

struct PahomEngineStruct {
    bool CVsync = true;
    uint64_t fCPoint = 0;
    int64_t fStep = 900;
    std::unique_ptr<CImage> img = std::make_unique<CImage>();

    bool StyleLoad();
    ImVec4 RGBA(float r, float g, float b, float a);
    void setTextCenter(const char* text);
    void setItemCenterX(float x);
    void setItemCenter(ImVec2 Size);

    // Заменено получение размеров окна через SDL_Window вместо HWND
    ImVec2 SdlWindowSize(SDL_Window* window) {
        int w = 0, h = 0;
        SDL_GetWindowSize(window, &w, &h);
        return ImVec2((float)w, (float)h);
    }

    STRINGSDATA strings;
    ASSETSDATA assets;
    IMAGEDATA ImageData;
    KEYMAPDATA keyMap;
    KurlikAUDIO audio;
    GameEvent Event;
    MEMORYDATA Mem;
    std::unique_ptr<EXCEPTIONS> Exceptions = std::make_unique<EXCEPTIONS>();

    bool bSettings = false;
    ImVec4 fillColorRGBA;
    int64_t i64WindowSize[2] = { 800, 600 };
    float fPahomPosX = 0;
    float fPahomPosY = 0;
    float fMaxPahomPosX = static_cast<float>(i64WindowSize[0]) - 128.0f;
    float fMaxPahomPosY = static_cast<float>(i64WindowSize[1]) - 128.0f;
    float fMinPahomPosX = 0;
    float fMinPahomPosY = 0;
    float updateInterval = 0;
    float fScoreCount = 0.000f;
    float fBreadPosX = 0;
    float fBreadPosY = 400;
    bool bLoadingFrame = true;
    bool bLoadingFrameOK = false;
    bool bControlsIsGamepad = false;
    bool bControlsIsKeyboard = false;
    bool bDebug = true;
    bool bFullscreen = false;
    bool bGameOver = false;
    int64_t i64CPUDelay = 10;
    bool bStartGame = false;
    float fStepMove = 500.0f;

    void InputI64(const char* id, int64_t* v);
    void InputFloat(const char* id, float* v);
    void reloadBreadPos();
    int64_t rand64(int64_t in_v);
    float randfloat(float in_v);
    bool CheckColiision();
    bool bStartGameFlag = false;
    bool bIsRevesed = false;
    bool bDebugText = false;
    bool GetGamepadKey(int64_t iKey);
    int64_t i64BreadSize[2] = { 64, 64 };
    int64_t i64PahomSize[2] = { 128, 128 };
    void Text(ImVec4 col, std::string text);
    void log(std::string text);
    void Tbuffer();
    void logo();
    void progress_bar(float fragtion);
};

void PahomEngineStruct::log(std::string text) {
    std::cout << " [PahomEngine::log] " << text << std::endl;
}
void PahomEngineStruct::Text(ImVec4 col, std::string text) {
    Event.Text(col, text);
}
void PahomEngineStruct::Tbuffer() {
    Event.TextBuffer();
}
bool PahomEngineStruct::GetGamepadKey(int64_t iKey) {
    return false;
}
int64_t PahomEngineStruct::rand64(int64_t in_v) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int64_t> dist(2, in_v);
    return dist(gen);
}
float PahomEngineStruct::randfloat(float in_v) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(2, in_v);
    return dist(gen);
}
void PahomEngineStruct::reloadBreadPos() {
    fBreadPosX = randfloat(fMaxPahomPosX);
}
void PahomEngineStruct::InputI64(const char* id, int64_t* v) {
    ImGui::InputInt64(id, v, 1, 100, 0);
}
void PahomEngineStruct::InputFloat(const char* id, float* v) {
    ImGui::InputFloat(id, v);
}
void PahomEngineStruct::setTextCenter(const char* text) {
    float fTextSize = ImGui::CalcTextSize(text).x;
    float fTextCenterPosition = (i64WindowSize[0] - fTextSize) / 2;
    ImGui::SetCursorPosX(fTextCenterPosition);
}
bool PahomEngineStruct::CheckColiision() {
    return (
        fPahomPosX < fBreadPosX + i64BreadSize[0] &&
        fPahomPosX + i64PahomSize[0] > fBreadPosX &&
        fPahomPosY < fBreadPosY + i64BreadSize[1] &&
        fPahomPosY + i64PahomSize[1] > fBreadPosY
        );
}
void PahomEngineStruct::setItemCenterX(float x) {
    float fItemXCenterPosition = (i64WindowSize[0] - x) / 2;
    ImGui::SetCursorPosX(fItemXCenterPosition);
}
void PahomEngineStruct::setItemCenter(ImVec2 Size) {
    float fItemXCenterPosition = (i64WindowSize[0] - Size.x) / 2;
    float fItemYCenterPosition = (i64WindowSize[1] - Size.y) / 2;
    ImGui::SetCursorPos(ImVec2(fItemXCenterPosition, fItemYCenterPosition));
}
ImVec4 PahomEngineStruct::RGBA(float r, float g, float b, float a) {
    float floatMaxColorRGBA = 255.0f;
    ImVec4 outRGBA{
        r / floatMaxColorRGBA,
        g / floatMaxColorRGBA,
        b / floatMaxColorRGBA,
        a / floatMaxColorRGBA,
    };
    return outRGBA;
}
void PahomEngineStruct::logo() {
    std::cout << strings.PAHOM_ENGINE << std::endl;
}
void PahomEngineStruct::progress_bar(float fragtion) {
    int64_t idx = static_cast<int64_t>(fragtion * 10);
    if (idx > 10) idx = 10;
    std::string str_array[] = {
        "[----------]",
        "[#---------]",
        "[##--------]",
        "[###-------]",
        "[####------]",
        "[#####-----]",
        "[######----]",
        "[#######---]",
        "[########--]",
        "[#########-]",
        "[##########]"
    };
    std::cout << str_array[idx] << std::endl;
}
bool PahomEngineStruct::StyleLoad() {
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg] = RGBA(7, 7, 9, 255);
    colors[ImGuiCol_ChildBg] = ImVec4(0.04f, 0.04f, 0.04f, 0.94f);
    colors[ImGuiCol_PopupBg] = RGBA(5, 5, 7, 255);
    colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 1.0f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.04f, 0.04f, 0.04f, 0.94f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.48f, 0.26f, 0.98f, 0.40f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.37f, 0.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.353f, 0.157f, 1.000f, 1.000f);
    colors[ImGuiCol_TitleBgActive] = RGBAtoIV4(135, 165, 255, 155);
    colors[ImGuiCol_Button] = RGBA(7, 7, 9, 255);
    colors[ImGuiCol_ButtonHovered] = RGBA(35, 35, 55, 255);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    ImGui::GetStyle().FrameBorderSize = 1;
    return true;
}

auto PahomEngine = std::make_unique<PahomEngineStruct>();
