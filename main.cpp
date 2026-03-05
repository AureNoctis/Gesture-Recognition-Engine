#include <stdio.h>
#include <Windows.h>


// global_variables

bool isRunning = false;

//======

LRESULT CALLBACK win32_mainWindowCallback(HWND window, UINT message, WPARAM wParam, LPARAM lParam){

}


int CALLBACK wWinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow){

    WNDCLASS windowClass = {
        .style = CS_VREDRAW | CS_HREDRAW,
        .lpfnWndProc = win32_mainWindowCallback,
        .hInstance = instance,
        .lpszClassName = L"G.R.E",
    };

    if(RegisterClass(&windowClass)){
        HWND window = CreateWindowEx(0, windowClass.lpszClassName, L"G.R.E", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                     CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                     CW_USEDEFAULT, 0, 0, instance, 0);
        isRunning = true;
        if(window){
            while(isRunning){
                MSG message;
                while(GetMessageA(&message, window,))

            }
        }else{}
    }else{}
}