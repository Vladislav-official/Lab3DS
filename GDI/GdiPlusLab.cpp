// GdiPlusLab.cpp : Defines the entry point for the application.
//

#include "pch.h"
#include "framework.h"
#include "GdiPlusLab.h"
#include <Gdiplus.h>
#include <string>
#include <windowsx.h>
using namespace std;

using namespace Gdiplus;

#define MAX_LOADSTRING 100

static wchar_t textArray[1024] = L"Text";
static wchar_t fontArray[1024] = L"Arial";

HRGN textRegion;
HRGN allRegion;

COLORREF ShowColorDialog(HWND);

COLORREF gColor = RGB(255, 255, 255);

static int fontSize = 48;
static int borderRed = 128, borderGreen = 128, borderBlue = 255;
static int innerRed = 255, innerGreen = 255, innerBlue = 255;
static int backgroundRed = 255, backgroundGreen = 255, backgroundBlue = 0;
static int posX = 0, posY = 0;
static float coeffX = 1.0F, coeffY = 1.0F;
static bool isHigh = true;
static bool isRegion = false;


static Color colorBorder = Color(borderRed, borderGreen, borderBlue);
static Color colorInner = Color(innerRed, innerGreen, innerBlue);
static Color colorBackground = Color(backgroundRed, backgroundGreen, backgroundBlue);

HWND hWnd;
HRGN r;
// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GDIPLUSLAB, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GDIPLUSLAB));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    GdiplusShutdown(gdiplusToken);

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GDIPLUSLAB));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = 0;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindowW(szWindowClass, szTitle, WS_BORDER,
       CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
   SetWindowLong(hWnd, GWL_STYLE, 0);
   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


INT_PTR CALLBACK Choose(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    
    switch (uMsg)
    {
    case WM_COMMAND:
    {
        if (LOWORD(wParam) == IDC_BORDERB) {
            Color gColor = ShowColorDialog(hWnd);
            colorBorder = Color((int)gColor.GetBlue(), (int)gColor.GetGreen(), (int)gColor.GetRed());
        }
        if (LOWORD(wParam) == IDC_INNERB) {
            Color gColor = ShowColorDialog(hWnd);
            colorInner = Color((int)gColor.GetBlue(), (int)gColor.GetGreen(), (int)gColor.GetRed());
        }
        if (LOWORD(wParam) == IDC_BGR) {
            Color gColor = ShowColorDialog(hWnd);
            colorBackground = Color((int)gColor.GetBlue(), (int)gColor.GetGreen(), (int)gColor.GetRed());
        }
        if (LOWORD(wParam) == IDC_BGG) {
            if (SendDlgItemMessage(hwnd, IDC_BGG, BM_GETCHECK, 0, 0)) {
                isHigh = true;
            }
            else {
                isHigh = false;
            }
        }
        if (LOWORD(wParam) == IDC_REGION) {
            if (SendDlgItemMessage(hwnd, IDC_REGION, BM_GETCHECK, 0, 0)) {
                isRegion = true;
            }
            else {
                isRegion = false;
            }
        }
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            BOOL bSuccess;
            TCHAR textArrayTemp[1024];
            TCHAR fontArrayTemp[1024];
            
            GetDlgItemText(hwnd, IDC_TEXT, textArrayTemp, 1000);
            GetDlgItemText(hwnd, IDC_FONT, fontArrayTemp, 1000);
            if (_tcslen(textArrayTemp) == 0) {
                wcscpy_s(textArrayTemp, L"Text");
            }
            if (_tcslen(fontArrayTemp) == 0) {
                wcscpy_s(fontArrayTemp, L"Arial");
            }
            int fontSizeTemp = GetDlgItemInt(hwnd, IDC_FONTSIZE, &bSuccess, FALSE);
           
            int posXTemp = GetDlgItemInt(hwnd, IDC_POSX, &bSuccess, FALSE), posYTemp = GetDlgItemInt(hwnd, IDC_POSY, &bSuccess, FALSE);
            float scaling = GetDlgItemInt(hwnd, IDC_EDIT3, &bSuccess, FALSE);
          
            wcscpy_s(textArray, textArrayTemp);
            wcscpy_s(fontArray, fontArrayTemp);


            if (fontSizeTemp > 0) {
                fontSize = fontSizeTemp;
            }
           
            posX = posXTemp;
            posY = posYTemp;
            if (scaling != 0) {
                coeffX = scaling;
                coeffY = scaling;
            }
            

            EndDialog(hwnd, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
    }
    break;
    }

    return (INT_PTR)FALSE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HRGN r;
    switch (message)
    {
    case WM_COMMAND:
        {
            
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            
            PAINTSTRUCT ps;
            RECT rClient;
            GetClientRect(hWnd, &rClient);
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            {
                Graphics graphics(hdc);
                if (isHigh) {
                    graphics.SetSmoothingMode(SmoothingModeHighQuality);
                    graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
                }
                else {
                    graphics.SetSmoothingMode(SmoothingModeDefault);
                    graphics.SetInterpolationMode(InterpolationModeDefault);
                }

                SolidBrush brushYellow(colorBackground);

                graphics.FillRectangle(&brushYellow, 0, 0, (int)rClient.right, (int)rClient.bottom);
                graphics.ScaleTransform(coeffX, coeffY, MatrixOrderAppend);
                

                FontFamily fontFamily(fontArray);
                StringFormat strformat;
          
                GraphicsPath path;
                path.AddString(textArray, wcslen(textArray), &fontFamily,
                    FontStyleRegular, fontSize, Gdiplus::Point(posX, posY), &strformat);
                Pen pen(colorBorder, 3);
                pen.SetLineJoin(LineJoinRound);
                
                graphics.DrawPath(&pen, &path);
                SolidBrush brushWhite(colorInner);
                graphics.FillPath(&brushWhite, &path);


                Region myRegion(&path);
 
                Rect *rect = new Rect(rClient.left, rClient.top, rClient.right, rClient.bottom);
                Region all(*rect);
                
                
                textRegion = myRegion.GetHRGN(&graphics);
                allRegion = all.GetHRGN(&graphics);
                if (isRegion) {
                    CombineRgn(allRegion, allRegion, textRegion, RGN_DIFF);
                }
                else {
                    CombineRgn(allRegion, allRegion, textRegion, RGN_OR);
                }
                
            }
            EndPaint(hWnd, &ps);
            SetWindowRgn(hWnd, allRegion, TRUE);
            

        }
        break;
    case WM_RBUTTONDOWN:
        if (PtInRegion(textRegion, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)) == 0) {
            isHigh = false;
            isRegion = false;
            DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, Choose);
            InvalidateRect(hWnd, NULL, true);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;

    }
    return (INT_PTR)FALSE;
}


COLORREF ShowColorDialog(HWND hwnd) {

    CHOOSECOLOR cc;
    static COLORREF crCustClr[16];

    ZeroMemory(&cc, sizeof(cc));
    cc.lStructSize = sizeof(cc);
    cc.hwndOwner = hwnd;
    cc.lpCustColors = (LPDWORD)crCustClr;
    cc.rgbResult = RGB(0, 255, 0);
    cc.Flags = CC_FULLOPEN | CC_RGBINIT;
    ChooseColor(&cc);

    return cc.rgbResult;
}
