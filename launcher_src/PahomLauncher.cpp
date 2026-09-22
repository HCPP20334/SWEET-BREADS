
#include <cstdlib>
#include <windows.h>
#include <dwmapi.h>
#include <commctrl.h>
#include <shellapi.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <winuser.h>

#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "comctl32.lib")

#pragma comment(linker, \
    "\"/manifestdependency:type='win32' "                     \
    "name='Microsoft.Windows.Common-Controls' "                \
    "version='6.0.0.0' processorArchitecture='*' "             \
    "publicKeyToken='6595b64144ccf1df' language='*'\"")

enum ACCENT_STATE {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_GRADIENT = 1,
    ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
    ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
    ACCENT_ENABLE_HOSTBACKDROP = 5,
    ACCENT_INVALID_STATE = 6
};

struct ACCENT_POLICY {
    ACCENT_STATE AccentState;
    DWORD        AccentFlags;
    DWORD        GradientColor; // ABGR
    DWORD        AnimationId;
};

enum WINDOWCOMPOSITIONATTRIB {
    WCA_ACCENT_POLICY = 19
};

struct WINDOWCOMPOSITIONATTRIBDATA {
    WINDOWCOMPOSITIONATTRIB Attrib;
    PVOID                   pvData;
    SIZE_T                  cbData;
};

typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);

static void ApplyBlur(HWND hwnd)
{
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    bool acrylicApplied = false;
    if (user32) {
        auto SetWindowCompositionAttribute =
            reinterpret_cast<pSetWindowCompositionAttribute>(
                GetProcAddress(user32, "SetWindowCompositionAttribute"));
        if (SetWindowCompositionAttribute) {
            ACCENT_POLICY accent = {};
            accent.AccentState = ACCENT_ENABLE_ACRYLICBLURBEHIND;
            accent.AccentFlags = 2; 
            accent.GradientColor = 0xAA1E1712;
            accent.GradientColor = (0xAA << 24) | (0x2A << 16) | (0x20 << 8) | 0x18; 
            WINDOWCOMPOSITIONATTRIBDATA data;
            data.Attrib = WCA_ACCENT_POLICY;
            data.pvData = &accent;
            data.cbData = sizeof(accent);
            acrylicApplied = SetWindowCompositionAttribute(hwnd, &data) != 0;
        }
    }


    MARGINS margins = { -1, -1, -1, -1 };
    DwmExtendFrameIntoClientArea(hwnd, &margins);

    if (!acrylicApplied) {
        DWM_BLURBEHIND bb = {};
        bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
        bb.fEnable = TRUE;
        bb.hRgnBlur = CreateRectRgn(0, 0, -1, -1); // весь регион
        DwmEnableBlurBehindWindow(hwnd, &bb);
    }
}


struct ConfigField {
    std::wstring key;
    std::wstring value;
    bool         isBool;
};

static std::vector<ConfigField> g_fields; 
static std::wstring g_cfgPath = L"PahomEngine.cfg";

static bool IsBoolLiteral(const std::wstring& v)
{
    return v == L"true" || v == L"false";
}

static std::string WStringToUtf8(const std::wstring& w)
{
    if (w.empty()) return std::string();
    int len = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(),
                                   nullptr, 0, nullptr, nullptr);
    std::string out(len, 0);
    WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(),
                         &out[0], len, nullptr, nullptr);
    return out;
}

static bool LoadConfig(const std::wstring& path)
{
    std::wifstream in(WStringToUtf8(path).c_str());
    if (!in.is_open()) return false;

    g_fields.clear();
    std::wstring line;
    while (std::getline(in, line)) {
        // обрезаем \r на случай CRLF
        while (!line.empty() && (line.back() == L'\r' || line.back() == L'\n'))
            line.pop_back();
        if (line.empty()) continue;

        size_t eq = line.find(L'=');
        if (eq == std::wstring::npos) continue;

        ConfigField f;
        f.key   = line.substr(0, eq);
        f.value = line.substr(eq + 1);
        f.isBool = IsBoolLiteral(f.value);
        g_fields.push_back(f);
    }
    return true;
}

static bool SaveConfig(const std::wstring& path)
{
    std::wofstream out(WStringToUtf8(path).c_str(), std::ios::trunc);
    if (!out.is_open()) return false;
    for (auto& f : g_fields)
        out << f.key << L"=" << f.value << L"\n";
    return true;
}

