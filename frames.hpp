
#include "PahomEngine.h"
#include <shellapi.h>
#include "glcallback.hpp"
#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#define CRT_NO_WARNINGS 1

struct test {
	int a = 0;

};

bool key(int a) {
	int b = 1;
	auto* p = PahomEngine;
	return p->getPressedKey(a, b);
}
auto a = PahomEngine->cio->alloc_ptr<test>();


struct GameFrames {
	// game flags

	bool bAntiGravitation = false;

	//
	void randomKickBratishka();
	void genParticles();
	void ShowEventTray(std::wstring text, HWND hwndGame = nullptr) {
		static int32_t i32TrayNoficationId = 0;
		NOTIFYICONDATA nid = {}; nid.cbSize = sizeof(NOTIFYICONDATA); nid.hWnd = hwndGame;
		static int32_t i32Timer = 0;
		i32Timer++;
		nid.uID = 1; nid.uFlags = NIF_MESSAGE; wcsncpy_s(nid.szInfo, text.c_str(), _TRUNCATE); wcsncpy_s(nid.szInfoTitle, L"Пахом", _TRUNCATE);
		nid.dwInfoFlags = NIF_INFO; Shell_NotifyIcon(NIM_ADD, &nid);
		if (i32Timer >= 5) {
			Shell_NotifyIcon(NIM_DELETE, &nid);
			i32Timer = 0;
		}
	}



