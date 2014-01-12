
#include "RegHook.h"
#include "detours.h"
#pragma comment(lib, "advapi32.lib")//release�汾 ��Ҫ���� ���й�ע���API��
#pragma comment(lib, "User32.lib")//release ��Ҫ���� ���й���Ϣ��API��
#pragma comment(lib, "detours.lib")//detous ��̬��

#pragma comment(linker, "/OPT:nowin98")
/**********************************************************************************************/
//DLL�������ݶ�
#pragma data_seg(".shareddata")
HWND	g_hWnd = NULL;//���Ӵ��ھ��
HHOOK	g_hHook= NULL;//Hook ���
DWORD	PID = 0; // Ҫ���ӵĽ��� pid��
DWORD	ExplorerPID = 0;//��Դ������PID
DWORD	RegMonPID = 0;//���ӽ���PID
//�Ƿ���ӵ���Ϊ
BOOL	HOOK_RegCreateKeyEx = TRUE;
BOOL	HOOK_RegSetValueEx = TRUE;
BOOL	HOOK_RegDeleteKey = TRUE;
BOOL	HOOK_RegDeleteValue = TRUE;
BOOL	HOOK_RegQueryValueEx = TRUE;
#pragma data_seg()
#pragma comment(linker,"/section:.shareddata,rws")

bool	g_bIntercepted	= false;
bool	g_bTryed = false;
//���Ӻ���
LRESULT CALLBACK ShellProc( int nCode, WPARAM wParam, LPARAM lParam );
void Intercept();//��װ����
void UnIntercept();//ж������

/************************************************************ԭ��******************************************************************/
// ����WIN32 API��ԭ�� 
// Detours�������������
//(1) Target������Ҫ���صĺ�����ͨ��ΪWindows��API��
//(2) Trampoline������Target�����Ĳ��ָ���Ʒ����ΪDetours�����дTarget�����������Ȱ�Target������ǰ5���ֽڸ��Ʊ���ã�һ������Ȼ����
//    Target�����Ĺ��̵������壬��һ��������Ժ�Ļָ���
//(3) Detour �������������Target�����ĺ����� 
// Detours��Target�����Ŀ�ͷ����JMP Address_of_ Detour_ Functionָ���5���ֽڣ��Ѷ�Target���� �ĵ����������Լ���Detour������ ��
// Target�����Ŀ�ͷ��5���ֽڼ���JMP Address_of_ Target _ Function+ 5��10���ֽ���ΪTrampoline������
/**********************************************************************************************************************************/


/************************************************************����******************************************************************/
/*DETOUR_TRAMPOLINE(trampoline_prototype, target_name);
���ܣ��ú����Ϊtarget_name ��Target��������Trampoline�������Ժ���� trampoline_prototype�������ϵ��ڵ���Target����*/

DETOUR_TRAMPOLINE(LONG WINAPI Real_RegCreateKeyExA(HKEY, LPCSTR, DWORD, LPSTR, DWORD, REGSAM, LPSECURITY_ATTRIBUTES, PHKEY,
				  LPDWORD), RegCreateKeyExA);

DETOUR_TRAMPOLINE(LONG WINAPI Real_RegCreateKeyExW(HKEY, LPCWSTR, DWORD, LPWSTR, DWORD,REGSAM, LPSECURITY_ATTRIBUTES, PHKEY, 
				  LPDWORD), RegCreateKeyExW);


DETOUR_TRAMPOLINE(LONG WINAPI Real_RegSetValueExA(HKEY, LPCSTR, DWORD, DWORD, CONST BYTE*, DWORD), RegSetValueExA);


DETOUR_TRAMPOLINE(LONG WINAPI Real_RegSetValueExW(HKEY, LPCWSTR, DWORD, DWORD, CONST BYTE*, DWORD), RegSetValueExW);


DETOUR_TRAMPOLINE(LONG WINAPI Real_RegDeleteKeyA(HKEY, LPCSTR), RegDeleteKeyA);

DETOUR_TRAMPOLINE(LONG WINAPI Real_RegDeleteKeyW(HKEY, LPCWSTR), RegDeleteKeyW);


DETOUR_TRAMPOLINE(LONG WINAPI Real_RegDeleteValueA(HKEY, LPCSTR), RegDeleteValueA);


