#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

// Compatible con TDM-GCC 32 bits

#define MAX 30
#define BTN_LINEAL 1
#define BTN_BINARIA 2
#define EDIT_NUMERO 3

int datos[MAX];
int n = MAX;
int objetivo = 0;
int indiceActual = -1;
bool ejecutando = false;

HWND hBtnLineal;
HWND hBtnBinaria;
HWND hEditNumero;

void GenerarDatos()
{
    srand((unsigned)time(NULL));
    for (int i = 0; i < n; i++)
    {
        datos[i] = rand() % 100 + 1;
    }
}

void OrdenarDatos()
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            if (datos[j] > datos[j + 1])
            {
                int tmp = datos[j];
                datos[j] = datos[j + 1];
                datos[j + 1] = tmp;
            }
        }
    }
}

void ObtenerNumeroBuscado()
{
    char buffer[50];
    GetWindowText(hEditNumero, buffer, sizeof(buffer));
    objetivo = atoi(buffer);
}

void DibujarDatos(HDC hdc)
{
    int ancho = 20;
    int espacio = 5;
    int baseY = 400;

    char texto[10];

    for (int i = 0; i < n; i++)
    {
        int x = 50 + i * (ancho + espacio);
        int altura = datos[i] * 3;

        HBRUSH brush;

        if (i == indiceActual)
            brush = CreateSolidBrush(RGB(0, 191, 255)); // celeste
        else
            brush = CreateSolidBrush(RGB(0, 0, 0));

        SelectObject(hdc, brush);
        Rectangle(hdc, x, baseY - altura, x + ancho, baseY);
        DeleteObject(brush);

        // Dibujar numero en cada barra
        sprintf(texto, "%d", datos[i]);
        TextOut(hdc, x, baseY - altura - 15, texto, lstrlen(texto));
    }

    // Mostrar numero buscado
    sprintf(texto, "Buscando: %d", objetivo);
    TextOut(hdc, 50, 110, texto, lstrlen(texto));
}

DWORD WINAPI BusquedaLineal(LPVOID param)
{
    HWND hwnd = (HWND)param;

    ejecutando = true;

    ObtenerNumeroBuscado();

    for (int i = 0; i < n; i++)
    {
        indiceActual = i;
        InvalidateRect(hwnd, NULL, TRUE);
        Sleep(200);

        if (datos[i] == objetivo)
        {
            MessageBox(hwnd, "Encontrado (Busqueda Lineal)", "Resultado", MB_OK);
            ejecutando = false;
            indiceActual = -1;
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        }
    }

    MessageBox(hwnd, "Numero no encontrado", "Resultado", MB_OK);

    ejecutando = false;
    indiceActual = -1;
    InvalidateRect(hwnd, NULL, TRUE);
    return 0;
}

DWORD WINAPI BusquedaBinaria(LPVOID param)
{
    HWND hwnd = (HWND)param;

    ejecutando = true;

    OrdenarDatos();
    ObtenerNumeroBuscado();

    int inicio = 0;
    int fin = n - 1;

    while (inicio <= fin)
    {
        int medio = (inicio + fin) / 2;
        indiceActual = medio;

        InvalidateRect(hwnd, NULL, TRUE);
        Sleep(400);

        if (datos[medio] == objetivo)
        {
            MessageBox(hwnd, "Encontrado (Busqueda Binaria)", "Resultado", MB_OK);
            ejecutando = false;
            indiceActual = -1;
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        }
        else if (objetivo < datos[medio])
        {
            fin = medio - 1;
        }
        else
        {
            inicio = medio + 1;
        }
    }

    MessageBox(hwnd, "Numero no encontrado", "Resultado", MB_OK);

    ejecutando = false;
    indiceActual = -1;
    InvalidateRect(hwnd, NULL, TRUE);
    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        GenerarDatos();

        CreateWindow(
            "STATIC",
            "Numero a buscar:",
            WS_VISIBLE | WS_CHILD,
            50,
            20,
            130,
            25,
            hwnd,
            NULL,
            NULL,
            NULL);

        hEditNumero = CreateWindow(
            "EDIT",
            "50",
            WS_VISIBLE | WS_CHILD | WS_BORDER,
            180,
            20,
            80,
            25,
            hwnd,
            (HMENU)EDIT_NUMERO,
            NULL,
            NULL);

        hBtnLineal = CreateWindow(
            "BUTTON",
            "Busqueda Lineal",
            WS_VISIBLE | WS_CHILD,
            280,
            20,
            150,
            40,
            hwnd,
            (HMENU)BTN_LINEAL,
            NULL,
            NULL);

        hBtnBinaria = CreateWindow(
            "BUTTON",
            "Busqueda Binaria",
            WS_VISIBLE | WS_CHILD,
            450,
            20,
            150,
            40,
            hwnd,
            (HMENU)BTN_BINARIA,
            NULL,
            NULL);
        break;

    case WM_COMMAND:
        if (!ejecutando)
        {
            if (LOWORD(wParam) == BTN_LINEAL)
            {
                CreateThread(NULL, 0, BusquedaLineal, hwnd, 0, NULL);
            }

            if (LOWORD(wParam) == BTN_BINARIA)
            {
                CreateThread(NULL, 0, BusquedaBinaria, hwnd, 0, NULL);
            }
        }
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        TextOut(hdc, 50, 80, "Visualizacion: Busqueda Lineal y Binaria", 41);

        DibujarDatos(hdc);

        EndPaint(hwnd, &ps);
    }
    break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    const char CLASS_NAME[] = "BusquedaWinAPI";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "Busqueda Lineal y Binaria - Animacion",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        950,
        520,
        NULL,
        NULL,
        hInstance,
        NULL);

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
