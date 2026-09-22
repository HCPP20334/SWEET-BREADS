 
 #include "imgui.h"
 #include "imgui_internal.h"
namespace ImGui {
    float CircleAxisXY(const char* label, float v, float v1) {
        ImGui::BeginGroup(); // Группируем элементы
        ImVec2 canvasSize = ImVec2(128, 128);

        // Добавляем текст и получаем позицию canvas
        ImGui::Text("%s\nx:%.3f\ny:%.3f", label, v, v1);
        ImVec2 canvasPos = ImGui::GetCursorScreenPos();

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImU32 border_color = ImGui::GetColorU32(ImGuiCol_Border);
        ImGuiStyle& style = ImGui::GetStyle();

        // Отрисовка фона и границы
        float radius = canvasSize.x * 0.5f;
        ImVec2 center = ImVec2(canvasPos.x + radius, canvasPos.y + radius);

        // Фоновый круг
        drawList->AddCircleFilled(
            center,
            radius - style.FrameBorderSize,
            IM_COL32(0, 0, 0, 0)
        );

        // Граница (рисуем круг вместо прямоугольника)
        if (style.FrameBorderSize > 0.0f) {
            drawList->AddCircle(
                center,
                radius - style.FrameBorderSize * 0.5f,
                border_color,
                0, // Автоматическое количество сегментов
                style.FrameBorderSize
            );
        }

        // Положение стика (предполагаем, что v - нормализованное значение [-1, 1])
        ImVec2 stickPos = ImVec2(
            center.x + v * (radius - 10.0f), // Учитываем размер маркера
            center.y + v1 * (radius - 10.0f)
        );

        // Маркер стика
        drawList->AddCircleFilled(
            stickPos,
            10.0f,
            ImGui::GetColorU32(ImVec4(0.34f, 0.06f, 0.98f, 1.00f))
        );

        // Перемещаем курсор для следующих элементов ImGui
        ImGui::Dummy(canvasSize);
        ImGui::EndGroup();

        return 0;
    }
};
 struct GamepadAPI{
    float fB_a = 0.0f;
    float fB_b = 0.0f;
    float fB_x = 0.0f;
    float fB_y = 0.0f;
    float fB_rt = 0.0f;
    float fB_rb = 0.0f;
    float fB_lb = 0.0f;
    float fB_lt = 0.0f;
    float fB_du = 0.0f;
    float fB_dd = 0.0f;
    float fB_dl = 0.0f;
    float fB_dr = 0.0f;
    float fB_ls = 0.0f;
    float fB_rs = 0.0f;
    float fLX = 0;
    float fLY = 0;
    float fRX = 0;
    float fRY = 0;
    float fLT = 0;
    float fRT = 0;
    bool b_Start = false;
    bool b_Back = false;
 };
 auto Gamepad = std::make_unique<GamepadAPI>();

 // shit func
