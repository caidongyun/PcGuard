#if !defined(AFX_KILLERDLG_H__2726E8E3_378F_4CA6_BAC8_89CCD811F933__INCLUDED_)
#define AFX_KILLERDLG_H__2726E8E3_378F_4CA6_BAC8_89CCD811F933__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KillerDlg.h : header file
//
#include <list> //STLͷ�ļ�
using namespace std;

typedef list<CString>  FileNameList;// �ļ�����ṹ����

#define MAX_THREAD_NUM 2 // ����߳���
#define VIRUSDATA_LEN 124 // 
//�������ݽṹ
typedef struct VIRUS_DATA   
	{
		char VirusName[49];    // ��������48�ֽ�
       LONG dataLen;        // �����볤��
	   char ExtName[21];   // ָ���ڴ����͵��ļ���չ���в���(Ϊ��ʱ��ʾ�����ļ�)60�ֽ�
	   LONG offset; // ����������ƫ����
	   char  VirusData[VIRUSDATA_LEN + 1];   // ����������
	   // ���ظ�ֵ�����
	   const VIRUS_DATA & operator=( const VIRUS_DATA temp )
	   {
		   dataLen = temp.dataLen;
		   ZeroMemory(VirusName,49);
		   ZeroMemory(ExtName,21);
		   strcpy(VirusName,temp.VirusName);
		   VirusName[48] = '\0';
		   strcpy(ExtName,temp.ExtName); 
		   ExtName[20] = '\0';
		   offset=temp.offset;
		   ZeroMemory(VirusData,VIRUSDATA_LEN+1);
		   memcpy(VirusData,temp.VirusData,VIRUSDATA_LEN); 
		   return *this;
	   }
	}VIRUS_DATA ,*PVIRUS_DATA ;
typedef list<VIRUS_DATA>  VirusList;//�ļ�����ṹ����
/////////////////////////////////////////////////////////////////////////////
// CKillerDlg dialog

class CKillerDlg : public CDialog
{
// Construction
public:
	CString m_strPathName;
	CKillerDlg(CWnd* pParent = NULL);   // standard constructor
	~CKillerDlg();
// Dialog Data
	//{{AFX_DATA(CKillerDlg)
	enum { IDD = IDD_DIALOG1 };
	CListCtrl	m_listCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKillerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CKillerDlg)
	afx_msg void OnBtnBrowse();
	afx_msg void OnButtonStart();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonStop();
	afx_msg void OnRclickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMenueOpen();
	afx_msg void OnMenueDel();
	afx_msg void OnButtonClear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	bool m_bStartScan;
	int	m_nTotalFileNum;  // ���ļ���
	int m_VirusNum;    // ���ֵĲ�����
	int m_KillVirusNum; // ���������
	int m_errNum;   // ���ִ�����
	FileNameList m_FileList;// Ҫɨ����ļ��б�
	FileNameList m_VirusList;// �����ļ��б�
	VirusList   m_VirusLibLIst;// �������б�
public:
	void ScanDisk();
	void ScanProcess(CString ModuleName);
	// ��ñ�����ָ��
	static CKillerDlg * m_pCKillerDlg;
	static CKillerDlg * GetCKillerDlg();
	void LoadUserSet();
	void SaveUserSet();
	bool IsVirus(CString strFilePath);
	HANDLE m_hMainrds;
	HANDLE m_hThread[MAX_THREAD_NUM];
	bool InitVirusData();
	bool m_bStop; // �Ƿ�ֹͣ
	void OnStartKillVirus(CString strDir=_T(""));
	int ScanFile(char *FileDir,char *FileType);
};

// ����߳�Ҫִ�е�����ĳ�Ա����,�������static����Ϊ��̬����
// һ�㶼��������ȫ�ֺ���  
DWORD WINAPI MainThreadProc(LPVOID lpParam);
DWORD WINAPI VirusCompareProc(LPVOID lpParam);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KILLERDLG_H__2726E8E3_378F_4CA6_BAC8_89CCD811F933__INCLUDED_)
