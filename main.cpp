#include <windows.h>
#include <iostream>
#include <cstdlib>
#include <ctime>

int main() {
    // Скрываем окно консоли, чтобы оно не мешало графике
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    // Инициализируем генератор случайных чисел
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Получаем размеры экрана
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Получаем контекст устройства (DC) для всего экрана
    HDC hdc = GetDC(NULL);
    if (!hdc) return 1;

    // Создаем черную кисть для заполнения фона
    HBRUSH hBlackBrush = CreateSolidBrush(RGB(0, 0, 0));
    RECT screenRect = { 0, 0, screenWidth, screenHeight };

    // Заливаем весь экран черным цветом один раз при запуске
    FillRect(hdc, &screenRect, hBlackBrush);

    // Настраиваем параметры текста
    SetTextColor(hdc, RGB(255, 255, 255)); // Белый цвет текста
    SetBkColor(hdc, RGB(0, 0, 0));         // Черный фон под текстом
    SetBkMode(hdc, OPAQUE);                // Включаем непрозрачный фон для текста

    // Бесконечный цикл вывода надписей (выход через Alt+F4 или Диспетчер задач)
    while (true) {
        // Генерируем случайные координаты в пределах экрана
        int x = std::rand() % (screenWidth - 50);
        int y = std::rand() % (screenHeight - 20);

        // Рисуем текст "TEST"
        TextOutA(hdc, x, y, "TEST", 4);

        // Небольшая задержка в миллисекундах, чтобы не перегружать процессор
        Sleep(10);
    }

    // Освобождаем ресурсы (код сюда не дойдет, но это хорошая практика)
    ReleaseDC(NULL, hdc);
    DeleteObject(hBlackBrush);
    return 0;
}
