#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <string>

int pressedButton = 0;
HWND buttonHandle;

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
       
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    
    case WM_COMMAND:
    	pressedButton = wParam;
    	buttonHandle = (HWND)lParam;
    	break;
    	
    case WM_LBUTTONDOWN:
    	pressedButton = 1;
    	break;
    
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
   
    return 0;
}

const int lineX = 25, lineY = 25, buttonX = 100, buttonY = 100, MAX_LEVEL = 36;

struct Menu{
	const char* title = "Chimptest";
	int level, **space, spaces, seed, restOfButtons;
	HWND *buttons, *handle, *texts;
	HINSTANCE *hInstance;
	bool waitingToReset;
	
	void reset(){
		for(int i = 0; i < spaces; i++){
			drawBorder("white", space[i][0]);
			DestroyWindow(buttons[i]);
			DestroyWindow(texts[i]);
		}		
		level = 0;	
		restOfButtons = 0;
		spaces = 0;
		seed = time(NULL);
		srand(seed);
		waitingToReset = false;
		delete buttons;
		delete texts;
		delete space;
		buttons = new HWND[MAX_LEVEL];	
		texts = new HWND[MAX_LEVEL];
		space = new int* [MAX_LEVEL];
		for(int i = 0; i < MAX_LEVEL; i++) space[i] = new int[2];
	}
	
	Menu(HWND &handle, HINSTANCE &hInstance){
		this->handle = &handle;
		this->hInstance = &hInstance;
		buttons = new HWND[MAX_LEVEL];	
		texts = new HWND[MAX_LEVEL];
		space = new int* [MAX_LEVEL];
		for(int i = 0; i < MAX_LEVEL; i++) space[i] = new int[2];
	}
	
	bool addButton(){
		if(level <= spaces) return false;
		seed = seed * seed % 10000;
		srand(seed);
		
		int rest = rand() % (36 - spaces) + 1, current = -1;
		for(int i = 0; i < spaces; i++){
			int difference = space[i][0] - current - 1;
			if(rest <= difference){
				for(int j = spaces; j > i; j--){
					space[j][0] = space[j - 1][0];
					space[j][1] = space[j - 1][1];
				}
				current += rest;
				space[i][0] = current;
				space[i][1] = spaces; 
				rest = 0;
				spaces++;
				break;
			}
			rest -= difference;
			current = space[i][0];
		}
		if(rest){
			current += rest;
			space[spaces][0] = current;	
			space[spaces][1] = spaces;
			spaces++;
		}
		texts[spaces - 1] = CreateWindowEx(0, "STATIC", NULL, WS_CHILD | WS_VISIBLE | SS_CENTER, calcX(current), calcY(current), buttonX, buttonY, *handle, NULL, *hInstance, NULL);
		SetWindowText(texts[spaces - 1], std::to_string(spaces).c_str());
		
		buttons[spaces - 1] = CreateWindowEx(0, "BUTTON", "", WS_CHILD | WS_VISIBLE, calcX(current), calcY(current), buttonX, buttonY, *handle, (HMENU)(200 + spaces), *hInstance, NULL);	
	}
	
	int indexOfButton(int number){
		for(int i = 0; i < spaces; i++)
			if(space[i][1] == number - 1) return space[i][0];
		return -1;
	}
	
	bool checkButton(){
		int pressedButton = ::pressedButton - 200, position = indexOfButton(pressedButton);
		if(pressedButton == level - restOfButtons + 1){
			ShowWindow(buttons[pressedButton - 1], SW_HIDE);
			if(position != -1) drawBorder("green", position);
			restOfButtons--;
			if(restOfButtons == 0){
				if(level < MAX_LEVEL) nextLevel();
				else end(true);
			}
			return false;
		}
		if(position != -1) drawBorder("red", position);
		return true;
	}
	
	void check(){
		if(!pressedButton) return;
		
		//MessageBox(*handle, std::to_string(pressedButton).c_str(), "pressedButton message number", MB_OK);
		
		if(pressedButton > 200){
			if(checkButton()) end(false);
		}
		else if(waitingToReset){
			//MessageBox(*handle, std::to_string(pressedButton).c_str(), "pressedButton message number for reset", MB_OK);
			reset();
			nextLevel();
		}
		pressedButton = 0;
	}
	
