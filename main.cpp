#include "frames.hpp"
#include "thread"
#include "resource.h"

#pragma once



struct WGL_WindowData { HDC hDC; };
static int64_t fwop = 0;
// Data
static HGLRC            g_hRC;
static WGL_WindowData   g_MainWindow;           
static int              g_Width;
static int              g_Height;
ImVec4 clear_color = ImVec4(0.01f, 0.01f, 0.02f, 0.00f);         

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//
uint64_t fMX = 0;
uint64_t fMY = 0;

bool b_FatalError = false;
   
struct remapkeys {
	std::string_view sKeyLeft;
	std::string_view sKeyRight;
	std::string_view sButtonLeft;
	std::string_view sButtonRight;
	int8_t GetKey(bool* pressed_key) {
		static const std::unordered_map<int, std::string_view> keyMap = {
					   {'1', "k1"}, {'Q', "kQ"},
					   {'2', "k2"}, {'W', "kW"}, 
					   {'3', "k3"}, {'E', "kE"},  
					   {'4', "k4"}, {'R', "kR"},
					   {'5', "k5"}, {'T', "kT"},
					   {'6', "k6"}, {'Y', "kY"}, 
					   {'7', "k7"}, {'U', "kU"},
					   {'8', "k8"}, {'I', "kI"}, 
					   {'9', "k9"}, {'O', "kO"},
					   {'0', "k0"}, {'P', "kP"},  
					   {'A', "kA"}, {'V', "kV"},  
					   {'S', "kS"}, {'B', "kB"},
					   {'D', "kD"}, {'N', "kN"},
					   {'F', "kF"}, {'M', "kM"},
					   {'G', "kG"}, {VK_DIVIDE, "k/"},
					   {'H', "kH"}, {'.', "k."},    
					   {'J', "kJ"}, {VK_ESCAPE, "kESC"},
					   {'K', "kK"}, {VK_F1, "kf1"},
					   {'L', "kL"}, {VK_F2, "kf2"},
					   {';', "k;"}, {VK_F3, "kf3"},   
					   {'"', "k'"}, {VK_F4, "kf4"}, 
					   {'Z', "kZ"}, {VK_F5, "kf5"},
					   {'X', "kX"}, {VK_F6, "kf6"},
					   {'C', "kC"}, {VK_F7, "kf7"},
					   {VK_F8, "kf8"}, {VK_F9, "kf9"},
					   {VK_F10, "kf10"}, {VK_F11, "kf11"},
					   {VK_F12, "kf12"}    
		};    
		if (pressed_key) {
			for (const auto& [key, keyName] : keyMap) {
				if (GetAsyncKeyState(key)) {
					PahomEngine->log(std::format("Клавиша нажата: ( {} )", static_cast<char>(key)));
					return static_cast<int8_t>(key);
					*pressed_key = false;
				}
			}
		}
	}
};  
auto Remap = std::make_unique<remapkeys>();
bool bSelectResolutionToGamepad = true;
int32_t i32Presets = 0;
uint8_t u8PreloadRes = 0;
int iResID = 0;
void checkResolution() {
	if (PahomEngine->i64WindowSize[0] == 640) {
		iResID = 0;
	}
	if (PahomEngine->i64WindowSize[0] == 800) {
		iResID = 1;  
	}
	if (PahomEngine->i64WindowSize[0] == 1024) {
		iResID = 2;
	}
	if (PahomEngine->i64WindowSize[0] == 1280) {
		iResID = 3;
	}
	if (PahomEngine->i64WindowSize[0] == 1366) {
		iResID = 4;
	}
	if (PahomEngine->i64WindowSize[0] == 1920) {
		iResID = 5;
	}
}
void DiffinitySetAdaptive() {
	PahomEngine->log(std::format("resid: {}", iResID));
	switch (PahomEngine->i64WindowSize[0]) { // width 
	case 640:
		PahomEngine->fStep = 10;
		PahomEngine->setSizePahom(128, 128);
		PahomEngine->setSizeBread(64, 64);
		break;
	case 800:
		PahomEngine->fStep = 12;
		PahomEngine->setSizePahom(192, 192);
		PahomEngine->setSizeBread(64, 64);
		break;
	case 1024:
		PahomEngine->fStep = 16;
		PahomEngine->setSizePahom(192, 192);
		PahomEngine->setSizeBread(64, 64);
		break;
	case 1280:
		PahomEngine->fStep = 18;
		PahomEngine->setSizePahom(256, 256);
		PahomEngine->setSizeBread(64, 64);
		break;
	case 1366:
		PahomEngine->fStep = 20;
		PahomEngine->setSizePahom(256, 256);
		PahomEngine->setSizeBread(64, 64);
		break;
	case 1920:
		PahomEngine->fStep = 28;
		PahomEngine->setSizePahom(512, 512);
		PahomEngine->setSizeBread(128, 128);

		break; 
	default:   
		if (PahomEngine->i64WindowSize[0] >= 1920) {  
			PahomEngine->fStep = 28;
			PahomEngine->setSizePahom(512, 512);
			PahomEngine->setSizeBread(128, 128);
		}
		else if (PahomEngine->i64WindowSize[0] >= 1366) {
			PahomEngine->fStep = 20;
			PahomEngine->setSizePahom(256, 256);
			PahomEngine->setSizeBread(64, 64); 
		} 
		else if (PahomEngine->i64WindowSize[0] >= 1280) {
			PahomEngine->fStep = 18;
			PahomEngine->setSizePahom(256, 256);
			PahomEngine->setSizeBread(64, 64);
		}
		else if (PahomEngine->i64WindowSize[0] >= 1024) {
			PahomEngine->fStep = 16;
			PahomEngine->setSizePahom(192, 192);
			PahomEngine->setSizeBread(64, 64);
		}
	};
	PahomEngine->fStepMove = PahomEngine->pDiff->diffArray[PahomEngine->pDiff->i64id].i64buffer;
}
void loadResulutionUI(ImFont* font0) {
	static const std::pair<int32_t, int32_t> i32WindowSizePresetsList[] = {
		{640, 480}, {800, 600}, {1024, 768}, {1280, 720},
		{1366, 768}, {1600, 900}, {1920, 1080}
	};
	if (!u8PreloadRes) {
		PahomEngine->SetSizeHWND(GetActiveWindow(), PahomEngine->i64WindowSize[0], PahomEngine->i64WindowSize[1]);
		PahomEngine->log(std::format(" (Win32) set size {}-{}", PahomEngine->i64WindowSize[0], PahomEngine->i64WindowSize[0]));
		if (PahomEngine->i64WindowSize[0] == 640) {
			iResID = 0;
		}
		if (PahomEngine->i64WindowSize[0] == 800) {
			iResID = 1;
		}
		if (PahomEngine->i64WindowSize[0] == 1024) {
			iResID = 2;
		}
		if (PahomEngine->i64WindowSize[0] == 1280) {
			iResID = 3;
		}
		if (PahomEngine->i64WindowSize[0] == 1366) {
			iResID = 4;
		}
		if (PahomEngine->i64WindowSize[0] == 1920) {
			iResID = 5;
		}
		u8PreloadRes = 1;
	}

	PahomEngine->i64WindowSizeGL[0] = i32WindowSizePresetsList[iResID].first;
	PahomEngine->i64WindowSizeGL[1] = i32WindowSizePresetsList[iResID].second;
	std::string width = std::to_string(i32WindowSizePresetsList[iResID].first);
	std::string height = std::to_string(i32WindowSizePresetsList[iResID].second);
	ImGui::SetCursorPosX(30);
	if (ImGui::Button("<<", { 32,32 })) {
		DiffinitySetAdaptive();
		// PahomEngine->log(std::format("func: LoadResolutionUI() -> {} : {} | line_size widt", PahomEngine->i64WindowSizeGL[0], PahomEngine->i64WindowSizeGL[1]));

		iResID--;
		if (iResID < 0) {
			iResID = 0;
		}
	}
	ImGui::SameLine();
	ImGui::PushFont(font0);
	PahomEngine->TextColored(PahomEngine->RGBA(237.2f, 27.1f, 89.8f, 255), "{:<2}x{:<2}", width, height);
	ImGui::PopFont();
	ImGui::SameLine();
	if (ImGui::Button(">>", { 32,32 })) {
		// PahomEngine->log(std::format("func: LoadReoslutionUI() -> {} : {} | line_size width:{} rwidth{}", PahomEngine->i64WindowSizeGL[0], PahomEngine->i64WindowSizeGL[1]));
		DiffinitySetAdaptive();
		iResID++;
		if (iResID > 6) {
			iResID = 6;
		}
	}


}

