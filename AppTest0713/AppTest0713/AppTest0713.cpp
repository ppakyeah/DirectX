// AppTest0713.cpp: 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "AppTest0713.h"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	MessageBox(NULL, TEXT("Hello"), TEXT("App"), MB_OK);
	return 0;
}