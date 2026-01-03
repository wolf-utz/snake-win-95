#include <windows.h>

#define MAX_SNAKE 64
typedef struct {
    int x;
    int y;
} Segment;

/* Snake body */
Segment snake[MAX_SNAKE];
int snakeLength = 3;
int i;
int timerTickTimeInMs = 400;

/* Direction */
int dirX = 1;
int dirY = 0;

/* Grid config */
int x;
int y;
int rows = 8;
int cols = 8;
int cell = 32;
int gap  = 4;
int startX = 4;
int startY = 4;

/* Food */
int foodX = -1;
int foodY = -1;

int gameOver = 0;

HDC     backDC = NULL;
HBITMAP backBmp = NULL;
HBITMAP oldBmp = NULL;
HDC hdc;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    PAINTSTRUCT ps;
    HDC hdc;
    HBRUSH hBrush;
    RECT r;

    switch (msg) {
        case WM_CREATE:
            hdc = GetDC(hwnd);
            backDC  = CreateCompatibleDC(hdc);
            backBmp = CreateCompatibleBitmap(hdc, 300, 318);
            oldBmp  = (HBITMAP)SelectObject(backDC, backBmp);

            ReleaseDC(hwnd, hdc);

            /* Initial snake */
            snake[0].x = 3; snake[0].y = 3;
            snake[1].x = 2; snake[1].y = 3;
            snake[2].x = 1; snake[2].y = 3;

            srand(GetTickCount());
            SpawnFood();

            SetTimer(hwnd, 1, timerTickTimeInMs, NULL); /* 200ms tick */
            return 0;
        case WM_KEYDOWN:
            switch (wParam) {
                case VK_UP:
                    dirX = 0; dirY = -1;
                    break;
                case VK_DOWN:
                    dirX = 0; dirY = 1;
                    break;
                case VK_LEFT:
                    dirX = -1; dirY = 0;
                    break;
                case VK_RIGHT:
                    dirX = 1; dirY = 0;
                    break;
                case 'R':
                    /* Reset game */
                    snakeLength = 3;
                    snake[0].x = 3; snake[0].y = 3;
                    snake[1].x = 2; snake[1].y = 3;
                    snake[2].x = 1; snake[2].y = 3;

                    dirX = 1;
                    dirY = 0;

                    gameOver = 0;
                    SetTimer(hwnd, 1, timerTickTimeInMs, NULL);
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;
            }
            return 0;
        case WM_TIMER:
            if (gameOver)
                return 0;
            
            /* Move body: tail follows head */
            for (i = snakeLength - 1; i > 0; i--) {
                snake[i] = snake[i - 1];
            }

            /* Move head */
            snake[0].x += dirX;
            snake[0].y += dirY;

            /* Eat food */
            if (snake[0].x == foodX && snake[0].y == foodY) {
                if (snakeLength < MAX_SNAKE) {
                    snake[snakeLength] = snake[snakeLength - 1];
                    snakeLength++;
                }
                SpawnFood();
            }

            /* Wrap around */
            if (snake[0].x < 0) snake[0].x = cols - 1;
            if (snake[0].y < 0) snake[0].y = rows - 1;
            if (snake[0].x >= cols) snake[0].x = 0;
            if (snake[0].y >= rows) snake[0].y = 0;

            /* Self collision check */
            for (i = 1; i < snakeLength; i++) {
                if (snake[0].x == snake[i].x &&
                    snake[0].y == snake[i].y) {
                    gameOver = 1;
                    KillTimer(hwnd, 1);
                    break;
                }
            }

            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);

            /* Clear back buffer */
            hBrush = CreateSolidBrush(RGB(255,255,255));
            FillRect(backDC, &ps.rcPaint, hBrush);
            DeleteObject(hBrush);

            // Draw grid
            hBrush = CreateSolidBrush(RGB(192, 192, 192));
            for (x = 0; x < rows; x++) { 
                for (y = 0; y < cols; y++) { 
                    r.left   = startX + x * (cell + gap);
                    r.top    = startY + y * (cell + gap);
                    r.right  = r.left + cell;
                    r.bottom = r.top  + cell;

                    FillRect(backDC, &r, hBrush);
                }
            }

            /* Draw snake head */
            hBrush = CreateSolidBrush(RGB(0, 160, 0));
            r.left   = startX + snake[0].x * (cell + gap);
            r.top    = startY + snake[0].y * (cell + gap);
            r.right  = r.left + cell;
            r.bottom = r.top  + cell;
            FillRect(backDC, &r, hBrush);

            /* Draw body */
            hBrush = CreateSolidBrush(RGB(0, 100, 0)); /* darker */
            for (i = 1; i < snakeLength; i++) {
                r.left   = startX + snake[i].x * (cell + gap);
                r.top    = startY + snake[i].y * (cell + gap);
                r.right  = r.left + cell;
                r.bottom = r.top  + cell;
                FillRect(backDC, &r, hBrush);
            }

            /* Draw food */
            hBrush = CreateSolidBrush(RGB(200, 0, 0));
            r.left   = startX + foodX * (cell + gap);
            r.top    = startY + foodY * (cell + gap);
            r.right  = r.left + cell;
            r.bottom = r.top  + cell;
            FillRect(backDC, &r, hBrush);

            if (gameOver) {
                SetBkMode(backDC, TRANSPARENT);
                SetTextColor(backDC, RGB(200, 0, 0));
                TextOut(backDC, 60, 140, "GAME OVER", 9);
                SetTextColor(backDC, RGB(40, 20, 20));
                TextOut(backDC, 40, 160, "Pres \"R\" to try again", 21);
            }

            /* Copy buffer to screen */
            BitBlt(
                hdc,
                0, 0,
                300, 318,
                backDC,
                0, 0,
                SRCCOPY
            );

            DeleteObject(hBrush);

            EndPaint(hwnd, &ps);
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_DESTROY:
            KillTimer(hwnd, 1);

            SelectObject(backDC, oldBmp);
            DeleteObject(backBmp);
            DeleteDC(backDC);

            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(
    HINSTANCE hInstance, 
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, 
    int nCmdShow
) {
    WNDCLASS wc;
    HWND hwnd;
    MSG msg;

    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = "SnakeWin95";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&wc);

    hwnd = CreateWindow(
        "SnakeWin95", "Snake 95",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 318,
        NULL, NULL, hInstance, NULL
    );

    ShowWindow(hwnd, nCmdShow);
    SetFocus(hwnd);

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

void SpawnFood(void) {
    int i;
    int ok;

    do {
        ok = 1;
        foodX = rand() % cols;
        foodY = rand() % rows;

        /* Make sure food is not on snake */
        for (i = 0; i < snakeLength; i++) {
            if (snake[i].x == foodX && snake[i].y == foodY) {
                ok = 0;
                break;
            }
        }
    } while (!ok);
}