DETOUR_TRAMPOLINE(LONG WINAPI Real_RegDeleteValueW(HKEY, LPCWSTR), RegDeleteValueW);


DETOUR_TRAMPOLINE(LONG WINAPI Real_RegQueryValueExA(HKEY, LPCSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD), RegQueryValueExA);

DETOUR_TRAMPOLINE(LONG APIENTRY Real_RegQueryValueExW(HKEY, LPCWSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD), RegQueryValueExW);

/**********************************************///�����������///**************************************************************/

// Replace_ �����ʵ�� API  Real_ ��Trampoline����  

LONG WINAPI Replace_RegCreateKeyExA(HKEY hKey,LPCSTR lpSubKey,DWORD Reserved,LPSTR lpClass,DWORD dwOptions,REGSAM samDesired,
									LPSECURITY_ATTRIBUTES lpSecurityAttributes,PHKEY phkResult,LPDWORD lpdwDisposition);

LONG WINAPI Replace_RegCreateKeyExW(HKEY hKey,LPCWSTR lpSubKey,DWORD Reserved,LPWSTR lpClass,DWORD dwOptions,REGSAM samDesired,
									LPSECURITY_ATTRIBUTES lpSecurityAttributes,PHKEY phkResult,LPDWORD lpdwDisposition);

LONG WINAPI Replace_RegSetValueExA(HKEY hKey,LPCSTR lpValueName,DWORD Reserved,DWORD dwType,CONST BYTE* lpData,DWORD cbData);				

LONG WINAPI Replace_RegSetValueExW(HKEY hKey,LPCWSTR lpValueName,DWORD Reserved,DWORD dwType,CONST BYTE* lpData,DWORD cbData);

LONG WINAPI Replace_RegDeleteKeyA(HKEY hKey,LPCSTR lpSubKey);

LONG WINAPI Replace_RegDeleteKeyW(HKEY hKey,LPCWSTR lpSubKey );

LONG WINAPI Replace_RegDeleteValueA(HKEY hKey,LPCSTR lpValueName);

LONG WINAPI Replace_RegDeleteValueW(HKEY hKey,LPCWSTR lpValueName);

LONG WINAPI Replace_RegQueryValueExA(HKEY hKey, LPCSTR lpValueName,LPDWORD lpReserved,LPDWORD lpType, LPBYTE lpData,LPDWORD lpcbData);

LONG APIENTRY Replace_RegQueryValueExW(HKEY hKey,LPCWSTR lpValueName,LPDWORD lpReserved,LPDWORD lpType,LPBYTE lpData,LPDWORD lpcbData);

/**************************************************************************************************************************************/



BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH: // ж�ز���
		{		
			if (g_bIntercepted )
			{
				g_bIntercepted = false;
				UnIntercept(); // ֹͣ����
			}
		}
		break;
    }
    return TRUE;
}

/****************************************************************************************************************/

LONG WINAPI Replace_RegCreateKeyExA(HKEY hKey,LPCSTR lpSubKey,DWORD Reserved,LPSTR lpClass,DWORD dwOptions,REGSAM samDesired,
									LPSECURITY_ATTRIBUTES lpSecurityAttributes,PHKEY phkResult,LPDWORD lpdwDisposition)
{
	LONG nRet;
	__try // �����쳣
	{
		// Real_RegCreateKeyExA���滻 RegCreateKeyExA �ĺ���
		nRet = Real_RegCreateKeyExA(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired,
			lpSecurityAttributes, phkResult, lpdwDisposition);

		if(!HOOK_RegCreateKeyEx)// �ж��Ƿ����
			return nRet;
		if(GetCurrentProcessId() == ExplorerPID)//�ж��Ƿ�Ϊ��Դ����������ID  
			return nRet;
		if(GetCurrentProcessId() == RegMonPID)//�Ƿ�Ϊ���ӽ���ID
			return nRet;
		// PID = 0 ���������н���  || �ж��Ƿ�ΪҪ���ӵĽ���
		if((PID == 0) || ((GetCurrentProcessId() == PID) && (PID>0)) ) 
		{
			if(lpSubKey == NULL)
				return nRet;
			COPYDATASTRUCT data; // ����COPYDATASTRUCT�ṹ ���ڽ��̼䷢������
			PARAMS params;
			memset(&params,0,sizeof(PARAMS));
			params.PID = GetCurrentProcessId();// �õ���ǰ���� ID
			params.hKey = hKey;
			memcpy(params.buf1, lpSubKey, strlen(lpSubKey)); // �洢�Ӽ�������Ϣ
			params.result = nRet; // ȡ�ò�������ֵ
			
			data.cbData = sizeof(PARAMS); // ��ò�����С
			data.lpData =(void *)&params; // ��ֵ������ָ��
			data.dwData = TYPE_RegCreateKeyExA; // ���嶯������
			/*�����漰�����̼�ͨ�ţ�ͨ�������ַ�����
			 1��ʹ���ڴ�ӳ���ļ���
			 2��ͨ�������ڴ�DLL�����ڴ�
			 3��ʹ��SendMessage ����һ���̷���WM_COPYDATA��Ϣ
			�ڴ�ʹ�õ����ַ���
		     WM_COPYDATA��Ϣ�÷���
              http://wenku.baidu.com/view/1a078b3283c4bb4cf7ecd1ae.html
          
			*/
			// ����Ӵ��ڷ�����Ϣ
			SendMessage(g_hWnd, WM_COPYDATA, (WPARAM)g_hWnd, (LPARAM)&data);
			
		}
	}__finally
	{
	};
	return nRet;
}

