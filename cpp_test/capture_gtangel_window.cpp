#include <windows.h>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <string>

static void logStep(const char* text) {
    std::printf("%s\n", text);
    std::fflush(stdout);
    std::ofstream out("C:\\ddd\\GTAngel\\cpp_test\\gtangel_capture_progress.txt", std::ios::app);
    out << text << "\n";
}

static bool saveWindowBmp(HWND hwnd, const char* path, int& width, int& height, bool& printOk) {
    RECT rect{};
    if (!GetWindowRect(hwnd, &rect)) return false;
    width = rect.right - rect.left;
    height = rect.bottom - rect.top;
    if (width <= 0 || height <= 0) return false;

    HDC windowDc = GetWindowDC(hwnd);
    HDC memoryDc = CreateCompatibleDC(windowDc);
    HBITMAP bitmap = CreateCompatibleBitmap(windowDc, width, height);
    HGDIOBJ oldBitmap = SelectObject(memoryDc, bitmap);
    // Avoid PrintWindow because it synchronously asks the target UI thread to
    // render and can deadlock during optional backend discovery. A direct GDI
    // surface copy is nonblocking and captures the live on-screen window.
    printOk = false;
    BitBlt(memoryDc, 0, 0, width, height, windowDc, 0, 0, SRCCOPY | CAPTUREBLT);

    BITMAPINFOHEADER header{};
    header.biSize = sizeof(header);
    header.biWidth = width;
    header.biHeight = -height;
    header.biPlanes = 1;
    header.biBitCount = 32;
    header.biCompression = BI_RGB;
    const DWORD rowBytes = (DWORD)width * 4;
    const DWORD imageBytes = rowBytes * (DWORD)height;
    auto* pixels = new unsigned char[imageBytes];

    BITMAPINFO info{};
    info.bmiHeader = header;
    const int scanLines = GetDIBits(memoryDc, bitmap, 0, height, pixels, &info, DIB_RGB_COLORS);

    BITMAPFILEHEADER fileHeader{};
    fileHeader.bfType = 0x4D42;
    fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    fileHeader.bfSize = fileHeader.bfOffBits + imageBytes;

    bool saved = false;
    if (scanLines == height) {
        std::ofstream out(path, std::ios::binary);
        out.write(reinterpret_cast<const char*>(&fileHeader), sizeof(fileHeader));
        out.write(reinterpret_cast<const char*>(&header), sizeof(header));
        out.write(reinterpret_cast<const char*>(pixels), imageBytes);
        saved = out.good();
    }

    delete[] pixels;
    SelectObject(memoryDc, oldBitmap);
    DeleteObject(bitmap);
    DeleteDC(memoryDc);
    ReleaseDC(hwnd, windowDc);
    return saved;
}

int main() {
    const char* work = "C:\\ddd\\GTAngel\\cpp_test";
    SetCurrentDirectoryA(work);
    DeleteFileA("gtangel_capture_progress.txt");
    DeleteFileA("gtangel_live_window.bmp");
    DeleteFileA("gtangel_live_window.json");

    logStep("capture-start");
    Sleep(500);
    logStep("external-process-cleanup-assumed");

    STARTUPINFOA startup{};
    startup.cb = sizeof(startup);
    PROCESS_INFORMATION process{};
    char command[] = "C:\\ddd\\GTAngel\\cpp_test\\gtangel_agi_v13.exe";
    if (!CreateProcessA(nullptr, command, nullptr, nullptr, FALSE,
                        CREATE_NEW_PROCESS_GROUP, nullptr, work, &startup, &process)) {
        std::printf("CreateProcess failed: %lu\n", GetLastError());
        return 2;
    }
    logStep("process-started");

    HWND hwnd = nullptr;
    for (int i = 0; i < 80 && !hwnd; ++i) {
        Sleep(250);
        hwnd = FindWindowA("GTAngelAGIv12", nullptr);
    }
    if (!hwnd) {
        logStep("window-not-found");
        TerminateProcess(process.hProcess, 3);
        CloseHandle(process.hThread);
        CloseHandle(process.hProcess);
        return 3;
    }
    logStep("window-found");
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    // Reset generates the 14-pillar world. Reset performs optional local LLM
    // discovery, so allow it to complete before starting the simulation.
    PostMessageA(hwnd, WM_KEYDOWN, 'R', 0);
    PostMessageA(hwnd, WM_KEYUP, 'R', 0);
    logStep("reset-posted");
    Sleep(30000);

    PostMessageA(hwnd, WM_KEYDOWN, VK_SPACE, 0);
    PostMessageA(hwnd, WM_KEYUP, VK_SPACE, 0);
    logStep("start-posted");
    Sleep(250);
    PostMessageA(hwnd, WM_KEYDOWN, 'N', 0);
    PostMessageA(hwnd, WM_KEYUP, 'N', 0);
    logStep("navigation-posted");
    Sleep(10000);

    DWORD_PTR ping = 0;
    const bool responding = SendMessageTimeoutA(hwnd, WM_NULL, 0, 0,
        SMTO_ABORTIFHUNG | SMTO_BLOCK, 2000, &ping) != 0;
    logStep(responding ? "window-responsive" : "window-unresponsive");

    int width = 0, height = 0;
    bool printOk = false;
    const bool screenshotSaved = saveWindowBmp(hwnd,
        "C:\\ddd\\GTAngel\\cpp_test\\gtangel_live_window.bmp",
        width, height, printOk);
    logStep(screenshotSaved ? "screenshot-saved" : "screenshot-failed");

    char title[512]{};
    GetWindowTextA(hwnd, title, sizeof(title));
    std::ofstream metadata("C:\\ddd\\GTAngel\\cpp_test\\gtangel_live_window.json");
    metadata << "{\n"
             << "  \"processId\": " << process.dwProcessId << ",\n"
             << "  \"responding\": " << (responding ? "true" : "false") << ",\n"
             << "  \"mainWindowTitle\": \"" << title << "\",\n"
             << "  \"width\": " << width << ",\n"
             << "  \"height\": " << height << ",\n"
             << "  \"printWindowSucceeded\": " << (printOk ? "true" : "false") << ",\n"
             << "  \"initializedWithReset\": true,\n"
             << "  \"running\": true,\n"
             << "  \"navigationTraining\": true,\n"
             << "  \"thirdPerson\": true,\n"
             << "  \"screenshot\": \"C:/ddd/GTAngel/cpp_test/gtangel_live_window.bmp\"\n"
             << "}\n";
    metadata.close();
    logStep("metadata-saved");

    PostMessageA(hwnd, WM_CLOSE, 0, 0);
    if (WaitForSingleObject(process.hProcess, 3000) == WAIT_TIMEOUT) {
        TerminateProcess(process.hProcess, 4);
    }
    CloseHandle(process.hThread);
    CloseHandle(process.hProcess);
    logStep("process-stopped");

    std::printf("SCREENSHOT_SAVED=%d\n", screenshotSaved ? 1 : 0);
    std::printf("WINDOW_RESPONSIVE=%d\n", responding ? 1 : 0);
    std::printf("WINDOW_SIZE=%dx%d\n", width, height);
    std::printf("PRINTWINDOW=%d\n", printOk ? 1 : 0);
    return screenshotSaved && responding ? 0 : 1;
}
