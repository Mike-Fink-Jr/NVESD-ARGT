#include<Windows.h>
#include<iostream>
#include<cstdio>
#include<cstdlib>
#include <math.h>


#define PI (3.141592653589793)

//Angle Test program
//This program receives a string representation of a decimal angle from the serial port, and displays the string
//as well as an arc representation of the angle
//Used for testing angle calculations for the NVESD ARGT Display project for EE403W, Oct. 2017

LRESULT CALLBACK wndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

HWND hWnd;
HANDLE hComm;

LPWSTR tStr = (LPWSTR)malloc(20 * sizeof(WCHAR));
DWORD totalRead = 0;
DWORD bRead = 0;
char*rStr = (char*)malloc(100 * sizeof(char)),*cStr = (char*)malloc(100*sizeof(char));
int valid = FALSE;


DWORD WINAPI ThreadProc(LPVOID param);
//Entry point
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR szcmdLine, int nCmdShow) {
	tStr[0] = (WCHAR)0;
	//create and register the window class
	WNDCLASS wc = {};
	wc.hInstance = hInstance;
	wc.lpszClassName = L"AT";
	wc.lpfnWndProc = wndProc;
	RegisterClass(&wc);
	DCB serialControl = { 0 };
	//Open the serial port - accessible as file handle hComm
	hComm = CreateFile(L"\\\\.\\COM3", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	//Setup serial port- 9600 baud, no parity, one stop bit
	serialControl.DCBlength = sizeof(serialControl);
	GetCommState(hComm, &serialControl);
	serialControl.BaudRate = CBR_9600;
	serialControl.ByteSize = 8;
	serialControl.StopBits = ONESTOPBIT;
	serialControl.Parity = NOPARITY;
	SetCommState(hComm, &serialControl);
	//create and show the window
	hWnd = CreateWindow(L"AT", L"Angle Test", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 400, 400, NULL, NULL, hInstance, 0);
	ShowWindow(hWnd,nCmdShow);
	if (hComm == NULL) MessageBox(hWnd, L"Cannot open port", L"Error", MB_OK); //print error if serial port failed to open
	
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = FALSE;
	DWORD tid;
	CreateThread(&sa, 0, ThreadProc, NULL, 0,&tid); //create the serial port thread
	
	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0)) { //get and dispatch windows messages
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


//Windows message handling routine - only need to implement WM_PAINT(to paint window) and WM_DESTROY (to exit when window is closed)
LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	
	double angle=0; 
	
	
	switch (uMsg) {
	
	
	case WM_PAINT: //Repaint the window
		PAINTSTRUCT ps;
		BeginPaint(hWnd, &ps);
		RECT pos;
		
		
		if (valid) {
			
			
			for (int i = 0; i < wcslen(tStr); i++) cStr[i] = tStr[i]; //convert unicode angle string to ascii
			angle = atof(cStr); //convert angle to float
			
			//Clear the window
			pos.top = 0;
			pos.bottom = 200;
			pos.left = 0;
			pos.right = 200;
			FillRect(ps.hdc, &pos, (HBRUSH)GetStockObject(WHITE_BRUSH));
			//Print the angle value on the window - top left
			pos.top = 0.0;
			pos.right = 100.0;
			pos.left = 0.0;
			pos.bottom = 30.0;
			DrawText(ps.hdc, tStr, 10, &pos, DT_CENTER);
			//Coordinates for a box for the angle reference
			pos.top = 70.0;
			pos.right = 100.0;
			pos.bottom = 75.0;
			pos.left = 0.0;
			HBRUSH hbr = CreateSolidBrush(RGB(255, 0, 0)); //red brush
			//start the angle at 25,70
			MoveToEx(ps.hdc, 25, 70, NULL);
			SelectObject(ps.hdc, hbr);
			//Draw the angle and arc
			AngleArc(ps.hdc, 25, 70, 50, angle, -angle);
			//Draw the reference box
			FillRect(ps.hdc, &pos, hbr);
		}
		EndPaint(hWnd,&ps);
		return 0;
	
	case WM_DESTROY: //window closed - exit
		PostQuitMessage(0);
		return 0;
	default:return DefWindowProc(hWnd, uMsg, wParam, lParam); //otherwise default message handler
	}
	
}

//Thread procedure - create a simple thread to do nothing but wait on the serial port and read in chars as they come in
//then transfer the chars to the angle string and redraw the window when a complete angle string has been read in
DWORD WINAPI ThreadProc(LPVOID param) {
	SetCommMask(hComm, EV_RXCHAR);
	WaitCommEvent(hComm, &bRead, NULL);
	ReadFile(hComm, rStr, 1, &bRead, NULL); //wait for first input on serial port, then read first char
	DWORD test;
	
	do {
		if (bRead != 0) { //if a char was read
			tStr[totalRead] = rStr[0]; //copy it to unicode angle string (this will just pad null bytes onto the ascii char)
			
			if (rStr[0] == '$') { //end of line char detected - see arduino program, $ is used as end of line char

				tStr[totalRead] = (TCHAR)0; //replace $ in angle string with normal string terminator

				valid = TRUE; //OK to use angle string now,it contains a valid complete angle
				InvalidateRect(hWnd, NULL, TRUE); //force window redraw
				totalRead = 0; //set angle string pointer back to 0
			}
			else totalRead++; //end of line not detected - increase string pointer
		}
		
		ReadFile(hComm, rStr, 1, &bRead, NULL); //read next char
	} while (TRUE); //repeat forever
	return 0;
}