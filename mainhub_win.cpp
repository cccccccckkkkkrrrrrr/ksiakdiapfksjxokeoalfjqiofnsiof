#include <windows.h>
#include <commctrl.h>
#include <string>
#include <vector>
#include <mmsystem.h>
#include <gdiplus.h>
#include <shlwapi.h>
#include <algorithm>
#include "resource.h"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "shlwapi.lib")

using namespace Gdiplus;

HWND g_hSquaresWnd = NULL;
HWND g_hImageWnd = NULL;
HBITMAP g_hBitmap = NULL;
int g_imgW = 0, g_imgH = 0;

HBITMAP LoadPNGFromResource(HINSTANCE hInstance, int resId, int* width, int* height) {
    HRSRC hRes = FindResource(hInstance, MAKEINTRESOURCE(resId), RT_RCDATA);
    if (!hRes) return NULL;
    HGLOBAL hMem = LoadResource(hInstance, hRes);
    DWORD size = SizeofResource(hInstance, hRes);
    void* pData = LockResource(hMem);
    IStream* pStream = SHCreateMemStream((const BYTE*)pData, size);
    if (!pStream) return NULL;
    Bitmap* bmp = new Bitmap(pStream);
    pStream->Release();
    if (bmp->GetLastStatus() != Ok) { delete bmp; return NULL; }
    *width = bmp->GetWidth();
    *height = bmp->GetHeight();
    HBITMAP hbm = NULL;
    bmp->GetHBITMAP(Color(0,0,0), &hbm);
    delete bmp;
    return hbm;
}

void PlayWavFromResource(HINSTANCE hInstance, int resId) {
    HRSRC hRes = FindResource(hInstance, MAKEINTRESOURCE(resId), RT_RCDATA);
    if (!hRes) return;
    HGLOBAL hMem = LoadResource(hInstance, hRes);
    void* pData = LockResource(hMem);
    PlaySoundA((LPCSTR)pData, NULL, SND_MEMORY | SND_ASYNC | SND_LOOP);
}

class SquareWindow {
public:
    int x[2], y[2], vx[2], vy[2], size;
    COLORREF colors[2];
    int width, height;
    SquareWindow() {
        size = 60;
        for (int i = 0; i < 2; i++) {
            x[i] = rand() % 500; y[i] = rand() % 500;
            vx[i] = 10 + rand() % 10; vy[i] = 10 + rand() % 10;
            colors[i] = RGB(rand() % 255, rand() % 255, rand() % 255);
        }
    }
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        SquareWindow* self = (SquareWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        if (msg == WM_CREATE) {
            CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)cs->lpCreateParams);
            return 0;
        }
        if (msg == WM_PAINT && self) {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP memBmp = CreateCompatibleBitmap(hdc, self->width, self->height);
            SelectObject(memDC, memBmp);
            RECT rect = {0, 0, self->width, self->height};
            HBRUSH bg = CreateSolidBrush(RGB(0, 0, 0));
            FillRect(memDC, &rect, bg);
            DeleteObject(bg);
            for (int i = 0; i < 2; i++) {
                HBRUSH brush = CreateSolidBrush(self->colors[i]);
                RECT r = {self->x[i], self->y[i], self->x[i] + self->size, self->y[i] + self->size};
                FillRect(memDC, &r, brush);
                DeleteObject(brush);
            }
            BitBlt(hdc, 0, 0, self->width, self->height, memDC, 0, 0, SRCCOPY);
            DeleteObject(memBmp);
            DeleteDC(memDC);
            EndPaint(hwnd, &ps);
            return 0;
        }
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
};

SquareWindow* g_squares = NULL;

DWORD WINAPI AnimationThread(LPVOID lpParam) {
    int scrW = GetSystemMetrics(SM_CXSCREEN);
    int scrH = GetSystemMetrics(SM_CYSCREEN);
    g_squares->width = scrW;
    g_squares->height = scrH;
    g_hSquaresWnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW, L"STATIC", L"Squares", WS_POPUP, 0, 0, scrW, scrH, NULL, NULL, GetModuleHandle(NULL), g_squares);
    SetWindowLongPtrW(g_hSquaresWnd, GWLP_WNDPROC, (LONG_PTR)SquareWindow::WndProc);
    SetLayeredWindowAttributes(g_hSquaresWnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
    ShowWindow(g_hSquaresWnd, SW_SHOW);
    g_hImageWnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW, L"STATIC", L"Image", WS_POPUP, 0, 0, g_imgW, g_imgH, NULL, NULL, GetModuleHandle(NULL), NULL);
    SetLayeredWindowAttributes(g_hImageWnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
    ShowWindow(g_hImageWnd, SW_SHOW);
    while (true) {
        for (int i = 0; i < 2; i++) {
            g_squares->x[i] += g_squares->vx[i]; g_squares->y[i] += g_squares->vy[i];
            if (g_squares->x[i] <= 0) { g_squares->x[i] = 0; g_squares->vx[i] = abs(g_squares->vx[i]); }
            if (g_squares->x[i] + g_squares->size >= scrW) { g_squares->x[i] = scrW - g_squares->size; g_squares->vx[i] = -abs(g_squares->vx[i]); }
            if (g_squares->y[i] <= 0) { g_squares->y[i] = 0; g_squares->vy[i] = abs(g_squares->vy[i]); }
            if (g_squares->y[i] + g_squares->size >= scrH) { g_squares->y[i] = scrH - g_squares->size; g_squares->vy[i] = -abs(g_squares->vy[i]); }
        }
        InvalidateRect(g_hSquaresWnd, NULL, FALSE);
        int px = rand() % std::max(1, scrW - g_imgW);
        int py = rand() % std::max(1, scrH - g_imgH);
        SetWindowPos(g_hImageWnd, HWND_TOPMOST, px, py, g_imgW, g_imgH, SWP_SHOWWINDOW | SWP_NOACTIVATE);
        UpdateWindow(g_hSquaresWnd);
        Sleep(20);
    }
    return 0;
}

LRESULT CALLBACK ImageWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_PAINT) {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        HDC memDC = CreateCompatibleDC(hdc);
        SelectObject(memDC, g_hBitmap);
        BitBlt(hdc, 0, 0, g_imgW, g_imgH, memDC, 0, 0, SRCCOPY);
        DeleteDC(memDC);
        EndPaint(hwnd, &ps);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_CREATE) {
        int res = MessageBoxW(hwnd, L"This is Malware", L"Are You Sure?", MB_OKCANCEL | MB_ICONWARNING);
        if (res == IDOK) {
            PlayWavFromResource(GetModuleHandle(NULL), IDR_SOUND1);
            g_hBitmap = LoadPNGFromResource(GetModuleHandle(NULL), IDR_IMAGE1, &g_imgW, &g_imgH);
            if (g_hBitmap == NULL) {
                MessageBoxW(hwnd, L"Error loading PNG from resources!", L"Error", MB_OK | MB_ICONERROR);
            }
            g_squares = new SquareWindow();
            SetWindowLongPtrW(g_hImageWnd, GWLP_WNDPROC, (LONG_PTR)ImageWndProc);
            CreateThread(NULL, 0, AnimationThread, NULL, 0, NULL);
        } else {
            DestroyWindow(hwnd);
        }
        return 0;
    }
    if (msg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = MainWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"MainHubWnd";
    RegisterClassW(&wc);
    HWND hwnd = CreateWindowExW(0, L"MainHubWnd", L"Malware", 0, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    GdiplusShutdown(gdiplusToken);
    return 0;
}