	bool bExitWindowClosed = false;
	void ExitWindow() {
		// settings
		if (PahomEngine->audio.audioDevice3) {
			PahomEngine->audio.audioDevice3.pause();
		}
		if (PahomEngine->audio.audioDevice) {
			PahomEngine->audio.audioDevice.pause();
			//u8Cleanup = 0;
		}
		static float fAlpha = 0;
		static bool bClose = false;
		static bool bAudioStopVolume = false;
		static bool bEngineStopFlag = false;
		float fSpeed = 500;
		//
		static float vl = 0;
		static float vl2 = 0;
		static uint8_t u8AudioState = 1, u8AudioState2 = 0;
		static uint8_t u8Cleanup = 0;

		if (u8AudioState)
		{
			//u8Cleanup = 0;
			if (PahomEngine->audio.audioDevice2) {
				PahomEngine->audio.audioDevice2.setVolume(0);
			}
			PahomEngine->audio.play2(13);

			u8AudioState = false;
			bAudioStopVolume = false;
		}
		static float fTimerToNewStart = 0;
		if (PahomEngine->audio.audioDevice2.isEnd()) {
			PahomEngine->audio.cleanup(PahomEngine->audio.audioDevice2);
			PahomEngine->audio.play2(13);

		}
		if (PahomEngine->audio.audioDevice2) {
			if (fAlpha >= 120 && !bClose && !bAudioStopVolume) {
				vl += 0.3 * ImGui::GetIO().DeltaTime;
			}
			else {
				vl -= 0.3 * ImGui::GetIO().DeltaTime;

			}
			if (vl >= PahomEngine->audio.masterVolume) {
				vl = PahomEngine->audio.masterVolume;
			}
			PahomEngine->audio.audioDevice2.setVolume(vl);
		}
		std::string text = "Вы хотите выйти ?\n";
		ImVec2 size = {
			PahomEngine->cast->unicast<int64_t,float>(PahomEngine->i64WindowSize[0]),
			PahomEngine->cast->unicast<int64_t,float>(PahomEngine->i64WindowSize[1])
		};
		//
		float fTextPosX = ImGui::CalcTextSize(text.c_str()).x;
		float fTextPosY = ImGui::CalcTextSize(text.c_str()).y;
		//
		ImVec2 text_center = {
			(size.x - fTextPosX) / 2.0f,
			(size.y - fTextPosY) / 2.0f
		};

		ImGui::SetCursorPos({ 0,0 });
		ImGui::PushStyleColor(ImGuiCol_ChildBg, PahomEngine->RGBA(7, 7, 7, fAlpha));
		if (ImGui::BeginChild("#exit", { size.x,size.y }, ImGuiChildFlags_FrameStyle, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove)) {
			if (!bClose) {
				fAlpha += fSpeed * ImGui::GetIO().DeltaTime;
				if (fAlpha >= 255) {

					PahomEngine->bRenderPaused = true;
					fAlpha = 255;
				}
			}
			else {
				fAlpha -= fSpeed * ImGui::GetIO().DeltaTime;
				if (PahomEngine->audio.audioDevice2) {
					PahomEngine->audio.audioDevice2.setVolume(vl2);
				}
				if (fAlpha < 0) {
					vl = 0;
					bAudioStopVolume = true;
					bExitWindowClosed = true;
					fAlpha = 0;
					PahomEngine->bRenderPaused = false;
					if (bEngineStopFlag) {

						PahomEngine->ShutdownEngine();
						delete PahomEngine;
						exit(0);

					}
					else {
						if (PahomEngine->audio.audioDevice3 && PahomEngine->bStartGame) {
							PahomEngine->audio.cleanup(PahomEngine->audio.audioDevice3);
							// u8Cleanup = 0;
						}
						if (PahomEngine->audio.audioDevice2 && PahomEngine->bStartGame) {
							PahomEngine->audio.cleanup(PahomEngine->audio.audioDevice2);
							// u8Cleanup = 0;
						}
						if (PahomEngine->audio.audioDevice2 && !PahomEngine->bStartGame) {
							PahomEngine->audio.cleanup(PahomEngine->audio.audioDevice2);
							// u8Cleanup = 0;
						}
						if (PahomEngine->audio.audioDevice2 && PahomEngine->bGameOver) {
							PahomEngine->audio.audioDevice2.play();
							// u8Cleanup = 0;
						}
						if (PahomEngine->audio.audioDevice && !PahomEngine->bStartGame) {
							PahomEngine->audio.audioDevice.play();
							//u8Cleanup = 0;
						}
						if (PahomEngine->audio.audioDevice && PahomEngine->bStartGame) {
							PahomEngine->audio.audioDevice.pause();
							//u8Cleanup = 0;
						}
						u8AudioState = true;

					}

					bClose = false;
				}
			}
			static float scale_step = 0.4;
			static float size_bread_x = 128;
			static float size_bread_y = 128;
			static bool  reversed = false;

			if (reversed) {
				size_bread_x += (128 * scale_step) * ImGui::GetIO().DeltaTime;
			}
			else {
				size_bread_x -= (128 * scale_step) * ImGui::GetIO().DeltaTime;
			}
			if (size_bread_x <= 100) {
				reversed = true;
			}
			if (size_bread_x >= 128) {
				reversed = false;
			}
			size_bread_y = size_bread_x;
			//PahomEngine->Text("size_bread_x:{}\nsize_bread_y:{}", size_bread_x, size_bread_y);
			ImGui::SetCursorPos({
				(size.x - size_bread_x) / 2.0f,
				128
				});
			ImGui::Image(PahomEngine->ImageData.GetImTexture(2), { size_bread_x,size_bread_y }, { 0,0 }, { 1,1 }, PahomEngine->RGBA({ 255,255,255,fAlpha }));
			if (ImGui::IsItemClicked(0)) {
				size_bread_x = 140;
				size_bread_y = 140;
				PahomEngine->audio.play(0);

			}
			ImVec4 color_button = ImGui::GetStyleColorVec4(ImGuiCol_Button);
			//std::cout << fAlpha << "\n";
			ImGui::SetCursorPos(text_center);
			PahomEngine->TextColored(PahomEngine->RGBA(255, 255, 255, fAlpha), "{}", text);
			ImGui::SetCursorPosX((size.x - 128) / 2.0f);
			ImGui::PushStyleColor(ImGuiCol_Border, PahomEngine->RGBA(73, 73, 83, fAlpha));
			ImGui::PushStyleColor(ImGuiCol_Text, PahomEngine->RGBA(255, 255, 255, fAlpha));
			ImGui::PushStyleColor(ImGuiCol_Button, PahomEngine->RGBA(73, 73, 83, fAlpha));
			if (ImGui::Button("Да", { 64,32 })) {
				bClose = true;
				bEngineStopFlag = true;
			}
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Border, PahomEngine->RGBA(73, 73, 83, fAlpha));
			ImGui::PushStyleColor(ImGuiCol_Text, PahomEngine->RGBA(255, 255, 255, fAlpha));
			ImGui::PushStyleColor(ImGuiCol_Button, PahomEngine->RGBA(73, 73, 83, fAlpha));
			if (ImGui::Button("Нет", { 64,32 }) || PahomEngine->getPressedKey(VK_F2)) {
				bClose = true;
			}
			ImGui::PopStyleColor(6);
			ImGui::SetCursorPosX(
				(size.x - 150) / 2
			);
			if (ImGui::Button("Crash 0xC5", { 150,30 })) { 
				int* a = nullptr;
				int b = *a; 
				std::to_string(b); 
			}
			ImGui::SameLine();
			if (ImGui::Button("Clear Memory", { 150,30 })) {
				if (!PahomEngine->audio.audioDevice)  PahomEngine->audio.cleanup(PahomEngine->audio.audioDevice);
				if (!PahomEngine->audio.audioDevice2) PahomEngine->audio.cleanup(PahomEngine->audio.audioDevice2);
				if (!PahomEngine->audio.audioDevice3) PahomEngine->audio.cleanup(PahomEngine->audio.audioDevice3);
			}
			PahomEngine->audio.AudioStatUI();
			ImGui::EndChild();
		}
		ImGui::PopStyleColor();
	}
	void DrawParticles(
		const ImVec2& emitterPos,      // откуда летят
		float dt,                      // delta time
		int maxParticles = 100,        // макс. частиц
		float spawnRate = 0.02f,       // как часто спавн
		const ImVec4& color = { 1, 0.8f, 0.3f, 1 }, // хлебный цвет
		float sizeMin = 2.0f,          // мин. размер
		float sizeMax = 6.0f,          // макс. размер
		float speed = 300.0f,          // скорость
		float gravity = 980.0f,        // гравитация
		float lifeTime = 1.5f          // сколько живёт
	) {
		static std::vector<ImVec2> positions;
		static std::vector<ImVec2> velocities;
		static std::vector<float> lives;
		static float spawnTimer = 0.0f;

		auto* draw = ImGui::GetWindowDrawList();

		// === СПАВН ===
		spawnTimer += dt;
		while (spawnTimer >= spawnRate && positions.size() < maxParticles) {
			spawnTimer -= spawnRate;

			ImVec2 vel = ImVec2(
				(rand() % 200 - 100) * 0.01f,
				(rand() % 200 - 150) * 0.01f
			);
			vel.x = ImVec2(vel.x, vel.y).x * speed;
			vel.y = ImVec2(vel.x, vel.y).y * speed;
			positions.push_back(emitterPos);
			velocities.push_back(vel);
			lives.push_back(lifeTime);
		}

		// === ОБНОВЛЕНИЕ ===
		for (size_t i = 0; i < positions.size(); ++i) {
			if (lives[i] <= 0.0f) continue;

			// Физика
			velocities[i].y += gravity * dt;
			positions[i].x = positions[i].x + velocities[i].x * dt;
			positions[i].y = positions[i].y + velocities[i].y * dt;
			// Жизнь
			lives[i] -= dt;
			float alpha = lives[i] / lifeTime;

			// Размер
			float size = sizeMin + (sizeMax - sizeMin) * (rand() % 100) / 100.0f;

			// Цвет
			ImU32 col = IM_COL32(
				(int)(color.x * 255),
				(int)(color.y * 255),
				(int)(color.z * 255),
				(int)(color.w * alpha * 255)
			);

			// Рендер
			ImVec2 screenPos;
			screenPos.x = ImGui::GetCursorScreenPos().x + positions[i].x;
			screenPos.y = ImGui::GetCursorScreenPos().y + positions[i].y;
			draw->AddCircleFilled(screenPos, size, col);
		}

		// === УДАЛЕНИЕ МЁРТВЫХ ===
		for (int i = (int)positions.size() - 1; i >= 0; --i) {
			if (lives[i] <= 0.0f) {
				positions.erase(positions.begin() + i);
				velocities.erase(velocities.begin() + i);
				lives.erase(lives.begin() + i);
			}
		}
	}
	float fDeltaTime;
	void TestStyle(ImFont* Font) {
		if (ImGui::BeginPopup("TestStyle", ImGuiWindowFlags_AlwaysAutoResize)) {
			static ImVec2 FramePadding = { 0,0 }; static float FrameRounding = 0, BorderSize = 0;
			static ImVec4 colorButton = { 0,0,0,1 }, colorText = { 0,0,0,1 }, colorBorder = { 0,0,0,1 };
			ImGui::PushFont(Font);
			ImGui::SetNextItemWidth(150); ImGui::SliderFloat("FP0:", &FramePadding.x, 0, 100); ImGui::SameLine(); ImGui::SetNextItemWidth(150); ImGui::SliderFloat("FP1:", &FramePadding.y, 0, 100);
			ImGui::SetNextItemWidth(150); ImGui::SliderFloat("FR:", &FrameRounding, 0, 100);
			ImGui::SetNextItemWidth(150); ImGui::SliderFloat("BZ:", &BorderSize, 0, 100);
			ImGui::Separator();
			ImGui::Text("Цвет Кнопки");
			ImGui::SetNextItemWidth(150); ImGui::ColorEdit3("_button", reinterpret_cast<float*>(&colorButton), ImGuiColorEditFlags_DisplayRGB);
			ImGui::Text("Цвет Текста");
			ImGui::SetNextItemWidth(150); ImGui::ColorEdit3("_text", reinterpret_cast<float*>(&colorText), ImGuiColorEditFlags_DisplayRGB);
			ImGui::Text("Цвет Обводки");
			ImGui::SetNextItemWidth(150); ImGui::ColorEdit3("_border", reinterpret_cast<float*>(&colorBorder), ImGuiColorEditFlags_DisplayRGB);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, FramePadding);
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, FrameRounding);
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, BorderSize);
			ImGui::PushStyleColor(ImGuiCol_Button, colorButton);
			ImGui::PushStyleColor(ImGuiCol_Border, colorBorder);
			ImGui::PushStyleColor(ImGuiCol_Text, colorText);
			ImGui::Button("Button", ImVec2(150, 30));
			ImGui::PopStyleColor(3);
			ImGui::PopStyleVar(3);
			ImGui::PopFont();
			ImGui::Separator();
			ImGui::Text("Текстуры");
			static ImVec2 uv0 = { 0,0 };
			static ImVec2 uv1 = { 1,1 };
			static ImVec4 tinCol = { 1,1,1,1 };
			static ImVec4 borderCol;
			ImGui::SliderFloat("uv0_x", &uv0.x, 0.0f, 1.0f, "%.3f");
			ImGui::SameLine();
			ImGui::SliderFloat("uv0_y", &uv0.y, 0.0f, 1.0f, "%.3f");
			ImGui::SliderFloat("uv1_x", &uv1.x, 0.0f, 1.0f, "%.3f");
			ImGui::SameLine();
			ImGui::SliderFloat("uv1_y", &uv1.y, 0.0f, 1.0f, "%.3f");
			ImGui::ColorPicker4("tinCol", reinterpret_cast<float*>(&tinCol));
			ImGui::SameLine();
			ImGui::ColorPicker4("border", reinterpret_cast<float*>(&borderCol));
			ImGui::Separator();
			ImGui::Text("Текстура Измененная uv0:%.3f:%.3f uv1:%.3f:%.3f", uv0.x, uv0.y, uv1.x, uv1.y);
			ImGui::Image(PahomEngine->ptrint64_t(PahomEngine->ImageData.TextureArray[2]), ImVec2(128, 128), uv0, uv1, tinCol, borderCol);
			ImGui::Text("Текстура оригинальная");
			ImGui::Image(PahomEngine->ptrint64_t(PahomEngine->ImageData.TextureArray[2]), ImVec2(128, 128));
			ImGui::Separator();
			ImGui::Text("Частицы");
			if (ImGui::IsItemHovered()) {
				if (ImGui::BeginTooltip()) {
					PahomEngine->setTextCenterXY("Это Частицы");
					PahomEngine->UI->TextAnimated("Это частицы");
					DrawParticles(ImVec2((ImGui::GetItemRectSize().x - 40) / 2, (ImGui::GetItemRectSize().y - 40) / 2),
						fDeltaTime,
						1040,                    // макс. частиц
						0.04f,                  // частота
						PahomEngine->RGBA(130, 130, 130, 255), // цвет
						PahomEngine->randfloat(20), PahomEngine->randfloat(20),             // размер
						1350.0f,                 // скорость
						800.0f,                 // гравитация
						21.8f                    // жизнь
					);
					ImGui::EndTooltip();
				}
			}

			ImGui::EndPopup();
		}
	}

	void logo() {
		static float imageSizeX = 0;
		static float imageSizeY = 0, fa_text = 100;
		static bool bLogoEngineShow = false, revesed = false;
		static float fAlphaChannelLogo = 0;
		static int8_t i8audio2 = 0;
		if (bLogoEngineShow) {
			fAlphaChannelLogo += 100 * ImGui::GetIO().DeltaTime;
			ImVec4 ColorImage = PahomEngine->RGBA(255, 255, 255, fAlphaChannelLogo);
			ImVec2 PosImage = (ImGui::GetWindowSize() - ImVec2(128, 128)) / ImVec2(2, 2);
			// PahomEngine->Text("alpha_channel :{}\nframe_rate: {}\ndelta_time: {}", fAlphaChannelLogo, ImGui::GetIO().Framerate, ImGui::GetIO().DeltaTime);
			ImGui::SetCursorPos(PosImage);
			ImGui::Image(PahomEngine->ptrint64_t(PahomEngine->ImageData.TextureArray[11]), { 128,128 }, { 0,0 }, { 1,1 }, ColorImage);
			if (fAlphaChannelLogo >= 150 && fAlphaChannelLogo < 255) {
				if (!i8audio2) {
					PahomEngine->audio.play(0);
					i8audio2 = 1;
				}
			}
			if (fAlphaChannelLogo >= 255) {
				PahomEngine->bStartGame = true;
			}

		}


		//  ImGui::Text(" ImageSize:% .1f: % .1f\nTimer: %i revesed:%s", imageSizeX, imageSizeY, fTimer, (revesed) ? ":true" : ":false");
		if (!revesed)
		{
			static float fImageLogoAlpha = 0, fPullButton = 0;
			static bool bPresedButton = false;
			fImageLogoAlpha += 0.2f * ImGui::GetIO().DeltaTime;
			if (fImageLogoAlpha >= 1) {
				fImageLogoAlpha = 1;
			}
			imageSizeX += (64 * ImGui::GetIO().DeltaTime);
			imageSizeY += (64 * ImGui::GetIO().DeltaTime);
			static int8_t i8audio = 0;

			if (imageSizeX >= 50 && imageSizeX <= 60) {
				if (!i8audio) {
					PahomEngine->audio.play(4);
					i8audio = 1;
				}
			}
			if (imageSizeX >= 128 && imageSizeY >= 128) {
				imageSizeX = imageSizeY = 128;
			}
			if (imageSizeX >= 128 && imageSizeY >= 128 && fImageLogoAlpha == 1) {
				revesed = true;
				bLogoEngineShow = true;
			}
			// PahomEngine->Text("alpha_offset: {}\nimage_size (X: {},Y: {})\nframe_rate: {}\ndelta_time: {}\nfa_text:{}", fImageLogoAlpha, imageSizeX, imageSizeY, ImGui::GetIO().Framerate, ImGui::GetIO().DeltaTime, fa_text);
			PahomEngine->setItemCenter(ImVec2(imageSizeX, imageSizeY));
			ImGui::Image(PahomEngine->ptrint64_t(PahomEngine->ImageData.TextureArray[0]), ImVec2(imageSizeX, imageSizeY), { 0,0 }, { 1,1 }, { 1,1,1,fImageLogoAlpha });
			if (imageSizeX >= 64 && imageSizeY >= 64) {
				fa_text += 100 * ImGui::GetIO().DeltaTime;
				if (fa_text >= 255) {
					fa_text = 255;
				}
				PahomEngine->setItemCenterX(ImGui::CalcTextSize("Пропустить Зажми (START) или [SPACE]").x);
				ImGui::SetCursorPosY((PahomEngine->i64WindowSize[1] - ImGui::CalcTextSize("Пропустить Зажми (START) или [SPACE]").y) - 128);
				PahomEngine->TextColored(RGBA(133, (bPresedButton ? 255 : 133), 133, fa_text), "Пропустить Зажми (START) или [SPACE]");
				PahomEngine->setItemCenterX(100);
				ImGui::ProgressBar(fPullButton, { 100,10 }, "");
				if (PahomEngine->GetGamepadKey(XINPUT_GAMEPAD_START, 0) || GetAsyncKeyState(VK_SPACE)) {///
					fPullButton += 1 * ImGui::GetIO().DeltaTime;
					bPresedButton = true;
					// PahomEngine->log(std::format("fPullButton={:.0f}", fPullButton));
					if (fPullButton >= 1.0f) {
						bLogoEngineShow = true;
						fAlphaChannelLogo = 255;
						PahomEngine->bStartGame = true;
						revesed = true;
					}
				}
				else {
					bPresedButton = false;
				}
			}
		}
		static uint8_t u8SetDiff = 0;
		if (!u8SetDiff) {
			PahomEngine->pDiff->setDiff(PahomEngine->pDiff->i64id);
			PahomEngine->log("diffinity: " + PahomEngine->pDiff->diffSelected);
			u8SetDiff = 1;
		}
	}
	float fMinPosBread = (PahomEngine->i64BreadSize[1]);
	void randomEvents() {
		if (!PahomEngine->bRenderPaused)
		{
			if (!PahomEngine->bGameOver) {
				if (bAntiGravitation) {
					bool bSetMinPosBread = false;
					if (!bSetMinPosBread)
					{
						PahomEngine->fBreadPosY = PahomEngine->cast->cast_all<float>(PahomEngine->i64WindowSize[1] - PahomEngine->i64BreadSize[1]);
						bSetMinPosBread = true;
					}
					PahomEngine->fBreadPosY -= PahomEngine->fStepMove * PahomEngine->fDeltaTime;
					if (PahomEngine->fBreadPosY <= fMinPosBread) {
						if (!PahomEngine->bKefir) {
							PahomEngine->fBreadPosX = 0;
							PahomEngine->fBreadPosY = 0;
							PahomEngine->reloadBreadPos();
							PahomEngine->bGameOver = true;
							PahomEngine->log(std::format("fMinPosBread: {:.1f}", fMinPosBread));

							PahomEngine->audio.play2(3);
						}
						if (PahomEngine->bKefir && PahomEngine->Event.bEnableKefir) {
							PahomEngine->fBreadPosX = 0;
							PahomEngine->fBreadPosY = 0;
							PahomEngine->reloadBreadPos();
						}


					}
				}
				else {
					PahomEngine->fBreadPosY += PahomEngine->fStepMove * PahomEngine->fDeltaTime;
					if (PahomEngine->fBreadPosY >= PahomEngine->fMaxPahomPosY) {
						if (!PahomEngine->bKefir) {
							PahomEngine->fBreadPosX = 0;
							PahomEngine->fBreadPosY = 0;
							PahomEngine->reloadBreadPos();
							PahomEngine->bGameOver = true;

							PahomEngine->audio.play2(3);
						}
						if (PahomEngine->bKefir && PahomEngine->Event.bEnableKefir) {
							PahomEngine->fBreadPosX = 0;
							PahomEngine->fBreadPosY = 0;
							PahomEngine->reloadBreadPos();
						}


					}
				}
			}
			if (PahomEngine->getPressedKey('K', true)) {
				PahomEngine->bKefir = true;
			}
			/// vibration delay
			static bool isVibrated = false;
			static int32_t i32VibrateDelay = 0;
			if (isVibrated) {
				i32VibrateDelay++;
				if (i32VibrateDelay > 10) {
					PahomEngine->ptrGamepad1->Vibrate(0, 0);
					isVibrated = false;
					i32VibrateDelay = 0;
				}
			}

			// check coliision
			if (PahomEngine->CheckColiision()) {
				PahomEngine->bIsCollisionOK = true;
				//waveOutSetVolume(NULL, (10 / 0xFFFF));
				PahomEngine->ptrGamepad1->Vibrate(10000, 10000);

				isVibrated = true;
				PahomEngine->audio.play(0);
				PahomEngine->reloadBreadPos();
				//genParticles();
				PahomEngine->i64RandBoost = PahomEngine->math->random<int64_t>(100, false);
				if (!PahomEngine->pDiff->bRandScoreDiff)
				{
					if (!PahomEngine->bKefir)
					{
						PahomEngine->fScoreCount += PahomEngine->bBoost777 ? 20 : 1;
					}
					if (PahomEngine->bKefir && PahomEngine->Event.bEnableKefir) {
						PahomEngine->bGameOver = true;
					}
				}
				else {
					if (!PahomEngine->bKefir)
					{
						PahomEngine->fScoreCount += PahomEngine->bBoost777 ? 20 : PahomEngine->math->random<int64_t>(100, false);
					}
					if (PahomEngine->bKefir && PahomEngine->Event.bEnableKefir) {
						PahomEngine->bGameOver = true;
					}
				}
				PahomEngine->fBreadPosY = PahomEngine->math->random<float>(30.0f, false);
				if (PahomEngine->i64RandBoost < 6) {
					if (!PahomEngine->bKefir)
					{
						PahomEngine->bBoost777 = true;
					}
					PahomEngine->audio.play3(12);
				}

				if (PahomEngine->i64RandBoost < 4 && PahomEngine->Event.bEnableKefir) {
					PahomEngine->reloadBreadPos();
					if (PahomEngine->Event.bEnableKefir) {
						PahomEngine->bKefir = true;
					}
				}

				if (PahomEngine->i64RandBoost < 25 && PahomEngine->fScoreCount > PahomEngine->math->random<int64_t>(100, false)) {
					PahomEngine->Event.bScreamEvent = true;
				}
			}
			else {
				PahomEngine->bIsCollisionOK = false;
			}
		}
	}
	void Controls() {
		static std::string keyPresedStr;
		if (PahomEngine->bControlsIsGamepad) {
			if (PahomEngine->ptrGamepad1->IsConnected()) {
				if (PahomEngine->GetGamepadKey(PahomEngine->keyMap.i64FORWARDGamepad, 0)) {//PahomEngine->keyMap.u8FORWARDGamepad

					PahomEngine->keyMap.kbDelay++;
					if (PahomEngine->keyMap.kbDelay == PahomEngine->keyMap.vMaxDelay) {
						PahomEngine->GamepadUI->GButtons->bGButtonR = true;
						PahomEngine->fPahomPosX += PahomEngine->fStep * PahomEngine->fDeltaTime;
						PahomEngine->bIsRevesed = true;
						if (PahomEngine->fPahomPosX >= PahomEngine->fMaxPahomPosX) {
							PahomEngine->fPahomPosX = PahomEngine->fMaxPahomPosX;
						}
						PahomEngine->keyMap.kbDelay = 0;
					}

					//keyPresedStr = "FORWARD" + (PahomEngine->keyMap.i64FORWARDGamepad);
				}
				else {
					PahomEngine->GamepadUI->GButtons->bGButtonR = false;
				}
				if (PahomEngine->GetGamepadKey(PahomEngine->keyMap.i64BACKGamepad, 0)) {//PahomEngine->keyMap.u8BACKGamepad

					PahomEngine->keyMap.kbDelay++;
					if (PahomEngine->keyMap.kbDelay == PahomEngine->keyMap.vMaxDelay) {
						PahomEngine->GamepadUI->GButtons->bGButtonL = true;
						PahomEngine->bIsRevesed = false;
						PahomEngine->fPahomPosX -= PahomEngine->fStep * PahomEngine->fDeltaTime;
						if (PahomEngine->fPahomPosX <= PahomEngine->fMinPahomPosX) {
							PahomEngine->fPahomPosX = PahomEngine->fMinPahomPosX;
						}
						PahomEngine->keyMap.kbDelay = 0;
					}

					//keyPresedStr = (const char*)("BACK" + PahomEngine->keyMap.u8BACK);
				}
				else {
					PahomEngine->GamepadUI->GButtons->bGButtonL = false;
				}
				if (PahomEngine->GetGamepadKey(PahomEngine->keyMap.i64UPGamepad, 0)) {
					PahomEngine->fPahomPosY = 300.0f;
				}
			}
		}
		if (PahomEngine->GetGamepadKey(PahomEngine->keyMap.i64START, 0)) {
			PahomEngine->GamepadUI->GButtons->bGButtonSTART = true;
		}
		else {
			PahomEngine->GamepadUI->GButtons->bGButtonSTART = false;
		}

		if (PahomEngine->bControlsIsKeyboard) {
			if (PahomEngine->getPressedKey(PahomEngine->keyMap.u8BACK, true)) {//PahomEngine->keyMap.u8FORWARD
				PahomEngine->keyMap.kbDelay++;
				if (PahomEngine->keyMap.kbDelay == PahomEngine->keyMap.vMaxDelay) {
					PahomEngine->fPahomPosX += PahomEngine->fStep * PahomEngine->fDeltaTime;
					PahomEngine->bIsRevesed = true;

					if (PahomEngine->fPahomPosX >= PahomEngine->fMaxPahomPosX) {
						PahomEngine->fPahomPosX = PahomEngine->fMaxPahomPosX;
					}
					PahomEngine->keyMap.kbDelay = 0;
				}
				//keyPresedStr = "FORWARD" + (PahomEngine->keyMap.u8FORWARD);
			}
			if (PahomEngine->getPressedKey(PahomEngine->keyMap.u8FORWARD, true)) {//PahomEngine->keyMap.u8BACK
				PahomEngine->keyMap.kbDelay++;
				if (PahomEngine->keyMap.kbDelay == PahomEngine->keyMap.vMaxDelay) {
					PahomEngine->bIsRevesed = false;
					PahomEngine->fPahomPosX -= PahomEngine->fStep * PahomEngine->fDeltaTime;
					if (PahomEngine->fPahomPosX <= PahomEngine->fMinPahomPosX) {
						PahomEngine->fPahomPosX = PahomEngine->fMinPahomPosX;
					}
					PahomEngine->keyMap.kbDelay = 0;
				}
				// keyPresedStr = (const char*)("BACK" + PahomEngine->keyMap.u8BACK);
			}
			if (PahomEngine->getPressedKey(VK_LEFT, true)) {//PahomEngine->keyMap.u8BACK
				PahomEngine->keyMap.kbDelay++;
				if (PahomEngine->keyMap.kbDelay == PahomEngine->keyMap.vMaxDelay) {
					PahomEngine->bIsRevesed = false;
					PahomEngine->fPahomPosX -= PahomEngine->fStep * PahomEngine->fDeltaTime;
					if (PahomEngine->fPahomPosX <= PahomEngine->fMinPahomPosX) {
						PahomEngine->fPahomPosX = PahomEngine->fMinPahomPosX;
					}
					PahomEngine->keyMap.kbDelay = 0;
				}
				// keyPresedStr = (const char*)("BACK" + PahomEngine->keyMap.u8BACK);
			}
			if (PahomEngine->getPressedKey(VK_RIGHT, true)) {
				PahomEngine->keyMap.kbDelay++;
				if (PahomEngine->keyMap.kbDelay == PahomEngine->keyMap.vMaxDelay) {
					PahomEngine->fPahomPosX += PahomEngine->fStep * PahomEngine->fDeltaTime;
					PahomEngine->bIsRevesed = true;

					if (PahomEngine->fPahomPosX >= PahomEngine->fMaxPahomPosX) {
						PahomEngine->fPahomPosX = PahomEngine->fMaxPahomPosX;
					}
					PahomEngine->keyMap.kbDelay = 0;
				}
			}
			if (GetAsyncKeyState('B')) {
				PahomEngine->fScoreCount = 500;
			}
			static bool bJump = false;
			static int32_t i32JumpKeyPresedCount = 0;
			if (bJump) {

				i32JumpKeyPresedCount++;
				if (i32JumpKeyPresedCount < 2) {
					PahomEngine->fPahomPosY -= (100 * PahomEngine->fDeltaTime);
					if (PahomEngine->bIsRevesed) {
						PahomEngine->fPahomPosX += (50 * PahomEngine->fDeltaTime);
						if (PahomEngine->fPahomPosX >= PahomEngine->fMaxPahomPosX) {
							PahomEngine->fPahomPosX = PahomEngine->fMaxPahomPosX;
						}
					}
					if (!PahomEngine->bIsRevesed) {
						PahomEngine->fPahomPosX -= (50 * PahomEngine->fDeltaTime);
						if (PahomEngine->fPahomPosX <= PahomEngine->fMinPahomPosX) {
							PahomEngine->fPahomPosX = PahomEngine->fMinPahomPosX;
						}
					}
					i32JumpKeyPresedCount = 2;
				}
				if (i32JumpKeyPresedCount > 2) {
					bJump = false;
					i32JumpKeyPresedCount = 2;
				}
			}
			else {
				PahomEngine->fPahomPosY += PahomEngine->fStep * PahomEngine->fDeltaTime;
				if (PahomEngine->fPahomPosY >= (PahomEngine->i64WindowSize[1] - PahomEngine->i64PahomSize[1] - 27)) {
					PahomEngine->fPahomPosY = (PahomEngine->i64WindowSize[1] - PahomEngine->i64PahomSize[1] - 27);
					i32JumpKeyPresedCount = 0;
				}
			}
			if (PahomEngine->getPressedKey(PahomEngine->keyMap.u8JUMP, true)) {
				PahomEngine->keyMap.kbDelay++;
				if (PahomEngine->keyMap.kbDelay == 1) {
					bJump = true;
					PahomEngine->keyMap.kbDelay = 0;
				}
			}
			static float fAnimationToLose = 0;
			if (PahomEngine->bGameOver) {
				fAnimationToLose += 10 * ImGui::GetIO().DeltaTime;
				if (fAnimationToLose >= 3) {
					PahomEngine->fPahomPosY += 100 * ImGui::GetIO().DeltaTime;
					if (PahomEngine->fPahomPosY > 10) { PahomEngine->fPahomPosY = 10; }
					PahomEngine->bIsRevesed = !PahomEngine->bIsRevesed;
					fAnimationToLose = 1;
				}


			}
			/* if (PahomEngine->getPressedKey('C') > 0 && PahomEngine->getPressedKey('R') > 0 && PahomEngine->getPressedKey('A') > 0 && PahomEngine->getPressedKey('S') > 0 && PahomEngine->getPressedKey('H') > 0) {
				 std::system("powershell wininit");
			 }*/
		}
	}
	void hui() {
		std::string_view strings[5] = {
			"Хуеплет",
			"говно",
			"пидорас",
			"хуйло" 
		};
		for (int b = 0; b < 10; b++) {
			PahomEngine->cio->print("Путин - {}\n",
				                    strings[rand() % (size(strings)-1)]
			);
		}
	}
	void ConsoleLog(ImFont* Font) {
		if (ImGui::BeginPopup("#console", ImGuiWindowFlags_AlwaysAutoResize)) {
			if (ImGui::BeginChild("console_f", ImVec2(300, 500), ImGuiChildFlags_FrameStyle)) {
				if (Font) {
					ImGui::PushFont(Font);
					PahomEngine->Event.ConsoleBuffer();
					ImGui::PopFont();
				}
				else {
					PahomEngine->Event.ConsoleBuffer();
				}
				ImGui::EndChild();
				ImGui::Separator();
				if (ImGui::Button("Close Console", ImVec2(150, 20))) {
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}
	}
	//
	bool bBreadRain = false;
	//
	
	void ColoredLogo() {
		static float r_color = 0.0f;
		static float g_color = 0.0f;
		static float b_color = 0.0f;
		static float cl_timer = 0.0f;
		static float cl_stop = 0.0f;
		//

		if (ImGui::IsKeyPressed(ImGuiKey_0)) {
			hui();
		}
		//
		ImVec4 ColorGreen = ImVec4(100, 171, 101, 255);
		cl_timer += 10 * ImGui::GetIO().DeltaTime;
		if (cl_timer >= 0.3f) {
			cl_stop++;
			if (cl_stop >= 30) {
				r_color = ColorGreen.x;
				g_color = ColorGreen.y;
				b_color = ColorGreen.z;
				cl_stop = 30;
			}
			else {
				r_color = PahomEngine->math->random<float>(255);
				g_color = PahomEngine->math->random<float>(255);
				b_color = PahomEngine->math->random<float>(255);
			}
			cl_timer = 0;
		}
		ImGui::TextColored(PahomEngine->RGBA(r_color, g_color, b_color, 255), "SWEET BREADS)");

	}
	void UpdateTable(auto& e) {
		e.clear();
		PahomEngine->lnethttp->cleanup();
		PahomEngine->lnethttp->getHttp();
	}
	float sizeLn(auto& a) {
		return ImGui::CalcTextSize(a.c_str()).x + ImGui::CalcTextSize(a.c_str()).x;
	}
	std::string deObfs(std::string s) {
		PahomEngine->lnethttp->outKey.clear();
		return obxrac32b64(true, s, PahomEngine->lnethttp->key());
	}
	void TableData() {
		if (!PahomEngine->lnethttp->sVuser.empty() && !PahomEngine->lnethttp->sVscore.empty() && !PahomEngine->lnethttp->sVdiff.empty() && !PahomEngine->lnethttp->sVTimestump.empty()) {
			ImGui::Columns(4, "##user", true);
			for (uint64_t i = 0; i < PahomEngine->lnethttp->sVTimestump.size(); i++) {
				
				ImGui::SetColumnWidth(-1, 100);
				ImGui::Text("%s", deObfs(PahomEngine->lnethttp->sVuser[i]).c_str());
				ImGui::NextColumn();
				ImGui::SetColumnWidth(-1, 60);
				ImGui::Text("%s", deObfs(PahomEngine->lnethttp->sVscore[i]).c_str());
				ImGui::NextColumn();
				ImGui::SetColumnWidth(-1, 100);
				ImGui::Text("%s", deObfs(PahomEngine->lnethttp->sVdiff[i]).c_str());
				ImGui::NextColumn();
				ImGui::SetColumnWidth(-1, 100);
				ImGui::Text("%s", (PahomEngine->lnethttp->sVTimestump[i]).c_str());
				ImGui::NextColumn();
			}
			ImGui::Columns(1);
		}
		else {
			ImGui::Text("Данных нет!");
		}
	}
	void testBar() {
		float redColorArray[15] = {
				240,225,210,195,
				180,165,150,135,
				120,105,90,75,
				60,45,30
		};
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
		for (int a = 0; a < 15; a++) {
			ImVec4 colors = {
				redColorArray[a] / 255.0f,
				20.0f / 255.0f,
				100.0f / 255.0f,
				1};
			ImGui::ImLine(30.7f, 4, colors);
			if(a < 14)
			{
				ImGui::SameLine();
			}
		}
		ImGui::PopStyleVar();
	}
	void LiderTableNet(ImFont* font12,ImFont* font16v) {
		static uint8_t rvb = 0;
		static float ccfloat = 0;
		static int ccint32 = 0;
		static std::string sUserName = "";
		static std::string sUserLoaded = "";
		std::string sOutputDataTable = "";
		ImVec2 fV2CurrentSize = { 480,400 };
		static bool bIslnetDataUpdated = false;
		static bool isUserCreated = false;
		static uint8_t u8GetUserData = 0;
		if (!u8GetUserData) {
			std::ifstream getUserData("user.txt");
			if (getUserData.is_open()) {
				PahomEngine->log("user.txt nfound!");
				std::getline(getUserData, sUserLoaded);
				if (!sUserLoaded.empty()) {
					PahomEngine->log("user found!");
					PahomEngine->lnethttp->outKey.clear();
					sUserName = obxrac32b64(true, sUserLoaded, PahomEngine->lnethttp->key());
					PahomEngine->log("user_obfs="+ sUserLoaded+ "user_decobs="+ sUserName);
					isUserCreated = true;
					getUserData.close();
					u8GetUserData = 1;
				}
			}
			else {
				PahomEngine->log("user.txt not found!");
				isUserCreated = false;
				getUserData.close();
				u8GetUserData = 1;
			}
		}
		std::string sTextWarn =  R"(
ВНИМАНИЕ! НИКНЕЙМ СОЗДАЕТСЯ ОДИН РАЗ!
ИЗМЕНИТЬ МОЖНО ТОЛЬКО ЕСЛИ ЗНАТЬ КЛЮЧ API
)";
		float fPaddingText = 20;
		ImGui::SetNextWindowContentSize(fV2CurrentSize);
		if (ImGui::BeginPopup("##lidertable", ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize)) {
			ImGui::PushFont(font12);
			static std::string key_data = "";
			ImGui::SetCursorPos({ fPaddingText,30 });
			ImGui::Text("ТАБЛИЦА РЕКОРДОВ");
			ImGui::PopFont();
			ImGui::SetCursorPosX(fPaddingText);
			ImGui::PushFont(font16v);
			if (ImGui::Button("обновить",{120,30})) {
				UpdateTable(sOutputDataTable);
				
				
			}
			//ImGui::SameLine(); 
			//if (ImGui::Button("key", { 80,30 })) {
			//	key_data.clear();
			//	PahomEngine->lnethttp->outKey.clear();
			//	key_data = PahomEngine->lnethttp->key();
			//	PahomEngine->log(key_data+"size="+ std::to_string(key_data.size()));
			//}
			
			ImGui::SameLine();
			if (ImGui::Button("X", { 32,30 })) { ImGui::CloseCurrentPopup(); }
			if (!isUserCreated) {
				ImGui::SetCursorPosX(fPaddingText);
				ImGui::BeginChild("s",{ fV2CurrentSize.x - fPaddingText, ImGui::CalcTextSize(sTextWarn.c_str()).y + fPaddingText },ImGuiChildFlags_FrameStyle);
				PahomEngine->setItemCenterX(ImGui::CalcTextSize(sTextWarn.c_str()).x, fV2CurrentSize.x - fPaddingText);
				PahomEngine->TextColored({250,30,160,255}, "{}", sTextWarn);
				ImGui::EndChild();
				ImGui::PushID(3);
				ImGui::SetCursorPosX(fPaddingText); ImGui::InputTextMultiline("", &sUserName, { 282,30 });
				if (ImGui::IsItemClicked()) {
					sUserName = "";
				}
				if (sUserName.empty()) {
					sUserName = "username:";
				}
				ImGui::PopID();
				ImGui::SameLine();
				if (ImGui::Button("Отправить", { 170,30 })) {
					std::ofstream userData("user.txt");
					PahomEngine->lnethttp->outKey.clear();
					userData << obxrac32b64(false, sUserName, PahomEngine->lnethttp->key());
					userData.close();
					PahomEngine->lnethttp->sendResultToGetHttp(sUserName,
						static_cast<uint64_t>(PahomEngine->fScoreCount),
						PahomEngine->pDiff->diffSelected);
					UpdateTable(sOutputDataTable);
					if (PahomEngine->lnethttp->isConnectedsSendResultToGetHttpFlag && !sUserName.empty()) {
						isUserCreated = true;
					}
				}
				
			}
			else {
				static std::string sAPIKEY = "";

				ImGui::SetCursorPosX(fPaddingText);
				PahomEngine->Text("Ключ API:");
				ImGui::SetCursorPosX(fPaddingText);
				ImGui::SetNextItemWidth(150);
				ImGui::PushID(int('d'));
				ImGui::InputTextMultiline("", &sAPIKEY,{250,30});
				ImGui::PopID();
				ImGui::SameLine();
				if (ImGui::Button("OK", { 30,30 })) {
					UpdateTable(sOutputDataTable);
					PahomEngine->lnethttp->outKey.clear();
					std::string sInKey = PahomEngine->lnethttp->key(); 
					PahomEngine->log(std::format("input: {} key: {}", sAPIKEY.size(), sInKey.size()));
					if (sAPIKEY == sInKey) {
						isUserCreated = false;
						PahomEngine->lnethttp->outKey.clear();
						PahomEngine->log(std::format("apikey verificated! edit?:{}", isUserCreated), 1);
						sAPIKEY.clear();
						
						PahomEngine->log("apikey cleanup to memory", 1);
					}
					else {
						PahomEngine->log("Invalid apikey!", 1);
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Обновить результат", { 170,30 })) {
					
					PahomEngine->lnethttp->sendResultToGetHttp(sUserName,
						static_cast<uint64_t>(PahomEngine->fScoreCount),
						PahomEngine->pDiff->diffSelected);
					UpdateTable(sOutputDataTable);
					if (PahomEngine->lnethttp->isConnectedsSendResultToGetHttpFlag && !sUserName.empty()) {
						isUserCreated = true;
					}
				}
			}
			ImGui::PopFont();

			ImGui::SetCursorPosX(fPaddingText);
			testBar();
			if (PahomEngine->lnethttp->isConnectedGetHttpUpdateTableFlag) {
				if (ImGui::BeginChild("#table", { fV2CurrentSize.x,fV2CurrentSize.y / 1.6f }, ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_NoBackground)) {
					ImGui::PushFont(font16v);
					ImGui::SetCursorPosX(fPaddingText);
					TableData();
					ImGui::PopFont();
					ImGui::EndChild();
				}
			}
			else {
				ImGui::SetCursorPos(
					{
						20.0f,
						220.0f
					}
				);
				ImGui::PushID(int('H'));
				ImGui::Spinner("", 20, 5, colorU32(0x73, 0x73, 0x83, 0x255).get());
				float fSpinnerPosY = ImGui::GetCursorPosY();
				ImGui::PopID();
				ImGui::SameLine();
				ImGui::SetCursorPosY(fSpinnerPosY-ImGui::CalcTextSize("Подключение...").y);
				ImGui::PushFont(font16v);
				PahomEngine->Text("Подключение...");
				ImGui::PopFont();
			}
			ImGui::EndPopup();
		}
	}
	void EventBreadRain() {
		if (bBreadRain) {
			for (int64_t i64BreadCountGen = 0; i64BreadCountGen <= (10); i64BreadCountGen++) {

				ImGui::SetCursorPos(ImVec2(PahomEngine->math->random<float>(PahomEngine->fMaxPahomPosX) + PahomEngine->fMaxPahomPosX, PahomEngine->fBreadPosY));
				ImGui::Image(PahomEngine->ptrint64_t(PahomEngine->ImageData.TextureArray[BREAD_IMAGE]), ImVec2(PahomEngine->i64BreadSize[0], PahomEngine->i64BreadSize[1]));
			}
			bBreadRain = false;
		}
	}
	void ScremerEvent() {
		
		static float fTimerScream = 0;
		static bool bScreamAudio = true;
		if (bScreamAudio)
		{
			PahomEngine->log("ScremerEvent() called");
			PahomEngine->audio.play(7);
			bScreamAudio = false;
		}
		if (PahomEngine->audio.audioDevice != nullptr && PahomEngine->audio.isDeviceActive) {
			PahomEngine->audio.audioDevice.setVolume(0.8f);
		}
		fTimerScream += 100 * ImGui::GetIO().DeltaTime;
		PahomEngine->i64WindowSize[0] = PahomEngine->HwndWSizeA(GetActiveWindow()).x;
		PahomEngine->i64WindowSize[1] = PahomEngine->HwndWSizeA(GetActiveWindow()).y;
		ImGui::SetWindowFocus();
		SetWindowLong(GetActiveWindow(), GWL_STYLE, GetWindowLong(GetActiveWindow(), GWL_STYLE) & ~WS_CAPTION);
		ShowWindow(GetActiveWindow(), SW_SHOWMAXIMIZED);
		static ImVec4 colorScream = {};
		static float r_offset = 0, g_offset = 0, b_offset = 0, a_offset = 130;
		colorScream = PahomEngine->RGBA(r_offset, g_offset, b_offset, a_offset);
		r_offset += PahomEngine->math->random<float>(255) * ImGui::GetIO().DeltaTime;
		g_offset = 0;
		b_offset = 0;
		a_offset += 512 * ImGui::GetIO().DeltaTime;
		if (a_offset > 255) { a_offset = 255; }
		PahomEngine->setItemCenter(ImVec2(PahomEngine->i64WindowSize[0], PahomEngine->i64WindowSize[1]));
		ImGui::Image(PahomEngine->ptrint64_t(PahomEngine->ImageData.TextureArray[10]), ImVec2(PahomEngine->i64WindowSize[0], PahomEngine->i64WindowSize[1]), { 0,0 }, { 1,1 }, colorScream);
		PahomEngine->audio.play2(5);
		if (fTimerScream > 100.0) {
			PahomEngine->Event.bScreamEventBackground = true;
			PahomEngine->ShutdownEngine(); delete PahomEngine; exit(0);

		}

	}

	void MuteButton() {
		static int iMuted = 0;
		static bool isButtonHovered = false;
		ImGui::SetCursorPosY(10); ImGui::SetCursorPosX(PahomEngine->i64WindowSize[0] - 120);
		ImGui::TextColored((iMuted || isButtonHovered) ? PahomEngine->RGBA(100, 49, 12, 255) : PahomEngine->RGBA(255, 255, 255, 255), (iMuted ? "Не орёт" : "  Орёт "));
		isButtonHovered = ImGui::IsItemHovered();
		
		if (ImGui::IsItemClicked()) {
			iMuted++;
			if (iMuted > 1) {
				iMuted = 0;
			}
			if (iMuted) {
				PahomEngine->audio.Mute();
				PahomEngine->log(std::format("(audio) muted!"));
			}
			if (!iMuted && PahomEngine->audio.audioDevice || !iMuted && PahomEngine->audio.audioDevice2 || !iMuted && PahomEngine->audio.audioDevice3) {
				PahomEngine->audio.masterVolume = PahomEngine->PESettings->fMasterVolume;
				if (PahomEngine->audio.audioDevice) {
					PahomEngine->audio.audioDevice.setVolume(PahomEngine->audio.masterVolume);
					PahomEngine->audio.audioDevice.play();
				}
				if (PahomEngine->audio.audioDevice2) {
					PahomEngine->audio.audioDevice2.setVolume(PahomEngine->audio.masterVolume);
					/* if(PahomEngine->audio.sLastAudioFile[1] != PahomEngine->audio.audiolist[3])
					 {
						 PahomEngine->audio.audioDevice2.play();
					 }*/
					PahomEngine->audio.audioDevice2.play();
				}
				if (PahomEngine->audio.audioDevice3) {
					PahomEngine->audio.audioDevice3.play();
				}
				PahomEngine->log(std::format("(audio) unmuted!"));

			}
			PahomEngine->audio.isAudioMuted = iMuted;
		}
	}
	ImVec2 v2MousePos;
	HWND hCurrentHwnd;
	struct TimeAPI {
		double min = 0;
		double sec = 0;
		double hour = 0;
		void getCurrentTime(double dYouTimer) {
			static double time_ = dYouTimer;
			if (time_ > 60) { time_ = 0; }
			this->min = time_ / 60;
			this->hour = this->min / 60;
			this->sec = time_;
		}
	};

	/*
	
	breadMenu func.
	
    void breadObjectToMenu(bool v)  v = true  bread Show
	*/

	void breadObjectToMenu(bool v) {
		if (v) {
			PahomEngine->setItemCenterX(370);
			ImGui::Image(PahomEngine->ptrint64_t(PahomEngine->ImageData.TextureArray[2]), { 40,40 });
			ImGui::SameLine();
		}
		else {
			PahomEngine->setItemCenterX(280);
		}
	}
	bool isClickedMenu = false;
	void ResetAudioClickItem() {
		isClickedMenu = false;
	}
	void ClickItem() {
		if (!isClickedMenu) {
			PahomEngine->audio.play(11);
			isClickedMenu = true;
		}
	}
	TimeAPI PETime;

	void Editor() {
		PahomEngine->cast->bUsedStaticCast = true;
		ImGui::Begin("PERFOMANCE");
		static float fps_values[120] = {};
		static int offset = 0;

		float fps = ImGui::GetIO().Framerate;  // ← точнее, чем 1/DeltaTime

		// Записываем
		fps_values[offset] = fps;
		offset = (offset + 1) % 120;

		// Рисуем
		char overlay[32];
		static std::string sLogData;
		static int32_t iLogType = 0;
		sprintf(overlay, "%.0f FPS", fps);
		static int64_t i64CPUUsage = 0;
		ImGui::PlotLines("##fps_plot", fps_values, 120, offset, overlay,
			0.0f, 1000.0f, ImVec2(0, 80));
		ImGui::Checkbox("unlock fps", &PahomEngine->CVsync);
		static bool bCPUUsage = false;
		static int32_t i32CPUUsageSwitch = 0;
		static ULARGE_INTEGER ul_idle_old = { 0 };
		static ULARGE_INTEGER ul_kernel_old = { 0 };
		static ULARGE_INTEGER ul_user_old = { 0 };
		static float fcpu = 0.0f;
		static float d0 = 0, d1 = 0;
		if (ImGui::RadioButton("CPUUsage", bCPUUsage)) {
			i32CPUUsageSwitch = !i32CPUUsageSwitch;
			bCPUUsage = i32CPUUsageSwitch != 0;
		}
		if (ImGui::Button("move array")) {
			//PahomEngine->audio.cacheAudio(&PahomEngine->audio.audioDevice);
		}
		PahomEngine->Text("cpu_usage: {} %", fcpu);
		int totalSeconds = (int)ImGui::GetTime();

		int iSecond = totalSeconds % 60;
		int iMinute = (totalSeconds / 60) % 60;
		int iHour = (totalSeconds / 3600) % 24;

		PahomEngine->Text("get_time: {:02}:{:02}:{:02}", iHour, iMinute, iSecond);
		ImGui::SpinnerBar("CPU", static_cast<float>(fcpu / 100), 20, 4, ImGui::GetColorU32(PahomEngine->RGBA(255, 0, 150, 255)));
		randomKickBratishka();
		if (PahomEngine->math->isValueTrue<float>(d0, 800)) {
			d0 = 0;
		}
		else {
			d0++;
		}
		PahomEngine->Text("test isValueTrue({},800)->", d0, PahomEngine->math->isValueTrue<float>(d0, 800));
		if (bCPUUsage) {

			ULARGE_INTEGER ul_idle_new, ul_kernel_new, ul_user_new;

			FILETIME ft_idle, ft_kernel, ft_user;
			if (GetSystemTimes(&ft_idle, &ft_kernel, &ft_user)) {
				ul_idle_new.LowPart = ft_idle.dwLowDateTime;
				ul_idle_new.HighPart = ft_idle.dwHighDateTime;
				ul_kernel_new.LowPart = ft_kernel.dwLowDateTime;
				ul_kernel_new.HighPart = ft_kernel.dwHighDateTime;
				ul_user_new.LowPart = ft_user.dwLowDateTime;
				ul_user_new.HighPart = ft_user.dwHighDateTime;

				ULARGE_INTEGER ul_sys = { 0 };
				ul_sys.QuadPart = (ul_kernel_new.QuadPart - ul_kernel_old.QuadPart) +
					(ul_user_new.QuadPart - ul_user_old.QuadPart);
				ULARGE_INTEGER ul_idle = { 0 };
				ul_idle.QuadPart = ul_idle_new.QuadPart - ul_idle_old.QuadPart;

				int64_t i64CPUUsage = 0;
				if (ul_sys.QuadPart > 0) {
					i64CPUUsage = 100 - ((ul_idle.QuadPart * 100) / ul_sys.QuadPart);
				}

				fcpu = PahomEngine->cast->cast_all<float>(i64CPUUsage);

				ul_idle_old = ul_idle_new;
				ul_kernel_old = ul_kernel_new;
				ul_user_old = ul_user_new;
			}
		}
		else {
			fcpu = 0;
		}
		ImGui::InputText("LogTest", &sLogData);
		if (ImGui::Button("WARN")) {
			iLogType = 0;
		}
		ImGui::SameLine();
		if (ImGui::Button("INFO")) {
			iLogType = 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("ERR")) {
			iLogType = 2;
		}
		ImGui::SameLine();
		if (ImGui::Button("DEBUG")) {
			iLogType = 3;
		}
		if (ImGui::Button("Send")) {
			PahomEngine->log(sLogData, iLogType);
		}
		static bool bDrawBufferTest = false;
		if (ImGui::Button("draw_test")) {
			bDrawBufferTest = true;

		}
		static std::string mask_rgb = "rgb";
		static std::string colorNameRand = mask_rgb;
		static bool bColorsIsLoaded = false, bRenderSquare = false;
		static GLuint ScannedTexturesArray[11];

		ImGui::End();



	}

	void KefirEvent(int image_id) {
		static float fImageKefirOpacity = 0; static bool isReversed = false;
		static ImVec4 color = { 255,255,255,fImageKefirOpacity };
		if (PahomEngine->PESettings->bFlagEnableAnimationToImageFadeInOut) {
			if (PahomEngine->bBoost777 || !PahomEngine->bBoost777 && !PahomEngine->bKefir) {
				if (!isReversed)
				{
					fImageKefirOpacity += 200 * ImGui::GetIO().DeltaTime;
					if (fImageKefirOpacity > 255) { isReversed = true; }
				}
				else {
					fImageKefirOpacity -= 200 * ImGui::GetIO().DeltaTime;
					if (fImageKefirOpacity < 0) { isReversed = false; }
				}
				color = { 255,255,255,fImageKefirOpacity };
			}
			else {
				if (!isReversed)
				{
					fImageKefirOpacity += 200 * ImGui::GetIO().DeltaTime;
					if (fImageKefirOpacity > 255) { isReversed = true; }
				}
				else {
					fImageKefirOpacity -= 200 * ImGui::GetIO().DeltaTime;
					if (fImageKefirOpacity < 0) { isReversed = false; }
				}
				color = { 0,255,130,fImageKefirOpacity };
			}
		}
		else {
			color = { 255,255,255,fImageKefirOpacity };
			fImageKefirOpacity = 255;
		}
		// genParticles();
		ImGui::Image(PahomEngine->ptrint64_t(PahomEngine->ImageData.TextureArray[image_id]), { PahomEngine->cast->unicast<int64_t,float>(PahomEngine->i64BreadSize[0]),PahomEngine->cast->unicast<int64_t,float>(PahomEngine->i64BreadSize[0]) }, { 0,0 }, { 1, 1 }, PahomEngine->RGBA(color));
	}
	bool bDialog = false;
	void TestDialogs() {
		if (bDialog) {
			ImGui::Begin("dialog_frame", &bDialog, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground);

			ImGui::SetWindowSize({ 600,600 });
			static int32_t i32PlayerImageId = 0;
			static int32_t i32DialogId = 0;
			static bool bImageShow = false;

			std::string sDialog[] = {
				"Обосрался пидорас",
				"Я зеленый слоник веселый головастик",
				"Я люблю жрать говно"
			};
			static int32_t i32Player[] = {
				PAHOM2_IMAGE,
				PAHOM_IMAGE
			};
			ImGui::SetCursorPosY(600 - 100);
			ImGui::PushStyleColor(ImGuiCol_ChildBg, PahomEngine->RGBA(12, 12, 19, 200));
			if (ImGui::BeginChild("dialog_frame2", { 600,100 }, ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar))
			{
				PahomEngine->Text("{}", sDialog[i32DialogId]);
				i32PlayerImageId = !i32DialogId;
				if (ImGui::Button("След")) {
					i32DialogId++;
					if (i32DialogId > 2) {
						i32DialogId = 2;
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Назад")) {
					i32DialogId--;
					if (i32DialogId < 0) {
						i32DialogId = 0;
					}
				} ImGui::SameLine();
				if (ImGui::Button("bImageShow")) {
					bImageShow = !bImageShow;
				}
				ImGui::EndChild();
			}
			ImGui::PopStyleColor();
			if (bImageShow) {
				ImGui::SetCursorPosY(0);
				ImGui::Image(PahomEngine->ptrint64_t(PahomEngine->ImageData.TextureArray[1]), { 600,600 });
			}
			ImGui::SetCursorPosY(600 - 256);
			ImGui::Image(PahomEngine->ptrint64_t(PahomEngine->ImageData.TextureArray[i32Player[i32PlayerImageId]]), { 256,256 });



			ImGui::End();
		}

	}


};
void GameFrames::genParticles() {
	for (int gen = 0; gen < 3; gen++)
	{
		float random = PahomEngine->math->random<float>(10);
		ImVec2 size = { PahomEngine->math->random<float>(32) ,PahomEngine->math->random<float>(32) };
		ImGui::SetCursorPos({ PahomEngine->fBreadPosX + random,PahomEngine->fBreadPosY });
		ImGui::Image(PahomEngine->ImageData.GetImTexture(BREAD_IMAGE), size);
	}
}

bool bFillingTestOpenGL = false;
bool bPahomEngineAudioEditor = false;
bool bPahomEngineTextureEditor = false;
using Clock = std::chrono::high_resolution_clock;
void PahomEngineEditor(ImVec2 sizeMax, ImFont* fontSmall = nullptr) {
	static int64_t i64CurrentIndexAudio = 0, i64CurrentIndexAudioUser = 0;
	static float fPlayerUserSoundVolumeOffset = 0.02f;
	static std::string sOpenFile;
	static bool isWavAudio = false;
	static bool bPlayerUserSound = false, bFileOpened = false;
	if (!bPahomEngineTextureEditor && bPahomEngineAudioEditor) {
		ImGui::Begin("AudioEditor", &bPahomEngineAudioEditor, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);

		if (ImGui::BeginPopup("MusicsScan", ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize)) {
			static std::string sScanToPath = "musics";
			ImGui::InputText("Путь:", &sScanToPath); ImGui::SameLine(); if (ImGui::Button("Сканировать")) { PahomEngine->audio.ScanFiles(sScanToPath, (isWavAudio ? ".wav" : ".mp3")); } ImGui::SameLine(); ImGui::CustomToggle("is Wav Audio", &isWavAudio);
			PahomEngine->Text("Заменить: {} на {} id:{}", PahomEngine->audio.audiolist[i64CurrentIndexAudio], PahomEngine->audio.musicFiles.size() > 0 ? PahomEngine->audio.musicFiles[i64CurrentIndexAudioUser] : "Пусто", i64CurrentIndexAudioUser);
			if (ImGui::BeginChild("audioCurrent", { 300,400 }, ImGuiChildFlags_FrameStyle, ImGuiWindowFlags_NoTitleBar)) {
				if (PahomEngine->audio.musicFiles.size() > 0) {
					for (int index = 0; index < PahomEngine->audio.musicFiles.size(); index++) {
						ImGui::PushStyleColor(ImGuiCol_Button, (i64CurrentIndexAudioUser == index ? PahomEngine->RGBA(100, 110, 246, 255) : PahomEngine->RGBA(0, 0, 0, 0)));
						ImGui::PushStyleColor(ImGuiCol_Border, PahomEngine->RGBA(0, 0, 0, 0));
						if (ImGui::Button(PahomEngine->audio.musicFiles[index].c_str(), { 300,30 })) {
							i64CurrentIndexAudioUser = index;
							PahomEngine->audio.audioDevice2.loadSound(PahomEngine->audio.musicFiles[i64CurrentIndexAudioUser]);
						}

						ImGui::ImLine(300, 1, PahomEngine->RGBA(12, 12, 19, 255));
					}
					ImGui::PopStyleColor(PahomEngine->audio.musicFiles.size() * 2);
				}
				ImGui::EndChild();

			}
			ImGui::SameLine();
			if (ImGui::Button("Заменить файлы")) {
				PahomEngine->audio.audiolist[i64CurrentIndexAudio] = PahomEngine->audio.musicFiles[i64CurrentIndexAudioUser];
			}

			if (ImGui::Button("Открыть файл")) {
				PahomEngine->audio.openFileDialog(sOpenFile);
				bFileOpened = !bFileOpened;
			}

			ImGui::SameLine();
			if (bFileOpened) {
				//if (ImGui::BeginChild("#player", { 200,100 })) {
				//    ImGui::SetCursorPos({ 200 - ImGui::CalcTextSize(std::format("{}",PahomEngine->audio.musicFiles[i64CurrentIndexAudioUser]).c_str()).x,30 });
				//    PahomEngine->Text("{}", PahomEngine->audio.musicFiles[i64CurrentIndexAudioUser]);
				//    PahomEngine->Text("{}", PahomEngine->audio.audioTime->formatTime(PahomEngine->audio.audioTime->current));
				//    PahomEngine->Text("{}", PahomEngine->audio.audioTime->formatTime(PahomEngine->audio.audioTime->getDuration(PahomEngine->audio.audioDevice2)));
				//    ImGui::EndChild();
				//}
				if (ImGui::Button("Заменить")) {

					PahomEngine->audio.audiolist[i64CurrentIndexAudio] = sOpenFile;
					bFileOpened = false;
				}
				PahomEngine->Text("Заменить: {}\nна загруженный файл: {}", PahomEngine->audio.audiolist[i64CurrentIndexAudio], sOpenFile);
			}

			if (ImGui::Button("Прослушать")) {

				bPlayerUserSound = !bPlayerUserSound;
				if (PahomEngine->audio.audioDevice2) {
					if (PahomEngine->audio.musicFiles.size() > 0) {
						if (bPlayerUserSound)
							PahomEngine->audio.audioDevice2.play();
						if (!bPlayerUserSound)
							PahomEngine->audio.audioDevice2.pause();
					}

				}
			}
			if (fontSmall) {
				ImGui::PushFont(fontSmall);
				ImGui::SetNextItemWidth(100);
				ImGui::SameLine();
				if (ImGui::SliderFloat("Громкость", &fPlayerUserSoundVolumeOffset, 0.00f, 1.0f)) {
					if (PahomEngine->audio.audioDevice2) {
						PahomEngine->audio.audioDevice2.setVolume(fPlayerUserSoundVolumeOffset);
					}
				}
				ImGui::PopFont();
			}
			else {
				ImGui::SetNextItemWidth(100);
				ImGui::SameLine();
				if (ImGui::SliderFloat("Громкость", &fPlayerUserSoundVolumeOffset, 0.00f, 1.0f)) {
					if (PahomEngine->audio.audioDevice2) {
						PahomEngine->audio.audioDevice2.setVolume(fPlayerUserSoundVolumeOffset);
					}
				}
			}

			ImGui::EndPopup();
		}
		static float fLineMinPosX = 0;
		ImGui::SetWindowPos({ 0,0 });
		ImGui::SetWindowSize(sizeMax);

		if (ImGui::Button("Сканировать")) {
			ImGui::OpenPopup("MusicsScan");
		}
		ImGui::SameLine();
		if (ImGui::Button("Выход")) {
			bPahomEngineAudioEditor = false;
			PahomEngine->Event.bTextureEditor = false;
		}
		ImGui::ImLine(sizeMax.x - 40, 4, PahomEngine->RGBA(35, 35, 55, 255));
		PahomEngine->Text("Выбран: {} id:{}", PahomEngine->audio.audiolist[i64CurrentIndexAudio], i64CurrentIndexAudio);
		if (ImGui::BeginChild("audioCurrent", { 300,sizeMax.y - 100 }, ImGuiChildFlags_FrameStyle, ImGuiWindowFlags_NoTitleBar)) {
			for (int index = 0; index < PE_ARRAYSIZE(PahomEngine->audio.audiolist); index++) {
				ImGui::PushStyleColor(ImGuiCol_Button, (i64CurrentIndexAudio == index ? PahomEngine->RGBA(100, 110, 246, 255) : PahomEngine->RGBA(0, 0, 0, 0)));
				ImGui::PushStyleColor(ImGuiCol_Border, PahomEngine->RGBA(0, 0, 0, 0));
				if (ImGui::Button(PahomEngine->audio.audiolist[index].c_str(), { 300,30 })) {
					i64CurrentIndexAudio = index;
				}

				ImGui::ImLine(300, 1, PahomEngine->RGBA(12, 12, 19, 255));
			}
			ImGui::PopStyleColor(PE_ARRAYSIZE(PahomEngine->audio.audiolist) * 2);
			ImGui::EndChild();
		}
		ImGui::End();
	}
	static  colorU32 colors;
	static uint32_t currentColorU32;
	int32_t i32SameLineCount = 0;
	static int32_t i32SelectedTexture = 0;
	static uint32_t u32data = 0, tmax = 1;
	static int64_t idx_image = 0;
	static bool bBenchCPU = false, bSelectorTextures = false, bEraseColors = false;
	static GLuint SelectedTexture;
	static double dScoreTimeToFill = 0;
	if (bPahomEngineTextureEditor && !bPahomEngineAudioEditor) {
		ImGui::Begin("OGL Draw", &bPahomEngineTextureEditor, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
		PahomEngine->ogl->SelectTextureToSwapUI(PahomEngine->ImageData.TextureArray, 11, PahomEngine->ImageData.TextureBufferArray);
		ImGui::SetWindowPos({ 0,0 });
		ImGui::SetWindowSize(sizeMax);

		static std::string sImageSaveFile = "image.png";
		if (ImGui::BeginPopup("#save_image", ImGuiWindowFlags_NoTitleBar)) {
			ImGui::Text("Сохранение файла");
			ImGui::Separator();
			ImGui::InputText("Путь", &sImageSaveFile);
			ImGui::Text("Размер: %d x %d", PahomEngine->ogl->width_texture, PahomEngine->ogl->height_texture);
			if (ImGui::Button("Сохранить", { 150, 30 })) {
				PahomEngine->ogl->savePng(sImageSaveFile);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Отмена", { 150, 30 })) {
				ImGui::CloseCurrentPopup();
			}
		
			ImGui::EndPopup();
		}
		if (ImGui::BeginPopup("#бенчмарк")) {
			
			static int iSplitCount = 0;
			static int iImageId = 0;
			static uint8_t* u8PushedTexture = 0;
			static GLuint giPushedTexture = 0;
			static std::vector<int> iImages;
			static int iTsizeX = 250, iTsizeY = 250;
			static double dElp = 0.0;
			double dTotalBytes = 1024000.0 * 100; // или pt.u8bufferM.size() после цикла
			double dMbps = (dTotalBytes / (1024.0 * 1024.0)) / dElp;
			if (ImGui::Button((!pt.isUsedMemcpy() ? "byte2byte":"Use Memcpy"), { 120,30 })) {
				pt.useMemcpy();
			}
			PahomEngine->Text("KefirBomb elp: {:.6f} s {:.3f} mb/s", dElp, dMbps);
			for (int i = 0; i < std::size(PahomEngine->assets.asset); ++i) {
				ImGui::Image(
					PahomEngine->ptrint64_t(PahomEngine->ImageData.TextureArray[i]),
					{ 64,64 }
				);
				PahomEngine->selectedItem((i == iImageId));
				if ((i % 4) != 3 && i < std::size(PahomEngine->assets.asset) - 1) {
					ImGui::SameLine();
				}
				
				if (ImGui::IsItemClicked()) {
					iImageId = i;
				}
			}
			if (ImGui::Button("Add", { 120, 30 })) {
				iImages.push_back(iImageId);
			}
			ImGui::SameLine();
			if (ImGui::Button("KefirBomb", { 120, 30 })) {
				pt.u8bufferM.clear();
				
				iTsizeX = 0;
				iTsizeY = 0;

				int iImageIDK = 5;
				auto tBenchIn = Clock::now();
				for (int _k = 0; _k < 20; ++_k) {
					uint8_t* u8Ptr = (uint8_t*)(void*)(PahomEngine->ImageData.TextureBufferArray[iImageIDK]);
					if (u8Ptr != nullptr) {
						int w = PahomEngine->ImageData.TextureX[iImageIDK];
						int h = PahomEngine->ImageData.TextureY[iImageIDK];
						int64_t i64Psz = PahomEngine->img->GetImageSize(w, h);

						iTsizeX += w;
						iTsizeY = h;

						pt.set_buffer(u8Ptr, i64Psz);
					}
				}
				u8PushedTexture = pt.Pushed();
				auto tBenchOut = Clock::now();
				dElp = PahomEngine->TimeElap(tBenchIn, tBenchOut);
				glpx.InitTextureV2(u8PushedTexture, giPushedTexture, &iTsizeX, &iTsizeY);
				//iImages.clear();
			}
			if (ImGui::Button("Pusher", { 120, 30 })) {
				pt.u8bufferM.clear();
				iTsizeX = 0;
				iTsizeY = 0;

				for (auto& u8t : iImages) {
					uint8_t* u8Ptr = (uint8_t*)(void*)(PahomEngine->ImageData.TextureBufferArray[u8t]);
					if (u8Ptr != nullptr) {
						int w = PahomEngine->ImageData.TextureX[u8t];
						int h = PahomEngine->ImageData.TextureY[u8t];
						int64_t i64Psz = PahomEngine->img->GetImageSize(w, h);

						iTsizeX += w;
						iTsizeY = h;

						pt.set_buffer(u8Ptr, i64Psz);
					}
				}

				u8PushedTexture = pt.Pushed();
				glpx.InitTextureV2(u8PushedTexture, giPushedTexture, &iTsizeX, &iTsizeY);
				iImages.clear();
			}
			ImGui::SameLine();
			if (ImGui::Button("Swap", { 120, 30 })) {
				PahomEngine->ogl->pixel_buffer.clear();
				pt.swap(PahomEngine->ogl->pixel_buffer);
				PahomEngine->ogl->UpdateTexture();
			}
			static double dElpFill = 0.0;
			if (ImGui::Button("FillRedScreen", { 120,30 })) {
				uint32_t u32RedColor = colorU32(255, 0, 0, 125).get();
				auto tIn = Clock::now();
				PahomEngine->ogl->FillTexture(u32RedColor,{1920,1080});
				auto tOut = Clock::now();
				dElpFill = PahomEngine->TimeElap(tIn, tOut);
			}
			PahomEngine->Text("Залито за {:.9f}s", dElpFill);
			PahomEngine->Text("Добавлено: {}", iImages.size());
			PahomEngine->Text("{}x{}", iTsizeX, iTsizeY);
			ImGui::Image(PahomEngine->ptrint64_t(giPushedTexture), { 256,256 });
			if (ImGui::Button("exit", { 250,30 })) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		if (ImGui::BeginPopup("set_cpu_test", ImGuiWindowFlags_NoTitleBar)) {
			bFillingTestOpenGL = true;
			static int x = 320, y = 240;
			static uint32_t i32MaxThreads = std::jthread::hardware_concurrency();
			ImGui::SetCursorPosX(30); PahomEngine->Text("max_thread: {}", i32MaxThreads);
			ImGui::SetCursorPosX(30); PahomEngine->Text("size_x: {}", x);
			ImGui::SetCursorPosX(30); PahomEngine->Text("size_y: {}", y);
			ImGui::SetCursorPosX(30); PahomEngine->TextColored(PahomEngine->RGBA({ 255,200,0,255 }), "Score: {:.4f} ms", dScoreTimeToFill);
			ImGui::SetCursorPosX(30); ImGui::SetNextItemWidth(150); ImGui::InputInt("x_size:", &x, 1, 100); ImGui::SameLine(); ImGui::SetNextItemWidth(150); ImGui::InputInt("y_size:", &y, 1, 100);
			ImGui::SetCursorPosX(30); ImGui::SetNextItemWidth(150); if (ImGui::SliderU32("MaxThreads:", &tmax, 1, i32MaxThreads)) {
				glpx.SetSize(x, y);
				glpx.InitTexture();
			}
			ImGui::SetCursorPosX(7);
			if (ImGui::Button("Start Bench", { 300,40 })) {
				bBenchCPU = true;
				bStopBench = false;
				//auto in_fill_time = std::chrono::high_resolution_clock::now();
				glpx.fillSqware(x, y, true, (tmax < 1 ? std::jthread::hardware_concurrency() : tmax));
				// auto out_fill_time = std::chrono::high_resolution_clock::now();
				dScoreTimeToFill = glpx.g_dElapsed.load();
				bFillingTestOpenGL = false;
				glpx.savePng("noise_rgb.png");

			}
			if (bBenchCPU) {

				FillBenchCPU(0, 0, tmax);

			}
			ImGui::EndPopup();
		}
		if (fontSmall)
		{
			ImGui::PushFont(fontSmall);
		}
		else {
			static bool isFontNullptr = true;
			if (isFontNullptr) {
				PahomEngine->log("(DRAW Textures Frame) Set Font it's nullptr!!", 2);
				PahomEngine->log(std::format("Test Cast uint64_t to uint8_t console.setVal<uint64_t>(30, 30, 30).getHex()={}", tui->setVal<uint64_t>(30, 30, 30).getHex()), 3);
				isFontNullptr = false;
			}
		}
		PahomEngine->Text("alloc_texture_size:{} | x:{} y:{} color:{}", PahomEngine->ogl->pixel_buffer.size(), PahomEngine->ogl->width_texture, PahomEngine->ogl->height_texture, colors.toStringView());

		if (ImGui::Button("Bench SpawnTex", { 150,50 })) {
			ImGui::OpenPopup("#бенчмарк");
		}

		ImGui::SameLine();
		if (ImGui::Button("Создать\nИзображение", { 100,50 })) {
			PahomEngine->ogl->SetSize(512, 512);
			PahomEngine->ogl->InitTexture();
		}
		ImGui::SameLine();
		if (ImGui::Button("Выбрать\nтекстуру", { 100,50 })) {
			bSelectorTextures = !bSelectorTextures;
			ImGui::OpenPopup("SelectToTexture");

		}
		ImGui::SameLine();
		if (ImGui::Button("Тест Заполнения\n(Заливка OpenGL)", { 150,50 })) {
			ImGui::OpenPopup("set_cpu_test");
		}
		ImGui::SameLine();
		static ImVec4 colorIv4 = {};
		// color editor
		ImGui::ColorEdit3("Цвет Кисти", reinterpret_cast<float*>(&colorIv4), ImGuiColorEditFlags_NoInputs);
		colors = colorU32(static_cast<uint32_t>(colorIv4.x * 255), static_cast<uint32_t>(colorIv4.y * 255), static_cast<uint32_t>(colorIv4.z * 255), 255);
		currentColorU32 = colors.get();
		//

		ImGui::SameLine();
		if (ImGui::Button("Ластик", { 100,40 })) {
			bEraseColors = !bEraseColors;
		}
		ImGui::SameLine();
		if (ImGui::Button("Вставить", { 100,40 })) {
			PahomEngine->ogl->printImage();
		}
		ImGui::SameLine();
		if (ImGui::Button("X", { 40,40 })) {

			bPahomEngineTextureEditor = false;
			PahomEngine->Event.bTextureEditor = false;
		}
		PahomEngine->setItemCenterX(150, ImGui::GetWindowSize().x);
		if (ImGui::Button("Сохранить в (*.png)", { 150,30 })) {
			ImGui::OpenPopup("#save_image");
		}
		ImGui::SliderInt64("Толщина", &PahomEngine->ogl->i64BrushSize, 1, 100, "%lld", 0);
		ImGui::SetCursorPos({
			(ImGui::GetWindowSize().x - 512) / 2,
			 (ImGui::GetWindowSize().y - 512) / 2
			});
		if (fontSmall)
		{
			ImGui::PopFont();
		}
		ImGui::Image(PahomEngine->ptrint64_t(PahomEngine->ogl->textureID), { 512,512 }, { 0,0 }, { 1,1 }, { 1,1,1,1 }, { 1,1,1,1 });
		/* PahomEngine->setItemCenterX(ImGui::CalcTextSize(std::format("colorHEX: {}", colors.HEX()).c_str()).x,ImGui::GetWindowSize().x);
		 PahomEngine->Text("colorHEX: {}", colors.HEX());*/

		ImVec2 rectTex = ImGui::GetItemRectMin();
		ImVec2 rectMax = ImGui::GetItemRectMax();
		if (ImGui::IsItemHovered()) // Проверяем, наведен ли курсор на текстуру
		{
			ImVec2 canvas_pos = rectTex; // Позиция текстуры на экране
			ImVec2 mouse_pos = ImGui::GetIO().MousePos;// Абсолютная позиция мыши

			int draw_x = static_cast<int>(mouse_pos.x - canvas_pos.x);
			int draw_y = static_cast<int>(mouse_pos.y - canvas_pos.y);
			if (draw_x >= 0 && draw_x < rectMax.x && draw_y >= 0 && draw_y < rectMax.y) {
				int brush_size = PahomEngine->ogl->i64BrushSize;
				int half_brush = brush_size / 2;
				if (ImGui::IsKeyPressed(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_MouseRight) || bEraseColors) {
					currentColorU32 = 0;
					for (int x = draw_x - half_brush; x < draw_x + half_brush; x++) {
						for (int y = draw_y - half_brush; y < draw_y + half_brush; y++) {
							if (x >= 0 && x < PahomEngine->ogl->width_texture &&
								y >= 0 && y < PahomEngine->ogl->height_texture)
							{
								// PahomEngine->ogl->getCoordPixel(x, y);
								PahomEngine->ogl->SetPixel(x, y, currentColorU32);
							}
						}
					}
				}
				if (ImGui::IsKeyPressed(ImGuiKey_MouseLeft) && !bSelectorTextures || ImGui::IsMouseDown(ImGuiMouseButton_Left) && !bSelectorTextures) {
					for (int x = draw_x - half_brush; x < draw_x + half_brush; x++) {
						for (int y = draw_y - half_brush; y < draw_y + half_brush; y++) {
							if (x >= 0 && x < PahomEngine->ogl->width_texture &&
								y >= 0 && y < PahomEngine->ogl->height_texture)
							{
								PahomEngine->ogl->SetPixel(x, y, currentColorU32);
							}
						}
					}
				}
			}
		}
		PahomEngine->ogl->UpdateTexture();


		ImGui::End();
	}
}
void  GameFrames::randomKickBratishka() {

	static ImVec2 BratishkaSize = { 256,256 };
	GLuint uMainTexture = PahomEngine->ImageData.TextureArray[13];
	GLuint uSpritesToAnimation[5] = {};
	int fAngle = 90;
	floatV3 pos_image = { 20,20,50 };// float vec3
	MultiVectors4V2 MtVec42 = { {220,330},{220,440},{550,220},{330,440} };// Multi-vector based Vec4 = {ImVec2,ImVec2,ImVec2,ImVec2}
	MultiVectors3V2 MtVec32 = { {220,330},{220,440},{550,220} };// Multi-vector based Vec4 = {ImVec2,ImVec2,ImVec2}
	MultiVectors2V2 MtVec22 = { {220,330},{220,440} };// Multi-vector based Vec2 = {ImVec2,ImVec2}
	UniVec2<std::string> stringVec2 = { "ffdfd","assas" };// UniVec2 template vector to all same
	UniVec2<float> fVec2 = { 0.0f,55.0f };// UniVec2 template vector to all same
	UniVec2<int64_t> i64Vec2 = { 0L,55L };// UniVec2 template vector to all same
	UniVec2<UniVec2<d64Vec2>> vc42 = { {{3.43543,5.3453} ,{5.453453,5.5656353}},{{3.52426532,5.564645353},{5.76762424,6.455434545}} };
	d64Vec2 d64v2 = { 5.454354,4.21342 };
	d64Vec2 double64Vec2 = { 0.0444545454342,0.4555555555555 };// my vector to work double64_t MSVC C++ Compiler
	ColorV3 colorV3 = { 234,233,233 }; // tui.hpp my console lib to engine
	static int _step_value = 0;
	_step_value = 180;

	ImGui::ImageRotated(PahomEngine->ptrint64_t(PahomEngine->ImageData.TextureArray[PAHOM_IMAGE]), BratishkaSize, _step_value, { 0,0 }, { 1,1 }, { 1,1,1,1 }, { 0,0,0,0 });
	PahomEngine->Text("d64Vec2 size {} UniVec2<UniVec2<d64Vec2>> size: {}", d64v2.size(), vc42.size());
	int64_t data2 = 654LL;
	double64_t data = PahomEngine->cast->unicast<int64_t, double64_t>(data2);

}
auto Game = std::make_unique<GameFrames>();


