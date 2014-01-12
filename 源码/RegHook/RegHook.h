/*************************************************************
*	RegHook API										
*   ע����ع���
/*************************************************************/
#ifdef __cplusplus //dll�����Զ�����
#define REG_HOOK_H extern "C" __declspec (dllexport)
#else
#define REG_HOOK_H  __declspec(dllimport)
#endif

#include <windows.h>
#include <Winternl.h>   

/**********************��������******************************/
#define		TYPE_RegCreateKeyExA	0 //�������Ӽ�(խ�ַ�A)
#define		TYPE_RegCreateKeyExW	1 //�������Ӽ�(UNICODE W)
#define		TYPE_RegSetValueExA		2 //���ü�ֵ(A)
#define		TYPE_RegSetValueExW		3
#define		TYPE_RegDeleteKeyA		4 //ɾ���Ӽ�
#define		TYPE_RegDeleteKeyW		5
#define		TYPE_RegDeleteValueA	6 //ɾ����ֵ
#define		TYPE_RegDeleteValueW	7
#define		TYPE_RegQueryValueExA	8 //��ѯ��ֵ
#define		TYPE_RegQueryValueExW	9
/************************************************************/
#define      MAX_BUF_LEN            1024 //��������󳤶�
// ��������ӽ���ͨ�� ���͵����ݽṹ
typedef struct 
{
	DWORD	type;//��ֵ��������
	DWORD	PID; //���� ID
	HKEY	hKey;//�Ӽ����
	char	buf1[MAX_BUF_LEN ];//������1  ���ȡ��ȡ��ֵ������ 
	int		cbbuf1;//���ݴ洢��1 ����
	char	buf2[MAX_BUF_LEN ];//װ��ָ��ֵ�Ļ�����
	int		cbbuf2;//���ݴ洢��2 ����
	long	result; //�������
}PARAMS;

//KEY_FULL_INFORMATION �ṹ ZwQueryKey ������Ҫ�˽ṹ
typedef enum _KEY_INFORMATION_CLASS {  
    	KeyBasicInformation,  
		KeyNodeInformation,
		KeyFullInformation,
		KeyNameInformation,  
		KeyCachedInformation,  
		KeyVirtualizationInformation
}
KEY_INFORMATION_CLASS;

//ö������ ��ѯ��Ϣ��һ���ؼ�, �� NTDLL ������ʱ��̬���� (�˺���ΪNT�ں˺���)
typedef LONG(WINAPI*ZwQueryKey)(IN HANDLE, IN KEY_INFORMATION_CLASS, OUT PVOID, IN ULONG, OUT PULONG); 

///////////////////////����Ҫ�����ĺ���///////////////////////////////////////////////////
	/************************************************************************/
	/*	����  :	��װ���Ӻ��������Զ��平�Ӱ�װ��ϵͳ						*/
	/*	����ֵ:	�ɹ�����true, ʧ�ܷ��� false								*/
	/************************************************************************/
REG_HOOK_H	bool InstallHook(
							 HWND hWnd //��װ���Ӻ����Ĵ��ھ��
							 );

	/************************************************************************/
	/*	����  :	ж�ع��Ӻ�������ϵͳ�еĹ���ж�س�ȥ						*/
	/*	����ֵ:	�ɹ�����true, ʧ�ܷ��� false								*/
	/************************************************************************/
REG_HOOK_H	bool UninstallHook();

	/************************************************************************/
	/*	����  :	���˺�������ָ���Ľ��̽��йҹ�����							*/
	/*	����ֵ:	��															*/
	/************************************************************************/
REG_HOOK_H	void SetFilter(
						   DWORD pid//����ID��
						   );

	/*************************************************************************/
	/*  ���� :���Ӽ��ӵ�����,��ָ������Ϊ���йҹ�����(��Ӧ�Ĳ���Ϊtrue��ʾ��)*/		
	/*	����ֵ:	��															 */
	/*************************************************************************/
REG_HOOK_H	void SetHookFunction(
								 BOOL RegCreateKeyEx, //�Ƿ���Ӵ����Ӽ�
								 BOOL RegSetValueEx, //�Ƿ���Ӽ�ֵ����
								 BOOL RegDeleteKey, //�Ƿ����ɾ���Ӽ�
								 BOOL RegDeleteValue, //�Ƿ����ɾ����ֵ
								 BOOL RegQueryValueEx //�Ƿ���Ӳ�ѯ��ֵ
			);

	/*************************************************************************/
	/*  ���� :������Դ���������� ID��										*/		
	/*	����ֵ:	��															 */
	/*************************************************************************/
REG_HOOK_H	void SetExplorerPID(
								DWORD pid //����ID ��
								);