void traceGamepadAPI(){
 ImGuiKeyData* fLS_left = ImGui::GetKeyData(ImGuiKey_GamepadLStickLeft);
 ImGuiKeyData* fLS_Right = ImGui::GetKeyData(ImGuiKey_GamepadLStickRight);
 ImGuiKeyData* fLS_Up = ImGui::GetKeyData(ImGuiKey_GamepadLStickUp);
 ImGuiKeyData* fLS_Down = ImGui::GetKeyData(ImGuiKey_GamepadLStickDown);
 //
 ImGuiKeyData* fRS_left = ImGui::GetKeyData(ImGuiKey_GamepadRStickLeft);
 ImGuiKeyData* fRS_Right = ImGui::GetKeyData(ImGuiKey_GamepadRStickRight);
 ImGuiKeyData* fRS_Up = ImGui::GetKeyData(ImGuiKey_GamepadRStickUp);
 ImGuiKeyData* fRS_Down = ImGui::GetKeyData(ImGuiKey_GamepadRStickDown);
 ImGuiKeyData* fLTv = ImGui::GetKeyData(ImGuiKey_GamepadL2);
 ImGuiKeyData* fRTv = ImGui::GetKeyData(ImGuiKey_GamepadR2);
 Gamepad->fLX = fLS_Right->AnalogValue - fLS_left->AnalogValue;
 Gamepad->fLY = fLS_Down->AnalogValue - fLS_Up->AnalogValue;
 Gamepad->fRY = fRS_Down->AnalogValue - fRS_Up->AnalogValue;
 Gamepad->fRX = fRS_Right->AnalogValue - fRS_left->AnalogValue;
 fRTv->Down ? Gamepad->fRT += 10 : Gamepad->fRT -= 10;
 fLTv->Down ? Gamepad->fLT += 10 : Gamepad->fLT -= 10;

 if (Gamepad->fRT > 255) {
     Gamepad->fRT = 255;
 }
 if (Gamepad->fLT > 255) {
     Gamepad->fLT = 255;
 }
 if (Gamepad->fRT <= 0) {
     Gamepad->fRT = 0;
 }
 if (Gamepad->fLT <= 0) {
     Gamepad->fLT = 0;
 }
 if ( ImGui::IsKeyDown(ImGuiKey_GamepadFaceRight)) { Gamepad->fB_b = Gamepad->fB_b + 0.01f; if (Gamepad->fB_b > 1.000f) { Gamepad->fB_b = 1.000f; } }
 if ( ImGui::IsKeyDown(ImGuiKey_GamepadFaceDown)) { Gamepad->fB_a = Gamepad->fB_a + 0.01f; if (Gamepad->fB_a > 1.000f) { Gamepad->fB_a = 1.000f; } }
 if ( ImGui::IsKeyDown(ImGuiKey_GamepadFaceLeft)) { Gamepad->fB_x = Gamepad->fB_x + 0.01f; if (Gamepad->fB_x > 1.000f) { Gamepad->fB_x = 1.000f; } }
 if ( ImGui::IsKeyDown(ImGuiKey_GamepadFaceUp)) { Gamepad->fB_y = Gamepad->fB_y + 0.01f; if (Gamepad->fB_y > 1.000f) { Gamepad->fB_y = 1.000f; } }
 if ( ImGui::IsKeyDown(ImGuiKey_GamepadR1)) { Gamepad->fB_rs = Gamepad->fB_rs + 0.01f; if (Gamepad->fB_rs > 1.000f) { Gamepad->fB_rs = 1.000f; } }
 if ( ImGui::IsKeyDown(ImGuiKey_GamepadL1)) { Gamepad->fB_ls = Gamepad->fB_ls + 0.01f; if (Gamepad->fB_ls > 1.000f) { Gamepad->fB_ls = 1.000f; } }
 if ( ImGui::IsKeyDown(ImGuiKey_GamepadDpadUp)) { Gamepad->fB_du = Gamepad->fB_du + 0.01f; if (Gamepad->fB_du > 1.000f) { Gamepad->fB_du = 1.000f; } }
 if ( ImGui::IsKeyDown(ImGuiKey_GamepadDpadDown)) { Gamepad->fB_dd = Gamepad->fB_dd + 0.01f; if (Gamepad->fB_dd > 1.000f) { Gamepad->fB_dd = 1.000f; } }
 if ( ImGui::IsKeyDown(ImGuiKey_GamepadDpadLeft)) { Gamepad->fB_dl = Gamepad->fB_dl + 0.01f; if (Gamepad->fB_dl > 1.000f) { Gamepad->fB_dl = 1.000f; } }
 if ( ImGui::IsKeyDown(ImGuiKey_GamepadDpadRight)) { Gamepad->fB_dr = Gamepad->fB_dr + 0.01f; if (Gamepad->fB_dr > 1.000f) { Gamepad->fB_dr = 1.000f; } }
 if ( ImGui::IsKeyDown(ImGuiKey_GamepadStart)) { Gamepad->b_Start = !Gamepad->b_Start; }
 if ( ImGui::IsKeyDown(ImGuiKey_GamepadBack)) { Gamepad->b_Back = !Gamepad->b_Back; }
 //
 if (! ImGui::IsKeyDown(ImGuiKey_GamepadFaceRight)) { Gamepad->fB_b = Gamepad->fB_b - 0.01f; if (Gamepad->fB_b < 0.1f) { Gamepad->fB_b = 0.0f; } }
 if (! ImGui::IsKeyDown(ImGuiKey_GamepadFaceDown)) { Gamepad->fB_a = Gamepad->fB_a - 0.01f; if (Gamepad->fB_a < 0.1f) { Gamepad->fB_a = 0.0f; } }
 if (! ImGui::IsKeyDown(ImGuiKey_GamepadFaceLeft)) { Gamepad->fB_x = Gamepad->fB_x - 0.01f; if (Gamepad->fB_x < 0.1f) { Gamepad->fB_x = 0.0f; } }
 if (! ImGui::IsKeyDown(ImGuiKey_GamepadFaceUp)) { Gamepad->fB_y = Gamepad->fB_y - 0.01f; if (Gamepad->fB_y < 0.1f) { Gamepad->fB_y = 0.0f; } }
 if (! ImGui::IsKeyDown(ImGuiKey_GamepadR1)) { Gamepad->fB_rs = Gamepad->fB_rs - 0.01f; if (Gamepad->fB_rs < 0.1f) { Gamepad->fB_rs = 0.0f; } }
 if (! ImGui::IsKeyDown(ImGuiKey_GamepadL1)) { Gamepad->fB_ls = Gamepad->fB_ls - 0.01f; if (Gamepad->fB_ls < 0.1f) { Gamepad->fB_ls = 0.0f; } }
 if (! ImGui::IsKeyDown(ImGuiKey_GamepadDpadUp)) { Gamepad->fB_du = Gamepad->fB_du - 0.01f; if (Gamepad->fB_du < 0.1f) { Gamepad->fB_du = 0.0f; } }
 if (! ImGui::IsKeyDown(ImGuiKey_GamepadDpadDown)) { Gamepad->fB_dd = Gamepad->fB_dd - 0.01f; if (Gamepad->fB_dd < 0.1f) { Gamepad->fB_dd = 0.0f; } }
 if (! ImGui::IsKeyDown(ImGuiKey_GamepadDpadLeft)) { Gamepad->fB_dl = Gamepad->fB_dl - 0.01f; if (Gamepad->fB_dl < 0.1f) { Gamepad->fB_dl = 0.0f; } }
 if (! ImGui::IsKeyDown(ImGuiKey_GamepadDpadRight)) { Gamepad->fB_dr = Gamepad->fB_dr - 0.01f; if (Gamepad->fB_dr < 0.1f) { Gamepad->fB_dr = 0.0f; } }
}
//  Обработчик Нажатий Геймпада ImGui
//  Параметр u8Key принимает Xinput Макросы кнопок
//   Например :
//   if (getKeyPresed(XINPUT_GAMEPAD_A)) {
//       std::cout << "Нажата Кнопка А\n"
//   }
std::string sPresedKeyGamepad;
bool bEnableImGuiAPIGamepad = true;
void log(std::string f) {
    std::cout << f << std::flush;
}
bool getKeyPresed(DWORD u8key) {
    ImGuiKey key;
    std::string sKey;
    if (bEnableImGuiAPIGamepad) {
        switch (u8key) {
        case XINPUT_GAMEPAD_A:
            key = ImGuiKey_GamepadFaceDown;
            sKey = "button_A";
            break;
        case XINPUT_GAMEPAD_B:
            key = ImGuiKey_GamepadFaceRight;
            sKey = "button_B";
            break;
        case XINPUT_GAMEPAD_X:
            key = ImGuiKey_GamepadFaceLeft;
            sKey = "button_X";
            break;
        case XINPUT_GAMEPAD_Y:
            key = ImGuiKey_GamepadFaceUp;
            sKey = "button_Y";
            break;
        case XINPUT_GAMEPAD_DPAD_UP:
            key = ImGuiKey_GamepadDpadUp;
            sKey = "button_dpad_up";
            break;
        case XINPUT_GAMEPAD_DPAD_DOWN:
            key = ImGuiKey_GamepadDpadDown;
            sKey = "button_dpad_down";
            break;
        case XINPUT_GAMEPAD_DPAD_LEFT:
            key = ImGuiKey_GamepadDpadLeft;
            sKey = "button_dpad_left";
            break;
        case XINPUT_GAMEPAD_DPAD_RIGHT:
            key = ImGuiKey_GamepadDpadRight;
            sKey = "button_dpad_right";
            break;
        case XINPUT_GAMEPAD_LEFT_THUMB:
            key = ImGuiKey_GamepadL2;
            sKey = "button_LT";
            break;
        case XINPUT_GAMEPAD_RIGHT_THUMB:
            key = ImGuiKey_GamepadR2;
            sKey = "button_RT";
            break;
        case XINPUT_GAMEPAD_LEFT_SHOULDER:
            key = ImGuiKey_GamepadL1;
            sKey = "button_LS";
            break;
        case XINPUT_GAMEPAD_RIGHT_SHOULDER:
            key = ImGuiKey_GamepadR1;
            sKey = "button_RS";
            break;
        case XINPUT_GAMEPAD_START:
            key = ImGuiKey_GamepadStart;
            sKey = "button_START";
            break;
        case XINPUT_GAMEPAD_BACK:
            key = ImGuiKey_GamepadBack;
            sKey = "button_BACK";
            break;
        default:
            key = ImGuiKey_None;
            sPresedKeyGamepad = "No Pressed";
            break;
        }
        if (ImGui::IsKeyDown(key)) {
            sPresedKeyGamepad = sKey;
            return true;
        }
        else {
            return false;
        }
    }
}
ImVec4 getAxisGamepad() {
    ImGuiKeyData* fLS_left = ImGui::GetKeyData(ImGuiKey_GamepadLStickLeft);
    ImGuiKeyData* fLS_Right = ImGui::GetKeyData(ImGuiKey_GamepadLStickRight);
    ImGuiKeyData* fLS_Up = ImGui::GetKeyData(ImGuiKey_GamepadLStickUp);
    ImGuiKeyData* fLS_Down = ImGui::GetKeyData(ImGuiKey_GamepadLStickDown);
    //
    ImGuiKeyData* fRS_left = ImGui::GetKeyData(ImGuiKey_GamepadRStickLeft);
    ImGuiKeyData* fRS_Right = ImGui::GetKeyData(ImGuiKey_GamepadRStickRight);
    ImGuiKeyData* fRS_Up = ImGui::GetKeyData(ImGuiKey_GamepadRStickUp);
    ImGuiKeyData* fRS_Down = ImGui::GetKeyData(ImGuiKey_GamepadRStickDown);
    float fLAxisXOffset = fLS_Right->AnalogValue - fLS_left->AnalogValue;
    float fLAxisYOffset = fLS_Down->AnalogValue - fLS_Up->AnalogValue;
    float fRAxisXOffset = fRS_Down->AnalogValue - fRS_Up->AnalogValue;
    float fRAxisYOffset = fRS_Right->AnalogValue - fRS_left->AnalogValue;
    return {fLAxisXOffset,fLAxisYOffset,fRAxisXOffset,fRAxisYOffset};
}
ImVec4 rgba(ImVec4 col) {
    return { col.x / 255.0f,col.y / 255.0f,col.z / 255.0f,col.w / 255.0f };
}
void buttonItemUI(const char* buttonName,bool state) {
    ImVec4 colorButton = (state ? ImVec4{12, 12, 19, 255} : ImVec4{37, 37, 37, 255});
    ImGui::PushStyleColor(ImGuiCol_Button, rgba(colorButton));
    ImGui::Button(buttonName, { 32,32 });
    ImGui::PopStyleColor();
}
void pos_x(float x) {
    ImGui::SetCursorPosX(x);
}
void toLine() {
    ImGui::SameLine();
}
void setCenterToFrame(ImVec2 size, ImVec2 sizeWindow) {
    ImGui::SetCursorPos({
        (sizeWindow - size) / ImVec2{2,2}
        });
}
void GamepadTestUI() {
    bool bA = false, bB = false, bX = false, bY = false;
    bool bDU = false, bDD = false, bDL = false, bDR = false;
    setCenterToFrame({ 330,360 }, ImGui::GetWindowSize());
    static ImVec2 fAxisL = { 0,0 }, fAxisR = { 0,0 };
    if (ImGui::BeginChild("GamepadTest", { 330,360 }, ImGuiChildFlags_FrameStyle, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar)) {
        fAxisL.x = getAxisGamepad().x;
        fAxisL.y = getAxisGamepad().y;
        fAxisR.x = getAxisGamepad().z;
        fAxisR.y = getAxisGamepad().w; 
        pos_x(30); ImGui::Text("GamepadTest");
        pos_x(30); ImGui::Separator();
        pos_x(60);
        buttonItemUI("Y", getKeyPresed(XINPUT_GAMEPAD_Y)); toLine(); pos_x(174); buttonItemUI("U", getKeyPresed(XINPUT_GAMEPAD_DPAD_UP));
        pos_x(30);
        buttonItemUI("X", getKeyPresed(XINPUT_GAMEPAD_X)); toLine(); pos_x(90); buttonItemUI("B", getKeyPresed(XINPUT_GAMEPAD_B)); toLine(); pos_x(140); buttonItemUI("L", getKeyPresed(XINPUT_GAMEPAD_DPAD_LEFT)); toLine(); pos_x(204); buttonItemUI("R", getKeyPresed(XINPUT_GAMEPAD_DPAD_RIGHT));
        pos_x(60);
        buttonItemUI("A", getKeyPresed(XINPUT_GAMEPAD_A));  toLine(); pos_x(174); buttonItemUI("D", getKeyPresed(XINPUT_GAMEPAD_DPAD_DOWN));
        pos_x(30); ImGui::CircleAxisXY("LStick", fAxisL.x, fAxisL.y); toLine(); ImGui::CircleAxisXY("RStick", fAxisR.x, fAxisR.y);
        ImGui::Text("Key: %s", sPresedKeyGamepad.c_str());
        pos_x(30);
        ImGui::TextColored({ 255,200,0,255 }, "Close [ESC]");
        ImGui::EndChild();
    }
}
//