// ���º�����Ĳ������Ʋ�����ϸ����

 // RegCreateKeyExW �滻
LONG WINAPI Replace_RegCreateKeyExW(HKEY hKey,LPCWSTR lpSubKey,DWORD Reserved,LPWSTR lpClass,DWORD dwOptions,REGSAM samDesired,
									LPSECURITY_ATTRIBUTES lpSecurityAttributes,PHKEY phkResult,LPDWORD lpdwDisposition)
{
	LONG nRet;
	__try
	{
		nRet = Real_RegCreateKeyExW(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired,
										lpSecurityAttributes, phkResult, lpdwDisposition);
		if(!HOOK_RegCreateKeyEx)
			return nRet;
		if(GetCurrentProcessId() == ExplorerPID)
			return nRet;
		if(GetCurrentProcessId() == RegMonPID)
			return nRet;
		if((PID == 0) || ((GetCurrentProcessId() == PID) && (PID>0)) )
		{
			if(lpSubKey == NULL)
				return nRet;
			COPYDATASTRUCT data;
			PARAMS params ;
			memset(&params,0,sizeof(PARAMS));
			params.PID = GetCurrentProcessId();
			params.hKey = hKey;
			memcpy(params.buf1, lpSubKey, wcslen(lpSubKey)*2);
			params.result = nRet;
			
			data.cbData = sizeof(PARAMS);
			data.lpData = (void *)&params;
			data.dwData = TYPE_RegCreateKeyExW;
			SendMessage(g_hWnd, WM_COPYDATA, (WPARAM)g_hWnd, (LPARAM)&data);
		}
	}__finally
	{
	};
	return nRet;
}
  
// RegCreateKeyExA �滻

LONG WINAPI Replace_RegSetValueExA(HKEY hKey,LPCSTR lpValueName,DWORD Reserved,DWORD dwType,CONST BYTE* lpData,DWORD cbData)
{
	LONG nRet;
	__try
	{
		nRet = Real_RegSetValueExA(hKey, lpValueName, Reserved, dwType, lpData, cbData);
		if(!HOOK_RegSetValueEx)
			return nRet;
		if(GetCurrentProcessId() == ExplorerPID)
			return nRet;
		if(GetCurrentProcessId() == RegMonPID)
			return nRet;
		if((PID == 0) || ((GetCurrentProcessId() == PID) && (PID>0)) )
		{
			if(lpValueName == NULL || lpData == NULL)
				return nRet;
			COPYDATASTRUCT data;
			PARAMS params ;
			memset(&params,0,sizeof(PARAMS));
			params.PID = GetCurrentProcessId();
			params.hKey = hKey;
			memcpy(params.buf1, lpValueName, strlen(lpValueName));
			params.type = dwType;
			if(cbData>MAX_BUF_LEN )
			{
				memcpy(params.buf2, lpData, 1024);
				params.cbbuf2 = MAX_BUF_LEN;
			}
			else
			{
				memcpy(params.buf2, lpData, cbData);
				params.cbbuf2 = cbData;
			}
			params.result = nRet;

			data.cbData = sizeof(PARAMS);
			data.lpData = (void *)&params;
			data.dwData = TYPE_RegSetValueExA;
			SendMessage(g_hWnd, WM_COPYDATA, (WPARAM)g_hWnd, (LPARAM)&data);
		}
	}__finally
	{
	};
	return nRet;
}

