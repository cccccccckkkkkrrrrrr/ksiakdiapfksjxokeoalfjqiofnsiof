; Assembler Screamer (Black Screen)
; x64 NASM / Win32 API
; Compile: nasm -f win64 main.asm -o main.obj
; Link: x86_64-w64-mingw32-gcc -o screamer.exe main.obj -mwindows

BITS 64
default rel

extern ExitProcess
extern CreateWindowExW
extern ShowWindow
extern UpdateWindow
extern GetMessageW
extern TranslateMessage
extern DispatchMessageW
extern DefWindowProcW
extern RegisterClassW
extern GetSystemMetrics
extern GetStockObject
extern LoadCursorW
extern BeginPaint
extern EndPaint
extern CreateSolidBrush
extern FillRect
extern DeleteObject
extern DestroyWindow

section .data
    className   dw 'B','l','a','c','k','S','c','r','e','e','n','C','l','a','s','s',0
    windowName  dw 'B','l','a','c','k',' ','S','c','r','e','e','n',0
    brushColor  dd 0
    cursorId    dd 32512 ; IDC_ARROW

section .bss
    wc          resb 72   ; WNDCLASSW структура (x64 размер ~72 байта)
    msg         resb 48   ; MSG структура (x64 размер ~48 байт)
    ps          resb 72   ; PAINTSTRUCT (x64 размер ~72 байта)
    rect        resb 16   ; RECT
    hwnd        resq 1
    hInstance   resq 1
    screenW     resd 1
    screenH     resd 1

section .text
global WinMain

WinMain:
    push rbp
    mov rbp, rsp
    sub rsp, 32

    ; Сохраняем hInstance
    mov [hInstance], rcx

    ; Получаем размеры экрана
    mov ecx, 0 ; SM_CXSCREEN
    call GetSystemMetrics
    mov [screenW], eax
    mov ecx, 1 ; SM_CYSCREEN
    call GetSystemMetrics
    mov [screenH], eax

    ; Заполняем WNDCLASSW
    ; hbrBackground = BLACK_BRUSH (4)
    mov ecx, 4
    call GetStockObject
    mov [wc + 56], rax      ; hbrBackground (смещение 56)

    ; hCursor = LoadCursorW(NULL, IDC_ARROW)
    xor ecx, ecx
    mov edx, 32512          ; IDC_ARROW
    call LoadCursorW
    mov [wc + 40], rax      ; hCursor (смещение 40)

    ; lpfnWndProc = WndProc
    lea rax, [WndProc]
    mov [wc + 8], rax       ; lpfnWndProc (смещение 8)

    ; hInstance
    mov rax, [hInstance]
    mov [wc + 24], rax      ; hInstance (смещение 24)

    ; lpszClassName = className
    lea rax, [className]
    mov [wc + 64], rax      ; lpszClassName (смещение 64)

    ; RegisterClassW(&wc)
    lea rcx, [wc]
    call RegisterClassW

    ; CreateWindowExW(WS_EX_TOPMOST, className, windowName, WS_POPUP, 0, 0, screenW, screenH, NULL, NULL, hInstance, NULL)
    mov ecx, 8              ; WS_EX_TOPMOST
    lea rdx, [className]
    lea r8, [windowName]
    mov r9d, 0x80000000     ; WS_POPUP
    mov eax, [screenW]
    mov r10d, eax           ; nWidth
    mov eax, [screenH]
    mov r11d, eax           ; nHeight
    ; Остальные аргументы (0, 0, NULL, NULL, hInstance, NULL)
    sub rsp, 64
    mov dword [rsp], 0          ; X = 0
    mov dword [rsp+8], 0        ; Y = 0
    mov qword [rsp+16], 0       ; hWndParent = NULL
    mov qword [rsp+24], 0       ; hMenu = NULL
    mov rax, [hInstance]
    mov qword [rsp+32], rax     ; hInstance
    mov qword [rsp+40], 0       ; lpParam = NULL
    call CreateWindowExW
    add rsp, 64

    mov [hwnd], rax

    ; ShowWindow(hwnd, SW_SHOW)
    mov rcx, rax
    mov edx, 5 ; SW_SHOW
    call ShowWindow

    ; UpdateWindow(hwnd)
    mov rcx, [hwnd]
    call UpdateWindow

    ; Цикл сообщений
    msg_loop:
        lea rcx, [msg]
        xor edx, edx
        xor r8d, r8d
        xor r9d, r9d
        call GetMessageW
        cmp eax, 0
        jle exit_loop

        lea rcx, [msg]
        call TranslateMessage

        lea rcx, [msg]
        call DispatchMessageW

        jmp msg_loop

    exit_loop:
    xor ecx, ecx
    call ExitProcess

; --- WndProc ---
WndProc:
    push rbp
    mov rbp, rsp
    sub rsp, 32

    ; rcx = hwnd, edx = msg, r8 = wParam, r9 = lParam
    cmp edx, 0x000F    ; WM_PAINT
    je .wm_paint
    cmp edx, 0x0100    ; WM_KEYDOWN
    je .wm_keydown
    cmp edx, 0x0002    ; WM_DESTROY
    je .wm_destroy

    ; DefWindowProcW(hwnd, msg, wParam, lParam)
    call DefWindowProcW
    jmp .end

.wm_paint:
    ; BeginPaint(hwnd, &ps)
    lea rdx, [ps]
    call BeginPaint
    mov r10, rax ; hdc

    ; CreateSolidBrush(RGB(0,0,0)) = 0
    xor ecx, ecx
    call CreateSolidBrush
    mov r11, rax ; hbrush

    ; FillRect(hdc, &ps.rcPaint, hbrush)
    mov rcx, r10
    lea rdx, [ps + 32] ; ps.rcPaint (смещение 32)
    mov r8, r11
    call FillRect

    ; DeleteObject(hbrush)
    mov rcx, r11
    call DeleteObject

    ; EndPaint(hwnd, &ps)
    lea rdx, [ps]
    call EndPaint

    xor eax, eax
    jmp .end

.wm_keydown:
    ; Если нажат Esc (0x1B) — закрываем окно
    cmp r8d, 0x1B
    jne .wm_keydown_end
    ; DestroyWindow(hwnd)
    call DestroyWindow
.wm_keydown_end:
    xor eax, eax
    jmp .end

.wm_destroy:
    ; PostQuitMessage(0) — но мы используем ExitProcess, так что просто выходим
    xor ecx, ecx
    call ExitProcess

.end:
    leave
    ret
