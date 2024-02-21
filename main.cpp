#include <windows.h>
#include <cstdlib>
#include <ctime>

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
       
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
   
    return 0;
}

const int lineX = 25, lineY = 25, buttonX = 100, buttonY = 100;

struct Menu{
	const char* title = "Chimptest";
	int level, *space, spaces, seed;
	HWND *buttons;
	Menu(){
		level = 1;	
		buttons = new HWND[36];	
		space = new int[36];
		spaces = 0;
		seed = time(NULL);
		srand(seed);
	}
	bool addButton(HWND &handle, HINSTANCE &hInstance){
		if(level <= spaces) return false;
		seed = seed * seed % 10000;
		srand(seed);
		
		int rest = rand() % (36 - spaces) + 1, current = -1;
		for(int i = 0; i < spaces; i++){
			int difference = space[i] - current - 1;
			if(rest <= difference){
				for(int j = spaces; j > i; j--)
					space[j] = space[j - 1];
				current += rest;
				space[i] = current;
				rest = 0;
				spaces++;
				break;
			}
			rest -= difference;
			current = space[i];
		}
		if(rest){
			current += rest;
			space[spaces++] = current;	
		}
		
		int row = current / 6, column = current % 6;
		buttons[spaces - 1] = CreateWindowEx(0, "BUTTON", "", WS_CHILD | WS_VISIBLE, lineX * (column + 1) + buttonX * column, lineY * (row + 1) + buttonY * row, buttonX, buttonY, handle, NULL, hInstance, NULL);
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
	
	if(!RegisterClassEx(&window)) return false;
	
	const int X = CW_USEDEFAULT, Y = CW_USEDEFAULT, sizeX = 8 * lineX + 6 * buttonX - 5, sizeY = 9 * lineY + 6 * buttonY;
	
	handle = CreateWindowEx(WS_EX_CLIENTEDGE, menu.title, menu.title, WS_OVERLAPPEDWINDOW, X, Y, sizeX, sizeY, NULL, NULL, hInstance, NULL);
	
	return handle != NULL;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	WNDCLASSEX window;
	HWND handle;
	Menu menu;
	MSG message;
	
	if(makeWindow(hInstance, window, handle, menu)){
		ShowWindow(handle, nCmdShow);
		
		menu.level = 20;
		for(int i = 0; i < 20; i++)
			menu.addButton(handle, hInstance);
		
		while(GetMessage(&message, NULL, 0, 0)){
			
		    UpdateWindow(handle);
		    TranslateMessage(&message);
		    DispatchMessage(&message);
		    
		}
	    return message.wParam;	
	}
	else{
		MessageBox(NULL, "Bug happened!", "ERROR", MB_ICONEXCLAMATION);
		return 1;
	}
}