	void restoreButtons(){
		for(int i = 0; i < spaces; i++){
			drawBorder("white", space[i][0]);
			ShowWindow(buttons[i], SW_SHOW);
		}
	}
	
	void hideButtons(){
		for(int i = 0; i < spaces; i++)
			ShowWindow(buttons[i], SW_HIDE);
	}
	
	void drawLine(std::string color, int fromX, int fromY, int toX, int toY){
		COLORREF penColor = (color == "red" ? 0x0000FF : color == "green" ? 0x00FF00 : 0xFFFFFF);
		HPEN pen = CreatePen(PS_SOLID, 5, penColor);
		HDC hdc = GetDC(*handle);
		SelectObject(hdc, pen);
		MoveToEx(hdc, fromX, fromY, NULL);
		LineTo(hdc, toX, toY);
		ReleaseDC(*handle, hdc);
		DeleteObject(pen);
	}
	
	void drawBorder(std::string color, int number){
		int x = calcX(number), y = calcY(number);
		drawLine(color, x, y, x + buttonX, y);	
		drawLine(color, x, y, x, y + buttonY);
		drawLine(color, x + buttonX, y, x + buttonX, y + buttonY);
		drawLine(color, x, y + buttonY, x + buttonX, y + buttonY);
	}
	
	void end(bool success){
		hideButtons();
		
		waitingToReset = true;
	}
	
	void nextLevel(){
		if(level >= MAX_LEVEL) end(true);
		restoreButtons();
		level++;
		restOfButtons = level;
		addButton();
	}
	
	int calcRow(int number){
		return number / 6;
	}
	
	int calcColumn(int number){
		return number % 6;
	}
	
	int calcX(int number){
		return lineX * (calcColumn(number) + 1) + buttonX * calcColumn(number);
	}
	
	int calcY(int number){
		return lineY * (calcRow(number) + 1) + buttonX * calcRow(number);
	}
};

bool makeWindow(HINSTANCE &hInstance, WNDCLASSEX &window, HWND &handle, Menu &menu)
{
	window.cbSize = sizeof(WNDCLASSEX);
	window.style = 0;
	window.lpfnWndProc = WindowProcedure;
	window.cbClsExtra = 0;
	window.cbWndExtra = 0;
	window.hInstance = hInstance;
	window.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	window.hCursor = LoadCursor(NULL, IDC_ARROW);
	window.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	window.lpszMenuName = NULL;
	window.lpszClassName = menu.title;
	window.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	
	if(FindWindow(window.lpszClassName, NULL)){
		PostQuitMessage(0);
		return false;
	}
	if(!RegisterClassEx(&window)) return false;
	
	const int X = CW_USEDEFAULT, Y = CW_USEDEFAULT, sizeX = 8 * lineX + 6 * buttonX - 5, sizeY = 9 * lineY + 6 * buttonY;
	
	handle = CreateWindowEx(WS_EX_CLIENTEDGE, menu.title, menu.title, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX, X, Y, sizeX, sizeY, NULL, NULL, hInstance, NULL);
	
	return handle != NULL;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX window;
	HWND handle;
	Menu menu(handle, hInstance);
	MSG message;
	
	HANDLE appOpenedMutex = CreateMutex(NULL, FALSE, "appOpenedMutex");
	if(appOpenedMutex && GetLastError()) return 0;
	
	if(makeWindow(hInstance, window, handle, menu)){
		ShowWindow(handle, nCmdShow);
		
		menu.reset();
		menu.nextLevel();
		
		while(GetMessage(&message, NULL, 0, 0)){
			
		    UpdateWindow(handle);
		    TranslateMessage(&message);
		    DispatchMessage(&message);
		    
		    menu.check();
		}
	    return message.wParam;
	}
	else{
		MessageBox(NULL, "Bug happened!", "ERROR", MB_ICONEXCLAMATION);
		return 1;
	}
}


