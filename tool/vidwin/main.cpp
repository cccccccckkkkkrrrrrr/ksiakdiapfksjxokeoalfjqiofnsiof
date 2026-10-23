#include <windows.h>
#include <mmsystem.h>
#include <fstream>

DWORD WINAPI CheckThread(LPVOID lpParam) {
    while (true) {
        keybd_event(VK_VOLUME_UP, 0, 0, 0);
        keybd_event(VK_VOLUME_UP, 0, KEYEVENTF_KEYUP, 0);
        Sleep(100);
    }
    return 0;
}

DWORD WINAPI PlayThread(LPVOID lpParam) {
    HRSRC hRes = FindResourceA(NULL, "VIDEO", RT_RCDATA);
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

    MCI_OPEN_PARMS mciOpen = {0};
    mciOpen.lpstrDeviceType = "mpegvideo";
    mciOpen.lpstrElementName = path;
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
