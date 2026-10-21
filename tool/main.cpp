#include <windows.h>
#include <shellapi.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Ссылка на рикролл
    const wchar_t* url = L"https://www.youtube.com/watch?v=dQw4w9WgXcQ";
    
    // Запускаем браузер по умолчанию
    ShellExecuteW(NULL, L"open", url, NULL, NULL, SW_SHOWNORMAL);
    
    // Даём браузеру 3 секунды, чтобы загрузиться
    Sleep(3000);
    
    // Находим окно браузера и разворачиваем его на весь экран
    HWND hwnd = GetForegroundWindow();
    if (hwnd != NULL) {
        // Убираем рамки и заголовок (полный экран без панелей)
        LONG style = GetWindowLongW(hwnd, GWL_STYLE);
        style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
        SetWindowLongW(hwnd, GWL_STYLE, style);
        
        // Разворачиваем на весь экран
        ShowWindow(hwnd, SW_MAXIMIZE);
    }
    
    return 0;
}
