#include <windows.h>
#include <gdiplus.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <vector>
#include <cmath>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

using namespace Gdiplus;

volatile bool g_Running = true;
HHOOK g_HookKb = NULL;
HHOOK g_HookMs = NULL;
HWND g_Wnd = NULL;
HWAVEOUT g_WaveOut = NULL;

LRESULT CALLBACK KbHook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
        if (p->vkCode == VK_ESCAPE || p->vkCode == VK_TAB || p->vkCode == VK_LWIN || p->vkCode == VK_RWIN || p->vkCode == VK_F4 || p->vkCode == VK_DELETE || p->vkCode == VK_CONTROL || p->vkCode == VK_MENU) {
            return 1;
        }
    }
    return CallNextHookEx(g_HookKb, nCode, wParam, lParam);
}

LRESULT CALLBACK MsHook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN || wParam == WM_MBUTTONDOWN || wParam == WM_MOUSEMOVE) {
            return 1;
        }
    }
    return CallNextHookEx(g_HookMs, nCode, wParam, lParam);
}

DWORD WINAPI AudioThread(LPVOID) {
    WAVEFORMATEX wfx;
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 2;
    wfx.nSamplesPerSec = 48000;
    wfx.wBitsPerSample = 16;
    wfx.nBlockAlign = (wfx.nChannels * wfx.wBitsPerSample) / 8;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    wfx.cbSize = 0;

    if (waveOutOpen(&g_WaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR) {
        return 0;
    }

    int duration_samples = 48000 * 2;
    std::vector<short> audio(duration_samples * 2);

    for (int i = 0; i < duration_samples; i++) {
        double t = (double)i / 48000.0;
        double val = sin(2.0 * 3.141592653589793 * 440.0 * t) * 0.5;
        val += sin(2.0 * 3.141592653589793 * 880.0 * t) * 0.3;
        val += sin(2.0 * 3.141592653589793 * 1760.0 * t) * 0.2;
        short s = (short)(val * 32767.0);
        audio[i * 2] = s;
        audio[i * 2 + 1] = s;
    }

    WAVEHDR hdr;
    ZeroMemory(&hdr, sizeof(WAVEHDR));
    hdr.lpData = (LPSTR)audio.data();
    hdr.dwBufferLength = audio.size() * sizeof(short);
    hdr.dwFlags = WHDR_BEGINLOOP | WHDR_ENDLOOP;
    hdr.dwLoops = 0xFFFFFFFF;

    waveOutPrepareHeader(g_WaveOut, &hdr, sizeof(WAVEHDR));
    waveOutWrite(g_WaveOut, &hdr, sizeof(WAVEHDR));

    while (g_Running) {
        Sleep(100);
    }

    waveOutReset(g_WaveOut);
    waveOutUnprepareHeader(g_WaveOut, &hdr, sizeof(WAVEHDR));
    waveOutClose(g_WaveOut);
    return 0;
}

void LoadImageFromResource(const wchar_t* resName, const wchar_t* resType, Bitmap** outBmp) {
    HRSRC hRes = FindResourceW(NULL, resName, resType);
    if (!hRes) return;
    HGLOBAL hMem = LoadResource(NULL, hRes);
    if (!hMem) return;
    DWORD size = SizeofResource(NULL, hRes);
    void* data = LockResource(hMem);
    if (!data || size == 0) return;

    HGLOBAL hBuffer = GlobalAlloc(GMEM_MOVEABLE, size);
    if (!hBuffer) return;
    void* pBuffer = GlobalLock(hBuffer);
    memcpy(pBuffer, data, size);
    GlobalUnlock(hBuffer);

    IStream* pStream = NULL;
    if (CreateStreamOnHGlobal(hBuffer, TRUE, &pStream) == S_OK) {
        *outBmp = Bitmap::FromStream(pStream);
        pStream->Release();
    }
    GlobalFree(hBuffer);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        Bitmap* pBmp = nullptr;
        LoadImageFromResource(L"E_JPG", L"JPG", &pBmp);
        
        if (pBmp && pBmp->GetLastStatus() == Ok) {
            Graphics g(hdc);
            g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
            g.DrawImage(pBmp, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
            delete pBmp;
        } else {
            HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
            FillRect(hdc, &ps.rcPaint, brush);
            DeleteObject(brush);
        }
        EndPaint(hWnd, &ps);
        return 0;
    }
    case WM_TIMER:
        SetForegroundWindow(hWnd);
        return 0;
    case WM_CLOSE:
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    WNDCLASSEXW wc = { sizeof(WNDCLASSEXW) };
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszClassName = L"PentaenaClass";
    RegisterClassExW(&wc);

    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);

    g_Wnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, L"PentaenaClass", L"Pentaena", WS_POPUP | WS_VISIBLE, 0, 0, screenW, screenH, NULL, NULL, hInstance, NULL);
    
    ShowWindow(g_Wnd, SW_SHOWMAXIMIZED);
    SetWindowPos(g_Wnd, HWND_TOPMOST, 0, 0, screenW, screenH, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
    SetTimer(g_Wnd, 1, 100, NULL);

    g_HookKb = SetWindowsHookExW(WH_KEYBOARD_LL, (HOOKPROC)KbHook, hInstance, 0);
    g_HookMs = SetWindowsHookExW(WH_MOUSE_LL, (HOOKPROC)MsHook, hInstance, 0);

    CreateThread(NULL, 0, AudioThread, NULL, 0, NULL);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    g_Running = false;
    if (g_HookKb) UnhookWindowsHookEx(g_HookKb);
    if (g_HookMs) UnhookWindowsHookEx(g_HookMs);
    GdiplusShutdown(gdiplusToken);
    return 0;
}