// RegSetValueExW �滻

LONG WINAPI Replace_RegSetValueExW(HKEY hKey,LPCWSTR lpValueName,DWORD Reserved,DWORD dwType,CONST BYTE* lpData,DWORD cbData)
{
	LONG nRet;
	__try
	{
		nRet = Real_RegSetValueExW(hKey, lpValueName, Reserved, dwType, lpData, cbData);
		if(!HOOK_RegSetValueEx)
			return nRet;
		if(GetCurrentProcessId() == ExplorerPID)
			return nRet;
		if(GetCurrentProcessId() == RegMonPID)
			return nRet;
		if((PID == 0) || ((GetCurrentProcessId() == PID) && (PID>0)) )
		{
			if(lpValueName == NULL || lpData == NULL)
				return nRet;
			COPYDATASTRUCT data;
			PARAMS params;
			memset(&params,0,sizeof(PARAMS));
			params.PID = GetCurrentProcessId();
			params.hKey = hKey;
			memcpy(params.buf1, lpValueName, wcslen(lpValueName)*2);
			params.type = dwType;
			if(cbData>MAX_BUF_LEN ) // �ж��Ƿ���ڵ�һ������������
			{
				memcpy(params.buf2, lpData, MAX_BUF_LEN );
				params.cbbuf2 = MAX_BUF_LEN ;
			}
			else // ����ڵڶ���������
			{
				memcpy(params.buf2, lpData, cbData);
				params.cbbuf2 = cbData;
			}
			params.result = nRet;
			
			data.cbData = sizeof(PARAMS);
			data.lpData = (void *)&params;
			data.dwData = TYPE_RegSetValueExW;
			SendMessage(g_hWnd, WM_COPYDATA, (WPARAM)g_hWnd, (LPARAM)&data);
		}
	}__finally
	{
	};
	return nRet;
}

// RegSetValueExA �滻
LONG WINAPI Replace_RegDeleteKeyA(HKEY hKey,LPCSTR lpSubKey)
{
	LONG nRet;
	__try
	{
		nRet = Real_RegDeleteKeyA(hKey, lpSubKey);
		if(!HOOK_RegDeleteKey)
			return nRet;
		if(GetCurrentProcessId() == ExplorerPID)
			return nRet;
		if(GetCurrentProcessId() == RegMonPID)
			return nRet;
		if((PID == 0) || ((GetCurrentProcessId() == PID) && (PID>0)) )
		{
			if(lpSubKey == NULL)
				return nRet;
			COPYDATASTRUCT data;
			PARAMS params;
			memset(&params,0,sizeof(PARAMS));
			params.PID = GetCurrentProcessId();
			params.hKey = hKey;
			memcpy(params.buf1, lpSubKey, strlen(lpSubKey));
			data.cbData = sizeof(PARAMS);
			data.lpData = (void *)&params;
			data.dwData = TYPE_RegDeleteKeyA;
			SendMessage(g_hWnd, WM_COPYDATA, (WPARAM)g_hWnd, (LPARAM)&data);
		}
	}__finally
	{
	};
	return nRet;
}

// RegDeleteKeyW�滻
LONG WINAPI Replace_RegDeleteKeyW(HKEY hKey,LPCWSTR lpSubKey )
{
	LONG nRet;
	__try
	{
		nRet = Real_RegDeleteKeyW(hKey, lpSubKey);
		if(!HOOK_RegDeleteKey)
			return nRet;
		if(GetCurrentProcessId() == ExplorerPID)
			return nRet;
		if(GetCurrentProcessId() == RegMonPID)
			return nRet;
		if((PID == 0) || ((GetCurrentProcessId() == PID) && (PID>0)) )
		{
			if(lpSubKey == NULL)
				return nRet;
			COPYDATASTRUCT data;
			PARAMS params;
			memset(&params,0,sizeof(PARAMS));
			params.PID = GetCurrentProcessId();
			params.hKey = hKey;
			memcpy(params.buf1, lpSubKey, wcslen(lpSubKey)*2);
			data.cbData = sizeof(PARAMS);
			data.lpData = (void *)&params;
			data.dwData = TYPE_RegDeleteKeyW;
			SendMessage(g_hWnd, WM_COPYDATA, (WPARAM)g_hWnd, (LPARAM)&data);
		}
	}__finally
	{
	};
	return nRet;
}