static HWND g_hwnd = nullptr;
static HWND g_hLaunch = nullptr;
static HWND g_hSave = nullptr;
static HFONT g_font = nullptr;
static HFONT g_fontBold = nullptr;
static HBRUSH g_transparentBrush = nullptr;

static std::map<int, size_t> g_checkboxToField;
static std::map<int, size_t> g_editToField;

static const int ID_FIELD_BASE   = 1000;
static const int ID_LAUNCH_BTN   = 900;
static const int ID_SAVE_BTN     = 901;
static const int ID_TITLE_STATIC = 902;

static const COLORREF CLR_TEXT      = RGB(235, 238, 245);
static const COLORREF CLR_SUBTEXT   = RGB(170, 180, 200);
static const COLORREF CLR_ACCENT    = RGB(90, 170, 255);

static void CreateFonts()
{
    g_font = CreateFontW(
        -15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");

    g_fontBold = CreateFontW(
        -22, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");
}

static void BuildControls(HWND hwnd)
{
    int y = 70;
    const int labelW = 190;
    const int rowH   = 30;
    const int leftX  = 30;
    const int ctrlX  = leftX + labelW + 10;

    HWND title = CreateWindowExW(0, L"STATIC", L"PahomEngine — Launcher",
        WS_CHILD | WS_VISIBLE, leftX, 20, 400, 32, hwnd,
        (HMENU)(INT_PTR)ID_TITLE_STATIC, nullptr, nullptr);
    SendMessageW(title, WM_SETFONT, (WPARAM)g_fontBold, TRUE);

    int idCounter = ID_FIELD_BASE;

    for (size_t i = 0; i < g_fields.size(); ++i) {
        auto& f = g_fields[i];

        HWND label = CreateWindowExW(0, L"STATIC", f.key.c_str(),
            WS_CHILD | WS_VISIBLE, leftX, y + 4, labelW, rowH, hwnd,
            nullptr, nullptr, nullptr);
        SendMessageW(label, WM_SETFONT, (WPARAM)g_font, TRUE);

        if (f.isBool) {
            int id = idCounter++;
            HWND cb = CreateWindowExW(0, L"BUTTON", L"",
                WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE,
                ctrlX, y, 90, 26, hwnd, (HMENU)(INT_PTR)id, nullptr, nullptr);
            SendMessageW(cb, WM_SETFONT, (WPARAM)g_font, TRUE);
            SetWindowTextW(cb, f.value == L"true" ? L"ON" : L"OFF");
            SendMessageW(cb, BM_SETCHECK, f.value == L"true" ? BST_CHECKED : BST_UNCHECKED, 0);
            g_checkboxToField[id] = i;
        } else {
            int id = idCounter++;
            HWND ed = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", f.value.c_str(),
                WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                ctrlX, y, 150, 24, hwnd, (HMENU)(INT_PTR)id, nullptr, nullptr);
            SendMessageW(ed, WM_SETFONT, (WPARAM)g_font, TRUE);
            g_editToField[id] = i;
        }

        y += rowH + 4;
    }

    y += 12;
    g_hSave = CreateWindowExW(0, L"BUTTON", L"Сохранить конфиг",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, leftX, y, 170, 34, hwnd,
        (HMENU)(INT_PTR)ID_SAVE_BTN, nullptr, nullptr);
    SendMessageW(g_hSave, WM_SETFONT, (WPARAM)g_font, TRUE);

    g_hLaunch = CreateWindowExW(0, L"BUTTON", L"Запустить движок",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, leftX + 180, y, 170, 34, hwnd,
        (HMENU)(INT_PTR)ID_LAUNCH_BTN, nullptr, nullptr);
    SendMessageW(g_hLaunch, WM_SETFONT, (WPARAM)g_font, TRUE);
}

static void SyncControlsToFields()
{
    for (auto& kv : g_checkboxToField) {
        HWND cb = GetDlgItem(g_hwnd, kv.first);
        bool checked = SendMessageW(cb, BM_GETCHECK, 0, 0) == BST_CHECKED;
        g_fields[kv.second].value = checked ? L"true" : L"false";
        SetWindowTextW(cb, checked ? L"ON" : L"OFF");
    }
    for (auto& kv : g_editToField) {
        HWND ed = GetDlgItem(g_hwnd, kv.first);
        wchar_t buf[256];
        GetWindowTextW(ed, buf, 256);
        g_fields[kv.second].value = buf;
    }
}

static void LaunchEngine()
{
    wchar_t exeDir[MAX_PATH];
    GetModuleFileNameW(nullptr, exeDir, MAX_PATH);
    std::wstring dir(exeDir);
    size_t slash = dir.find_last_of(L"\\/");
    if (slash != std::wstring::npos) dir = dir.substr(0, slash + 1);

    std::wstring exePath = dir + L"SweetBreads(PahomEngine).exe";
    HINSTANCE r = ShellExecuteW(g_hwnd, L"open", exePath.c_str(), nullptr, dir.c_str(), SW_SHOWNORMAL);
    if ((INT_PTR)r <= 32) {
        MessageBoxW(g_hwnd,
            L"Не удалось найти SweetBreads(PahomEngine).exe рядом с лаунчером.",
            L"PahomEngine", MB_OK | MB_ICONWARNING);
    }else{
        std::exit(1);
    }
}
static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg) {
    case WM_CREATE:
        CreateFonts();
        BuildControls(hwnd);
        return 0;

    case WM_ERASEBKGND: {
        HDC hdc = (HDC)wp;
        RECT rc; GetClientRect(hwnd, &rc);
        HBRUSH brush = CreateSolidBrush(RGB(18, 20, 26));
        FillRect(hdc, &rc, brush);
        DeleteObject(brush);
        return 1;
    }

    case WM_CTLCOLORSTATIC: {
        HDC hdc = (HDC)wp;
        SetTextColor(hdc, CLR_TEXT);
        SetBkMode(hdc, TRANSPARENT);
        if (!g_transparentBrush)
            g_transparentBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
        return (LRESULT)g_transparentBrush;
    }

    case WM_CTLCOLOREDIT: {
        HDC hdc = (HDC)wp;
        SetTextColor(hdc, CLR_TEXT);
        SetBkColor(hdc, RGB(30, 33, 40));
        static HBRUSH editBrush = CreateSolidBrush(RGB(30, 33, 40));
        return (LRESULT)editBrush;
    }

    case WM_COMMAND: {
        int id = LOWORD(wp);
        if (id == ID_SAVE_BTN) {
            SyncControlsToFields();
            if (SaveConfig(g_cfgPath))
                MessageBoxW(hwnd, L"Конфиг сохранён.", L"PahomEngine", MB_OK | MB_ICONINFORMATION);
            else
                MessageBoxW(hwnd, L"Не удалось записать файл конфига.", L"PahomEngine", MB_OK | MB_ICONERROR);
        } else if (id == ID_LAUNCH_BTN) {
            SyncControlsToFields();
            SaveConfig(g_cfgPath);
            LaunchEngine();
        } else if (HIWORD(wp) == BN_CLICKED && g_checkboxToField.count(id)) {
            HWND cb = GetDlgItem(hwnd, id);
            bool checked = SendMessageW(cb, BM_GETCHECK, 0, 0) == BST_CHECKED;
            SetWindowTextW(cb, checked ? L"ON" : L"OFF");
        }
        return 0;
    }

    case WM_NCCALCSIZE:
        if (wp) return 0;
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, int nCmdShow)
{
    if (!LoadConfig(g_cfgPath)) {
        g_fields = {
            {L"vsync", L"false", true},
            {L"random_engine", L"false", true},
            {L"volume", L"0.08700009", false},
            {L"cpu_delay", L"1974", false},
            {L"render_blur", L"false", true},
            {L"bind_key_right", L"65", false},
            {L"bind_key_left", L"68", false},
            {L"EnableAnimationToImageFadeInOut", L"true", true},
            {L"use_custom_render", L"false", true},
            {L"gl_major", L"3", false},
            {L"gl_minor", L"2", false},
            {L"noise", L"true", true},
            {L"use_low_textures", L"true", true},
            {L"use_low_audio_quality", L"false", true},
            {L"width", L"1936", false},
            {L"height", L"1056", false},
            {L"preset", L"1", false},
            {L"xor64random", L"true", true},
            {L"fullscreen", L"true", true},
        };
    }

    INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_STANDARD_CLASSES | ICC_BAR_CLASSES };
    InitCommonControlsEx(&icc);

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;
    wc.lpszClassName = L"PahomLauncherWnd";
    wc.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
    RegisterClassExW(&wc);

    int winH = 70 + (int)g_fields.size() * 34 + 90;

    HWND hwnd = CreateWindowExW(
        WS_EX_LAYERED,
        wc.lpszClassName, L"PahomEngine Launcher",
        WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 420, winH,
        nullptr, nullptr, hInst, nullptr);

    g_hwnd = hwnd;

    SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
    ApplyBlur(hwnd);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return (int)msg.wParam;
}