uint8_t u8PresetID = 0;
void GraphicsPresetUI(ImFont* font0) {
	static const std::pair <std::string, int> GraphicsPresets[] = {
	   {"НИЗКИЕ",0},
	   {"ВЫСОКИЕ",1},
	   {"УЛЬТРА",2}
	};
	static const std::array<std::string, 3> setGraph = {
		"low",
		"high",
		"ultra"
	};
	static int iResID = 0;
	if (!u8PresetID) {
		iResID = PahomEngine->iReloadTexturesPresetID;
		u8PresetID = 1;
	}
	PahomEngine->iReloadTexturesPresetID = GraphicsPresets[iResID].second;
	ImGui::SetCursorPosX(30);
	ImGui::PushID(4);
	PahomEngine->sGraphPreset = setGraph[iResID];
	if (ImGui::Button("<<", { 32,32 })) {

		iResID--;
		if (iResID < 0) {
			iResID = 0;
		}
	}
	ImGui::PopID();
	ImGui::SameLine();
	ImGui::PushFont(font0);
	PahomEngine->TextColored(PahomEngine->RGBA(237.2f, 27.1f, 89.8f, 255), "{}", GraphicsPresets[iResID].first);
	ImGui::PopFont();
	ImGui::SameLine();
	ImGui::PushID(5);
	if (ImGui::Button(">>", { 32,32 })) {

		iResID++;
		if (iResID > 2) {
			iResID = 2;
		}
	}
	ImGui::PopID();


}
struct SettingsUIStruct
{
	bool bAudio = false;
	bool bCPU = true;
	bool bGPU = false;
	bool bControls = false;
	bool bGamepadTestUI = false;
	// Add New Presets
	bool bWindowAppBlur = false;
	bool bNetwork = false;
};
std::unique_ptr<SettingsUIStruct> SettingsUI = std::make_unique<SettingsUIStruct>();
std::unique_ptr<window> backend = std::make_unique<window>();
void update_settings() {
	PahomEngine->i64WindowSizeGL[0] = PahomEngine->i64WindowSize[0];
	PahomEngine->i64WindowSizeGL[1] = PahomEngine->i64WindowSize[1];
	SetWindowTextA(GetConsoleWindow(), "PahomEngine:::logs");
	PahomEngine->PESettings->ParseConfig();
	//PahomEngine->Render->gl_ver.major = PahomEngine->PESettings->major_gl;
	//PahomEngine->Render->gl_ver.minor = PahomEngine->PESettings->minor_gl;
	PahomEngine->i64WindowSize[0] = PahomEngine->PESettings->i64WindowSize[0];
	PahomEngine->i64WindowSize[1] = PahomEngine->PESettings->i64WindowSize[1];
	DiffinitySetAdaptive();
	PahomEngine->iReloadTexturesPresetID = PahomEngine->PESettings->iPresetID;
	PahomEngine->bFullscreen = PahomEngine->PESettings->bFlagFullScreen;
	PahomEngine->log(std::format("(GPU) OGL{}.{}", PahomEngine->PESettings->major_gl, PahomEngine->PESettings->minor_gl));
	//EnableVT();
	PahomEngine->math->bIsRandomEngineUsed = PahomEngine->PESettings->bFlagRandomEngine;
	PahomEngine->math->bIsRandomXor64 = PahomEngine->PESettings->bFlagXor64random;
	PahomEngine->log(std::format("proxy: [{}:{}]", PahomEngine->PESettings->sProxyHost, PahomEngine->PESettings->iProxyPort));
	PahomEngine->lnethttp->sProxyHost = PahomEngine->PESettings->sProxyHost;
	PahomEngine->lnethttp->iProxyPort = PahomEngine->PESettings->iProxyPort;
	DiffinitySetAdaptive();
}
int main(int argc, char** argv)
{
	auto startTimeEngine = std::chrono::high_resolution_clock::now();
	std::system("cls");

	if (PahomEngine) {
		std::cout << "PahomEngine " << PahomEngine->sBuild << std::endl;
		PahomEngine->logo();
		PahomEngine->i64WindowSizeGL[0] = PahomEngine->i64WindowSize[0];
		PahomEngine->i64WindowSizeGL[1] = PahomEngine->i64WindowSize[1];
		SetWindowTextA(GetConsoleWindow(), "PahomEngine:::logs");
		PahomEngine->PESettings->ParseConfig();
		// PahomEngine->Render->gl_ver.major = PahomEngine->PESettings->major_gl;
		// PahomEngine->Render->gl_ver.minor = PahomEngine->PESettings->minor_gl;
		PahomEngine->i64WindowSize[0] = PahomEngine->PESettings->i64WindowSize[0];
		PahomEngine->i64WindowSize[1] = PahomEngine->PESettings->i64WindowSize[1];
		PahomEngine->bFullscreen = PahomEngine->PESettings->bFlagFullScreen;

		DiffinitySetAdaptive();

		PahomEngine->iReloadTexturesPresetID = PahomEngine->PESettings->iPresetID;
		PahomEngine->log(std::format("(GPU) OGL{}.{}", PahomEngine->PESettings->major_gl, PahomEngine->PESettings->minor_gl));
		//EnableVT();
		PahomEngine->math->bIsRandomEngineUsed = PahomEngine->PESettings->bFlagRandomEngine;
		PahomEngine->math->bIsRandomXor64 = PahomEngine->PESettings->bFlagXor64random;
		PahomEngine->log(std::format("proxy: [{}:{}]", PahomEngine->PESettings->sProxyHost, PahomEngine->PESettings->iProxyPort));
		PahomEngine->lnethttp->sProxyHost = PahomEngine->PESettings->sProxyHost;
		PahomEngine->lnethttp->iProxyPort = PahomEngine->PESettings->iProxyPort;
		if (PahomEngine->sse_41()) {
			PahomEngine->log("SSE4.1 Is Detected\n");

		}
		else {
			PahomEngine->log("SSE4.1 error\n");
			MessageBoxA(GetActiveWindow(), "Error: CPU not supported SSE4.1", "PahomEngine::Errors", 1);
			PahomEngine->ShutdownEngine();
			delete PahomEngine;
			exit(0);
		}
		DiffinitySetAdaptive(); 

	}
	else {
		std::cout << " [module] PahomEngine Error" << std::endl;
		std::cout << " Engine Stoped" << std::endl;
	}
	if (PahomEngine->Exceptions) {
		PahomEngine->log(" [module] Exceptions  OK", 3);
		PahomEngine->Exceptions->BugReport();
	}
	if (PahomEngine->ptrGamepad1) {
		PahomEngine->log(" [module] Gamepad     OK", 3);
	}
	if (PahomEngine->img) {
		PahomEngine->log(" [module] GLImage    OK", 3);
	}
	if (backend) {
		PahomEngine->log(" [module] backend    OK", 3);

	}
	if (!backend) {
		PahomEngine->log(" [module] backend    Fatal Error", 3);
	}
	if (PahomEngine->logs) {
		PahomEngine->log(" [module] logs    OK", 3);
		         
	}  
	if (!PahomEngine->logs) {
		PahomEngine->log(" [module] logs    Fatal Error", 3);
	}
	PahomEngine->log(std::format("argc: {} bIsArgumentsNull:{}", argc, PahomEngine->bIsArgumentsNull));
	if (argc > 1) {
		PahomEngine->bIsArgumentsNull = false;
		PahomEngine->log("[argc] true", 1);
	} 
	else {
		PahomEngine->bIsArgumentsNull = true;
		PahomEngine->log("[argc] false", 1);
	}
	backend->LoadResIcon(102);
	backend->SetSize(int(PahomEngine->i64WindowSize[0]), int(PahomEngine->i64WindowSize[1]));
	backend->setTitleText(L"SWEET BREADS " + PahomEngine->sWBuild + L" (CXX20) (MSVC)");
	backend->gl_major = PahomEngine->PESettings->major_gl;
	backend->gl_minor = PahomEngine->PESettings->minor_gl;

	backend->bUseCustomRender = PahomEngine->PESettings->bUseCustomRender;
	PahomEngine->Event.bShowNoiseBackground = PahomEngine->PESettings->bShowNoiseBackground;
	backend->createWindow(WndProc);
	if (backend->gl_major < 3) {
		PahomEngine->log("backend->initDeviceGl():=> gl_major <3", 2);
		backend->gl_major = 3; 
	}
	if (backend->gl_minor < 3) {
		PahomEngine->log("backend->initDeviceGl():=> gl_minor <3", 2);
		backend->gl_minor = 3;
	}      

	std::string sGLVersion = std::format("#version {}{}0", backend->gl_major, backend->gl_minor);
	//backend->EnableCustomRender(true, PahomEngine->Render->gl_ver.minor, PahomEngine->Render->gl_ver.major);
	backend->initDeviceGL(sGLVersion.c_str());
	backend->SetDarkStyle();
	backend->initIO(&backend->io);
	backend->io.Fonts->AddFontFromFileTTF("assets/Iosevka-Term.ttf", 20.0f, nullptr, backend->io.Fonts->GetGlyphRangesCyrillic());//
	struct STRGPUGL {
		std::string_view E_Brand = (const char*)glGetString(GL_VENDOR);
		std::string_view E_Model = (const char*)glGetString(GL_RENDERER);
		std::string_view E_GLVer = (const char*)glGetString(GL_VERSION);
	};
	auto sGPU = std::make_unique<STRGPUGL>();
	int CPUInfo[4] = { -1 };
	unsigned   nExIds, i = 0;
	char CPUBrandString[0x40];
	// Get the information associated with each extended ID.
	__cpuid(CPUInfo, 0x80000000);
	nExIds = CPUInfo[0];
	for (i = 0x80000000; i <= nExIds; ++i)
	{
		__cpuid(CPUInfo, i);
		// Interpret CPU brand string
		if (i == 0x80000002)
			memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
		else if (i == 0x80000003)
			memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
		else if (i == 0x80000004)
			memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
	}
	std::string_view dCPUBrandString = CPUBrandString;
	///
	// Main loop
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
	bool main_logo = true;
	bool done = false;  
	uint64_t a = 0; uint64_t b = 0;
	typedef int64_t* intptr64_t;
	uint64_t err = 0;
	int64_t i64Cidx = 0;   
	ImFontAtlas* fonts = ImGui::GetIO().Fonts;
	ImFont* font15 = backend->io.Fonts->AddFontFromFileTTF("assets/Iosevka-Term.ttf", 30.0f, nullptr, backend->io.Fonts->GetGlyphRangesCyrillic());
	ImFont* font20 = backend->io.Fonts->AddFontFromFileTTF("assets/Iosevka-Term.ttf", 20.0f, nullptr, backend->io.Fonts->GetGlyphRangesCyrillic());
	ImFont* font3  = backend->io.Fonts->AddFontFromFileTTF("assets/Iosevka-Term.ttf", 15, nullptr, backend->io.Fonts->GetGlyphRangesCyrillic());
	ImFont* font10 = backend->io.Fonts->AddFontFromFileTTF("assets/Iosevka-Term.ttf", 10, nullptr, backend->io.Fonts->GetGlyphRangesCyrillic());
	ImFont* font35 = backend->io.Fonts->AddFontFromFileTTF("assets/Iosevka-Term.ttf", 25, nullptr, backend->io.Fonts->GetGlyphRangesCyrillic());
	ImFont* font27 = backend->io.Fonts->AddFontFromFileTTF("assets/Iosevka-Term.ttf", 21, nullptr, backend->io.Fonts->GetGlyphRangesCyrillic());
	ImFont* font50 = backend->io.Fonts->AddFontFromFileTTF("assets/Iosevka-Term.ttf", 40, nullptr, backend->io.Fonts->GetGlyphRangesCyrillic());
	ImFont* font54 = backend->io.Fonts->AddFontFromFileTTF("assets/Iosevka-Term.ttf", 41, nullptr, backend->io.Fonts->GetGlyphRangesCyrillic());
	ImFont* font16iv = backend->io.Fonts->AddFontFromFileTTF("assets/Iosevka-Term.ttf", 30, nullptr, backend->io.Fonts->GetGlyphRangesCyrillic());
	ImFont* font14iv = backend->io.Fonts->AddFontFromFileTTF("assets/Iosevka-Term.ttf", 14, nullptr, backend->io.Fonts->GetGlyphRangesCyrillic());
	if (!font15) { 
		std::cout << " [PahomEngine::Font] file assets/Iosevka-Term.ttf not found!" << std::endl;
	}   
	else {
		std::cout << " [PahomEngine::Font] file assets/Iosevka-Term.ttf loaded" << std::endl;
	}
	if (PahomEngine->ptrGamepad1->IsConnected()) {
		std::cout << " [PahomEngine::Controls->JoyStickAPI] Gamepad 0 connected!" << std::endl;
		PahomEngine->log("[GAMEPAD] Gamepad 0 connected!");
	}
	//

	int32_t i32GameOverCount = 0;
	bool bGameCount = true;
	//  
	for (int32_t i32AudioFiles = 0; i32AudioFiles < PE_ARRAYSIZE(PahomEngine->audio.audiolist); i32AudioFiles++) {
		if (PahomEngine->audio.audioFileIsOK(PahomEngine->audio.audiolist[i32AudioFiles])) {
			PahomEngine->log("FS::audiofile:-> " + PahomEngine->audio.audiolist[i32AudioFiles] + ": OK", 1);
		}
		else {
			PahomEngine->log("FS::audiofile:-> " + PahomEngine->audio.audiolist[i32AudioFiles] + ": Falled!!", 2);
		}
	}
	PahomEngine->log("FS::TexturesLOW");
	for (int32_t i32TextureFiles = 0; i32TextureFiles < PE_ARRAYSIZE(PahomEngine->assets.asset_ultra_low); i32TextureFiles++) {
		if (PahomEngine->assets.validFiles(PahomEngine->assets.asset_ultra_low[i32TextureFiles])) {
			PahomEngine->log("FS::Textures:-> " + (PahomEngine->assets.asset_ultra_low[i32TextureFiles]) + ": OK", 1);
		}
		else {
			PahomEngine->log("FS::Textures:-> " + (PahomEngine->assets.asset_ultra_low[i32TextureFiles]) + ": Falled!!", 2);
		}
	}
	PahomEngine->log("FS::TexturesHIGH");
	for (int32_t i32TextureFiles = 0; i32TextureFiles < PE_ARRAYSIZE(PahomEngine->assets.asset); i32TextureFiles++) {
		if (PahomEngine->assets.validFiles(PahomEngine->assets.asset[i32TextureFiles])) {
			PahomEngine->log("FS::Textures:-> " + (PahomEngine->assets.asset[i32TextureFiles]) + ": OK", 1);
		}
		else {
			PahomEngine->log("FS::Textures:-> " + (PahomEngine->assets.asset[i32TextureFiles]) + ": Falled!!", 2);
		}
	}
	PahomEngine->log("FS::TexturesULTRA");
	for (int32_t i32TextureFiles = 0; i32TextureFiles < PE_ARRAYSIZE(PahomEngine->assets.UltraTextures); i32TextureFiles++) {
		if (PahomEngine->assets.validFiles(PahomEngine->assets.UltraTextures[i32TextureFiles])) {
			PahomEngine->log("FS::Textures:-> " + (PahomEngine->assets.UltraTextures[i32TextureFiles]) + ": OK", 1);
		}
		else {
			PahomEngine->log("FS::Textures:-> " + (PahomEngine->assets.UltraTextures[i32TextureFiles]) + ": Falled!!", 2);
		}
	}
	if (PahomEngine->bFullscreen) {
		PahomEngine->i64WindowSize[0] = PahomEngine->HwndWSizeA(backend->hwnd).x;
		PahomEngine->i64WindowSize[1] = PahomEngine->HwndWSizeA(backend->hwnd).y;
		PahomEngine->fStepMove = PahomEngine->pDiff->diffArray[PahomEngine->pDiff->i64id].i64buffer + 10;
		// PahomEngine->fStep = PahomEngine->pDiff->diffArray[PahomEngine->pDiff->i64id].i64buffer1 + 10;
		ShowWindow(backend->hwnd, SW_SHOWMAXIMIZED);

	}
	else {
		ShowWindow(backend->hwnd, SW_SHOWNORMAL);
		PahomEngine->i64WindowSize[0] = PahomEngine->HwndWSizeA(backend->hwnd).x;
		PahomEngine->i64WindowSize[1] = PahomEngine->HwndWSizeA(backend->hwnd).y;
	}
	// preload noise filter
	if (PahomEngine->Event.bShowNoiseBackground)
	{
		PahomEngine->log(" GLM::Noise() Enable", 1);
		PahomEngine->log(" GLM::Noise() generate", 1);
		PahomEngine->ogl->SetSize(PahomEngine->i64WindowSize[0], PahomEngine->i64WindowSize[1]);
		PahomEngine->ogl->noise(PahomEngine->i64WindowSize[0], PahomEngine->i64WindowSize[1]);
		PahomEngine->ogl->InitTexture();
		PahomEngine->log(" GLM::Noise() Created", 1);
	}
	else {
		PahomEngine->log(" GLM::Noise() Disable", 1);
	}
	//
	PahomEngine->bIsRandomEngineUsed = PahomEngine->PESettings->bFlagRandomEngine;
	PahomEngine->CVsync = PahomEngine->PESettings->bFlagVsync;
	PahomEngine->audio.masterVolume = PahomEngine->PESettings->fMasterVolume;
	PahomEngine->i64CPUDelay = PahomEngine->PESettings->i64CPUDelay;
	SettingsUI->bWindowAppBlur = PahomEngine->PESettings->bRenderBlur;
	PahomEngine->Render->bIsUsedGLCustom = PahomEngine->PESettings->bUseCustomRender;
	PahomEngine->assets.bUseLowTextures = PahomEngine->PESettings->bUseLowTextures;
	PahomEngine->audio.bUseLowAudioQuality = PahomEngine->PESettings->bUseLowAudioQuality;
	// Left move key
	if (PahomEngine->PESettings->bKeyLDetected)
	{
		PahomEngine->keyMap.u8BACK = PahomEngine->PESettings->u8BindKeyLeft;
	}
	// Right move key
	if (PahomEngine->PESettings->bKeyRDetected)
	{
		PahomEngine->keyMap.u8FORWARD = PahomEngine->PESettings->u8BindKeyRight;
	}
	bool bUpdateFrame = false;
	if (SettingsUI->bWindowAppBlur) {
		ImGui::RenderBlur(backend->hwnd);
	}
	checkResolution();
	PahomEngine->ImageData.fill_memory();
	SettingsUI->bWindowAppBlur ? PahomEngine->StyleLoadBlur() : PahomEngine->StyleLoad();
	Game->hCurrentHwnd = GetDesktopWindow();
	PahomEngine->SetSizeHWND(backend->hwnd, PahomEngine->i64WindowSize[0], PahomEngine->i64WindowSize[1]);
	while (backend->render)
	{
		backend->io = ImGui::GetIO();
		// Poll and handle messages (inputs, window resize, etc.)
		// See the WndProc() function below for our to dispatch events to the Win32 backend.
		PahomEngine->Exceptions->GetProcessMemoryUsage();
		backend->NewFrame();
		backend->getMessage();


		if (done)
			break;

		static std::string sError;
		// cpu frame limiter begin part code
		if (PahomEngine->CVsync) { 
			auto start = std::chrono::steady_clock::now();
			auto awake_time = start + std::chrono::microseconds(PahomEngine->i64CPUDelay);
			std::this_thread::sleep_until(awake_time);
			auto end = std::chrono::steady_clock::now();
		}
		// cpu frame limiter end part code
		 // Set deltatime and framerate 
		PahomEngine->fDeltaTime = backend->io.DeltaTime * 100;
		PahomEngine->fFrameRate = backend->io.Framerate;
		static int64_t i64FrameRateInt = 0;
		//
		static float fColorAlpha = 0, fFadeChildFrame = 0;
		static int32_t i32DonationNameId = 0;
		static float fScrollDelay = 0.0f, fSpeed = 4.0f, fScrollChildFrame = -10;
		static std::string_view sDonatersString = "";
		static bool bLoaderFiles = false, bAudioStart = false;
		static bool bScrollEventReverse = true;
		static float ftx = 0.0f, fLineMinPos = 0;
		static int64_t i64OGL3TxCount = 0, i64Exceptions = 0, i64OGL3Errors = 0, i64OGL3TxTotalSize = 0, i64VRAMSize = 0;
		static ImVec4 fTextDonatersColor = {};
		//
		static bool bExitWindow = false;
		static bool bSWWindow = false;
		// main frame

		ImGui::Begin("gameFrame", &bSWWindow, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

		// error ogl3 frame
		if (ImGui::BeginPopup("ogl3_error", ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("PahomEngine::exception");
			ImGui::Separator();
			ImGui::PushFont(font10);
			ImGui::TextColored(PahomEngine->RGBA(233, 0, 65, 255), "%s", sError.data());
			ImGui::PopFont();
			ImGui::EndPopup();
		}

		//
		// setup size and pos window
		ImGui::SetWindowSize(ImVec2(PahomEngine->i64WindowSize[0], PahomEngine->i64WindowSize[1]));
		ImGui::SetWindowPos(ImVec2(0, 0));
		//
		// Loader Textures  
		static bool bMainTrackStarted = true;
		static bool bMainIntroAudioLoaded = false, bMainIntroAudioLoadedReplayFlag = true;
		static uint8_t u8audioPlay = 0;
		if (GetKeyState(VK_ESCAPE) & 0x8000) {
			bExitWindow = true;
			Game->bExitWindowClosed = false;
		}

		if (PahomEngine->bLoadingFrame) {

			b_FatalError = false;
			if (!bMainIntroAudioLoaded) {
				PahomEngine->audio.play(2);
				bMainIntroAudioLoaded = true;
			}
			if (PahomEngine->audio.audioDevice && bMainIntroAudioLoadedReplayFlag) {
				if (PahomEngine->audio.audioDevice.isEnd()) {
					PahomEngine->audio.audioDevice.replay();
				}
			}
			static int64_t tid = 0, i64FrameDelay = 0, i64Size = 0;
			       
			if (bLoaderFiles) {           
				//PahomEngine->progr ess_bar(ftx);
				if (PahomEngine->iReloadTexturesPresetID == 0) {
					if (PahomEngine->img->LoadTextureFromFile(reinterpret_cast<const char*>(PahomEngine->assets.asset_ultra_low[tid].data()),
						&PahomEngine->ImageData.TextureArray[tid],
						&PahomEngine->ImageData.TextureX[tid],
						&PahomEngine->ImageData.TextureY[tid],
						PahomEngine->ImageData.TextureBufferArray[tid]);
						i64Size += (PahomEngine->ImageData.TextureX[tid] * PahomEngine->ImageData.TextureY[tid] * 4)) {
						PahomEngine->log(std::format(" (OGL) loaded_buffer {} MB ->{}", (i64Size / 1024) / 1024, PahomEngine->assets.asset_ultra_low[tid]), 1);
					}   
					else {            
						PahomEngine->log(std::format(" (OGL) loaded_buffer {} MB error", (i64Size / 1024) / 1024), 1);
					}
				}       
				if (PahomEngine->iReloadTexturesPresetID == 1) {
					if (PahomEngine->img->LoadTextureFromFile(reinterpret_cast<const char*>(PahomEngine->assets.asset[tid].data()),
						&PahomEngine->ImageData.TextureArray[tid],
						&PahomEngine->ImageData.TextureX[tid],
						&PahomEngine->ImageData.TextureY[tid],
						PahomEngine->ImageData.TextureBufferArray[tid]);
						i64Size += (PahomEngine->ImageData.TextureX[tid] * PahomEngine->ImageData.TextureY[tid] * 4)) {
						PahomEngine->log(std::format(" (OGL) loaded_buffer {} MB ->{}", (i64Size / 1024) / 1024, PahomEngine->assets.asset[tid]), 1);
					}
					else {
						PahomEngine->log(std::format(" (OGL) loaded_buffer {} MB error", (i64Size / 1024) / 1024), 1);
					}
				}

				if (PahomEngine->iReloadTexturesPresetID == 2) {
					if (PahomEngine->img->LoadTextureFromFile(reinterpret_cast<const char*>(PahomEngine->assets.UltraTextures[tid].data()),
						&PahomEngine->ImageData.TextureArray[tid],
						&PahomEngine->ImageData.TextureX[tid],
						&PahomEngine->ImageData.TextureY[tid],
						PahomEngine->ImageData.TextureBufferArray[tid]);
						i64Size += (PahomEngine->ImageData.TextureX[tid] * PahomEngine->ImageData.TextureY[tid] * 4)) {
						PahomEngine->log(std::format(" (OGL) loaded_buffer {} MB ->{}", (i64Size / 1024) / 1024, PahomEngine->assets.UltraTextures[tid]), 1);
					}
					else {
						PahomEngine->log(std::format(" (OGL) loaded_buffer {} MB error", (i64Size / 1024) / 1024), 1);
					}
				} 
				tid++;
				if (tid > 11) {
					tid = 11;
					auto endTimeEngine = std::chrono::high_resolution_clock::now();
					std::chrono::duration<double> startEngineTime = (endTimeEngine - startTimeEngine);
					PahomEngine->log(std::format("started elapsed {}", startEngineTime.count()));
					bLoaderFiles = false; 
					PahomEngine->bLoadingFrameOK = true;
					if (!PahomEngine->bIsArgumentsNull)
					{
						if (std::strcmp(argv[1], "-nologo") == 0) {
							PahomEngine->bLoadingFrame = false;
							bMainTrackStarted = true;
							PahomEngine->log("[argv] -nologo enable", 1);
							bMainIntroAudioLoaded = false;
							PahomEngine->bLoadingFrameOK = true;
							PahomEngine->bStartGame = true;
							update_settings(); 
						}
					}
				}  
				ftx = (float)tid / 11.0f;
			}
			 

			// main menu controls loader
			a++;
			if (a > 2) {
				a = 0;
				static float fcl = 0;
				fcl += 25.5f;
				if (fcl < 255) {
					std::cout << " [PahomEngine::TextAlpha] flc:" << fcl << "b: " << b << std::endl;
					PahomEngine->log("[TEXT] " + std::format(" flc:{} {} ", fcl, b), 0);
				}
				if (fcl == 255) {
					fcl = 255;
					b = 255;
					bLoaderFiles = true;


				}
				PahomEngine->fillColorRGBA = PahomEngine->RGBA(133, 133, 133, fcl);
			}


			if (PahomEngine->Event.bTextureEditor) {
				if (ImGui::GetWindowSize().x >= 800 && ImGui::GetWindowSize().x != 800) {
					PahomEngineEditor(ImGui::GetWindowSize(), font10);
					ImGui::CloseCurrentPopup();
				}
				else {
					ImGui::SetCursorPos({ 30,20 });
					ImGui::Text("Размер окна слишком маленький для редактора.\nВключите в Опции->Графика-> Фуллскрин -> вкл");

				}
			}
			if (ImGui::BeginPopup("Редактор", 0)) {
				if (ImGui::Button("Редактор Текстур")) {
					PahomEngine->Event.bTextureEditor = !PahomEngine->Event.bTextureEditor;
					bPahomEngineTextureEditor = true;
				}
				if (ImGui::Button("Редактор Аудио")) {
					PahomEngine->Event.bTextureEditor = !PahomEngine->Event.bTextureEditor;
					bPahomEngineAudioEditor = true;

				}
				ImGui::EndPopup();
			}
			ImGui::SetCursorPosY(PahomEngine->i64WindowSize[1] == 480 ? 30 : 150);
			ImGui::PushFont(PahomEngine->i64WindowSize[1] == 480 ? font20 : font35);
			PahomEngine->setTextCenter("В ИГРЕ ПРИСУТСТВУЮТ МЕРЦАЮЩИЕ ЭЛЕМЕНТЫ");
			ImGui::TextColored(PahomEngine->RGBA(255, 0, 100, 255), "В ИГРЕ ПРИСУТСТВУЮТ МЕРЦАЮЩИЕ ЭЛЕМЕНТЫ");
			ImGui::PopFont();
			ImGui::PushFont(PahomEngine->i64WindowSize[1] == 480 ? font3 : font20);
			PahomEngine->setTextCenter("НЕ РЕКОМЕНДУЕТСЯ ИГРАТЬ ЛЮДЯМ C ЭПИЛЕПСИЕЙ");
			ImGui::TextColored(PahomEngine->RGBA(133, 133, 133, 255), "НЕ РЕКОМЕНДУЕТСЯ ИГРАТЬ ЛЮДЯМ C ЭПИЛЕПСИЕЙ");
			ImGui::SetCursorPosX(fLineMinPos);
			PahomEngine->TextColored(PahomEngine->RGBA({ 255,255,255,255 }), "ДОНАТЕРЫ ({:.0f}%%) {:.0f}/190 count:{} speed:{:.0f}", (fScrollChildFrame / 190) * 100, fScrollChildFrame, i64DonatersBufferSize, float(fSpeed * backend->io.DeltaTime));
			PahomEngine->setItemCenterX(500);
			ImGui::ImLine(500, 3, PahomEngine->RGBA(35, 35, 55, fColorAlpha));
			fLineMinPos = ImGui::GetItemRectMin().x;
			static float fMaxChildSizeY = 200;
			fFadeChildFrame += 300 * backend->io.DeltaTime;
			if (fFadeChildFrame >= fMaxChildSizeY) {
				fFadeChildFrame = fMaxChildSizeY;
				fColorAlpha += 140 * backend->io.DeltaTime;
				if (fColorAlpha >= 255.0f) {
					fColorAlpha = 255.0f;
				}
			}
			ImGui::SetCursorPosX(fLineMinPos);
			if (ImGui::BeginChild("donaters", ImVec2(500, fFadeChildFrame), ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground)) {
				if (fFadeChildFrame >= fMaxChildSizeY) {
					ImGui::SetScrollY(fScrollChildFrame);
					fScrollChildFrame += fSpeed * backend->io.DeltaTime;
					if (fScrollChildFrame >= 50) {
						fSpeed = 30.0f;
					}
					else {
						fSpeed = 10;
					}
					fTextDonatersColor = {
								0,
								255,
								200,
								255
					};
					if (fScrollChildFrame >= 310) {
						fScrollChildFrame = -10;
					}
					ImGui::SetCursorPosX(20);
					PahomEngine->TextColored(PahomEngine->RGBA({ fTextDonatersColor.x,fTextDonatersColor.y,fTextDonatersColor.z,fTextDonatersColor.w }), "{}", donaters);
				} 
				ImGui::EndChild();
			}
			PahomEngine->setItemCenterX(500);
			ImGui::ImLine(500, 3, PahomEngine->RGBA(35, 35, 55, fColorAlpha));
			ImGui::PopFont();
			if (GetAsyncKeyState(VK_F1)) {
				ImGui::OpenPopup("about");
			}
			//Game->TestStyle(font3);
			Game->fDeltaTime = backend->io.DeltaTime;
			Game->v2MousePos = backend->io.MousePos;

			Game->ConsoleLog(font3);

			// debug func and test
			//if (PahomEngine->getPressedKey('0', true) && !PahomEngine->getPressedKey('1', false) && !bFillingTestOpenGL) {
			//    ImGui::OpenPopup("TestStyle");
			//}
			//if (PahomEngine->getPressedKey('1', false) && !bFillingTestOpenGL) {
			//    Game->Editor();
			//}
			//if (PahomEngine->getPressedKey('2',true) && !PahomEngine->getPressedKey('1', false) && !bFillingTestOpenGL) {
			//    ImGui::OpenPopup("#console");
			//}
			//if (PahomEngine->getPressedKey('3', true) && !PahomEngine->getPressedKey('1', false) && !bFillingTestOpenGL) {
			//    Game->bDialog = true;
			//}
			//Game->TestDialogs();
			if (ImGui::BeginPopup("about", ImGuiWindowFlags_AlwaysAutoResize)) {

				static int64_t i64ValueDiff = 0;
				static bool bMessageTest = false;
				PahomEngine->pDiff->setDiff(PahomEngine->pDiff->diffArray[i64ValueDiff].i64buffer);
				ImGui::Image(PahomEngine->ptrint64_t(PahomEngine->ImageData.TextureArray[5]), ImVec2(320, 200));
				ImGui::Separator();
				ImGui::Text("PahomEngine %s", PahomEngine->sBuild.data());
				ImGui::Text("Writtein C++20 (MSVC-amd64)");
				ImGui::Text("GAPI: OGL3");
				ImGui::Text("AAPI: MinAudio_0.001");
				ImGui::Text("********* DEBUG ***********");
				ImGui::Text("exceptions        : %d", i64Exceptions);
				ImGui::Text("render_errors     : %d", i64OGL3Errors);
				ImGui::Text("textures_to_loaded: %d/6", i64OGL3TxCount);
				ImGui::Text("vram_used_textures: %d KB", i64OGL3TxTotalSize / 1024);

				ImGui::Text("VRAM: %lld", i64VRAMSize);
				ImGui::Text("Diff: %s id:%lld value: %lld", PahomEngine->pDiff->diffSelected.data(), i64ValueDiff, PahomEngine->pDiff->diffArray[i64ValueDiff].i64buffer);
				if (ImGui::Button("Console")) {

				}
				ImGui::SameLine();
				if (ImGui::Button("Diff_1")) {

					ImGui::OpenPopup("dialog");
				}
				ImGui::SameLine();
				if (ImGui::Button("Diff_2")) {
					i64ValueDiff = 2;
				}
				if (bMessageTest) {
					PahomEngine->UI->Message(std::format("cpu:{}\ngpu:{}\n memory:\n{}", dCPUBrandString, sGPU->E_Model, ptrMemory->MemoryInfo()), ImVec2(600, 200), 400, 500, &bMessageTest);
				}
				ImGui::EndPopup();
			}
			// ftx = 0.0 to 1;
			// max_size / (ftx * 10);
			static float fmax_size = static_cast<float>(PahomEngine->i64WindowSize[1] == 480 ? 250 : 500);
			static float fp_offset = 0;
			PahomEngine->setTextCenter(std::format("{} ({}/{})", PahomEngine->assets.asset[tid], tid, PE_ARRAYSIZE(PahomEngine->assets.asset)).data());
			fp_offset = (fmax_size / (ftx * 10)) * 10;
			PahomEngine->Text("{} ({}/{})", PahomEngine->assets.asset[tid], tid, PE_ARRAYSIZE(PahomEngine->assets.asset));
			PahomEngine->setItemCenterX(500);
			ImGui::ImLine(fp_offset, 10, PahomEngine->RGBA(255, 0, 160, 255));

			if (ImGui::IsItemClicked()) {
				ImGui::OpenPopup("tx");
			}
			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Нажми для просмотра");
			}
			if (ImGui::BeginPopup("tx", ImGuiWindowFlags_AlwaysAutoResize)) {
				static bool isParsed = false, bIsShow = false;
				static int32_t i32IndexImage = 0;
				static float fImageScale = 5;
				static int32_t i32ImageId = 0, i32SwitchPreview = 0;
				static float fImagePreviewSize = PahomEngine->i64WindowSize[1] == 480 ? 128 : 256;
				static bool bAudioPlay = false;          
				static int32_t i32DelayToHover = 0; 
				Game->DrawParticles(ImVec2(backend->io.MousePos.x, backend->io.MousePos.y),
					backend->io.DeltaTime,
					1040,                    // макс. частиц
					0.04f,                  // частота
					PahomEngine->RGBA(255, 68, 0, 255), // цвет
					PahomEngine->randfloat(5), PahomEngine->randfloat(15),             // размер
					1350.0f,                 // скорость
					800.0f,                 // гравитация
					1.8f                    // жизнь
				);
				if (ImGui::IsItemHovered()) {
					bIsShow = false;
				}
				for (int64_t tx = 0; tx <= 11; tx++) {
					ImGui::Image(PahomEngine->ptrint64_t(PahomEngine->ImageData.TextureArray[tx]), ImVec2(32, 32));
					ImGui::SameLine();
					ImGui::PushFont(PahomEngine->i64WindowSize[1] == 480 ? font3 : font20);
					ImGui::TextColored((i32ImageId == tx) ? PahomEngine->RGBA(0, 255, 230, 255) : PahomEngine->RGBA(255, 255, 255, 255), "%lld: %s", tx, (PahomEngine->assets.bUseLowTextures ? PahomEngine->assets.asset_ultra_low[tx].data() : PahomEngine->assets.asset[tx].data()));
					ImGui::PopFont();
					if (ImGui::IsItemHovered()) {

						i32ImageId = tx;
						i32DelayToHover++;
						if (i32DelayToHover > 3) {
							i32DelayToHover = 3;
							PahomEngine->audio.play(0);
							PahomEngine->audio.cleanup(PahomEngine->audio.audioDevice);
						}
						else {
							i32DelayToHover = 0;
						}

					}
					if (ImGui::IsItemClicked()) {
						i32SwitchPreview++;
						if (i32SwitchPreview > 1) {
							i32SwitchPreview = 0;
						}
						bIsShow = PahomEngine->cast->cast_all<bool>(i32SwitchPreview);
					}
					ImGui::Separator();
					if (!isParsed) {
						std::cout << " (PahomEngine)" << tx << " " << (PahomEngine->assets.bUseLowTextures ? PahomEngine->assets.asset_ultra_low[tx] : PahomEngine->assets.asset[tx]) << " : vram_used: " << PahomEngine->img->GetImageSize(PahomEngine->ImageData.TextureX[tx], PahomEngine->ImageData.TextureY[tx]) / 1024 << " KB" << std::endl;
						PahomEngine->log("[OGL] " + std::format(" {}: {} vram:{} KB", tx, PahomEngine->assets.asset[tx].data(), PahomEngine->img->GetImageSize(PahomEngine->ImageData.TextureX[tx], PahomEngine->ImageData.TextureY[tx]) / 1024), 3);
					}

				}
				if (bIsShow) {
					ImGui::BeginTooltip();
					ImGui::PushFont(PahomEngine->i64WindowSize[1] == 480 ? font3 : font20);
					ImGui::Text("%lld: %s", i32ImageId, PahomEngine->assets.asset[i32ImageId].data());
					ImGui::Text(std::format("{} x {}", PahomEngine->ImageData.TextureX[i32ImageId], PahomEngine->ImageData.TextureY[i32ImageId]).data());
					ImGui::Text("VRAM Used: %lld KB/ FileSize:%lld KB", PahomEngine->img->GetImageSize(PahomEngine->ImageData.TextureX[i32ImageId], PahomEngine->ImageData.TextureY[i32ImageId]) / 1024, PahomEngine->img->GetFileSize(PahomEngine->assets.asset[i32ImageId]) / 1024);
					ImGui::PopFont();
					ImGui::Image(PahomEngine->ptrint64_t(PahomEngine->ImageData.TextureArray[i32ImageId]), ImVec2(fImagePreviewSize, fImagePreviewSize));
					ImGui::EndTooltip();
				}
				isParsed = true;
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
				if (PahomEngine->audio.is_ptr_valid(PahomEngine->audio.audioDevice3))
				{
					PahomEngine->audio.audioDevice3.pause();

				}
				static int32_t iFileIdx = 0;
				static int64_t i64SettingsItemId = 0, i64MaxSettingsItems = 3 + i32Presets;
				static ImVec2 SizeSettings = { 440,520 }, minPosButton = {};

				if (b == 255) {
					ImGui::SetNextWindowContentSize(SizeSettings);
					if (ImGui::BeginPopupModal("settings_engine", &PahomEngine->bSettings, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysHorizontalScrollbar)) {

						ImGui::SetCursorPosY(30); ImGui::SetCursorPosX(30);
						if (PahomEngine->UI->CustomButton(ImVec4(20, 20, 30, 255), ImVec4(7, 7, 7, 255), ImVec4(255, 255, 255, 255), 20, ImVec2(0, 0), "Графика", font14iv, ImVec2(70, 30))) {
							SettingsUI->bGPU = true;
							SettingsUI->bAudio = false;
							SettingsUI->bControls = false;
							SettingsUI->bCPU = false;
							SettingsUI->bNetwork = false;
						}
						minPosButton = ImGui::GetItemRectMin();
						PahomEngine->selectedItem(SettingsUI->bGPU);
						ImGui::SameLine();
						if (PahomEngine->UI->CustomButton(ImVec4(20, 20, 30, 255), ImVec4(7, 7, 7, 255), ImVec4(255, 255, 255, 255), 20, ImVec2(0, 0), "CPU", font14iv, ImVec2(50, 30))) {
							SettingsUI->bGPU = false;
							SettingsUI->bAudio = false;
							SettingsUI->bControls = false;
							SettingsUI->bCPU = true;
							SettingsUI->bNetwork = false;
						}
						PahomEngine->selectedItem(SettingsUI->bCPU);
						ImGui::SameLine();
						if (PahomEngine->UI->CustomButton(ImVec4(20, 20, 30, 255), ImVec4(7, 7, 7, 255), ImVec4(255, 255, 255, 255), 20, ImVec2(0, 0), "Аудио", font14iv, ImVec2(70, 30))) {
							SettingsUI->bGPU = false;
							SettingsUI->bAudio = true;
							SettingsUI->bControls = false;
							SettingsUI->bCPU = false;
							SettingsUI->bNetwork = false;
						}
						PahomEngine->selectedItem(SettingsUI->bAudio);
						ImGui::SameLine();
						if (PahomEngine->UI->CustomButton(ImVec4(20, 20, 30, 255), ImVec4(7, 7, 7, 255), ImVec4(255, 255, 255, 255), 0, ImVec2(0, 0), "Управление", font14iv, ImVec2(120, 30))) {
							SettingsUI->bGPU = false;
							SettingsUI->bAudio = false;
							SettingsUI->bControls = true;
							SettingsUI->bCPU = false;
							SettingsUI->bNetwork = false;
						}
						PahomEngine->selectedItem(SettingsUI->bControls);
						ImGui::SameLine();
						if (PahomEngine->UI->CustomButton(ImVec4(20, 20, 30, 255), ImVec4(7, 7, 7, 255), ImVec4(255, 255, 255, 255), 0, ImVec2(0, 0), "Интернет", font14iv, ImVec2(80, 30))) {
							SettingsUI->bGPU = false;
							SettingsUI->bAudio = false;
							SettingsUI->bControls = false;
							SettingsUI->bCPU = false;
							SettingsUI->bNetwork = true;
						}
						PahomEngine->selectedItem(SettingsUI->bNetwork);
						ImGui::SetCursorPosX(30);
						Game->testBar();

						ImGui::PushFont(font3);
						if (SettingsUI->bNetwork) {
							static uint8_t u8loadedProxyConfig = 0;
							
							static std::string sPortProxy = "";
							static std::string sHostProxy = "";
							if (!u8loadedProxyConfig && !PahomEngine->lnethttp->sProxyHost.empty()) {
								sHostProxy = PahomEngine->lnethttp->sProxyHost;
								sPortProxy = std::to_string(PahomEngine->lnethttp->iProxyPort);
								u8loadedProxyConfig = 1;
							}
							ImVec2 fV2CurrentSize = ImGui::GetWindowSize() - ImVec2(30, 30);
							static bool bMessageOk = false;
							float fPaddingText = 30;
							static float fTimerToFadeOut = 0;
							std::string sTextInfo = "ПРОКСИ ПРИМЕНЕН!\n"
								"ДЛЯ ТОГО ЧТОБЫ ОТМЕНИТЬ УДАЛИТЕ ДАННЫE";
							ImGui::SetCursorPosX(30);
							PahomEngine->Text("ПРОКСИ:");
							ImGui::SetCursorPosX(30);
							ImGui::ImLine(300, 3, PahomEngine->RGBA(0x73, 0x73, 0x73, 255.0f));
							ImGui::PushFont(font14iv);
							ImGui::SetCursorPosX(30);
							PahomEngine->Text("Хост");
							ImGui::PopFont();
							ImGui::SetCursorPosX(30);
							ImGui::PushID(int('T'));
							ImGui::InputTextMultiline("", &sHostProxy,{300,30});
							ImGui::PopID();
							ImGui::PushFont(font14iv);
							ImGui::SetCursorPosX(30);
							PahomEngine->Text("Порт");
							ImGui::PopFont();
							ImGui::SetCursorPosX(30);
							ImGui::PushID(int('T1'));
							ImGui::InputTextMultiline("", &sPortProxy, { 300,30 }, ImGuiInputTextFlags_CharsDecimal);
							ImGui::PopID();
							ImGui::PushFont(font14iv);
							ImGui::SetCursorPosX(30);
							PahomEngine->Text("URL-ping");
							ImGui::PopFont();
							ImGui::SetCursorPosX(30);
							ImGui::PushID(int('T2'));
							ImGui::InputTextMultiline("", &PahomEngine->lnethttp->sTestProxyURL, { 300,30 }, ImGuiInputTextFlags_None);
							ImGui::PopID();
							ImGui::SetCursorPosX(30);
							if (ImGui::Button("Применить", { 120,30 })) {
								if (!sPortProxy.empty()) {
									PahomEngine->lnethttp->iProxyPort = std::stoi(sPortProxy);
								}
								if (!sHostProxy.empty()) {
									PahomEngine->lnethttp->sProxyHost = sHostProxy;
								}
								bMessageOk = (!sPortProxy.empty() && !sHostProxy.empty());
							}ImGui::SameLine();
							if (ImGui::Button("Тест", { 100,30 })) {
								if (!sPortProxy.empty()) {
									PahomEngine->lnethttp->iProxyPort = std::stoi(sPortProxy);
								}
								if (!sHostProxy.empty()) {
									PahomEngine->lnethttp->sProxyHost = sHostProxy;
								}
								bMessageOk = (!sPortProxy.empty() && !sHostProxy.empty());
								PahomEngine->lnethttp->TestProxy();
							}
							ImGui::SetCursorPosX(30);
							PahomEngine->Text("Прокси: ");
							ImGui::SameLine();
							PahomEngine->TextColored(
								(PahomEngine->lnethttp->bIsProxyValid ? PahomEngine->RGBA(0, 255, 130, 255) : PahomEngine->RGBA(255, 0, 130, 255)),"{}",
								(PahomEngine->lnethttp->bIsProxyValid ? " Доступен" : " Не доступен")
							);
							if (bMessageOk) {
								fTimerToFadeOut += 255 * ImGui::GetIO().DeltaTime;
								if (fTimerToFadeOut > 255) { fTimerToFadeOut = 0; bMessageOk = false; }
								ImGui::SetCursorPosX(
									(ImGui::GetWindowWidth() - fV2CurrentSize.x) / 2.0f
								);
								ImGui::BeginChild("s", { fV2CurrentSize.x - fPaddingText, ImGui::CalcTextSize(sTextInfo.c_str()).y + fPaddingText }, ImGuiChildFlags_FrameStyle);
								PahomEngine->setItemCenterX(ImGui::CalcTextSize(sTextInfo.c_str()).x, fV2CurrentSize.x - fPaddingText);
								PahomEngine->TextColored({ 250,30,160,255 }, "{}", sTextInfo);
								ImGui::EndChild();
							}
							ImGui::SetCursorPosX(30);
							ImGui::ImLine(300, 3, PahomEngine->RGBA(0x73, 0x73, 0x73, 255.0f));
						}
						if (SettingsUI->bAudio) {
							ImGui::SetCursorPosX(30); ImGui::TextColored(PahomEngine->RGBA(35, 35, 55, 255), "громкость");

							ImGui::SameLine();
							PahomEngine->audio.vue();
							if (PahomEngine->getPressedKey('K')) {
								ImGui::OpenPopup("vue");
							}
							if (ImGui::IsItemClicked()) {
								ImGui::OpenPopup("volume_custom");
							}
							if (ImGui::BeginPopup("volume_custom", ImGuiWindowFlags_AlwaysAutoResize)) {
								ImGui::SetCursorPosX(10);
								ImGui::TextColored(PahomEngine->RGBA(35, 35, 55, 255), "громкость");
								ImGui::SetCursorPosX(30); if (ImGui::SliderFloat("Vol", &PahomEngine->audio.masterVolume, 0.03f, 1.0f)) {
									PahomEngine->audio.setVolume(PahomEngine->audio.masterVolume);
									PahomEngine->audio.play2(iFileIdx);
								}
								if (PahomEngine->audio.masterVolume == PahomEngine->audio.masterVolume + 0.01) {
									PahomEngine->audio.play2(iFileIdx);
								}
								ImGui::EndPopup();
							}
							ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10);
							ImGui::ProgressBar(PahomEngine->audio.masterVolume, ImVec2(200, 20), (std::to_string(int(PahomEngine->audio.masterVolume * 100)) + "%").data());
							if (ImGui::IsItemHovered()) {
								i64SettingsItemId = 0;
								ImDrawList* drawList = ImGui::GetWindowDrawList();
								drawList->AddRect(
									ImGui::GetItemRectMin(),
									ImGui::GetItemRectMax(),
									IM_COL32(255, 255, 0, 255),
									0.0f, 0, 2.0f
								);
							}
							PahomEngine->selectedItem(i64SettingsItemId == 0);
							ImGui::PopStyleVar();
							ImGui::SameLine();
							ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10);
							if (ImGui::Button("+", ImVec2(30, 30))) {
								PahomEngine->audio.masterVolume += 0.01;
								PahomEngine->audio.masterVolumeLast = PahomEngine->audio.masterVolume;
								if (PahomEngine->audio.masterVolume == 0) {
									PahomEngine->audio.masterVolume = 0;
								}
								PahomEngine->audio.setVolume(PahomEngine->audio.masterVolume);
								PahomEngine->audio.play2(iFileIdx);
								PahomEngine->audio.cleanup(PahomEngine->audio.audioDevice2);
							}
							ImGui::PopStyleVar();
							ImGui::SameLine();
							ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10);
							if (ImGui::Button("-", ImVec2(30, 30))) {
								PahomEngine->audio.masterVolume -= 0.01;
								PahomEngine->audio.masterVolumeLast = PahomEngine->audio.masterVolume;
								if (PahomEngine->audio.masterVolume == 1) {
									PahomEngine->audio.masterVolume = 1;
								}
								PahomEngine->audio.setVolume(PahomEngine->audio.masterVolume);
								PahomEngine->audio.play2(iFileIdx);
								PahomEngine->audio.cleanup(PahomEngine->audio.audioDevice2);
							}
							ImGui::PopStyleVar();
							ImGui::SetCursorPosX(30);
							ImGui::CustomToggle("Low Quality Audio", &PahomEngine->audio.bUseLowAudioQuality);
							ImGui::SetCursorPosX(30);
							if (ImGui::CustomToggle("Отключить музыку", &PahomEngine->audio.isAudioMuted)) {
								static int iMuted = 0;
								static bool isButtonHovered = false;
								iMuted++;
								if (iMuted > 1) {
									iMuted = 0;
								}
								if (iMuted) {
									PahomEngine->audio.Mute();
								}
								else {
									PahomEngine->audio.masterVolume = PahomEngine->audio.masterVolumeLast;
									if (PahomEngine->audio.audioDevice) {
										PahomEngine->audio.audioDevice.setVolume(PahomEngine->audio.masterVolume);
										PahomEngine->audio.audioDevice.play();
									}
									if (PahomEngine->audio.audioDevice2) {
										PahomEngine->audio.audioDevice2.setVolume(PahomEngine->audio.masterVolume);
										if (PahomEngine->audio.sLastAudioFile[1] != PahomEngine->audio.audiolist[3])
										{
											PahomEngine->audio.audioDevice2.play();
										}
									}
									if (PahomEngine->audio.audioDevice3) {
										PahomEngine->audio.audioDevice3.setVolume(PahomEngine->audio.masterVolume);
									}
								}
								PahomEngine->audio.isAudioMuted = iMuted;

							}
							//
							static bool bAudioState = false; static int32_t i32AudioStateInt32 = 0;
							static int32_t i32CurrentAudio = 0;
							static int iAudioSelected = 0;
							static bool bHw_monitor = false;
							//
							ImGui::SetCursorPosX(30);
							PahomEngine->TextColored(PahomEngine->RGBA(255, 205, 0, 255), "SLONIK PLAYER ({}/{}", i32CurrentAudio, PahomEngine->audio.musicFiles.size());
							ImGui::SetCursorPosX(30);
							ImGui::ImLine(400, 3, PahomEngine->RGBA(35, 35, 55, 255));
							ImGui::SetCursorPosX(30);
							if (ImGui::BeginChild("slonik_player", { 400,100 }, ImGuiChildFlags_FrameStyle, ImGuiWindowFlags_NoTitleBar)) {
								static ImVec4 color = PahomEngine->RGBA(35, 35, 55, 255), colorSelected = PahomEngine->RGBA(255, 205, 0, 255);
								for (int iAudioList = 0; iAudioList < PahomEngine->audio.musicFiles.size(); iAudioList++) {
									PahomEngine->TextColored((iAudioSelected == iAudioList ? colorSelected : color), "({}). {}", iAudioList, PahomEngine->audio.musicFiles[iAudioList]);
									if (ImGui::IsItemClicked()) {
										i32CurrentAudio = iAudioList;
										iAudioSelected = i32CurrentAudio;
										PahomEngine->audio.music_player(iAudioSelected);
									}
								}

								ImGui::EndChild();
							}
							ImGui::SetCursorPosX(30);
							ImGui::ImLine(400, 3, PahomEngine->RGBA(35, 35, 55, 255));
							ImGui::SetCursorPosX(30);
							if (ImGui::Button(bAudioState ? "Play" : "Pause", { 64,32 })) {
								if (PahomEngine->audio.audioDevice) {
									PahomEngine->audio.isDeviceActive = !PahomEngine->audio.isDeviceActive;
									i32AudioStateInt32++;
									if (i32AudioStateInt32 > 1) { i32AudioStateInt32 = 0; }
									bAudioState = (i32AudioStateInt32 ? true : false);
									if (bAudioState)
									{
										PahomEngine->audio.audioDevice.pause();
									}
									else {
										PahomEngine->audio.audioDevice.play();
									}


								}
							}
							ImGui::SameLine();
							if (ImGui::Button("Сканировать", { 128,32 })) {
								PahomEngine->audio.ScanFiles("musics", ".mp3");

							}
							ImGui::SameLine();
							if (ImGui::BeginChild("PlayerInfo", { 194,32 }, ImGuiChildFlags_FrameStyle, ImGuiWindowFlags_NoTitleBar)) {
								ImGui::SetCursorPosY(
									32 / 2 * 0.5f

								);
								if (i32CurrentAudio >= (PahomEngine->audio.musicFiles.size())) {
									i32CurrentAudio = 0;
								}
								if (PahomEngine->audio.FilesScanned) {
									PahomEngine->TextColored(PahomEngine->RGBA(0, 255, 280, 255), "След {}", PahomEngine->audio.musicFiles[(i32CurrentAudio > PahomEngine->audio.musicFiles.size()) ? PahomEngine->audio.musicFiles.size() : i32CurrentAudio]);
								}
								ImGui::EndChild();
							}
							if (PahomEngine->audio.audioDevice) {
								ImGui::SetCursorPosX(30);
								ImGui::SetNextItemWidth(400);
								if (ImGui::SliderFloat("v:", &PahomEngine->audio.fMusicPlayerVolumeOffset, 0.00f, 1.00f)) {
									PahomEngine->audio.audioDevice.setVolume(PahomEngine->audio.fMusicPlayerVolumeOffset);
								}
							}
							ImGui::PushFont(font10);
							ImGui::SetCursorPosX(30); ImGui::TextColored(PahomEngine->RGBA(133, 133, 133, 255), "Нажми на надпись 'Громкость' чтобы настроить более точно");
							ImGui::PopFont();
						}
						if (SettingsUI->bGPU) {
							ImGui::SetCursorPosX(10);
							ImGui::SetCursorPosX(30); ImGui::TextColored(PahomEngine->RGBA(35, 35, 55, 255), "Фуллскрин");
							if (ImGui::IsItemHovered()) {
								i64SettingsItemId = 1;
							}
							PahomEngine->selectedItem(i64SettingsItemId == 1);
							ImGui::SameLine();
							if (ImGui::Checkbox(".", &PahomEngine->bFullscreen)) {
								PahomEngine->bRenderIsEdited = false;
							}
							if (PahomEngine->bFullscreen) {

								PahomEngine->i64WindowSizeGL[0] = PahomEngine->i64WindowSizeGL[0];
								PahomEngine->i64WindowSizeGL[1] = PahomEngine->i64WindowSizeGL[1];
								PahomEngine->i64WindowSize[0] = PahomEngine->HwndWSizeA(backend->hwnd).x;
								PahomEngine->i64WindowSize[1] = PahomEngine->HwndWSizeA(backend->hwnd).y;
								PahomEngine->fStepMove = PahomEngine->pDiff->diffArray[PahomEngine->pDiff->i64id].i64buffer + 13;
								// PahomEngine->fStep = PahomEngine->pDiff->diffArray[PahomEngine->pDiff->i64id].i64buffer1 + 10;
								ShowWindow(backend->hwnd, SW_SHOWMAXIMIZED);

							}
							else {
								ShowWindow(backend->hwnd, SW_SHOWNORMAL);
								PahomEngine->i64WindowSize[0] = PahomEngine->HwndWSizeA(backend->hwnd).x;
								PahomEngine->i64WindowSize[1] = PahomEngine->HwndWSizeA(backend->hwnd).y;
							}
							ImGui::SetCursorPosX(30);
							ImGui::PushFont(font20);
							PahomEngine->TextColored(PahomEngine->RGBA(130, 130, 130, 255), "РАЗРЕШЕНИЕ ЭКРАНА");
							ImGui::PopFont();
							loadResulutionUI(font15);
							ImGui::PushID(4);
							ImGui::SameLine();
							if (ImGui::Button("Применить", { 100,32 })) {
								PahomEngine->bRenderIsEdited = true;
								glViewport(0, 0, PahomEngine->i64WindowSizeGL[0], PahomEngine->i64WindowSizeGL[1]);
								PahomEngine->SetSizeHWND(backend->hwnd, PahomEngine->window_size_preset[iResID].first, PahomEngine->window_size_preset[iResID].second);
								PahomEngine->i64WindowSize[0] = PahomEngine->i64WindowSizeGL[0];
								PahomEngine->i64WindowSize[1] = PahomEngine->i64WindowSizeGL[1];
								std::cout << std::format(" [PahomEngine] Edited\nset size window {} x {}\n max_pos_x: {} max_pos_y:{}\n min_pos_x: {} min_pos_y: {}\n", PahomEngine->i64WindowSize[0], PahomEngine->i64WindowSize[1], PahomEngine->fMaxPahomPosX, PahomEngine->fMaxPahomPosY, PahomEngine->fMinPahomPosX, PahomEngine->fMinPahomPosY);
							}
							ImGui::PopID();
							ImGui::SetCursorPosX(30);
							ImGui::PushFont(font20);
							PahomEngine->TextColored(PahomEngine->RGBA(130, 130, 130, 255), "КАЧЕСТВО ТЕКСТУР");
							ImGui::PopFont();
							ImGui::SetCursorPosX(30);
							GraphicsPresetUI(font15);
							ImGui::SameLine();
							if (ImGui::Button("Применить", { 100,32 })) {
								PahomEngine->ReloadTextures(PahomEngine->iReloadTexturesPresetID);
							}  
							ImGui::SetCursorPosX(30); ImGui::Text("%lldx%lld x%lld", PahomEngine->i64WindowSizeGL[0], PahomEngine->i64WindowSizeGL[1], PahomEngine->i64ScaleRender);
							ImGui::SetCursorPosX(10);
							ImGui::SetCursorPosX(30); ImGui::TextColored(PahomEngine->RGBA(35, 35, 55, 255), "60гц (60 фпс лимит)"); ImGui::SameLine();
							ImGui::Checkbox("60 фпс", &PahomEngine->CVsync);//PahomEngine->CVsync
							ImGui::SameLine(); 
							ImGui::TextColored(PahomEngine->RGBA(35, 35, 55, 255), "%s", (PahomEngine->CVsync ? "60фпс" : "Неогран.фпс"));
							PahomEngine->TextColored(PahomEngine->RGBA(200, 100, 100, 255), "Для полного эффекта блюра требуется перезапуск");
							ImGui::SetCursorPosX(30); ImGui::CustomToggle("Включить Блюр", &SettingsUI->bWindowAppBlur);
							SettingsUI->bWindowAppBlur ? PahomEngine->StyleLoadBlur() : PahomEngine->StyleLoad();
							ImGui::ImLine(500, 3, PahomEngine->RGBA(35, 35, 55, 255));
							// 
							ImGui::SetCursorPosX(30); ImGui::CustomToggle("RGBA цвета в консоли", &s->bColoredConsole);
							if (ImGui::IsItemClicked()) {
								PahomEngine->log((s->bColoredConsole ? "Edited Flags::bColoredConsole = true" : " Edited Flags::bColoredConsole = false"), 3);
							}
							PahomEngine->TextColored(PahomEngine->RGBA(200, 100, 100, 255), "Работает только с Windows 10");
							ImGui::SetCursorPosX(30); ImGui::CustomToggle("Фон шума при проигрыше", &PahomEngine->Event.bShowNoiseBackground);
							static int64_t i64GlVersionIdx = 0;
							SizeSettings.y = 580;
						} 
						if (SettingsUI->bControls) {
							static bool bRemapButtons = false, bButtonEditL = false, bButtonEditR = false;
							static std::string_view buttons[] = { "Вперед" , "Назад" };
							static int8_t u8buttons[] = { 0,0 };
							ImGui::SetCursorPosX(30); ImGui::CustomToggle("Вибрация", &PahomEngine->ptrGamepad1->bVibrationEnable);
							ImGui::SetCursorPosX(30);
							if (ImGui::Button((bRemapButtons && bButtonEditR) ? "Нажми клавишу" : buttons[1].data(), { 140,30 })) {
								bRemapButtons = true;
								bButtonEditL = false;
								bButtonEditR = true;
								u8buttons[0] = u8buttons[1] = 0;
							}
							ImGui::SameLine();
							if (ImGui::Button((bRemapButtons && bButtonEditL) ? "Нажми клавишу" : buttons[0].data(), { 140,30 })) {
								bRemapButtons = true;
								bButtonEditR = false;
								bButtonEditL = true;
							}
							if (bRemapButtons) {
								if (bButtonEditL)
								{
									// buttons[1] = std::format("Назад  ({})", static_cast<char>(u8buttons[0]));
									u8buttons[1] = Remap->GetKey(&bButtonEditL);
									if (u8buttons[1] > 0) {
										bButtonEditL = false;
									}
									bButtonEditR = false;
									// PahomEngine->log(std::format("Назад -> {}", static_cast<char>(u8buttons[0])),1);
								}
								if (bButtonEditR)
								{
									//buttons[0] = std::format("Вперед ({})", static_cast<char>(u8buttons[1]));
									u8buttons[0] = Remap->GetKey(&bButtonEditR);
									//PahomEngine->log(std::format("Вперед -> {}", static_cast<char>(u8buttons[1])), 1);
									if (u8buttons[0] > 0) {
										bButtonEditR = false;
									}
									bButtonEditL = false;
								}

							}

							ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10);
							ImGui::SetCursorPosX(30); if (ImGui::Button("Применить", ImVec2(150, 30))) {
								PahomEngine->cast->bUsedStaticCast = true;
								bRemapButtons = false;
								PahomEngine->keyMap.u8FORWARD = u8buttons[0];
								PahomEngine->keyMap.u8BACK = u8buttons[1];
								buttons[0] = std::format("Назад   ({})", static_cast<char>(u8buttons[0]));
								buttons[1] = std::format("Вперед  ({})", static_cast<char>(u8buttons[1]));
								PahomEngine->log(std::format("Controls:: bind_key_left={} | bind_key_right={}", static_cast<char>(u8buttons[0]), static_cast<char>(u8buttons[1])), 1);
							}
							ImGui::PopStyleVar();

							static int64_t i64ControlTest = 0;
							ImGui::SetCursorPosX(30); ImGui::TextColored(PahomEngine->RGBA(35, 35, 55, 255), "Задержка управления");
							ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10);
							ImGui::SetCursorPosX(30); if (ImGui::SliderInt64("kbDelay", &PahomEngine->keyMap.vMaxDelay, 1, 64, "%lld", 0)) {
								PahomEngine->keyMap.kbDelay = 0;
							}
							ImGui::PopStyleVar();
							if (GetKeyState(PahomEngine->keyMap.u8BACK) & 0x8000) {
								PahomEngine->keyMap.kbDelay++;
								if (PahomEngine->keyMap.kbDelay == PahomEngine->keyMap.vMaxDelay) {
									i64ControlTest++;
									if (i64ControlTest > 100) {
										i64ControlTest = 0;
									}
									PahomEngine->keyMap.kbDelay = 0;
								}
							}
							else {
								if (PahomEngine->keyMap.kbDelay == PahomEngine->keyMap.vMaxDelay) {
									i64ControlTest--;
									if (i64ControlTest < 0) {
										i64ControlTest = 0;
									}
									PahomEngine->keyMap.kbDelay = 0;
								}
							}
							if (GetKeyState(PahomEngine->keyMap.u8FORWARD) & 0x8000) {
								PahomEngine->keyMap.kbDelay++;
								if (PahomEngine->keyMap.kbDelay == PahomEngine->keyMap.vMaxDelay) {
									i64ControlTest--;
									if (i64ControlTest < 0) {
										i64ControlTest = 0;
									}
									PahomEngine->keyMap.kbDelay = 0;
								}
							}
							ImGui::SetCursorPosX(30); ImGui::TextColored(PahomEngine->RGBA(133, 133, 133, 255), "Проверьте сразу. Внизу слайдер и нажатием\nвлево или в вправо с какой скорость будет двигаться");
							if (SettingsUI->bGamepadTestUI) {
								GamepadTestUI();
							}
							ImGui::SetCursorPosX(30); ImGui::ProgressBar(PahomEngine->cast->cast_all<float>(i64ControlTest) / 100.0f, ImVec2(250, 20), (std::to_string(PahomEngine->cast->cast_all<float>(i64ControlTest))).data());
							ImGui::SetCursorPosX(30); ImGui::Text("%lld/100:%lld", i64ControlTest, i64ControlTest / 100);
							ImGui::SetCursorPosX(30); PahomEngine->Text("Геймпад");
							ImGui::SetCursorPosX(30); ImGui::ImLine(400, 3, PahomEngine->RGBA(70, 0, 255, 255));
							ImGui::SetCursorPosX(30); ImGui::CustomToggle("GamepadAPI", &PahomEngine->bIsImGuiAGamepadAPIUsed);
							ImGui::SameLine();
							PahomEngine->TextColored((PahomEngine->bIsImGuiAGamepadAPIUsed ? PahomEngine->RGBA(0, 255, 160, 255) : PahomEngine->RGBA(255, 45, 180, 255)),
								"{}",
								(PahomEngine->bIsImGuiAGamepadAPIUsed ? "ImGuiGamepadAPI" : "JoyStickAPI"));
							if (SettingsUI->bGamepadTestUI) {
								if (PahomEngine->getPressedKey(VK_ESCAPE, true)) {
									SettingsUI->bGamepadTestUI = false;
								}
							}
							ImGui::SetCursorPosX(30); if (ImGui::Button("Тест Геймпада", { 150,30 })) {
								SettingsUI->bGamepadTestUI = true;
							}
						}
						if (SettingsUI->bCPU) {
							if (ImGui::BeginPopupModal("warn", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
								ImGui::Text("BENCH CPU64");
								ImGui::Separator();
								//Game->KefirFriendlyEvent();
								ImGui::TextColored(PahomEngine->RGBA(255, 0, 60, 255), "Внимание!!\nДля Запуска теста надо 10 гб оперативной памяти!");
								if (ImGui::Button("OK")) {
									PahomEngine->Bench64ptr->mt();

								}ImGui::SameLine();
								if (ImGui::Button("Передумал")) {
									ImGui::CloseCurrentPopup();
								}
								ImGui::EndPopup();
							}
							if (ImGui::BeginPopup("frame_cpu_test", ImGuiWindowFlags_AlwaysAutoResize)) {
								ImGui::SetCursorPos(ImVec2{
									30,
									30
									});
								ImGui::Spinner("FPS", 32, 10, ImGui::GetColorU32(PahomEngine->RGBA(35, 35, 55, 255)));
								ImGui::SameLine();

								ImGui::SetCursorPosY(50);
								ImGui::SetCursorPosX(30); ImGui::Text("PahomEngine подбирает оптимальные\nнастройки под ваше устройство");
								ImGui::SetCursorPosX(30);
								ImGui::ProgressBar(backend->io.Framerate / 60, ImVec2(320, 20));
								ImGui::SameLine();
								ImGui::Text("%.0f", backend->io.Framerate);


								if (PahomEngine->fFrameRate < 60) {
									PahomEngine->i64CPUDelay--;
									if (!PahomEngine->i64CPUDelay) {
										PahomEngine->i64CPUDelay = 1;
									}
								}
								if (PahomEngine->fFrameRate > 60) {
									ImGui::CloseCurrentPopup();
								}
								ImGui::EndPopup();
							}
							ImGui::SetCursorPosX(30);
							PahomEngine->Text("FPS:{:.1f}/{} ms", backend->io.Framerate, PahomEngine->i64CPUDelay);
							ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10);
							ImGui::SetCursorPosX(30); ImGui::SliderInt64("Время кадра", &PahomEngine->i64CPUDelay, 0, 100000, "%lld ms", 0);
							ImGui::SetCursorPosX(30); if (ImGui::Button("Автонастройка", ImVec2(160, 30))) {
								ImGui::OpenPopup("frame_cpu_test");
							}
							ImGui::PopStyleVar();
							ImGui::PushFont(font10);
							ImGui::SetCursorPosX(30); ImGui::TextColored(PahomEngine->RGBA(133, 133, 133, 255), "Очень чувствительный параметр Если у вас очень много фпс\nи огромный фпс  то можно подстроить под себя");
							ImGui::PopFont();
							ImGui::SetCursorPosX(30); ImGui::ImLine(400, 3, PahomEngine->RGBA(45, 45, 65, 255));
							ImGui::SetCursorPosX(30); PahomEngine->Text("Параметр уменьшает точность рандома в игре");
							ImGui::SetCursorPosX(30); ImGui::CustomToggle("Использовать Энтропию", &PahomEngine->bIsRandomEngineUsed);
							if (!PahomEngine->bIsRandomEngineUsed) {
								ImGui::SetCursorPosX(30);
								ImGui::CustomToggle("Random type", &PahomEngine->math->bIsRandomXor64);
								ImGui::SameLine();
								PahomEngine->TextColored(
									(PahomEngine->math->bIsRandomXor64 ? PahomEngine->RGBA(255, 30, 50, 255) : PahomEngine->RGBA(60, 255, 140, 255)),
									"{}", (PahomEngine->math->bIsRandomXor64 ? " xor64 " : " rand ")
								);
							}
							ImGui::PushFont(font10);
							ImGui::SetCursorPosX(30); ImGui::TextColored(PahomEngine->RGBA(255, 160, 0, 255), "Включает плавное затухание на объектах (бутылка , кефир)");
							ImGui::PopFont();
							ImGui::SetCursorPosX(30); ImGui::CustomToggle("Анимация кефира", &PahomEngine->PESettings->bFlagEnableAnimationToImageFadeInOut);
							ImGui::SetCursorPosX(30); ImGui::ImLine(400, 3, PahomEngine->RGBA(45, 45, 65, 255));
							if (GetKeyState(VK_F3)) {
								ImGui::SetCursorPos({ (440 - 400) / 2,(420 - 150) / 2 });
								if (ImGui::BeginChild("Hardware", { 400,150 }, ImGuiChildFlags_FrameStyle, ImGuiWindowFlags_NoTitleBar)) {

									PahomEngine->TextColored(PahomEngine->RGBA(255, 30, 160, 255), "[F3] выйти");
									ImGui::SetCursorPosX(30);  PahomEngine->TextColored(PahomEngine->RGBA(255, 30, 160, 255), "CPU: {}", dCPUBrandString.data());
									ImGui::SetCursorPosX(30);  PahomEngine->TextColored(PahomEngine->RGBA(255, 30, 160, 255), "GPU: {}", sGPU->E_Model.data());
									ImGui::SetCursorPosX(30);  PahomEngine->TextColored(PahomEngine->RGBA(255, 30, 160, 255), "{}", ptrMemory->MemoryInfo());
									PahomEngine->Exceptions->GetProcessMemoryUsage();
									ImGui::SetCursorPosX(30);  PahomEngine->TextColored(PahomEngine->RGBA(30, 30, 60, 255), "(PahomEngine) mem_usage : {} MB", (PahomEngine->Exceptions->i64MemoryUsageProcess / 1024) / 1024);
									ImGui::EndChild();
								}
							}
							else {
								ImGui::SetCursorPosX(30); PahomEngine->TextColored(PahomEngine->RGBA(255, 30, 160, 255), "[F3] - HardwareInfo");
							}
							ImGui::SetCursorPosX(30); ImGui::SetCursorPosY(330); PahomEngine->TextColored(PahomEngine->RGBA(30, 230, 160, 255), "{} за {} сек", PahomEngine->Bench64ptr->i64MaxSize.load(), PahomEngine->Bench64ptr->i64BenchLastTime);
							ImGui::SetCursorPosY(325);
							ImGui::SetCursorPosX(ImGui::CalcTextSize(std::format("{} за {} сек", PahomEngine->Bench64ptr->i64MaxSize.load(), PahomEngine->Bench64ptr->i64BenchLastTime).data()).x + 40);
							if (ImGui::Button("CPU Bench", ImVec2(150, 30)))
							{
								ImGui::OpenPopup("warn");
							}
							ImGui::SetCursorPosX(30); ImGui::Text(("PahomEngine_ogl3_amd64 build " + PahomEngine->sBuild).data());
							ImGui::SetCursorPosX(10);


						}
						ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10);
						ImGui::SetCursorPosX(30); if (ImGui::Button("Сохранить", ImVec2(200, 40))) {
							PahomEngine->PESettings->Save(std::format("vsync={}\nrandom_engine={}\nvolume={}\ncpu_delay={}\nrender_blur={}\nbind_key_right={}\nbind_key_left={}\nEnableAnimationToImageFadeInOut={}\nuse_custom_render={}\ngl_major={}\ngl_minor={}\nnoise={}\nuse_low_textures={}\nuse_low_audio_quality={}\nwidth={}\nheight={}\npreset={}\nxor64random={}\nfullscreen={}\nproxy_host={}\nproxy_port={}", PahomEngine->CVsync, PahomEngine->bIsRandomEngineUsed, PahomEngine->audio.masterVolume, PahomEngine->i64CPUDelay, SettingsUI->bWindowAppBlur, PahomEngine->keyMap.u8FORWARD, PahomEngine->keyMap.u8BACK, PahomEngine->PESettings->bFlagEnableAnimationToImageFadeInOut, PahomEngine->Render->bIsUsedGLCustom, PahomEngine->Render->gl_ver.major, PahomEngine->Render->gl_ver.minor, PahomEngine->Event.bShowNoiseBackground, PahomEngine->assets.bUseLowTextures, PahomEngine->audio.bUseLowAudioQuality, PahomEngine->i64WindowSize[0], PahomEngine->i64WindowSize[1], PahomEngine->iReloadTexturesPresetID, PahomEngine->math->bIsRandomXor64, PahomEngine->bFullscreen,PahomEngine->lnethttp->sProxyHost,PahomEngine->lnethttp->iProxyPort));
							PahomEngine->log("PESettings сохранено!\n" + std::format("\tvsync={}\nrandom_engine={}\nvolume={}\ncpu_delay={}\nrender_blur={}\nbind_key_right={}\nbind_key_left={}\nEnableAnimationToImageFadeInOut={}\nuse_custom_render={}\ngl_major={}\ngl_minor={}\nnoise={}\nuse_low_textures={}\nuse_low_audio_quality={}\nwidth={}\nheight={}\npreset={}", PahomEngine->CVsync, PahomEngine->bIsRandomEngineUsed, PahomEngine->audio.masterVolume, PahomEngine->i64CPUDelay, SettingsUI->bWindowAppBlur, PahomEngine->keyMap.u8FORWARD, PahomEngine->keyMap.u8BACK, PahomEngine->PESettings->bFlagEnableAnimationToImageFadeInOut, PahomEngine->Render->bIsUsedGLCustom, PahomEngine->Render->gl_ver.major, PahomEngine->Render->gl_ver.minor, PahomEngine->Event.bShowNoiseBackground, PahomEngine->assets.bUseLowTextures, PahomEngine->audio.bUseLowAudioQuality, PahomEngine->i64WindowSize[0], PahomEngine->i64WindowSize[1], PahomEngine->iReloadTexturesPresetID));
							PahomEngine->bSettings = false;
							update_settings();
							ImGui::CloseCurrentPopup();
						}ImGui::SameLine();
						if (ImGui::Button("Загрузить", ImVec2(200, 40))) {
							PahomEngine->PESettings->ParseConfig();
						}
						ImGui::PopStyleVar();
						ImGui::PopFont();
						ImGui::EndPopup();
					}
					static float frctMainButton = 0;
					PahomEngine->setItemCenterX(PahomEngine->i64WindowSize[1] == 480 ? 310 : 410);
					ImGui::PushFont(PahomEngine->i64WindowSize[1] == 480 ? font3 : font20);
					if (ImGui::Button("Продолжить (A)", ImVec2(PahomEngine->i64WindowSize[1] == 480 ? 150 : 200, PahomEngine->i64WindowSize[1] == 480 ? 20 : 30))) {
						PahomEngine->bLoadingFrame = false;
						bMainTrackStarted = true;
						PahomEngine->strings.log("loading frame false", "Engine");
						bMainIntroAudioLoaded = false;

					}
					frctMainButton = ImGui::GetCursorPosY();
					ImGui::SameLine();
					if (ImGui::Button("Опции", ImVec2(PahomEngine->i64WindowSize[1] == 480 ? 150 : 200, PahomEngine->i64WindowSize[1] == 480 ? 20 : 30))) {
						PahomEngine->bSettings = true;
						ImGui::OpenPopup("settings_engine");
					}
					 
					PahomEngine->setItemCenterX(PahomEngine->i64WindowSize[1] == 480 ? 150 : 400);
					if (ImGui::Button("Редактор ресурсов", ImVec2(PahomEngine->i64WindowSize[1] == 480 ? 150 : 400, PahomEngine->i64WindowSize[1] == 480 ? 20 : 30))) {
						//PahomEngine->Event.bTextureEditor = !PahomEngine->Event.bTextureEditor;
						ImGui::OpenPopup("Редактор");
					}  
					ImGui::PopFont();  
					PahomEngine->bControlsIsKeyboard = true;
					PahomEngine->bControlsIsGamepad = true;
					if (PahomEngine->GetGamepadKey(PahomEngine->keyMap.i64AGamepad, 0) && !SettingsUI->bGamepadTestUI) {
						PahomEngine->bLoadingFrame = false;
						PahomEngine->bControlsIsGamepad = true;
						bMainIntroAudioLoaded = false;
					}
					if (GetAsyncKeyState(VK_RETURN)) {
						PahomEngine->bLoadingFrame = false;
						bMainIntroAudioLoaded = false;
						// PahomEngine->bStartGame    = true;
					} 

				}
			}
		}

		//
		else {
			//
		   // 
		   
			// loader logo
			if (!PahomEngine->bStartGame)
			{
				if (PahomEngine->Exceptions->ErrorTextures) {
					ImGui::SetCursorPos(
						ImVec2{
							20,20
						}
					);
					ImGui::TextColored(PahomEngine->RGBA(255, 0, 60, 255), "PahomEngine: %s stack=%p", PahomEngine->Exceptions->sLastError.data(), PahomEngine->Exceptions->pLastStack);
				}
				Game->logo(); 
			}  
			
			// end loader logo
			else {
				// debug settings
				//  Game->debug_offset();
				 // input events
				Game->Controls();
				ImGui::SetCursorPos(ImVec2(0, 0));
				PahomEngine->cast->bUsedStaticCast = true;
				static float fBackgroundSizeScale = 0;
				static float i32BackgroundSizeX = PahomEngine->cast->cast_all<float>(PahomEngine->i64WindowSize[0]) * fBackgroundSizeScale;
				static float i32BackgroundSizeY = PahomEngine->cast->cast_all<float>(PahomEngine->i64WindowSize[1]) * fBackgroundSizeScale;
				if (PahomEngine->Event.AutoScaleImage) {
					// std::cout << std::format("fBackgroundSizeScale: {} \n", fBackgroundSizeScale);
					i32BackgroundSizeX = 0;
					i32BackgroundSizeY = 0;
				}
				// 
				else {
					i32BackgroundSizeX = PahomEngine->cast->cast_all<float>(PahomEngine->i64WindowSize[0]);
					i32BackgroundSizeY = PahomEngine->cast->cast_all<float>(PahomEngine->i64WindowSize[1]);
				}

				ImGui::Image(PahomEngine->ptrint64_t(PahomEngine->ImageData.TextureArray[1]), ImVec2(i32BackgroundSizeX, i32BackgroundSizeY));
				// 
				static int64_t i64RandSeed = 0;
				if (PahomEngine->bStartGameFlag && PahomEngine->bStartGame) {
					ImGui::SetCursorPos(ImVec2{
						PahomEngine->fBreadPosX,
						PahomEngine->fBreadPosY
						});
					// movement game obj
					Game->randomEvents();
					// game over flag
					if (PahomEngine->bGameOver) {
						if (bGameCount) { 
							i32GameOverCount++;
							bGameCount = false;
						}
						if (PahomEngine->bKefir && PahomEngine->Event.bEnableKefir) {

							PahomEngine->Event.bScreamEventBackground = true;

						}
						else {


							PahomEngine->Event.mt_fill(PahomEngine->i64WindowSize[0], PahomEngine->i64WindowSize[1]);
							if (PahomEngine->Event.bShowNoiseBackground)
							{
								PahomEngine->ogl->Overlay(PahomEngine->i64WindowSize[0], PahomEngine->i64WindowSize[1]);
							} 
							PahomEngine->Event.isTextHidden = false;
							ImGui::PushFont(font50);
							ImGui::SetCursorPos(
								ImVec2( 
									(PahomEngine->i64WindowSize[0] - ImGui::CalcTextSize("Пидорас обосрался!!").x) / 2,
									(PahomEngine->i64WindowSize[1] - ImGui::CalcTextSize("Пидорас обосрался!!").y - 200) / 2
								) 
							);
							ImGui::Text("Пидорас обосрался!!");
							PahomEngine->setTextCenter(std::format("{} раз(а)", i32GameOverCount).data());
							static ImVec4 rgba_rand;
							static int32_t rgba_delay = 0;
							rgba_delay++;
							if (rgba_delay > 10) {
								rgba_rand = PahomEngine->RGBA(PahomEngine->math->random<float>(255.0f), PahomEngine->math->random<float>(255.0f), PahomEngine->math->random<float>(255.0f), 255);
								//   std::cout << std::format(" {} {} {} ", rgba_rand.x, rgba_rand.y, rgba_rand.z) << "\n";
								rgba_delay = 0;  
							}    
							ImGui::TextColored(rgba_rand, "%d раз(а)", i32GameOverCount);
							ImGui::PopFont(); 

							ImGui::SetCursorPosX(
								(PahomEngine->i64WindowSize[0] - 150) / 2
							);
							  
							if (ImGui::Button("Заново\n(A)(ENTER)", ImVec2(150, 60))) {
								PahomEngine->bGameOver = false;
								PahomEngine->fScoreCount = 0;
								PahomEngine->audio.play2(1);
								// PahomEngine->audio.cleanup(PahomEngine->audio.audioDevice);
								// PahomEngine->audio.cleanup(PahomEngine->audio.audioDevice2);
								PahomEngine->audio.cleanup(PahomEngine->audio.audioDevice3);
								ImGui::CloseCurrentPopup();
								bGameCount = true;
							}
							if (PahomEngine->GetGamepadKey(PahomEngine->keyMap.i64AGamepad, 0)) {
								PahomEngine->bGameOver = false;
								PahomEngine->fScoreCount = 0;
								PahomEngine->audio.play2(1);
								PahomEngine->audio.cleanup(PahomEngine->audio.audioDevice);
								// PahomEngine->audio.cleanup(PahomEngine->audio.audioDevice2);
								PahomEngine->audio.cleanup(PahomEngine->audio.audioDevice3);
								ImGui::CloseCurrentPopup();
								bGameCount = true; 
							}
							if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
								Sleep(10); 
								PahomEngine->bGameOver = false;
								PahomEngine->fScoreCount = 0;
								PahomEngine->audio.play2(1); 
								// PahomEngine->audio.cleanup(PahomEngine->audio.audioDevice);
								 // PahomEngine->audio.cleanup(PahomEngine->audio.audioDevice2);
								PahomEngine->audio.cleanup(PahomEngine->audio.audioDevice3);
								ImGui::CloseCurrentPopup();
								bGameCount = true;
							}
						}
					}
				}
				else {
					PahomEngine->Event.mt_clear();   
				}
				// main menu game

				static int32_t i32Item = 0; static float fInputDelay = 0;
				if (!PahomEngine->bStartGameFlag) {
					if (bMainTrackStarted) {

						PahomEngine->log(std::format("audio call 3 ->{}", PahomEngine->audio.audiolist[10]));
						PahomEngine->audio.play3(10);
						if (PahomEngine->audio.audioDevice) {
							PahomEngine->audio.Device(0, false);
							PahomEngine->audio.cleanup(PahomEngine->audio.audioDevice);
							//PahomEngine->audio.cleanup(PahomEngine->audio.audioDevice2);
						   // PahomEngine->audio.cleanup(PahomEngine->audio.audioDevice3);
						}
						if (PahomEngine->audio.is_ptr_valid(PahomEngine->audio.audioDevice2)) {
							PahomEngine->audio.cleanup(PahomEngine->audio.audioDevice2);
						}
						bMainTrackStarted = false;
					}
					ImGui::SetCursorPos(
						ImVec2(
							(PahomEngine->i64WindowSize[0] - 128) / 2,
							(PahomEngine->i64WindowSize[1] - 600) / 2
						)
					);

					/*ImGui::Image(PahomEngine->ptrint64_t(PahomEngine->ImageData.TextureArray[2]), ImVec2(128, 128));*/
					PahomEngine->img->CreateImage(PahomEngine->ImageData.TextureArray[2],
						backend->io.DeltaTime,
						ImVec2{ PahomEngine->cast->cast_all<float>(PahomEngine->i64WindowSize[0]),PahomEngine->cast->cast_all<float>(PahomEngine->i64WindowSize[1])
						}, ImVec2(256, 256));
					ImGui::PushFont((PahomEngine->i64WindowSize[1] == 480 ? font20 : font54));
					ImGui::SetCursorPos(
						ImVec2(
							(PahomEngine->i64WindowSize[0] - ImGui::CalcTextSize("SWEET BREADS").x) / 2,
							(PahomEngine->i64WindowSize[1] - ImGui::CalcTextSize("SWEET BREADS").y - 200) / 2
						)
					);
					 
					ImGui::TextColored(PahomEngine->RGBA(7, 7, 7, 255), "SWEET BREADS)");
					ImGui::PopFont();
					ImGui::PushFont((PahomEngine->i64WindowSize[1] == 480 ? font20 : font50));
					ImGui::SetCursorPos(
						ImVec2(
							(PahomEngine->i64WindowSize[0] - ImGui::CalcTextSize("SWEET BREADS").x) / 2,
							(PahomEngine->i64WindowSize[1] - ImGui::CalcTextSize("SWEET BREADS").y - 200) / 2
						)
					);
					Game->ColoredLogo(); 
					Game->LiderTableNet(font15, font14iv); 
					//

					if (PahomEngine->GetGamepadKey(XINPUT_GAMEPAD_DPAD_UP, 0) || PahomEngine->getPressedKey('W', true)) {
						fInputDelay += (40 * backend->io.DeltaTime);
						if (fInputDelay >= 5.0f) {
							PahomEngine->audio.play(11);
							i32Item -= (1);
							fInputDelay = 0;
						}
						if (i32Item < 0) {
							i32Item = 3;
						}
					}
					if (PahomEngine->GetGamepadKey(XINPUT_GAMEPAD_DPAD_DOWN, 0) || PahomEngine->getPressedKey('S', true)) {
						fInputDelay += (40 * backend->io.DeltaTime);
						if (fInputDelay >= 5.0f) {
							PahomEngine->audio.play(11);
							i32Item += (1);
							fInputDelay = 0;
						}
						if (i32Item > 5) {
							i32Item = 0;
						}
					}
					ImGui::PopFont();
					ImGui::PushFont((PahomEngine->i64WindowSize[1] == 480 ? font3 : font20));
					PahomEngine->setTextCenter(("PahomEngineGL(AMD64) build :" + PahomEngine->sBuild).data());
					ImGui::Text(("PahomEngineGL(AMD64) build :" + PahomEngine->sBuild).data());
					PahomEngine->setTextCenter("Написано на С++20 (MSVC)");
					ImGui::Text("Написано на С++20 (MSVC)");
					PahomEngine->setTextCenter("BY HCPP");
					ImGui::Text("BY HCPP");
					ImGui::PopFont();
					Game->breadObjectToMenu(i32Item == 0); 
					if (PahomEngine->UI->CustomButton(ImVec4(100, 171, 101, 255), ImVec4(7, 7, 7, 255), ImVec4(255, 255, 255, 255), 20, ImVec2(0, 0), "Играть (ENTER / (A))", font3, ImVec2(280, 40))) {
						Sleep(10);
						bMainTrackStarted = false;
						if (PahomEngine->audio.audioDevice3) {
							PahomEngine->audio.audioDevice3.pause();  
						}
						PahomEngine->bGameOver = false;
						PahomEngine->bStartGameFlag = true;
						PahomEngine->audio.play2(1);
						bGameCount = true;
						PahomEngine->log(std::format("PahomEngine->bGameOver={}\nPahomEngine->bStartGameFlag={}\nbGameCount={}", PahomEngine->bGameOver, PahomEngine->bStartGameFlag, bGameCount));
					}
					if (PahomEngine->bStartGameFlag) {
						PahomEngine->log("Game Started");
						//PahomEngine->log(std::format("PahomEngine->bGameOver={}\nPahomEngine->bStartGameFlag={}\nbGameCount={}", PahomEngine->bGameOver, PahomEngine->bStartGameFlag, bGameCount));
					}
					if (ImGui::IsItemHovered()) { i32Item = 0;  Game->ClickItem(); Game->ResetAudioClickItem(); }
					PahomEngine->selectedItem(i32Item == 0);
					if (PahomEngine->GetGamepadKey(PahomEngine->keyMap.i64AGamepad, 0) || ImGui::IsKeyDown(ImGuiKey_Enter)) {
						Sleep(10);
						switch (i32Item) {
						case 0:
							//bMainTrackStarted = false;
							if (PahomEngine->audio.audioDevice3) {
								PahomEngine->audio.audioDevice3.pause();
							}
							PahomEngine->bGameOver = false;
							PahomEngine->bStartGameFlag = true;
							PahomEngine->audio.play2(1);
							bGameCount = true;
							break;
						case 1:
							std::system("start https://donationalerts.com/r/hcpp");
							break;
						case 2:
							ImGui::OpenPopup("Сложность");
							break;
						case 3:
							ImGui::OpenPopup("##lidertable");
							break;
						case 4:
							PahomEngine->bLoadingFrame = true;
							PahomEngine->bStartGame = false;
							break;
						case 5:  
							bExitWindow = true;   
							Game->bExitWindowClosed = false;
							break;
						}
					} 
					Game->breadObjectToMenu(i32Item == 1);
					if (PahomEngine->UI->CustomButton(ImVec4(100, 171, 101, 255), ImVec4(7, 7, 7, 255), ImVec4(255, 255, 255, 255), 20, ImVec2(0, 0), "Донат", font3, ImVec2(280, 40))) {
						std::system("start https://donationalerts.com/r/hcpp");
					}
					if (ImGui::IsItemHovered()) { i32Item = 1;  Game->ClickItem(); Game->ResetAudioClickItem();
					}
					PahomEngine->selectedItem(i32Item == 1);
					int64_t i64diffImageId = 0;
					static std::string_view sInfo;
					if (ImGui::BeginPopupModal("Сложность", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
						//static int32_t i32DiffOffsetPahomStep = 0;
						static int32_t i32DiffOffsetBreadStep = 0;
						//PahomEngine->pDiff->setDiff(PahomEngine->pDiff->i64id);
						static bool bButtonL = false, bButtonR = false;
						static ImVec4 colorText = {};
						switch (PahomEngine->pDiff->i64id)
						{
						case 0:
							i64diffImageId = 6;
							sInfo = "Медленная скорость";
							colorText = { 0,255,133,255 };

							break;
						case 1:
							i64diffImageId = 2;
							sInfo = "Чуть быстрее скорость";
							colorText = { 255,155,33,255 };
							break;
						case 2:
							i64diffImageId = 7;
							sInfo = "Поехавшая скорость\nС такой скорость братишка чистит вилкой";
							colorText = { 255,0,133,255 };
							break;
						}
						ImGui::SetCursorPos({ 40,30 });
						switch (PahomEngine->pDiff->i64id) {
						case 0:
							ImGui::Image(PahomEngine->ptrint64_t(PahomEngine->ImageData.TextureArray[BREAD_IMAGE]), { 65,65 });
							break;
						case 1:
							ImGui::Image(PahomEngine->ptrint64_t(PahomEngine->ImageData.TextureArray[BREAD_IMAGE]), { 65,65 });
							ImGui::SameLine(); ImGui::Image(PahomEngine->ptrint64_t(PahomEngine->ImageData.TextureArray[BREAD_IMAGE]), { 65,65 });
							break;
						case 2:
							ImGui::Image(PahomEngine->ptrint64_t(PahomEngine->ImageData.TextureArray[BREAD_IMAGE]), { 65,65 });
							ImGui::SameLine(); ImGui::Image(PahomEngine->ptrint64_t(PahomEngine->ImageData.TextureArray[BREAD_IMAGE]), { 65,65 });
							ImGui::SameLine(); ImGui::Image(PahomEngine->ptrint64_t(PahomEngine->ImageData.TextureArray[BREAD_IMAGE]), { 65,65 });
							break;
						}
						ImGui::SliderInt64("Сложность", &PahomEngine->pDiff->i64id, 0, 2, "%lld", ImGuiSliderFlags_NoInput);
						if (PahomEngine->pDiff->i64id >= 2) { PahomEngine->pDiff->i64id = 2; }
						PahomEngine->selectedItem(bButtonR);
						ImGui::Text("%s", std::format("{} P:{} B:{}", PahomEngine->pDiff->diffSelected, PahomEngine->pDiff->diffArray[PahomEngine->pDiff->i64id].i64buffer, PahomEngine->pDiff->diffArray[PahomEngine->pDiff->i64id].i64buffer1).data());
						PahomEngine->TextColored(PahomEngine->RGBA(colorText), "{}", sInfo.data());
						ImGui::Separator();
						ImGui::CustomToggle("Рандомный счет", &PahomEngine->pDiff->bRandScoreDiff);
						ImGui::CustomToggle("Нарастающая сложность", &PahomEngine->pDiff->bRandDiff);
						ImGui::CustomToggle("DemoPlay", &PahomEngine->Event.bDemoPlay);
						ImGui::CustomToggle("Кефир", &PahomEngine->Event.bEnableKefir);

						if (ImGui::CustomToggle("AutoScaleImage", &PahomEngine->Event.AutoScaleImage)) {
							std::cout << std::format("AutoScaleImage: {} \n", PahomEngine->Event.AutoScaleImage);
						}
						static bool bButtonB = false;

						if (ImGui::Button("OK (B/SPACE)", ImVec2(150, 30)) || PahomEngine->GetGamepadKey(XINPUT_GAMEPAD_B, 0) || PahomEngine->getPressedKey(32, true)) {
							bButtonB = true;
							PahomEngine->UI->Message(PahomEngine->pDiff->diffSelected, ImGui::CalcTextSize(PahomEngine->pDiff->diffSelected.data()), 10, 500L, &bButtonB);
							std::cout << (std::format(" [PahomEngine->Diffinity] set diff {} step:{} stepMove:{}\n", PahomEngine->pDiff->diffSelected, PahomEngine->pDiff->diffArray[PahomEngine->pDiff->i64id].i64buffer, PahomEngine->pDiff->diffArray[PahomEngine->pDiff->i64id].i64buffer1));
							DiffinitySetAdaptive();
							PahomEngine->pDiff->setDiff(PahomEngine->pDiff->i64id);

							ImGui::CloseCurrentPopup();
						}
						else {
							bButtonB = false;
						}
						ImGui::EndPopup();
					} 
					Game->breadObjectToMenu(i32Item == 2); 
					if (PahomEngine->UI->CustomButton(ImVec4(100, 171, 101, 255), ImVec4(7, 7, 7, 255), ImVec4(255, 255, 255, 255), 20, ImVec2(0, 0), "Сложность", font3, ImVec2(280, 40))) {
						ImGui::OpenPopup("Сложность");
					}
					if (ImGui::IsItemHovered()) {
						i32Item = 2;  Game->ClickItem();  Game->ResetAudioClickItem();
					}
					PahomEngine->selectedItem(i32Item == 2);
					Game->breadObjectToMenu(i32Item == 3);
					if (PahomEngine->UI->CustomButton(ImVec4(100, 171, 101, 255), ImVec4(7, 7, 7, 255), ImVec4(255, 255, 255, 255), 20, ImVec2(0, 0), "Таблица лидеров", font3, ImVec2(280, 40))) {
						ImGui::OpenPopup("##lidertable");
					}
					if (ImGui::IsItemHovered()) { i32Item = 3;  Game->ClickItem(); Game->ResetAudioClickItem();
					}
					PahomEngine->selectedItem(i32Item == 3);
					Game->breadObjectToMenu(i32Item == 4);
					if (ImGui::IsItemHovered()) { i32Item = 3; }
					if (PahomEngine->UI->CustomButton(ImVec4(100, 171, 101, 255), ImVec4(7, 7, 7, 255), ImVec4(255, 255, 255, 255), 20, ImVec2(0, 0), "На главную", font3, ImVec2(280, 40))) {
						PahomEngine->bLoadingFrame = true;
						PahomEngine->bStartGame = false;
						bMainIntroAudioLoaded = false;
					}
					if (ImGui::IsItemHovered()) { i32Item = 4;  Game->ClickItem(); Game->ResetAudioClickItem();
					}
					PahomEngine->selectedItem(i32Item == 4);
					Game->breadObjectToMenu(i32Item == 5);
					if (PahomEngine->UI->CustomButton(ImVec4(100, 171, 101, 255), ImVec4(7, 7, 7, 255), ImVec4(255, 255, 255, 255), 20, ImVec2(0, 0), "Выйти на Рабочий Стол", font3, ImVec2(280, 40))) {
						backend->render = false;
					}
					PahomEngine->selectedItem(i32Item == 5);
					ImGui::PushFont(font27);
					PahomEngine->setItemCenterX(ImGui::CalcTextSize("Вверх - ").x
						+ ImGui::CalcTextSize("Вниз - ").x
						+ ImGui::CalcTextSize("OK - ").x
						+ ImGui::CalcTextSize("U/W").x
						+ ImGui::CalcTextSize("D/S").x
						+ ImGui::CalcTextSize("(A) / ENTER").x
					);
					PahomEngine->TextColored(PahomEngine->RGBA(227, 227, 227, 255), "Вверх - "); ImGui::SameLine(); PahomEngine->TextColored(PahomEngine->RGBA(227, 227, 227, 255), "U/W");
					ImGui::SameLine();
					PahomEngine->TextColored(PahomEngine->RGBA(227, 227, 227, 255), "Вниз - "); ImGui::SameLine();  PahomEngine->TextColored(PahomEngine->RGBA(227, 227, 227, 255), "D/S");
					ImGui::SameLine();
					PahomEngine->TextColored(PahomEngine->RGBA(227, 227, 227, 255), "OK - "); ImGui::SameLine();  PahomEngine->TextColored(PahomEngine->RGBA(227, 227, 227, 255), "(A) / ENTER");
					ImGui::PopFont();



				}
				else {

					// bMainTrackStarted = false;
					if (PahomEngine->Event.bScreamEventBackground) {
						Game->ScremerEvent();
					}
					if (!PahomEngine->Event.bScreamEventBackground) {
						if (PahomEngine->pDiff->bRandDiff) {
							PahomEngine->fStepMove = PahomEngine->randfloat(PahomEngine->pDiff->diffArray[PahomEngine->rand64(2)].i64buffer);
							// PahomEngine->fStep = PahomEngine->randfloat(PahomEngine->pDiff->diffArray[PahomEngine->rand64(2)].i64buffer1);
						}
						if (PahomEngine->i64RandBoost == i64RandSeed) {
							PahomEngine->bBoost777 = true;
							if (!u8audioPlay)
							{
								PahomEngine->audio.play3(12);
								u8audioPlay = 2;
							}

						}
						if (PahomEngine->fScoreCount > 666 && PahomEngine->fScoreCount <= 680) {
							PahomEngine->audio.play3(6);
						}
						if (PahomEngine->fScoreCount > 500 && PahomEngine->fScoreCount <= 520) {
							static bool bAudio0 = true;
							if (bAudio0) {
								PahomEngine->audio.play2(8);
								bAudio0 = false;
							}
							PahomEngine->Text(PahomEngine->RGBA(255, 255, 255, 255), "Хули ты говном вымазался");
							PahomEngine->Event.TimerToClear();
						}

						if (PahomEngine->fScoreCount > 1000 && PahomEngine->fScoreCount <= 1200) {
							static bool bAudio1 = true;
							if (bAudio1) {
								PahomEngine->audio.play2(9);
								bAudio1 = false;
							}
							PahomEngine->Text(PahomEngine->RGBA(255, 255, 255, 255), "Ты понимаешь что\n ты поехавший?");
							PahomEngine->Event.TimerToClear();
						}

						//
						/*if (PahomEngine->getPressedKey('B', false) && PahomEngine->getPressedKey('R', false)) {
							Game->bBreadRain = true;
							Game->EventBreadRain();
						}*/
						PahomEngine->fMaxPahomPosX = PahomEngine->cast->cast_all<float>(PahomEngine->i64WindowSize[0]) - 128.0f;
						PahomEngine->fMaxPahomPosY = PahomEngine->cast->cast_all<float>(PahomEngine->i64WindowSize[1]) - 128.0f;
						if (!PahomEngine->Event.bScreamEventBackground) {

							if (!PahomEngine->bKefir)
							{

								ImGui::Image(PahomEngine->ptrint64_t(PahomEngine->ImageData.TextureArray[PahomEngine->bBoost777 ? (6) : 2]), PahomEngine->bBoost777 ? ImVec2(PahomEngine->cast->unicast<int64_t, float>(PahomEngine->i64BreadSize[0] / 2), PahomEngine->cast->unicast<int64_t, float>(PahomEngine->i64BreadSize[0])) : ImVec2(PahomEngine->cast->unicast<int64_t, float>(PahomEngine->i64BreadSize[0]), PahomEngine->cast->unicast<int64_t, float>(PahomEngine->i64BreadSize[0])));
								PahomEngine->selectedItemCol(PahomEngine->bIsEnableHitbox, PahomEngine->bIsCollisionOK);
							}
							else {
								if (!PahomEngine->Event.bKefirFriend)
								{
									PahomEngine->Text(PahomEngine->RGBA(255, 255, 255, 255), "Кефир просрочен!! Не лови его");

								}
								PahomEngine->Event.TimerToClear();
								static  int64_t i64TimerKefir = 0, i64TimerMinKefir = 0;
								i64TimerMinKefir++;
								if (i64TimerMinKefir > 60) {
									i64TimerKefir++;
									if (i64TimerKefir > 10) {
										PahomEngine->bKefir = false;
										PahomEngine->reloadBreadPos();
										i64TimerKefir = 0;
									}
									i64TimerMinKefir = 0;

								}

								// ImGui::Image(PahomEngine->ptrint64_t(PahomEngine->ImageData.TextureArray[PahomEngine->bBoost777 ? (PahomEngine->bKefir ? 8 : 6) : 8]), PahomEngine->bBoost777 ? ImVec2(32, 64) : ImVec2(64, 64));
								Game->KefirEvent(PahomEngine->bBoost777 ? (PahomEngine->bKefir ? 8 : 6) : 8);
								PahomEngine->selectedItemCol(PahomEngine->bIsEnableHitbox, PahomEngine->bIsCollisionOK);
							}
						}
						ImGui::SetCursorPos(ImVec2{
						 PahomEngine->fPahomPosX,
						 PahomEngine->fPahomPosY
							});

						//  std::cout<<" (PahomEngine) (debug_gl) framerate:" << io.Framerate << " DeltaTime: " << PahomEngine->fDeltaTime << std::endl;
						//PahomEngine->i64PahomSize[0] = 128 * (PahomEngine->bFullscreen ? 2 : 1);
						//PahomEngine->i64PahomSize[1] = 128 * (PahomEngine->bFullscreen ? 2 : 1);
					   // PahomEngine->i64BreadSize[0] = 64 * (PahomEngine->bFullscreen ? 2 : 1);
					   // PahomEngine->i64BreadSize[1] = 64 * (PahomEngine->bFullscreen ? 2 : 1);
						if (Game->bAntiGravitation) {
							ImGui::ImageRotated(PahomEngine->ptrint64_t(PahomEngine->ImageData.TextureArray[!PahomEngine->bIsRevesed ? 4 : 3]), ImVec2(PahomEngine->i64PahomSize[0], PahomEngine->i64PahomSize[1]), 180, { 0,0 }, { 1,1 }, { 1,1,1,1 }, { 0,0,0,0 });
						}
						else {
							ImGui::Image(PahomEngine->ptrint64_t(PahomEngine->ImageData.TextureArray[!PahomEngine->bIsRevesed ? 4 : 3]), ImVec2(PahomEngine->i64PahomSize[0], PahomEngine->i64PahomSize[1]));
							PahomEngine->selectedItemCol(PahomEngine->bIsEnableHitbox, PahomEngine->bIsCollisionOK);
						}

						// ImGui::Image(PahomEngine->ptrint64_t(PahomEngine->ImageData.TextureArray[!PahomEngine->bIsRevesed ? 4 : 3]), ImVec2(PahomEngine->i64PahomSize[0], PahomEngine->i64PahomSize[1]));
						ImGui::SetCursorPosY(10); ImGui::SetCursorPosX(10);
						static int32_t i32DebugTextCounter = 0;
						if (ImGui::Button("/", ImVec2(32, 32))) {
							i32DebugTextCounter++;
							if (i32DebugTextCounter > 1) {
								i32DebugTextCounter = 0;
							}
							PahomEngine->bDebugText = static_cast<bool>(i32DebugTextCounter ? true : false);
						}
						Game->MuteButton();
						if (GetAsyncKeyState(PahomEngine->keyMap.u8RESET)) {
							PahomEngine->clearPos();
						}
						if (PahomEngine->getPressedKey_v2(ImGuiKey_Space) || PahomEngine->GetGamepadKey(PahomEngine->keyMap.i64START, 0)) {
							Sleep(10);
							PahomEngine->bStartGameFlag = false;
							bMainTrackStarted = true;

							// PahomEngine->audio.setReplay(true);
						}


						//

						ImGui::SetCursorPosY(60);
						ImGui::PushFont(font54);
						ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(std::format("{}", PahomEngine->fScoreCount).data()).x + 10) / 2);
						ImGui::TextColored(PahomEngine->RGBA(7, 7, 7, 255), "%.0f", PahomEngine->fScoreCount);
						ImGui::PopFont();
						ImGui::SetCursorPosY(60);
						ImGui::PushFont(font50);
						if (!PahomEngine->bBoost777) {
							ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(std::format("{}", PahomEngine->fScoreCount).data()).x) / 2);
							ImGui::TextColored(PahomEngine->RGBA(255, 255, 255, 255), "%.0f", PahomEngine->fScoreCount);
							PahomEngine->setTextCenterXY(PahomEngine->Event.TextBufferStr.data());
							PahomEngine->Event.i64TimerEvent++;
							if (PahomEngine->Event.i64TimerEvent < 100) {
								if (!PahomEngine->Event.isTextHidden && !PahomEngine->bBoost777 || !PahomEngine->Event.isTextHidden && !PahomEngine->bKefir)
								{
									ImGui::TextColored(PahomEngine->RGBA(255, 255, 255, 255), "%s", PahomEngine->Event.TextBufferStr.data());
								}

							}
							if (PahomEngine->Event.i64TimerEvent > 100) {
								PahomEngine->Event.clearEvent();
								PahomEngine->Event.i64TimerEvent = 0;
							}
							//PahomEngine->Tbuffer();
						}
						else {

							static  int64_t i64Timer = 0, i64TimerMin = 0;
							i64Timer++;
							if (i64Timer > 60) {
								i64TimerMin++;//

								if (i64TimerMin > 10) {
									PahomEngine->bBoost777 = false;
									PahomEngine->reloadBreadPos();
									i64TimerMin = 0;
								}
								i64Timer = 0;

							}

							ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(std::format("{}", PahomEngine->fScoreCount).data()).x) / 2);
							ImGui::TextColored(PahomEngine->RGBA(255, 255, 255, 255), "%.0f", PahomEngine->fScoreCount);
							ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(std::format("{}:{}", i64TimerMin, i64Timer).data()).x) / 2);
							ImGui::TextColored(PahomEngine->RGBA(0, 255, 60, 255), "%lld:%lld", i64TimerMin, i64Timer);
							PahomEngine->setTextCenterXY(PahomEngine->Event.TextBufferStr.data());
							PahomEngine->Event.setColorText(PahomEngine->RGBA(255, 255, 255, 255));
							ImGui::TextColored(PahomEngine->Event.col, "%s", PahomEngine->Event.TextBufferStr.data());
						}//
						ImGui::PopFont();

						if (PahomEngine->Event.bDemoPlay) {

							float speed = 0.1f;
							PahomEngine->fPahomPosX = std::lerp(
								PahomEngine->fPahomPosX,
								PahomEngine->fBreadPosX + (PahomEngine->bKefir ? 150.0f : 0.0f),
								speed
							);
							float abs_pahom = std::abs(PahomEngine->fPahomPosX - PahomEngine->fBreadPosX);
							if (PahomEngine->fBreadPosX > (PahomEngine->cast->unicast<int64_t, float>(PahomEngine->i64WindowSize[0] - PahomEngine->i64PahomSize[0]))) {
								PahomEngine->fBreadPosX = (PahomEngine->cast->unicast<int64_t, float>(PahomEngine->i64WindowSize[0] - PahomEngine->i64PahomSize[0]));
							}
							else if (PahomEngine->fBreadPosX < (PahomEngine->cast->unicast<int64_t, float>(PahomEngine->i64PahomSize[0]))) {
								PahomEngine->fBreadPosX = (PahomEngine->cast->unicast<int64_t, float>(PahomEngine->i64PahomSize[0]));
							}
							if (abs_pahom > 100) {
								PahomEngine->bIsRevesed = true;  // Пахом отстаёт
							}
							else {
								PahomEngine->bIsRevesed = false; // Идёт вплотную
							}

						}

						ImGui::SetCursorPosY(15); ImGui::SetCursorPosX(50);
						ImGui::TextColored(PahomEngine->RGBA(255, 255, 255, 255), "Сложность: %s", PahomEngine->pDiff->diffSelected.data());
						ImGui::SameLine();

						switch (PahomEngine->pDiff->i64id) {
						case 0:
							ImGui::Image(PahomEngine->ptrint64_t(PahomEngine->ImageData.TextureArray[BREAD_IMAGE]), { 24,24 });
							break;
						case 1:
							ImGui::Image(PahomEngine->ptrint64_t(PahomEngine->ImageData.TextureArray[BREAD_IMAGE]), { 24,24 });
							ImGui::SameLine(); ImGui::Image(PahomEngine->ptrint64_t(PahomEngine->ImageData.TextureArray[BREAD_IMAGE]), { 24,24 });
							break;
						case 2:
							ImGui::Image(PahomEngine->ptrint64_t(PahomEngine->ImageData.TextureArray[BREAD_IMAGE]), { 24,24 });
							ImGui::SameLine(); ImGui::Image(PahomEngine->ptrint64_t(PahomEngine->ImageData.TextureArray[BREAD_IMAGE]), { 24,24 });
							ImGui::SameLine(); ImGui::Image(PahomEngine->ptrint64_t(PahomEngine->ImageData.TextureArray[BREAD_IMAGE]), { 24,24 });
							break;
						}
						//  PahomEngine->bDebugText = (GetAsyncKeyState('G') && GetAsyncKeyState('A') && GetAsyncKeyState('Y')) ? true : false;

						ImGui::SetCursorPosY(PahomEngine->i64WindowSize[1] - 70);
						if (PahomEngine->ptrGamepad1->IsConnected()) {
							ImGui::SetCursorPosY(PahomEngine->i64WindowSize[1] - 120);
							PahomEngine->setItemCenterX((ImGui::CalcTextSize("Влево - ").x + ImGui::CalcTextSize("Вправо - ").x + ImGui::CalcTextSize("Пауза - ").x + (32 * 3 + 5)));
							ImGui::Text("Влево - "); ImGui::SameLine(); PahomEngine->GamepadUI->GamepadButtonRender("L", PahomEngine->RGBA(135, 135, 135, 255), PahomEngine->GamepadUI->GButtons->bGButtonL);
							ImGui::SameLine();
							ImGui::Text("Вправо - "); ImGui::SameLine(); PahomEngine->GamepadUI->GamepadButtonRender("R", PahomEngine->RGBA(135, 135, 135, 255), PahomEngine->GamepadUI->GButtons->bGButtonR);
							ImGui::SameLine();
							ImGui::Text("Пауза - "); ImGui::SameLine(); PahomEngine->GamepadUI->GamepadButtonRender("START", PahomEngine->RGBA(135, 135, 135, 255), PahomEngine->GamepadUI->GButtons->bGButtonSTART);
						}
						else {

							PahomEngine->setItemCenterX((ImGui::CalcTextSize("Влево - ").x + ImGui::CalcTextSize("Вправо - ").x + ImGui::CalcTextSize("Пауза - ").x + (32 * 3 + 5)));
							ImGui::Text("Влево - "); ImGui::SameLine(); PahomEngine->GamepadUI->GamepadButtonRender("A", PahomEngine->RGBA(135, 135, 135, 255), PahomEngine->GamepadUI->GButtons->bGButtonL);
							ImGui::SameLine();
							ImGui::Text("Вправо - "); ImGui::SameLine(); PahomEngine->GamepadUI->GamepadButtonRender("D", PahomEngine->RGBA(135, 135, 135, 255), PahomEngine->GamepadUI->GButtons->bGButtonR);
							ImGui::SameLine();
							ImGui::Text("Пауза - "); ImGui::SameLine(); PahomEngine->GamepadUI->GamepadButtonRender("SPACE", PahomEngine->RGBA(135, 135, 135, 255), PahomEngine->GamepadUI->GButtons->bGButtonSTART);
						}
						static ImVec4 colorTextToFrameRate = {};
						if (backend->io.Framerate >= 60) {
							colorTextToFrameRate = { 0,205,173,255 };
						}
						else if (backend->io.Framerate <= 30) {
							colorTextToFrameRate = { 255,0,133,255 };
						}
						else if (backend->io.Framerate <= 40) {
							colorTextToFrameRate = { 205,255,20,255 };
						}
						if (GetAsyncKeyState('G')) {
							PahomEngine->bKefir = true;
						}
						ImGui::SetCursorPos({ 30,ImGui::GetWindowSize().y - 60 });
						PahomEngine->TextColored(PahomEngine->RGBA(colorTextToFrameRate), "{:.1f}", backend->io.Framerate);
						if (PahomEngine->bDebugText) {
							ImGui::SetCursorPosX(20);
							ImGui::SetCursorPosY(60);
							PahomEngine->TextColored(PahomEngine->RGBA(0, 255, 139, 255), "frame_rate: {}\ndelta_time: {}\npos_pahom: {}.{} | pos_bread: {}.{}\nbuild: {}\nmem:{}\nspeed_pahom:{}\nWindowSize: {}x{}\nPahomSize:{}x{}\nBreadSize:{}:{}",
								PahomEngine->fFrameRate,
								backend->io.DeltaTime,
								PahomEngine->fPahomPosX,
								PahomEngine->fPahomPosY,
								PahomEngine->fBreadPosX,
								PahomEngine->fBreadPosY,
								PahomEngine->sBuild,
								PahomEngine->Exceptions->i64MemoryUsageProcess,
								PahomEngine->fStep,
								PahomEngine->i64WindowSize[0],
								PahomEngine->i64WindowSize[1],
								PahomEngine->i64PahomSize[0],
								PahomEngine->i64PahomSize[1],
								PahomEngine->i64BreadSize[0],
								PahomEngine->i64BreadSize[1]
							);
							ImGui::Checkbox("Game->bAntiGravitation", &Game->bAntiGravitation);
							if (ImGui::Button("Low")) {
								PahomEngine->ReloadTextures(0);
							}
							if (ImGui::Button("High")) {
								PahomEngine->ReloadTextures(1);
							}
							if (ImGui::Button("Ultra")) {
								PahomEngine->ReloadTextures(2);
							}
							ImGui::CustomToggle("Enable Hitbox", &PahomEngine->bIsEnableHitbox);
							static int64_t i64ScaleOffset = PahomEngine->i64PahomSize[0] / 64;
							ImGui::SetNextItemWidth(150);
							ImGui::SliderInt64("Scale", &i64ScaleOffset, 1, 16, "%lld", 0);
							PahomEngine->setSizePahom(64 * i64ScaleOffset, 64 * i64ScaleOffset);
							//if (ImGui::Button("Fill 64x64 Square Explorer")) {
							//    for (int x = 0; x < 32; x++) {
							//        for (int y = 0; y < 32; y++) {
							//            SetPixel(GetDC(FindWindowA(NULL, "HxD")), x, y, RGB(255, 0, 0));
							//        }
							//    }
							//}
							ImGui::Checkbox("RenderStop", &PahomEngine->bRenderPaused);
							ImGui::Checkbox("Noise", &PahomEngine->Event.bShowNoiseBackground);
							if (ImGui::Button("Crash")) {
								PahomEngine->ShutdownEngine();
								delete PahomEngine;
								backend->render = false;
							}
						}
						if (GetKeyState(VK_F12) & 0x8000) {
							PahomEngine->bRenderPaused = !PahomEngine->bRenderPaused;
						}

					}
				}

			}
		}
		// Text to Event
		if (!PahomEngine->bStartGameFlag) {
			ImGui::SetCursorPos({
			  (PahomEngine->cast->unicast<int64_t,float>(PahomEngine->i64WindowSize[0]) - ImGui::CalcTextSize("(ESC) Exit").x) - 50,
			   (PahomEngine->cast->unicast<int64_t,float>(PahomEngine->i64WindowSize[1]) - ImGui::CalcTextSize("(ESC) Exit").y) - 50,
				});
			PahomEngine->Text("(ESC) Exit");
		}
		else {
			ImGui::SetCursorPos({
		  (PahomEngine->cast->unicast<int64_t,float>(PahomEngine->i64WindowSize[0]) - ImGui::CalcTextSize("[F1 - Управление]").x) - 50,
		   (PahomEngine->cast->unicast<int64_t,float>(PahomEngine->i64WindowSize[1]) - ImGui::CalcTextSize("[F1 - Управление]").y) - 50,
				});
			PahomEngine->Text("[F2 - Управление]");

		}
		static bool isAboutControls = false;
		if (GetAsyncKeyState(VK_F2)) {
			isAboutControls = true;
		}
		if (isAboutControls) {
			PahomEngine->UI->Message("[ESC] - Exit | (BACK) - EXIT\n[A] - Влево | (DL) - Влево\n[D] - Вправо | (DR) - Вправо\n[SPACE] - Меню/Пауза | (START) - Меню/Пауза", { 550,250 }, 555, 1000.00, & isAboutControls, true);
		}
		if (bExitWindow && !Game->bExitWindowClosed) {
			Game->ExitWindow();
		}

		ImGui::End();

		backend->viewport();

	}
	PahomEngine->Exceptions->Cleanup();
	backend->shutdownDevice();
	return 0;
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (wParam != SIZE_MINIMIZED)
		{
			g_Width = LOWORD(lParam);
			g_Height = HIWORD(lParam);
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		PahomEngine->ShutdownEngine();
		delete PahomEngine; 
		exit(0);   
		return 0;
	}
	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}/////// source code len 9000 /////////////////