// RegDeleteKeyA�滻
LONG WINAPI Replace_RegDeleteValueA(HKEY hKey,LPCSTR lpValueName)
{
	LONG nRet;
	__try
	{
		nRet = Real_RegDeleteValueA(hKey, lpValueName);
		if(!HOOK_RegDeleteValue)
			return nRet;
		if(GetCurrentProcessId() == ExplorerPID)
			return nRet;
		if(GetCurrentProcessId() == RegMonPID)
			return nRet;
		if((PID == 0) || ((GetCurrentProcessId() == PID) && (PID>0)) )
		{
			if(lpValueName == NULL)
				return nRet;
			COPYDATASTRUCT data;
			PARAMS params;
			memset(&params,0,sizeof(PARAMS));
			params.PID = GetCurrentProcessId();
			params.hKey = hKey;
			memcpy(params.buf1, lpValueName, strlen(lpValueName));
			data.cbData = sizeof(PARAMS);
			data.lpData = (void *)&params;
			data.dwData = TYPE_RegDeleteValueA;
			SendMessage(g_hWnd, WM_COPYDATA, (WPARAM)g_hWnd, (LPARAM)&data);
		}
	}__finally
	{
	};
	return nRet;
}
 // RegDeleteValueW�滻
LONG WINAPI Replace_RegDeleteValueW(HKEY hKey,LPCWSTR lpValueName)
{
	LONG nRet;
	__try
	{
		nRet = Real_RegDeleteValueW(hKey,lpValueName);
		if(!HOOK_RegDeleteValue)
			return nRet;
		if(GetCurrentProcessId() == ExplorerPID)
			return nRet;
		if(GetCurrentProcessId() == RegMonPID)
			return nRet;
		if((PID == 0) || ((GetCurrentProcessId() == PID) && (PID>0)) )
		{
			if(lpValueName == NULL)
				return nRet;
			COPYDATASTRUCT data;
			PARAMS params;
			memset(&params,0,sizeof(PARAMS));
			params.PID = GetCurrentProcessId();
			params.hKey = hKey;
			memcpy(params.buf1, lpValueName, wcslen(lpValueName)*2);
			data.cbData = sizeof(PARAMS);
			data.lpData =(void *)&params;
			data.dwData = TYPE_RegDeleteValueW;
			SendMessage(g_hWnd, WM_COPYDATA, (WPARAM)g_hWnd, (LPARAM)&data);
		}
	}__finally
	{
	};
	return nRet;
}

 // RegDeleteValueA�滻
