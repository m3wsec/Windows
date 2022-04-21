#include <Windows.h>
#include <handleapi.h> //for DuplicateHandle() API
#include <tlhelp32.h> //for CreateToolhelp32Snapshot() and PROCESSENTRY32
#include <stdio.h>

#define PATH_MAX_LENGTH 0x100

DWORD GetProcessIdByName(LPCWSTR ProcessName) //LPCWSTR refer to const unicode string pointer
{
	//snap all process
	HANDLE processSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pEntry;
	pEntry.dwSize = sizeof(PROCESSENTRY32);		// must set the dwSize member of PROCESSENTRY32 to the size
	if (Process32First(processSnap, &pEntry))
	{
		do {
			if (!lstrcmpi(pEntry.szExeFile, ProcessName)) //compare strings according to the rules of the locale
			{
				CloseHandle(processSnap);
				return pEntry.th32ProcessID;
			}
		} while (Process32Next(processSnap,&pEntry));
		CloseHandle(processSnap);
	}	
	return 0;
}

BOOL FromDupHandleToAllAccess(DWORD processId,HANDLE* handle)
{
	HANDLE targetHandle = OpenProcess(PROCESS_DUP_HANDLE,FALSE,processId);
	BOOL res = DuplicateHandle(targetHandle, GetCurrentProcess(), GetCurrentProcess(), handle, FALSE, FALSE, DUPLICATE_SAME_ACCESS);
	return res;
}

int main()
{
	HANDLE allAccessHandle;
	LPCWSTR targetProcessName = L"explorer.exe";	//unicode const string
	wchar_t currentFilePath[PATH_MAX_LENGTH];
	HANDLE currentFileHandle;
	HANDLE targetFileHandle;

	DWORD targetProcessId = GetProcessIdByName(targetProcessName);
	printf("PID of Explorer.exe: %d\n", targetProcessId);

	if (FromDupHandleToAllAccess(targetProcessId,&allAccessHandle))
	{
		puts("FromDupHandleToAllAccess Success.");
	}
	
	GetModuleFileName(NULL, currentFilePath, PATH_MAX_LENGTH);
	if (currentFileHandle = CreateFile(currentFilePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL))		//value of The third argument 0 refers to can not open
	{
		DuplicateHandle(GetCurrentProcess(), currentFileHandle,allAccessHandle,&targetFileHandle,FALSE,FALSE,DUPLICATE_SAME_ACCESS);  //copy the current file handle back to explorer.exe process
	}


	return 0;
}