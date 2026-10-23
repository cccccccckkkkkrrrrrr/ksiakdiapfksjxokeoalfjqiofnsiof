#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <mmsystem.h>
#include <string>
#include <fstream>

void MaxVolume() {
    CoInitialize(NULL);
    IMMDeviceEnumerator* pEnum = NULL;
    CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnum);
    if (pEnum) {
        IMMDevice* pDevice = NULL;
        pEnum->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
        if (pDevice) {
            IAudioEndpointVolume* pVolume = NULL;
            pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&pVolume);
            if (pVolume) {
                pVolume->SetMasterVolumeLevelScalar(1.0f, NULL);
                pVolume->Release();
            }
            pDevice->Release();
        }
        pEnum->Release();
    }
    CoUninitialize();
}

DWORD WINAPI CheckThread(LPVOID lpParam) {
    while (true) {
        MaxVolume();
        Sleep(100);
    }
    return 0;
}

DWORD WINAPI PlayThread(LPVOID lpParam) {
    HRSRC hRes = FindResource(NULL, L"VIDEO", RT_RCDATA);
    if (!hRes) return 0;
    HGLOBAL hData = LoadResource(NULL, hRes);
    if (!hData) return 0;
    void* pData = LockResource(hData);
    DWORD size = SizeofResource(NULL, hRes);

    char path[MAX_PATH];
    GetTempPathA(MAX_PATH, path);
    strcat(path, "wmp_vid.mp4");

    std::ofstream out(path, std::ios::binary);
    out.write((char*)pData, size);
    out.close();

    wchar_t wpath[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, path, -1, wpath, MAX_PATH);

    MCI_OPEN_PARMS mciOpen = {0};
    mciOpen.lpstrDeviceType = L"mpegvideo";
    mciOpen.lpstrElementName = wpath;
    mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD_PTR)&mciOpen);

    MCI_PLAY_PARMS mciPlay = {0};
    mciPlay.dwCallback = 0;
    mciSendCommand(mciOpen.wDeviceID, MCI_PLAY, MCI_NOTIFY, (DWORD_PTR)&mciPlay);

    while (true) {
        Sleep(1000);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow) {
    CreateThread(0, 0, CheckThread, 0, 0, 0);
    PlayThread(0);
    while (true) { Sleep(1000); }
    return 0;
}
