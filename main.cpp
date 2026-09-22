#include "PahomEngine.h"
#include "imgui.h"
#include "imgui_internal.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <exception>
#include <thread>
#include <chrono>

static int64_t fwop = 0;
ImVec4 clear_color = ImVec4(0.01f, 0.01f, 0.02f, 0.80f);

uint64_t fMX = 0;
uint64_t fMY = 0;
bool b_FatalError = false;

int64_t ptrint64_t(GLuint tx) {
    return (int64_t)(void*)tx;
}

struct remapkeys {
    std::string sKeyLeft;
    std::string sKeyRight;
    std::string sButtonLeft;
    std::string sButtonRight;
};
auto Remap = std::make_unique<remapkeys>();
bool GetKeyState(auto& s,uint8_t k){
    return (s[k]);
}
bool GetAsyncKeyState(auto& s,uint8_t k){
    return (s[k]);
}
int main(int, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        std::cerr << "[Fatal Error] SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("SweetBreads(PahomEngineSDL2)_NoGamepad", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, window_flags);
    if (!window) {
        std::cerr << "[Fatal Error] SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (!gl_context) {
        std::cerr << "[Fatal Error] SDL_GL_CreateContext Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    if (!ImGui_ImplOpenGL2_Init()) {
        PahomEngine->log("[Fatal Error] OGL3 NOT SUPPORT ON YOUR GPU");
    } else {
        PahomEngine->logo();
        PahomEngine->log("(OGL3)::InitToSDL2 OK!");
    }

    io.Fonts->AddFontFromFileTTF("assets/Iosevka-Term.ttf", 20.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());

    struct STRGPUGL {
        std::string E_Brand = (const char*)glGetString(GL_VENDOR);
        std::string E_Model = (const char*)glGetString(GL_RENDERER);
        std::string E_GLVer = (const char*)glGetString(GL_VERSION);
    };
    auto sGPU = std::make_unique<STRGPUGL>();

    int CPUInfo[4] = { -1 };
    unsigned nExIds, i = 0;
    char CPUBrandString[0x40] = {0};
    // __cpuid(CPUInfo, 0x80000000);
    // nExIds = CPUInfo[0];
    // for (i = 0x80000000; i <= nExIds; ++i) {
    //     __cpuid(CPUInfo, i);
    //     if (i == 0x80000002)
    //         memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
    //     else if (i == 0x80000003)
    //         memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
    //     else if (i == 0x80000004)
    //         memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
    // }
    std::string dCPUBrandString = CPUBrandString;

    std::string mainText = " SWEETBREADS\n"
                           " Эта игра создана по фану от нехуй делать\n"
                           " demo_build_linux_sdl2\n"
                           " Разработал HCPP   \nDONATERS :)\n";
    static constexpr std::string_view donaters =
	"Kaban Films -    4403р\n"
	"qxlydo -         3422р\n"
	"Артём-е2п4ю -    2300р\n"
	"Dan Yabl -       20USD\n"
	"Lasti9k -        1400р\n"
	"Антон -           500р\n"
	"Imba -            500р\n"
	"Salvar II -       400р\n"
	"trqxxer -         400р\n"
	"redder -          400р\n"
	"Prosto_cheliik2 - 300р\n"
	"mr.hacker6435 -   222р\n"
	"Аноним -          200р\n"
	"Umbrella -        150р\n"
	"PRi8etA -         120р\n"
	"DIMA XP -         125р\n"
	"@AshickKerryb -   101р\n"
	"Егор 2 -          100р\n"
	"Егор -            100р\n"
	"Maya4ok_dev -     ХУЙ!\n"
	"kirillminecrafter- 100\n"
	"Intel HD Graphics- 100\n"
	"Igor Art -        100р\n"
	"lexa228pro -      100р\n"
	"Аноним -           50р\n"
	"Пук -              40р\n"
	"Aman_Legend0 -     20р\n";
    int64_t i64DonatersBufferSize = 0;
    bool donaters_parsed = false;
    for (int c = 0; c < donaters.size(); c++) {
	if (donaters[c] == '\n' && !donaters_parsed)
	{
		i64DonatersBufferSize++;
	}
    }
    bool done = false;
    uint64_t a = 0; uint64_t b = 0;
    uint64_t err = 0;

    ImFont* font15 = io.Fonts->AddFontFromFileTTF("assets/Iosevka-Term.ttf", 30.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());
    ImFont* font20 = io.Fonts->AddFontFromFileTTF("assets/Iosevka-Term.ttf", 20.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());
    ImFont* font3  = io.Fonts->AddFontFromFileTTF("assets/Iosevka-Term.ttf", 15, nullptr, io.Fonts->GetGlyphRangesCyrillic());
    ImFont* font10 = io.Fonts->AddFontFromFileTTF("assets/Iosevka-Term.ttf", 10, nullptr, io.Fonts->GetGlyphRangesCyrillic());
    ImFont* font50 = io.Fonts->AddFontFromFileTTF("assets/Iosevka-Term.ttf", 40, nullptr, io.Fonts->GetGlyphRangesCyrillic());

    if (!font15) {
        std::cout << "[PahomEngine::Font] file assets/BOUNDED.ttf not found!" << std::endl;
    } else {
        std::cout << "[PahomEngine::Font] file assets/BOUNDED.ttf loaded" << std::endl;
    }

    ImDrawListSplitter JEApp;

    while (!done) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        if (done)
            break;

        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        {
            JEApp.ClearFreeMemory();
            PahomEngine->StyleLoad();
        }
        JEApp.ClearFreeMemory();

        static std::string sError;
        if (PahomEngine->CVsync) {
            auto start = std::chrono::high_resolution_clock::now();
            auto end = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            if (elapsed < PahomEngine->i64CPUDelay) {
                std::this_thread::sleep_for(std::chrono::milliseconds(PahomEngine->i64CPUDelay - elapsed));
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(0));
        }

        static bool bLoaderFiles = false;
        static float ftx = 0.0f;
        static int64_t i64OGL3TxCount = 0, i64Exceptions = 0, i64OGL3Errors = 0, i64OGL3TxTotalSize = 0, i64VRAMSize = 0;
        static int64_t i64Cidx = 0;
        const Uint8* state = SDL_GetKeyboardState(NULL);
        ImGui::Begin("gameFrame", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
              if (ImGui::BeginPopup("ogl3_error", ImGuiWindowFlags_AlwaysAutoResize)) {
                  ImGui::Text("PahomEngine::exception");
                  ImGui::Separator();
                  ImGui::PushFont(font10);
                  ImGui::TextColored(PahomEngine->RGBA(233, 0, 65, 255), "%s",sError.c_str());
                  ImGui::PopFont();
                  ImGui::EndPopup();
              }
              ImGui::SetWindowSize(ImVec2(PahomEngine->i64WindowSize[0], PahomEngine->i64WindowSize[1]));
              ImGui::SetWindowPos(ImVec2(0, 0));
              if (PahomEngine->bLoadingFrame) {
                  b_FatalError = false;
                  static int64_t tid = 0, i64FrameDelay = 0;
                  if (bLoaderFiles) {
                      //PahomEngine->progress_bar(ftx);
                      if (PahomEngine->img->LoadTextureFromFile(reinterpret_cast<const char*>(PahomEngine->assets.asset[tid].c_str()),
                          &PahomEngine->ImageData.TextureArray[tid],
                          &PahomEngine->ImageData.TextureX[tid],
                          &PahomEngine->ImageData.TextureY[tid],
                          &PahomEngine->ImageData.TextureBufferArray[tid]))
                      {

                          i64OGL3TxTotalSize += PahomEngine->img->GetImageSize(PahomEngine->ImageData.TextureX[tid], PahomEngine->ImageData.TextureY[tid]);
                          i64OGL3TxCount++;

                          std::cout << "[PahomEngine::OGL] Load texture:"
                              << PahomEngine->assets.asset[tid]
                              << " id:" << tid << " ptrid(void*)"
                              << PahomEngine->ImageData.TextureArray[tid]
                              << " stack:" << &PahomEngine->ImageData.TextureArray[tid] << std::endl;
                      }
                      else {
                          std::cout << "[PahomEngine::OGL] Load texture:" << PahomEngine->assets.asset[tid] << " error!" << std::endl;
                          i64Exceptions++;
                          sError += "\n Load Texture Error!! \nstack=" + str_stack(&PahomEngine->ImageData.TextureBufferArray[tid], "PahomEngine->ImageData.TextureBufferArray[tid]") + "\nfunc:PahomEngine->img->LoadTextureFromFile(reinterpret_cast<const char*>(PahomEngine->assets.asset[tid].c_str()),\n"
                              " & PahomEngine->ImageData.TextureArray[tid],\n"
                              "  & PahomEngine->ImageData.TextureX[tid],\n"
                              "& PahomEngine->ImageData.TextureY[tid],\n"
                              "& PahomEngine->ImageData.TextureBufferArray[tid])\n";
                          if (PahomEngine->Exceptions) {
                              PahomEngine->Exceptions->Write("Load Texture Error!!", (void*)PahomEngine->ImageData.TextureArray[tid]);
                              ImGui::OpenPopup("ogl3_error");
                          }
                          else {
                              std::cerr << "Error: PahomEngine->Exceptions is nullptr!" << std::endl;
                          }
                          ImGui::OpenPopup("ogl3_error");
                      }
                      tid++;
                      if (tid > 5) {
                          tid = 5;
                          bLoaderFiles = false;
                          PahomEngine->bLoadingFrameOK = true;
                      }
                      ftx = (float)tid / 5;
                  }
                  a++;
                  if (a > 2) {
                      a = 0;
                      static float fcl = 0;
                      fcl += 25.5f;
                      if (fcl < 255) {
                          std::cout << "[PahomEngine::TextAlpha] flc:" << fcl << "b: " << b << std::endl;
                      }
                      if (fcl == 255) {
                          fcl = 255;
                          b = 255;
                          bLoaderFiles = true;


                      }
                      PahomEngine->fillColorRGBA = PahomEngine->RGBA(133, 133, 133, fcl);
                  }
                  static float fScrollChildFrame = 0.0f;
                  static float fMaxScroll = ImGui::CalcTextSize(donaters.data()).y + 20;
                  ImGui::SetCursorPosX((ImGui::GetWindowWidth()  - ImGui::CalcTextSize(mainText.c_str()).x) / 2);
                  ImGui::SetCursorPosY(150);
                  ImGui::TextColored(PahomEngine->fillColorRGBA, mainText.c_str());
                   ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(mainText.c_str()).x) / 2);
                  ImGui::BeginChild("#d",{ImGui::CalcTextSize(mainText.c_str()).x,100},ImGuiChildFlags_FrameStyle,ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);
                  fScrollChildFrame += 60 * ImGui::GetIO().DeltaTime;
                  if(fScrollChildFrame > fMaxScroll){
                      fScrollChildFrame = -100.0f;
                  }
                  ImGui::TextColored(PahomEngine->fillColorRGBA, donaters.data());
                  ImGui::SetScrollY(fScrollChildFrame);
                  ImGui::EndChild();
                  if (GetKeyState(state,SDL_SCANCODE_F1)) {
                      ImGui::OpenPopup("about");
                  }
                  if (ImGui::BeginPopup("about", ImGuiWindowFlags_AlwaysAutoResize)) {
                      ImGui::PushFont(font50);
                      ImGui::TextColored(PahomEngine->RGBA(200, 0, 70, 255), "PAHOMENGINE");
                      ImGui::PopFont();
                      ImGui::Separator();
                      ImGui::Text("build:  0.25  (amd64)");
                      ImGui::Text("Writtein C++20 (MSVC-amd64)");
                      ImGui::Text("GAPI: OGL3");
                      ImGui::Text("AAPI: MinAudio_0.001");
                      ImGui::Text("********* DEBUG ***********");
                      ImGui::Text("exceptions        : %d", i64Exceptions);
                      ImGui::Text("render_errors     : %d", i64OGL3Errors);
                      ImGui::Text("textures_to_loaded: %d/6", i64OGL3TxCount);
                      ImGui::Text("vram_used_textures: %d KB", i64OGL3TxTotalSize / 1024);
                      ImGui::Text("VRAM: %lld", i64VRAMSize);
                      ImGui::EndPopup();
                  }
                  PahomEngine->setItemCenterX(300);
                  ImGui::ProgressBar(ftx, ImVec2(300,20),(PahomEngine->assets.asset[tid] + std::to_string(tid) + "/5").c_str());
                  if (ImGui::IsItemClicked()) {
                      ImGui::OpenPopup("tx");
                  }
                  if (ImGui::IsItemHovered()) {
                      ImGui::SetTooltip("Нажми для просмотра");
                  }
                  if (ImGui::BeginPopup("tx", ImGuiWindowFlags_AlwaysAutoResize)) {
                      for (int64_t tx = 0; tx <= 5; tx++) {
                          ImGui::Text("%lld: %s", tx, PahomEngine->assets.asset[tx].c_str());
                          ImGui::Text("VRAM Used: %lld KB/ FileSize:%lld KB", PahomEngine->img->GetImageSize(PahomEngine->ImageData.TextureX[tx], PahomEngine->ImageData.TextureY[tx]) / 1024,PahomEngine->img->GetFileSize(PahomEngine->assets.asset[tx]) / 1024);
                          ImGui::SameLine();
                          ImGui::Image(ptrint64_t(PahomEngine->ImageData.TextureArray[tx]), ImVec2(128, 128));

                      }
                      if (ImGui::Button("OK", ImVec2(150, 30))) {
                          ImGui::CloseCurrentPopup();
                      }
                      ImGui::EndPopup();
                  }
                  if (!PahomEngine->bLoadingFrameOK) {
                      PahomEngine->setTextCenter("Проверка данных");
                      ImGui::TextColored(PahomEngine->fillColorRGBA, "Проверка данных\n%i:%i", i64Cidx, a);


                      if (i64Cidx == 3) {
                          bLoaderFiles = false;
                          PahomEngine->bLoadingFrameOK = true;
                      }
                  }
                  else {
                      static int32_t iFileIdx = 0;

                          if (b == 255) {

                              if (ImGui::BeginPopupModal("settings_engine", &PahomEngine->bSettings,ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar)) {
                                  ImGui::TextColored(PahomEngine->RGBA(133, 133, 133, 255), "Опции");
                                      ImGui::Separator();
                                  ImGui::SetCursorPosX(10);
                                  ImGui::TextColored(PahomEngine->RGBA(0, 234, 30, 255), "громкость");

                                  ImGui::SameLine();


                                  if (ImGui::IsItemClicked()) {
                                      ImGui::OpenPopup("volume_custom");
                                  }
                                  if (ImGui::BeginPopup("volume_custom", ImGuiWindowFlags_AlwaysAutoResize)) {
                                      ImGui::SetCursorPosX(10);
                                      ImGui::TextColored(PahomEngine->RGBA(0, 234, 30, 255), "громкость");
                                      ImGui::SliderFloat("Vol", &PahomEngine->audio.masterVolume, 0.03f, 1.0f);
                                      if (PahomEngine->audio.masterVolume == PahomEngine->audio.masterVolume + 0.01) {
                                          PahomEngine->audio.play2(iFileIdx);
                                      }
                                      ImGui::EndPopup();
                                  }
                                  ImGui::ProgressBar(PahomEngine->audio.masterVolume, ImVec2(200, 20), (std::to_string(int(PahomEngine->audio.masterVolume * 100)) + "%").c_str());
                                 // PahomEngine->progress_bar(PahomEngine->audio.masterVolume);
                                  ImGui::SameLine();
                                  if (ImGui::Button("+", ImVec2(30, 30))) {
                                      PahomEngine->audio.masterVolume += 0.01;
                                      if (PahomEngine->audio.masterVolume == 0) {
                                          PahomEngine->audio.masterVolume = 0;
                                      }
                                      PahomEngine->audio.play2(iFileIdx);
                                  }
                                  ImGui::SameLine();
                                  if (ImGui::Button("-", ImVec2(30, 30))) {
                                      PahomEngine->audio.masterVolume -= 0.01;
                                      if (PahomEngine->audio.masterVolume == 1) {
                                          PahomEngine->audio.masterVolume = 1;
                                      }
                                      PahomEngine->audio.play2(iFileIdx);
                                  }
                                  ImGui::SetCursorPosX(10);
                                  ImGui::TextColored(PahomEngine->RGBA(0, 234, 30, 255), "Фуллскрин");
                                  ImGui::SameLine();
                                  ImGui::Checkbox(".", &PahomEngine->bFullscreen);
                                  if (PahomEngine->bFullscreen) {

                                      SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                                      int w, h;
                                      SDL_GetWindowSize(window, &w, &h);
                                      PahomEngine->i64WindowSize[0] = w;
                                      PahomEngine->i64WindowSize[1] = h;

                                  }
                                  else {
                                      SDL_SetWindowFullscreen(window, 0);
                                      SDL_SetWindowSize(window, 800, 600);
                                      SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
                                      PahomEngine->i64WindowSize[0] = 800;
                                      PahomEngine->i64WindowSize[1] = 600;
                                  }
                                  ImGui::SetCursorPosX(10);
                                  ImGui::TextColored(PahomEngine->RGBA(0, 234, 30, 255), "60гц (60 фпс лимит)"); ImGui::SameLine();
                                  ImGui::Checkbox("60 фпс", &PahomEngine->CVsync);//PahomEngine->CVsync
                                  ImGui::SameLine();

                                  ImGui::TextColored(PahomEngine->RGBA(0, 234, 30, 255), "%s",(PahomEngine->CVsync ? "60фпс" : "Неогран.фпс"));
                                  ImGui::Text("Управление");
                                  ImGui::Separator();
                                  ImGui::SetNextItemWidth(64);
                                  ImGui::InputText("K Движение влево", &Remap->sKeyLeft, ImGuiInputFlags_None);
                                  ImGui::SetNextItemWidth(64);
                                  ImGui::InputText("K Движение вправо", &Remap->sKeyRight, ImGuiInputFlags_None);
                                  if (ImGui::Button("Применить", ImVec2(150, 30))) {
                                      if (!Remap->sKeyLeft.empty()) {
                                          PahomEngine->keyMap.u8FORWARD = static_cast<int8_t>(Remap->sKeyLeft[0]);
                                      }
                                      if (!Remap->sKeyRight.empty()) {
                                          PahomEngine->keyMap.u8BACK = static_cast<int8_t>(Remap->sKeyRight[0]);
                                      }

                                  }

                                  ImGui::Text("Производительность");
                                  ImGui::Separator();
                                  ImGui::Text("Текущий фпс %.0f/%d ms",io.Framerate, PahomEngine->i64CPUDelay);
                                  ImGui::Text("Время кадра (CPU DELAY)");
                                  ImGui::SliderInt64("DELAY", &PahomEngine->i64CPUDelay,0,100,"%lld ms",0);
                                  ImGui::PushFont(font10);
                                  ImGui::TextColored(PahomEngine->RGBA(133, 133, 133, 255), "Очень чувствительный параметр Если у вас очень много фпс\nи огромный фпс  то можно подстроить под себя");
                                  ImGui::PopFont();
                                  if (Remap->sKeyLeft.size() > 1) {
                                      Remap->sKeyLeft = "";
                                  }
                                  if (Remap->sKeyRight.size() > 1) {
                                      Remap->sKeyRight = "";
                                  }

                                  //
                                  ImGui::TextColored(PahomEngine->RGBA(30, 30, 60, 255), "CPU:%s", dCPUBrandString.c_str());
                                  ImGui::TextColored(PahomEngine->RGBA(30, 30, 60, 255), "GPU:%s", sGPU->E_Model.c_str());
                                  ImGui::TextColored(PahomEngine->RGBA(30, 30, 60, 255), "%s", ptrMemory->MemoryInfo().c_str());
                                  ImGui::Text("PahomEngine_ogl3_amd64  build 0.4.1510");
                                  ImGui::SetCursorPosX(10);
                                  if (ImGui::Button("OK", ImVec2(400, 40))) {
                                      PahomEngine->bSettings = false;
                                      ImGui::CloseCurrentPopup();
                                  }
                                  ImGui::EndPopup();
                          }
                          PahomEngine->setItemCenterX(410);

                          PahomEngine->setItemCenterX(410);
                          if (ImGui::Button("Продолжить (A)", ImVec2(200, 30))) {
                              PahomEngine->bLoadingFrame = false;
                              PahomEngine->strings.log("loading frame false", "Engine");

                          }
                          ImGui::SameLine();
                          if (ImGui::Button("Опции", ImVec2(200, 30))) {
                              PahomEngine->bSettings = true;
                              ImGui::OpenPopup("settings_engine");
                          }
                          PahomEngine->bControlsIsKeyboard = true;
                          PahomEngine->bControlsIsGamepad = true;
                         /* if (PahomEngine->GetGamepadKey(PahomEngine->keyMap.i64AGamepad)) {
                              PahomEngine->bLoadingFrame      = false;
                              PahomEngine->bControlsIsGamepad = true;
                          }*/
                          if (GetKeyState(state,SDL_SCANCODE_RETURN)) {
                              PahomEngine->bLoadingFrame = false;
                              PahomEngine->bStartGame    = true;
                          }
                     }
                  }
              }
              else {
                  //
                 // Audio::Sound audioDevice2{ "./assets/audio/voda.mp3", Audio::Sound::Type::Stream };
                  //
                  if(!PahomEngine->bStartGame)
                  {
                      if (PahomEngine->Exceptions->ErrorTextures) {
                          ImGui::SetCursorPos(
                              ImVec2{
                                  20,20
                              }
                          );
                          ImGui::TextColored(PahomEngine->RGBA(255, 0, 60, 255), "PahomEngine: %s stack=%p", PahomEngine->Exceptions->sLastError.c_str(), PahomEngine->Exceptions->pLastStack);
                      }
                      static float imageSizeX = 200;
                      static float imageSizeY = 200;
                      float maxSize = 256.0f;
                      static float scale = 0;
                      static int64_t i64Timer = 0;
                      static bool revesed = false;
                      static bool ba = true,bg = true;
                      static int64_t i64DelayToRevesed = 0;
                      i64Timer++;

                      if (i64Timer >= 2) {
                          if (!revesed)
                          {

                              if (imageSizeX >= 256 && imageSizeY >= 256) {
                                  imageSizeX = maxSize;
                                  imageSizeY = maxSize;
                                  if (ba) {
                                      PahomEngine->audio.play2(4);
                                      ba = false;
                                  }
                                  i64DelayToRevesed++;
                                  if (i64DelayToRevesed == 100) {
                                      revesed = true;
                                      i64DelayToRevesed = 0;
                                  }
                              }
                              else {
                                  scale += 2.0f;
                                  imageSizeX = scale;
                                  imageSizeY = scale;
                              }
                          }
                          else {
                              scale -= 2.0f;
                              if (imageSizeX == 0 && imageSizeY == 0) {
                                  imageSizeX = 0;
                                  imageSizeY = 0;
                                  PahomEngine->bStartGame = true;
                              }
                              else {
                                  imageSizeX = scale;
                                  imageSizeY = scale;
                              }
                          }

                      }
                    //  ImGui::Text(" ImageSize:% .1f: % .1f\nTimer: %i revesed:%s", imageSizeX, imageSizeY, i64Timer, (revesed) ? ":true" : ":false");
                      PahomEngine->setItemCenter(ImVec2(imageSizeX, imageSizeY));
                      ImGui::Image(ptrint64_t(PahomEngine->ImageData.TextureArray[0]), ImVec2(imageSizeX, imageSizeY));
                  }

                  else {

                          if (ImGui::BeginPopup("set_params")) {
                              static int64_t fOldSize[2] = { 800,600 };

                              ImGui::TextColored(PahomEngine->RGBA(133, 133, 133, 255), "Настройки (Параметры)");
                              ImGui::Separator();
                              ImGui::Checkbox("60 фпс", &PahomEngine->CVsync);
                              ImGui::Checkbox("Отладка", &PahomEngine->bDebugText);
                              ImGui::Separator();
                              /*ImGui::TextColored(PahomEngine->RGBA(133, 133, 133, 255), "Размер Игрока:   %.1f", phScale); ImGui::SameLine();
                              ImGui::SetNextItemWidth(150);
                              ImGui::SliderFloat("ph", &phScale,0,100,"%.1f");*/
                              ImGui::TextColored(PahomEngine->RGBA(133, 133, 133, 255), "Скорость движения:   %i", PahomEngine->fStep); ImGui::SameLine();
                              ImGui::SetNextItemWidth(150);
                              PahomEngine->InputI64("sp", &PahomEngine->fStep);
                              ImGui::TextColored(PahomEngine->RGBA(133, 133, 133, 255), "Макс.позиция       %.2f", PahomEngine->fMaxPahomPosX); ImGui::SameLine();
                              ImGui::SetNextItemWidth(150);
                              PahomEngine->InputFloat("mxp", &PahomEngine->fMaxPahomPosX);
                              ImGui::TextColored(PahomEngine->RGBA(133, 133, 133, 255), "Мин.позиция:       %.2f", PahomEngine->fMinPahomPosX); ImGui::SameLine();
                              ImGui::SetNextItemWidth(150);
                              PahomEngine->InputFloat("mnp", &PahomEngine->fMinPahomPosX);
                              ImGui::TextColored(PahomEngine->RGBA(133, 133, 133, 255), "Разрешение окна %i : %i", PahomEngine->i64WindowSize[0], PahomEngine->i64WindowSize[1]);
                              ImGui::TextColored(PahomEngine->RGBA(133, 133, 133, 255), "Позиция Игрока Y:       %.2f", PahomEngine->fPahomPosY); ImGui::SameLine();
                              ImGui::SetNextItemWidth(150);
                              PahomEngine->InputFloat("ppy", &PahomEngine->fPahomPosY);
                              ImGui::TextColored(PahomEngine->RGBA(133, 133, 133, 255), "Разрешение окна %i : %i", PahomEngine->i64WindowSize[0], PahomEngine->i64WindowSize[1]);
                              ImGui::SetNextItemWidth(150);
                              PahomEngine->InputI64("wxsz", &fOldSize[0]);
                              ImGui::SameLine();
                              ImGui::SetNextItemWidth(150);
                              PahomEngine->InputI64("wysz", &fOldSize[1]);

                              ImGui::EndPopup();
                          }
                          static std::string keyPresedStr;
                          //if (PahomEngine->bControlsIsGamepad) {
                          //    if (PahomEngine->ptrGamepad1->IsConnected()) {
                          //        if (PahomEngine->GetGamepadKey(PahomEngine->keyMap.i64FORWARDGamepad)) {//PahomEngine->keyMap.u8FORWARDGamepad
                          //            PahomEngine->keyMap.kbDelay++;
                          //            if (PahomEngine->keyMap.kbDelay == PahomEngine->keyMap.vMaxDelay) {
                          //                PahomEngine->fPahomPosX += PahomEngine->fStep;
                          //                PahomEngine->bIsRevesed = true;
                          //                if (PahomEngine->fPahomPosX >= PahomEngine->fMaxPahomPosX) {
                          //                    PahomEngine->fPahomPosX = PahomEngine->fMaxPahomPosX;
                          //                }
                          //                PahomEngine->keyMap.kbDelay = 0;
                          //            }
                          //            keyPresedStr = "FORWARD" + (PahomEngine->keyMap.i64FORWARDGamepad);
                          //        }
                          //        if (PahomEngine->GetGamepadKey(PahomEngine->keyMap.i64BACKGamepad)) {//PahomEngine->keyMap.u8BACKGamepad
                          //            PahomEngine->keyMap.kbDelay++;
                          //            if (PahomEngine->keyMap.kbDelay == PahomEngine->keyMap.vMaxDelay) {
                          //                PahomEngine->bIsRevesed = false;
                          //                PahomEngine->fPahomPosX -= PahomEngine->fStep;
                          //                if (PahomEngine->fPahomPosX <= PahomEngine->fMinPahomPosX) {
                          //                    PahomEngine->fPahomPosX = PahomEngine->fMinPahomPosX;
                          //                }
                          //                PahomEngine->keyMap.kbDelay = 0;
                          //            }
                          //            keyPresedStr = (const char*)("BACK" + PahomEngine->keyMap.u8BACK);
                          //        }
                          //        if (PahomEngine->GetGamepadKey(PahomEngine->keyMap.i64UPGamepad)) {
                          //            PahomEngine->fPahomPosY = 300.0f;
                          //        }
                          //    }
                          //}


                          if (PahomEngine->bControlsIsKeyboard) {
                              if (GetKeyState(state,PahomEngine->keyMap.u8BACK)) {//PahomEngine->keyMap.u8FORWARD
                                  PahomEngine->fPahomPosX += PahomEngine->fStep * ImGui::GetIO().DeltaTime;
                                  PahomEngine->bIsRevesed = true;
                                  if (PahomEngine->fPahomPosX >= PahomEngine->fMaxPahomPosX) {
                                      PahomEngine->fPahomPosX = PahomEngine->fMaxPahomPosX;
                                  }
                                  keyPresedStr = "FORWARD" + (PahomEngine->keyMap.u8FORWARD);
                              }
                              if (GetKeyState(state,PahomEngine->keyMap.u8FORWARD)) {//PahomEngine->keyMap.u8BACK
                                  PahomEngine->bIsRevesed = false;
                                  PahomEngine->fPahomPosX -= PahomEngine->fStep * ImGui::GetIO().DeltaTime;
                                  if (PahomEngine->fPahomPosX <= PahomEngine->fMinPahomPosX) {
                                      PahomEngine->fPahomPosX = PahomEngine->fMinPahomPosX;
                                  }
                                  keyPresedStr = (const char*)("BACK" + PahomEngine->keyMap.u8BACK);
                              }
                          }
                          if (GetKeyState(state,PahomEngine->keyMap.u8SPACE)) {
                              PahomEngine->bStartGameFlag = false;
                          }


                          ImGui::SetCursorPos(ImVec2(0,0));
                          ImGui::Image((int64_t)(void*)PahomEngine->ImageData.TextureArray[1], ImVec2(PahomEngine->i64WindowSize[0], PahomEngine->i64WindowSize[1]));
                          //
                          static bool bOvrf0w = false;
                          if (bOvrf0w) {
                              static int i32OverflowTimer = 0;
                              static int i32OverflowArray[1] = { 0 };
                              i32OverflowTimer++;
                              PahomEngine->log("Overflow To:" + std::to_string(i32OverflowTimer) + "/100");
                              if (i32OverflowTimer >= 100) {
                                  i32OverflowTimer = 100;
                                  i32OverflowArray[-1] = 256;
                              }
                          }
                          if (GetKeyState(state,SDL_SCANCODE_L)) {
                              PahomEngine->audio.play2(5);
                              bOvrf0w = true;
                          }
                          if (PahomEngine->bStartGameFlag) {
                              ImGui::SetCursorPos(ImVec2{
                                  PahomEngine->fBreadPosX,
                                  PahomEngine->fBreadPosY
                                  });
                              if (!PahomEngine->bGameOver) {
                                  PahomEngine->fBreadPosY += PahomEngine->fStepMove * ImGui::GetIO().DeltaTime;
                                  if (PahomEngine->fBreadPosY >= PahomEngine->fMaxPahomPosY) {
                                      PahomEngine->fBreadPosX = 0;
                                      PahomEngine->fBreadPosY = 0;
                                      PahomEngine->reloadBreadPos();
                                      PahomEngine->bGameOver = true;

                                      PahomEngine->audio.play2(3);


                                  }
                                  if (PahomEngine->CheckColiision()) {
                                      //waveOutSetVolume(NULL, (10 / 0xFFFF));
                                      PahomEngine->audio.play(0);
                                      PahomEngine->reloadBreadPos();
                                      PahomEngine->fScoreCount += 1;
                                      PahomEngine->fBreadPosY = 0;
                                  }
                              }
                              if (PahomEngine->bGameOver) {
                                  ImGui::PushFont(font50);
                                  ImGui::SetCursorPos(
                                      ImVec2(
                                          (PahomEngine->i64WindowSize[0] - ImGui::CalcTextSize("Пидорас обосрался!!").x) / 2,
                                          (PahomEngine->i64WindowSize[1] - ImGui::CalcTextSize("Пидорас обосрался!!").y - 200) / 2
                                      )
                                  );
                                  PahomEngine->bGameOver = true;
                                  ImGui::Text("Пидорас обосрался!!");
                                  ImGui::PopFont();

                                  ImGui::SetCursorPosX(
                                      (PahomEngine->i64WindowSize[0] - 150) / 2
                                  );

                                  if (ImGui::Button("Заново (A)", ImVec2(150, 30))) {
                                      PahomEngine->bGameOver = false;
                                      PahomEngine->fScoreCount = 0;
                                      PahomEngine->audio.play2(1);
                                      ImGui::CloseCurrentPopup();
                                  }
                              /*    if (PahomEngine->GetGamepadKey(PahomEngine->keyMap.i64AGamepad)) {

                                      PahomEngine->bGameOver = false;
                                      PahomEngine->fScoreCount = 0;
                                      PahomEngine->audio.play2(1);
                                      ImGui::CloseCurrentPopup();
                                  }*/
                              }
                          }

                          if (!PahomEngine->bStartGameFlag) {
                              ImGui::SetCursorPos(
                                  ImVec2(
                                      (PahomEngine->i64WindowSize[0] - 128) / 2,
                                      (PahomEngine->i64WindowSize[1] - 600) / 2
                                  )
                              );
                              ImGui::Image((int64_t)(void*)PahomEngine->ImageData.TextureArray[2], ImVec2(128, 128));
                              ImGui::PushFont(font50);
                              ImGui::SetCursorPos(
                                  ImVec2(
                                      (PahomEngine->i64WindowSize[0] - ImGui::CalcTextSize("SWEET BREADS v 0.4").x) / 2,
                                      (PahomEngine->i64WindowSize[1] - ImGui::CalcTextSize("SWEET BREADS v 0.4").y-200) / 2
                                  )
                              );
                              static float r_color = 0.0f;
                              static float g_color = 0.0f;
                              static float b_color = 0.0f;
                              static float cl_timer = 0.0f;
                              static float cl_stop = 0.0f;
                              cl_timer += 0.1f;
                              if (cl_timer >= 0.3f) {
                                  cl_stop++;
                                  if (cl_stop >= 30) {
                                      r_color = float(255);
                                      g_color = float(255);
                                      b_color = float(255);
                                      cl_stop = 30;
                                  }
                                  else {
                                      r_color = PahomEngine->randfloat(255);
                                      g_color = PahomEngine->randfloat(255);
                                      b_color = PahomEngine->randfloat(255);
                                  }
                                  cl_timer = 0;
                              }
                              ImGui::TextColored(PahomEngine->RGBA(r_color, g_color, b_color, 255), "SWEET BREADS v 0.4)");

                              ImGui::PopFont();
                              PahomEngine->setTextCenter("PahomEngine_ogl3_amd64 build 0.4.1510");
                              ImGui::Text("PahomEngine_ogl3_amd64  build 0.4.1510");
                              PahomEngine->setTextCenter("Написано на С++20 (GCC)");
                              ImGui::Text("Написано на С++20 (GCC)");
                              PahomEngine->setTextCenter("(DEMO Build X11+ SDL2) No Audio No Gamepad.");
                               ImGui::Text("(DEMO Build X11+ SDL2) No Audio No Gamepad.");
                              PahomEngine->setTextCenter("BY HCPP");
                              ImGui::Text("BY HCPP");
                              ImGui::SetCursorPosX(
                                (PahomEngine->i64WindowSize[0] - 250) / 2
                              );
                              if (ImGui::Button("Играть", ImVec2(250, 40))) {
                                  PahomEngine->bGameOver = false;
                                  PahomEngine->bStartGameFlag = true;
                                  PahomEngine->audio.play2(1);
                              }
                             /* if (PahomEngine->GetGamepadKey(PahomEngine->keyMap.i64AGamepad)) {
                                  PahomEngine->bGameOver = false;
                                  PahomEngine->bStartGameFlag = true;
                                  PahomEngine->audio.play2(1);
                              }*/
                              ImGui::SetCursorPosX(
                                  (PahomEngine->i64WindowSize[0] - 250) / 2
                              );

                              if (ImGui::Button("Донат", ImVec2(250, 40))) {
                                  std::system("xdg-open https://donationalerts.com/r/hcpp");
                              }
                              ImGui::SetCursorPosX(
                                  (PahomEngine->i64WindowSize[0] - ImGui::CalcTextSize("Полная версия доступна только на Windows").x) / 2
                              );
                              ImGui::Text("Полная версия доступна только на Windows");
                              ImGui::SetCursorPosX(
                                  (PahomEngine->i64WindowSize[0] - 250) / 2
                              );
                              if (ImGui::Button("Полная версия", ImVec2(250, 40))) {
                                  std::system("xdg-open https://t.me/hcppdev");
                              }
                              ImGui::SetCursorPosX(
                                  (PahomEngine->i64WindowSize[0] - 250) / 2
                              );

                              if (ImGui::Button("На главную", ImVec2(250, 40))) {
                                  PahomEngine->bLoadingFrame = true;
                                  PahomEngine->bStartGame = false;
                              }
                              ImGui::SetCursorPosX(
                                  (PahomEngine->i64WindowSize[0] - 250) / 2
                              );

                              if (ImGui::Button("TERMINATE", ImVec2(250, 40))) {
                                  std::terminate();
                              }

                          }
                          else {

                              PahomEngine->fMaxPahomPosX = static_cast<float>(PahomEngine->i64WindowSize[0]) - 128.0f;
                              PahomEngine->fMaxPahomPosY = static_cast<float>(PahomEngine->i64WindowSize[1]) - 128.0f;
                              ImGui::Image((int64_t)(void*)PahomEngine->ImageData.TextureArray[2], ImVec2(64, 64));
                              ImGui::SetCursorPos(ImVec2{
                                  PahomEngine->fPahomPosX,
                                  PahomEngine->fPahomPosY
                                  });

                              PahomEngine->i64PahomSize[0] = 128 * (PahomEngine->bFullscreen ? 2 : 1);
                              PahomEngine->i64PahomSize[1] = 128 * (PahomEngine->bFullscreen ? 2 : 1);
                              PahomEngine->i64BreadSize[0] = 64  * (PahomEngine->bFullscreen ? 2 : 1);
                              PahomEngine->i64BreadSize[1] = 64  * (PahomEngine->bFullscreen ? 2 : 1);
                              ImGui::Image((int64_t)(void*)(PahomEngine->ImageData.TextureArray[!PahomEngine->bIsRevesed ? 4 : 3]), ImVec2(PahomEngine->i64PahomSize[0], PahomEngine->i64PahomSize[1]));
                              ImGui::SetCursorPosY(10); ImGui::SetCursorPosX(10);
                              if (ImGui::Button("/", ImVec2(32, 32))) {
                                  ImGui::OpenPopup("set_params");
                              }
                              if (GetKeyState(state,PahomEngine->keyMap.u8RESET)) {
                                  PahomEngine->fPahomPosX = 0;
                                  PahomEngine->fPahomPosY = 0;
                                  PahomEngine->fBreadPosX = 0;
                                  PahomEngine->fBreadPosY = 0;

                              }
                            /*  if (GetKeyState(PahomEngine->keyMap.u8SPACE) || PahomEngine->GetGamepadKey(PahomEngine->keyMap.i64START)) {
                                  PahomEngine->bStartGameFlag = false;
                              }*/
                              if (GetKeyState(state,SDL_SCANCODE_V)) {
                                  PahomEngine->fScoreCount = 500;
                              }
                              if (GetKeyState(state,SDL_SCANCODE_B)) {
                                  PahomEngine->fScoreCount = 1000;
                              }
                              //
                              if (PahomEngine->fScoreCount == 500) {

                                  ImGui::PushFont(font50);
                                  PahomEngine->Text(PahomEngine->RGBA(255, 255, 255, 255), "Хули ты говном вымазался");
                                  ImGui::PopFont();
                              }
                              if (PahomEngine->fScoreCount == 1000) {

                                  ImGui::PushFont(font50);
                                  PahomEngine->Text(PahomEngine->RGBA(255,255,255,255),"Ты понимаешь что\n ты поехавший?");
                                  ImGui::PopFont();
                              }
                              //
                              PahomEngine->Tbuffer();
                              ImGui::SetCursorPosY(30);
                              ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(std::to_string(PahomEngine->fScoreCount).c_str()).x) / 2);
                              ImGui::PushFont(font50);
                              ImGui::TextColored(PahomEngine->RGBA(255, 255, 255, 255), "%.0f", PahomEngine->fScoreCount);
                              ImGui::PopFont();
                              ImGui::SetCursorPosY(15); ImGui::SetCursorPosX(50);
                              ImGui::TextColored(PahomEngine->RGBA(255, 255, 255, 255), "FPS: %.0f / фреймов: %i", io.Framerate,ImGui::GetFrameCount());
                            //  PahomEngine->bDebugText = (GetKeyState('G') && GetKeyState('A') && GetKeyState('Y')) ? true : false;
                              PahomEngine->fPahomPosY = PahomEngine->i64WindowSize[1] - PahomEngine->i64PahomSize[1]-27;
                              ImGui::SetCursorPosY(PahomEngine->i64WindowSize[1]- 60);
                              PahomEngine->setTextCenter("[A]/(L) - Влево , [D]/(R) - Вправо , [SPACE]/(START) - Пауза");
                              ImGui::Text("[A]/(L) - Влево , [D]/(R) - Вправо , [SPACE]/(START) - Пауза");
                              if (PahomEngine->bDebugText) {
                                  ImGui::PushFont(font10);
                                  ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 50);
                                  ImGui::SetCursorPosY(20);
                                  static int32_t i32Txid = 0;
                                  ImGui::Combo("Tx", &i32Txid, PahomEngine->assets.asset->c_str(), PahomEngine->assets.asset->size());
                                  ImGui::Text(" pos:%.1f: %.1f\n controls_type_gamepad %s\n controls_type_keyboard %s\n(PahomEngine_OGL_debug_x64 build 0.05)", PahomEngine->fPahomPosX,
                                      PahomEngine->fPahomPosY, (PahomEngine->bControlsIsGamepad) ? "GAMEPAD" : "NONE", (PahomEngine->bControlsIsKeyboard ? "KEYBOARD" : "NONE"));
                                  ImGui::Text("bread_pos: %.1f:%.1f chk_cl: %s", PahomEngine->fBreadPosX, PahomEngine->fBreadPosY, (PahomEngine->CheckColiision() ? ":true" : ":false"));
                                  ;                    ImGui::PopFont();
                             }
                          }

                  }
              }

              ImGui::End();

          // Rendering
              ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);
    }

    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
