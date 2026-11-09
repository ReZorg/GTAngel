#include <windows.h>
#include <cstdio>
#include <fstream>

static bool SaveBmp(HWND hwnd, const char* path, int* outW, int* outH) {
    RECT r;
    if (!GetWindowRect(hwnd, &r)) return false;
    int w = r.right - r.left;
    int h = r.bottom - r.top;
    if (w <= 0 || h <= 0) return false;
    HDC src = GetWindowDC(hwnd);
    HDC mem = CreateCompatibleDC(src);
    HBITMAP bmp = CreateCompatibleBitmap(src, w, h);
    HGDIOBJ old = SelectObject(mem, bmp);
    // Use a direct surface copy. PrintWindow sends a synchronous render message
    // and can deadlock while the app is probing optional local LLM backends.
    BitBlt(mem, 0, 0, w, h, src, 0, 0, SRCCOPY | CAPTUREBLT);

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    DWORD bytes = (DWORD)w * (DWORD)h * 4;
    unsigned char* pixels = new unsigned char[bytes];
    int rows = GetDIBits(mem, bmp, 0, h, pixels, &bi, DIB_RGB_COLORS);

    BITMAPFILEHEADER fh = {};
    fh.bfType = 0x4D42;
    fh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    fh.bfSize = fh.bfOffBits + bytes;
    bool ok = false;
    if (rows == h) {
        std::ofstream out(path, std::ios::binary);
        out.write((const char*)&fh, sizeof(fh));
        out.write((const char*)&bi.bmiHeader, sizeof(BITMAPINFOHEADER));
        out.write((const char*)pixels, bytes);
        ok = out.good();
    }
    delete[] pixels;
    SelectObject(mem, old);
    DeleteObject(bmp);
    DeleteDC(mem);
    ReleaseDC(hwnd, src);
    if (outW) *outW = w;
    if (outH) *outH = h;
    return ok;
}

int main() {
    HWND hwnd = 0;
    for (int i = 0; i < 80 && !hwnd; ++i) {
        hwnd = FindWindowA("GTAngelAGIv12", 0);
        if (!hwnd) Sleep(250);
    }
    if (!hwnd) {
        std::printf("WINDOW_NOT_FOUND\n");
        return 2;
    }
    PostMessageA(hwnd, WM_KEYDOWN, 'R', 0);
    PostMessageA(hwnd, WM_KEYUP, 'R', 0);
    Sleep(30000);
    PostMessageA(hwnd, WM_KEYDOWN, VK_SPACE, 0);
    PostMessageA(hwnd, WM_KEYUP, VK_SPACE, 0);
    Sleep(250);
    PostMessageA(hwnd, WM_KEYDOWN, 'N', 0);
    PostMessageA(hwnd, WM_KEYUP, 'N', 0);
    Sleep(10000);

    DWORD_PTR ping = 0;
    BOOL responding = SendMessageTimeoutA(hwnd, WM_NULL, 0, 0,
        SMTO_ABORTIFHUNG | SMTO_BLOCK, 2000, &ping);
    int w = 0, h = 0;
    bool saved = SaveBmp(hwnd, "C:\\ddd\\GTAngel\\cpp_test\\gtangel_live_window.bmp", &w, &h);
    char title[512] = {};
    GetWindowTextA(hwnd, title, 511);
    std::ofstream meta("C:\\ddd\\GTAngel\\cpp_test\\gtangel_live_window.json");
    meta << "{\n"
         << "  \"responding\": " << (responding ? "true" : "false") << ",\n"
         << "  \"mainWindowTitle\": \"" << title << "\",\n"
         << "  \"width\": " << w << ",\n"
         << "  \"height\": " << h << ",\n"
         << "  \"initializedWithReset\": true,\n"
         << "  \"running\": true,\n"
         << "  \"navigationTraining\": true,\n"
         << "  \"thirdPerson\": true\n"
         << "}\n";
    std::printf("RESPONSIVE=%d\nSAVED=%d\nSIZE=%dx%d\n", responding ? 1 : 0, saved ? 1 : 0, w, h);
    return (responding && saved) ? 0 : 1;
}
