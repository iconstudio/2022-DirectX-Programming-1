// pch.cpp: 미리 컴파일된 헤더에 해당하는 소스 파일
// 미리 컴파일된 헤더를 사용하는 경우 컴파일이 성공하려면 이 소스 파일이 필요합니다.
#include "pch.h"

WCHAR msgBuffer[128]{};
WCHAR msgTitle[128]{};

void ErrorDisplay(const wchar_t* description)
{
	ZeroMemory(msgBuffer, sizeof(msgBuffer));

	wsprintf(msgBuffer, L"오류 → %s", description);

	MessageBox(NULL, msgBuffer, L"오류", MB_ICONERROR | MB_OK);
}