LONG WINAPI Replace_RegQueryValueExA(HKEY hKey, LPCSTR lpValueName,LPDWORD lpReserved,LPDWORD lpType, LPBYTE lpData,LPDWORD lpcbData)
{
	LONG nRet;
	__try
	{
		nRet = Real_RegQueryValueExA(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
		if(!HOOK_RegQueryValueEx)
			return nRet;
		if(GetCurrentProcessId() == ExplorerPID)
			return nRet;
		if(GetCurrentProcessId() == RegMonPID)
			return nRet;
		if((PID == 0) || ((GetCurrentProcessId() == PID) && (PID>0)) )
		{
			if(nRet != ERROR_SUCCESS)
				return nRet;
			COPYDATASTRUCT data;
			PARAMS params;
			memset(&params,0,sizeof(PARAMS));
			params.PID = GetCurrentProcessId();
			params.hKey = hKey;
			if(lpValueName == NULL || /*lpType == NULL ||*/ lpData == NULL || lpcbData == NULL)
				return nRet;
			
			memcpy(params.buf1, lpValueName, strlen(lpValueName));
			
			if(lpType == NULL)
				params.type = 0;
			else
				params.type = *lpType;
			
			if(*lpcbData == 0)
			{
				return nRet;
			}
			else if(*lpcbData>MAX_BUF_LEN )
			{
				memcpy(params.buf2, lpData, 1024);
				params.cbbuf2 = MAX_BUF_LEN;
			}
			else
			{
				memcpy(params.buf2, lpData, *lpcbData);
				params.cbbuf2 = *lpcbData;
			}
			params.result = nRet;
			
			data.cbData = sizeof(PARAMS);
			data.lpData = (void *)&params;
			data.dwData = TYPE_RegQueryValueExA;

		::SendMessage(g_hWnd, WM_COPYDATA, (WPARAM)g_hWnd, (LPARAM)&data);
		}
		
	}__finally
	{
	};
	return nRet;
}
  // RegQueryValueExW�滻
LONG APIENTRY Replace_RegQueryValueExW(HKEY hKey,LPCWSTR lpValueName,LPDWORD lpReserved,LPDWORD lpType,LPBYTE lpData,LPDWORD lpcbData)
{
	LONG nRet;
	__try
	{
		nRet = Real_RegQueryValueExW(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
		if(!HOOK_RegQueryValueEx)
			return nRet;
		if(GetCurrentProcessId() == ExplorerPID)
			return nRet;
		if(GetCurrentProcessId() == RegMonPID)
			return nRet;
		if((PID == 0) || ((GetCurrentProcessId() == PID) && (PID>0)) )
		{
			if(nRet != ERROR_SUCCESS)
				return nRet;

			COPYDATASTRUCT data;
			PARAMS params;
			memset(&params,0,sizeof(PARAMS));
			params.PID = GetCurrentProcessId();
			params.hKey = hKey;
			
			if(lpValueName == NULL || /*lpType == NULL ||*/ lpData == NULL || lpcbData == NULL)
				return nRet;
			
			memcpy(params.buf1, lpValueName, wcslen(lpValueName)*2);
			
			if(lpType == NULL)
				params.type = 0;
			else
				params.type = *lpType;
			
			if(*lpcbData == 0)
			{
				return nRet;
			}
			else if(*lpcbData>MAX_BUF_LEN )
			{
				memcpy(params.buf2, lpData, MAX_BUF_LEN );
				params.cbbuf2 = MAX_BUF_LEN ;
			}
			else
			{
				memcpy(params.buf2, lpData, *lpcbData);
				params.cbbuf2 = *lpcbData;
			}

			params.result = nRet;
			
			data.cbData = sizeof(PARAMS);
			data.lpData =(void *)&params;
			data.dwData = TYPE_RegQueryValueExW;
			SendMessage(g_hWnd, WM_COPYDATA, (WPARAM)g_hWnd, (LPARAM)&data);
		}
		
	}__finally
	{
	};
	return nRet;
}
  





/****************************************************************************************************************/

// ���غ���
void Intercept()
{
	// ʹ��DetourFunctionWithTrampoline�������ػ�Ŀ�꺯�����������������������trampoline������
	// ���ػ�����ָ�롣��ΪĿ�꺯���Ѿ����ӵ�trampoline�����У����в���Ҫ�ڲ������ر�ָ����

	DetourFunctionWithTrampoline((PBYTE)Real_RegCreateKeyExA, (PBYTE)Replace_RegCreateKeyExA);
	DetourFunctionWithTrampoline((PBYTE)Real_RegCreateKeyExW, (PBYTE)Replace_RegCreateKeyExW);
	DetourFunctionWithTrampoline((PBYTE)Real_RegSetValueExA, (PBYTE)Replace_RegSetValueExA);
	DetourFunctionWithTrampoline((PBYTE)Real_RegSetValueExW, (PBYTE)Replace_RegSetValueExW);
	DetourFunctionWithTrampoline((PBYTE)Real_RegDeleteKeyA, (PBYTE)Replace_RegDeleteKeyA);
	DetourFunctionWithTrampoline((PBYTE)Real_RegDeleteKeyW, (PBYTE)Replace_RegDeleteKeyW);
	DetourFunctionWithTrampoline((PBYTE)Real_RegDeleteValueA, (PBYTE)Replace_RegDeleteValueA);
	DetourFunctionWithTrampoline((PBYTE)Real_RegDeleteValueW, (PBYTE)Replace_RegDeleteValueW);
	DetourFunctionWithTrampoline((PBYTE)Real_RegQueryValueExA, (PBYTE)Replace_RegQueryValueExA);
	DetourFunctionWithTrampoline((PBYTE)Real_RegQueryValueExW, (PBYTE)Replace_RegQueryValueExW);
}

// ж������
void UnIntercept()
{
	// ����DetourRemove��ȥ����һ��Ŀ�꺯���Ľػ�
	DetourRemove((PBYTE)Real_RegCreateKeyExA,(PBYTE)Replace_RegCreateKeyExA);
	DetourRemove((PBYTE)Real_RegCreateKeyExW,(PBYTE)Replace_RegCreateKeyExW);
	DetourRemove((PBYTE)Real_RegSetValueExA,(PBYTE)Replace_RegSetValueExA);
	DetourRemove((PBYTE)Real_RegSetValueExW,(PBYTE)Replace_RegSetValueExW);
	DetourRemove((PBYTE)Real_RegDeleteKeyA, (PBYTE)Replace_RegDeleteKeyA);
	DetourRemove((PBYTE)Real_RegDeleteKeyW, (PBYTE)Replace_RegDeleteKeyW);
	DetourRemove((PBYTE)Real_RegDeleteValueA, (PBYTE)Replace_RegDeleteValueA);
	DetourRemove((PBYTE)Real_RegDeleteValueW, (PBYTE)Replace_RegDeleteValueW);
	DetourRemove((PBYTE)Real_RegQueryValueExA, (PBYTE)Replace_RegQueryValueExA);
	DetourRemove((PBYTE)Real_RegQueryValueExW, (PBYTE)Replace_RegQueryValueExW);
}


// ���Ӻ���
LRESULT CALLBACK ShellProc( int nCode, WPARAM wParam, LPARAM lParam )
{
	if(!g_bTryed)
	{
		g_bTryed=true;
		Intercept();
		g_bIntercepted=true;
	}

	return CallNextHookEx(g_hHook, nCode, wParam, lParam); // ������
}



	/************************************************************************/
	/*	����  :	ж�ع��Ӻ�������ϵͳ�еĹ���ж�س�ȥ						*/
	/*	����ֵ:	�ɹ�����true, ʧ�ܷ��� false								*/
	/************************************************************************/
 bool InstallHook(HWND handle)
{
	 // ���ù���
	g_hHook=::SetWindowsHookEx(WH_SHELL, ShellProc, GetModuleHandle("RegHook.dll"), 0);
	g_hWnd = handle;
	RegMonPID = GetCurrentProcessId();
	if(g_hHook)
		return true;
	else
		return false;
}

 
	/************************************************************************/
	/*	����  :	ж�ع��Ӻ�������ϵͳ�еĹ���ж�س�ȥ						*/
	/*	����ֵ:	�ɹ�����true, ʧ�ܷ��� false								*/
	/************************************************************************/
bool UninstallHook()
{
	if(::UnhookWindowsHookEx(g_hHook))
	{
		g_hHook = NULL;
		return true;
	}
	else
		return false;
}

	/************************************************************************/
	/*	����  :	���˺�������ָ���Ľ��̽��йҹ�����							*/
	/*	����ֵ:	��															*/
	/************************************************************************/
 void SetFilter(DWORD pid)
{
	PID = pid;
}

	/*************************************************************************/
	/*  ���� :���Ӽ��ӵ�����,��ָ������Ϊ���йҹ�����(��Ӧ�Ĳ���Ϊtrue��ʾ��)*/		
	/*	����ֵ:	��															 */
	/*************************************************************************/
 void SetHookFunction(BOOL RegCreateKeyEx, BOOL RegSetValueEx, BOOL RegDeleteKey, BOOL RegDeleteValue, BOOL RegQueryValueEx)
{
	HOOK_RegCreateKeyEx = RegCreateKeyEx;
	HOOK_RegSetValueEx = RegSetValueEx;
	HOOK_RegDeleteKey = RegDeleteKey;
	HOOK_RegDeleteValue = RegDeleteValue;
	HOOK_RegQueryValueEx = RegQueryValueEx;
}

 	/*************************************************************************/
	/*  ���� :������Դ���������� ID��										*/		
	/*	����ֵ:	��															 */
	/*************************************************************************/

 void SetExplorerPID(DWORD pid)
{
	ExplorerPID = pid;
}
