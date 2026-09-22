// -------------------------------------------
// TUI (Terminal User Interface) by HCPP20334
// support Linux ,Windows ,macOS,Android
// 
//  writtein to C++20 
// 
//  github.com/hcpp20334
// ---------------------------------------------


#if __cplusplus >= 202002L || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L)
#include <iostream>
#include <string> 
#include <format>
#include <random>

// Color Struct (uint8_t range -255 to 255
struct ColorV3 {
    uint8_t r, g, b;
    constexpr ColorV3() : r(0), g(0), b(0) {}
    constexpr ColorV3(uint8_t _r, uint8_t _g, uint8_t _b) : r(_r), g(_g), b(_b) {}
    // func: getHex() return hex value 
    std::string getHex() const {
        std::string string_formatted = std::format("{:02X}{:02X}{:02X}", this->r, this->g, this->b);
        return string_formatted;
    }
    // func setU8Color(red,green,blue) convert to ColorV3
    ColorV3 setU8Color(uint32_t red, uint32_t green, uint32_t blue) {
        ColorV3 rgbU32 = {};
        rgbU32.r = static_cast<uint8_t>(red);
        rgbU32.g = static_cast<uint8_t>(green);
        rgbU32.b = static_cast<uint8_t>(blue);
        return rgbU32;
    }


};
// struct int64Vec2
// usage : int64Vec2 vec_64 = {0LL,0LL};
// vec_64.x - x same
// vec_64.y - y same
//
struct int64Vec2 {
    int64_t x = 0;
    int64_t y = 0;
    constexpr int64Vec2() : x(0), y(0) {}
    constexpr int64Vec2(int64_t _x, int64_t _y) : x(_x), y(_y) {}
    // func get(type) usage int a = vec_i64.get<uint8_t,int>((int)64):
    template <typename S0, typename S1>
    S0 get(S1 type) const {
        return static_cast<S0>(type);
    }
    // func get(type) usage int a = vec_i64.set<uint8_t>(255,43):
    template <typename S>
    void set(S x, S y) const {
        x = static_cast<int64_t>(x);
        y = static_cast<int64_t>(y);

    }
};
// struct main functions tui
// global param 
bool console_vt = false; // flags boolean to vt
//
struct ConsoleText {
    //pout(const std::string& str, const ColorV3& col, const ColorV3& colBack, bool isBackground = false, bool isCursorHide = true)
    // str     = std::string value <In>
    // Col     = ColorV3 <In>
    // ColBack = ColorV3 <In>
    // isBackground = bool <In> (true - Enable background,false - disable background)
    // isCursorHide = bool <In> (update string to terminal)
    void pout(const std::string& str, const ColorV3& col, const ColorV3& colBack, bool isBackground = false, bool isCursorHide = true) {
        if (isBackground) {
            if (isCursorHide)
            {
                std::cout << "\x1b[?25l" << std::endl;
            }
            std::cout << std::format("\x1b[48;2;{};{};{}m\x1b[38;2;{};{};{}m{}\x1b[0m",
                colBack.r, colBack.g, colBack.b,
                col.r, col.g, col.b,
                str);
        }
        else {
            std::cout << std::format("\x1b[38;2;{};{};{}m{}\x1b[0m",
                col.r, col.g, col.b, str);
        }
    }

    bool checkVT100() {
#ifdef _WIN32
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;

        if (hOut == INVALID_HANDLE_VALUE) return false;

        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            if (SetConsoleMode(hOut, dwMode)) {
                return true;
            }
        }
        return false;
#else
        // На Linux/macOS виртуальный терминал работает всегда
        return true;
#endif
    }


    // boolean-to-reset flag to one init std::random_device
    bool isRandomDevice = false;
    // random_device _random_dev 
    std::random_device _random_dev;
    // mt19937 generator
    std::mt19937 _mt;
    //func rnd(uint8_t max)
    // random value or 0..max
    uint8_t rnd(uint8_t max) {
        if (!isRandomDevice) {
            _mt.seed(_random_dev());
            isRandomDevice = true;
        }
        std::uniform_int_distribution<uint32_t> uid(0, max);
        return static_cast<uint8_t>(uid(_mt));
    }
    // fun func: generate random color to ColorV3
    ColorV3 randColor() {
        return ColorV3{ rnd(255),rnd(255),rnd(255) };
    }
    //
    // push color to text and background
    void push_color_rgba_v(ColorV3 color_text, ColorV3 color_background) {
        if (console_vt)
        {
            std::cout << std::format("\x1b[48;2;{};{};{}m\x1b[38;2;{};{};{}m",
                color_background.r, color_background.g, color_background.b,
                color_text.r, color_text.g, color_text.b);
        }
    }
    // push color to text
    void push_color_rgba(ColorV3 color_text) {
        if (console_vt)
        {
            std::cout << std::format("\x1b[38;2;{};{};{}m",
                color_text.r, color_text.g, color_text.b);
        }
    }
    // pop color to push_color_rgba_v and push_color_background
    //  Example C++ code:
    // only text
    // tui->push_color_rgba({242,242,247});
    // std::cout << " Hello World\n";
    // tui->pop_color_rgba();
    // to text and background
    // tui->push_color_rgba_v({7,7,7},{242,242,247});
    // std::cout << " Hello World\n";
    // tui->pop_color_rgba();
    //
    void pop_color_rgba() {
        if (console_vt)
        {
            std::cout << "\x1b[0m";
        }
    }
    //
    void TestColors(int64Vec2 sizeMax) {
        int nCount = 0;
        for (int cubeX = 0; cubeX < sizeMax.x; cubeX++) {
            for (int cubeY = 0; cubeY < sizeMax.y; cubeY++) {
                nCount++;
                if (nCount >= sizeMax.x) {
                    std::cout << "\n";
                    nCount = 0;
                }
                pout("  ", { 0,0,0 }, randColor(), true, false);
            }
        }
    }
    void GetHEXString(uint8_t r, uint8_t g, uint8_t b) {
        ColorV3 colorRGB = { r,g,b };
        pout(colorRGB.getHex(), { 255,255,255 }, { 0,0,0 }, false);
    }
    //ColorV3 SetValIV4(ImVec4 color) {
    //    ColorV3 outBuffer = {
    //        static_cast<uint8_t>(color.x * 255),
    //        static_cast<uint8_t>(color.y * 255),
    //        static_cast<uint8_t>(color.z * 255)
    //    };
    //    return outBuffer;
    //}
    template<typename Val>
    ColorV3 setVal(Val r, Val g, Val b) {
        ColorV3 rgbVal = {};
        if (std::is_arithmetic_v<Val> || std::is_floating_point_v<Val>) {
            rgbVal.r = static_cast<Val>(r);
            rgbVal.g = static_cast<Val>(g);
            rgbVal.b = static_cast<Val>(b);
        }
        else {
            pout("ColorV3 Vector Error: Val is Arthmetic && Floating value", { 255,0,120 }, { 0,0,0 }, false, false);
            rgbVal = { 0,0,0 };
        }
        return rgbVal;
    }
    void progress(int fragtion, ColorV3 colorFrag = { 242,242,255 }) {

        std::string _progress[9] = {
            "[=        ]",
            "[==       ]",
            "[===      ]",
            "[====     ]",
            "[=====    ]",
            "[======   ]",
            "[=======  ]",
            "[======== ]",
            "[=========]",
        };
        pout(_progress[fragtion], colorFrag, { 0,0,0 }, false, false);


    }
    ~ConsoleText() {
        std::cout << "  tui deleted!\n";
    }
};
ConsoleText* tui = new ConsoleText();
#else
#error "tui not supported C++ < C++20"
#endif 

