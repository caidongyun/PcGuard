// KillerDlg.cpp : implementation file
/////////////////////////////////////////////////////////////////////////
// ������ɱ�Ի��� ���������ǣ�ѡ��ɨ��Ŀ�ꡪ����ʼɨ��Ӳ�̡�������������ʶ
// ��������ļ��������Ҳ������̡��������������̡����г������ļ������û�ѡ��
// �Ƿ�ɾ�������ļ��������á�����������
/////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VirusKiller.h"
#include "KillerDlg.h"
#include <tlhelp32.h>
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
/////////////////////////////////////////////////////////////////////////
//    PathFindExtension();������ʹ��
//    ���ã�ȡ���ļ��ĺ�׺��
//   ע�⣺ͷ�ļ� shlwapi.h   �����⣺shlwapi.lib  
/////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// �ٽ��� ȫ�ֱ���
	CRITICAL_SECTION g_FileCs; //�ļ�ɨ�����
	CRITICAL_SECTION g_VirusCs;// �����ļ�����

/////////////////////////////////////////////////////////////////////////////
// CKillerDlg dialog


CKillerDlg::CKillerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKillerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CKillerDlg)
		// NOTE: the ClassWizard will add member initialization here
	m_bStartScan=false;
	m_bStop=false;
	//}}AFX_DATA_INIT
	m_hMainrds=INVALID_HANDLE_VALUE;
	for(int i=0;i<MAX_THREAD_NUM;i++)//��ʼ���߳�������
		m_hThread[i]= INVALID_HANDLE_VALUE;
}
// ��ʼ��ΪNULL
CKillerDlg * CKillerDlg::m_pCKillerDlg=NULL;
//����
CKillerDlg::~CKillerDlg()
{
	for(int i=0;i<MAX_THREAD_NUM;i++)//�رվ��
	{
		if(m_hThread[i] !=INVALID_HANDLE_VALUE)
			CloseHandle(m_hThread[i]);
	}
}

void CKillerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKillerDlg)
	DDX_Control(pDX, IDC_LIST1, m_listCtrl);
	DDX_Text(pDX, IDC_EDIT_PATH, m_strPathName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CKillerDlg, CDialog)
	//{{AFX_MSG_MAP(CKillerDlg)
	ON_BN_CLICKED(IDC_BUTTON_SELECT, OnBtnBrowse)
	ON_BN_CLICKED(IDC_BUTTON_START, OnButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnButtonStop)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, OnRclickList)
	ON_COMMAND(ID_MENUE_OPEN, OnMenueOpen)
	ON_COMMAND(ID_MENUE_DEL, OnMenueDel)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnButtonClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKillerDlg message handlers

void CKillerDlg::OnBtnBrowse() 
{
	TCHAR bufName[MAX_PATH]; //������ַ����ַ���,�ṹ��ʾ��  
	TCHAR bufPath[MAX_PATH]; //������ַ����ַ���,·��

	BROWSEINFO broInfo; // �������Ŀ¼�ṹ
	broInfo.hwndOwner = m_hWnd; // �����ڵľ��
	broInfo.pidlRoot = NULL; // ָ����Ŀ¼
	broInfo.pszDisplayName = bufName; 
	broInfo.lpszTitle = _T("ѡ��Ŀ¼"); // λ�ڶԻ��򶥶˵�һ������ 
	broInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT; // ��־��������λ��Ч
	broInfo.lpfn = NULL; // �ص�����   
	broInfo.lParam = NULL; // �����ص������Ĳ�����һ��32λֵ  

	//��ȡĿ¼·��
	LPCITEMIDLIST pidl = SHBrowseForFolder(&broInfo); 
	if( pidl == NULL )
		return;

	//�������ַ�����������ʾ
	SHGetPathFromIDList( pidl, bufPath );	
	m_strPathName = bufPath;	
	GetDlgItem(IDC_EDIT_SCAN)->SetWindowText("��ѡ��ļ��Ŀ¼Ϊ��" + m_strPathName);
	UpdateData(FALSE);
}

void CKillerDlg::OnButtonStart() 
{
	m_nTotalFileNum =0;  // ���ļ���
	m_VirusNum=0;    // ���ֵĲ�����
	m_KillVirusNum=0; // ���������
	m_errNum=0;   // ���ִ�����	
	m_FileList.clear();// ���Ҫɨ����ļ��б�
	m_VirusList.clear();// �����ļ��б�
	m_listCtrl.DeleteAllItems();// ����б�
	GetDlgItem(IDC_STATIC_TOTAL)->SetWindowText("���ļ�����0");
	GetDlgItem(IDC_STATIC_VIR_NUM)->SetWindowText("��������0");
	GetDlgItem(IDC_STATIC_KILLED_NUM)->SetWindowText("�������0");  
	GetDlgItem(IDC_STATIC_ERR)->SetWindowText("��������0");
	UpdateData(TRUE);
	GetDlgItem(IDC_BUTTON_START )->EnableWindow(FALSE);               
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE);
	m_bStop=false;
	if(!m_bStartScan)
	{
		//׼����ʼɨ��:  ɨ��·��  
		OnStartKillVirus(m_strPathName);
	}
}

//����һ�����̣߳�����ɨ��һָ��Ŀ¼
void CKillerDlg::OnStartKillVirus(CString strDir)
{
	    m_bStartScan = true;
		DWORD ThreadID =0;
		 m_hMainrds=::CreateThread(NULL,0,MainThreadProc,LPVOID(this),0,(LPDWORD)&ThreadID);
		::CloseHandle(m_hMainrds);
}  
//////////////////////////////////////////////////////////////
//�������߳̿�ʼ���в���
DWORD WINAPI  MainThreadProc(LPVOID lpParam)
{	
	CKillerDlg * pDlg=(CKillerDlg *)lpParam;
	CString str;
	//��ɨ��Ŀ¼���д���(Ϊ��ʱָȫ��Ŀ¼)  ע��˴�Ҫ�Ѵ��̷����ұߵ�\ɾ��/*��Ȼ �ͱ�� c:\\*/
	pDlg->ScanFile((LPSTR)(LPCTSTR)pDlg->m_strPathName.Left(pDlg->m_strPathName.GetLength()-1),"*.*");
	str.Format("%d",pDlg->m_FileList.size());
	pDlg->GetDlgItem(IDC_STATIC_TOTAL)->SetWindowText("���ļ���:"+str);
	DWORD ThreadID;
	//����ָ������ȶ��߳�
	for(int i = 0; i <MAX_THREAD_NUM; i++)
	{
		pDlg->m_hThread[i] = CreateThread(NULL,0,VirusCompareProc,lpParam,0, &ThreadID);
		
	}
	if(true==pDlg->m_bStop)
	{
		return 0; // ���ֹͣ�����򷵻�
	}
	// �ȴ��Ա��߳̽���
	::WaitForMultipleObjects(MAX_THREAD_NUM,pDlg->m_hThread, TRUE, INFINITE);
	for(i=0; i< MAX_THREAD_NUM; i++)
		::CloseHandle(pDlg->m_hThread[i]);
	pDlg->ScanDisk();
	int j=0;
	CString strTemp;
	//������������
	for (FileNameList::iterator it = pDlg->m_VirusList.begin(); it != pDlg->m_VirusList.end(); ++it)
	{
		strTemp.Format("%d",++j);
		pDlg->m_listCtrl.InsertItem(0, strTemp);
		pDlg->m_listCtrl.SetItemText(0,1 ,(CString)(*it) );
	}
	pDlg->m_bStartScan=false;
	pDlg->GetDlgItem(IDC_STATIC_VIR_NUM)->SetWindowText("�����ļ���:"+strTemp);
    pDlg->GetDlgItem(IDC_EDIT_SCAN)->SetWindowText("\t�ڴ����޶�..\n �������ļ��ɹ������");
	pDlg->GetDlgItem(IDC_EDIT_SAFE)->SetWindowText("\t\t��ɱ���....\n�����ļ�");
	pDlg->GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(false);
	pDlg->GetDlgItem(IDC_BUTTON_START )->EnableWindow(TRUE);               
	return 1;
}
int CKillerDlg::ScanFile(char *FileDir,char *FileType)
{   // ɨ���ļ�
    CString MyExt;
    char buffer[2*MAX_PATH]={'\0'};
    sprintf(buffer,"%s\\%s",FileDir,FileType);
	
    HANDLE hFind;
    WIN32_FIND_DATA dat;
    hFind = ::FindFirstFile(buffer,&dat);
    if( hFind == INVALID_HANDLE_VALUE )
    {
        GetDlgItem(IDC_EDIT_SCAN)->SetWindowText(" ɨ������г���");
        return -1;
    }
    else
    {
        char NewBuf[2*MAX_PATH];
        do
        {
			
			if(dat.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)// �ж��Ƿ�Ϊ�ļ���
			{
				if(dat.cFileName[0] == '.')
					continue;
				else
				{
					sprintf(NewBuf,"%s\\%s",FileDir,dat.cFileName);
					// �ݹ�
					ScanFile(NewBuf,"*.*");
				}
			}
			else// ���ļ�
			{
				sprintf(buffer,"%s\\%s",FileDir,dat.cFileName);
				MyExt = PathFindExtension(dat.cFileName);
				if( MyExt == ".exe" || MyExt == ".com" || MyExt == ".pif" || MyExt == ".scr" || MyExt == ".html" ||	MyExt 
					== ".asp" || MyExt == ".php" || MyExt == ".jsp" || MyExt==".dll" || MyExt==".txt" )
				{
					 m_FileList.push_back((CString)buffer);
					 GetDlgItem(IDC_EDIT_SCAN)->SetWindowText(((CString)buffer)+"����ɨ�����");
				}
			}
		}while(::FindNextFile(hFind,&dat));
		CString str;
		str.Format("%d",m_FileList.size());
		GetDlgItem(IDC_STATIC_TOTAL)->SetWindowText("���ļ���:"+str);
	}
	return 1;
}

//������ȶԹ����߳�(�����Է��� USE_MAXTRD_NUM���߳�)
DWORD WINAPI VirusCompareProc(LPVOID lpParam)
{
	CKillerDlg * pDlg=(CKillerDlg *)lpParam;
	CString curFilePath;
    while(!(pDlg->m_bStop))
	{
		if(pDlg->m_FileList.empty())
		{
			pDlg->m_bStop=true;
			break;
		}
		//�����ٽ���
		::EnterCriticalSection(&g_FileCs);
		curFilePath=pDlg->m_FileList.front();// ��ͷ������
		pDlg->GetDlgItem(IDC_EDIT_SCAN)->SetWindowText((CString)("����ɨ��"+curFilePath));
		pDlg->m_FileList.pop_front();
				//�뿪�ٽ��������ȶ��̴߳Ӷ�����ȡ���ļ���(�Ӷ���ͷȡ��)
		::LeaveCriticalSection(&g_FileCs);
		if(curFilePath.IsEmpty())
		{
			continue;
		}
		else
		{
			pDlg->IsVirus(curFilePath);
			curFilePath="";
		}
    }

	return 1;
}
bool CKillerDlg::InitVirusData()
{
	LoadUserSet();
	// �������ʼ��ʧ��
	if(m_VirusLibLIst.empty())
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool CKillerDlg::IsVirus(CString strFilePath)
{
	CString MyExt = PathFindExtension(strFilePath);
	// �򿪼����ļ�
	BOOL bValid = FALSE;
	DWORD dwRead;
	if(MyExt==".exe" || MyExt==".dll") // �� PE �ļ� ���м��
	{
		HANDLE hFile = ::CreateFile(strFilePath, GENERIC_READ, 
			FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
		{
			GetDlgItem(IDC_EDIT_SAFE)->SetWindowText( _T("��Ч�ļ�!"));
			return false;
		}
		// ����PE�ļ��е�DOSͷ
		IMAGE_DOS_HEADER dosHeader;	
		// ��ȡDOSͷ
		::ReadFile(hFile, &dosHeader, sizeof(dosHeader), &dwRead, NULL);
		if(dosHeader.e_magic == IMAGE_DOS_SIGNATURE) // �ǲ�����Ч��DOSͷ��
		{
			char Buffer[VIRUSDATA_LEN+1];
			// �ж��Ƿ񱻸�Ⱦ����Ϊ��һ����������λ��(0x7f)��Ӧ�ò��䣬Ϊ0x00
			::SetFilePointer(hFile,0x7f, NULL,FILE_BEGIN);
			::ReadFile(hFile,Buffer,sizeof(Buffer),&dwRead,NULL);
			if(Buffer[0]!=0x00)
			{
				GetDlgItem(IDC_EDIT_SAFE)->SetWindowText(strFilePath+"���ļ��ѱ���Ⱦ...");
			}
			else
			{
				//������������
				for (VirusList::iterator it = m_VirusLibLIst.begin(); it != m_VirusLibLIst.end(); ++it)
				{
					VIRUS_DATA da=(*it);
					::SetFilePointer(hFile,da.offset, NULL,FILE_BEGIN);
					::ReadFile(hFile,Buffer,da.dataLen,&dwRead,NULL);
					// ���ļ���·���еõ� �ļ���
					CString FileName=strFilePath.Right(strFilePath.GetLength()-strFilePath.ReverseFind('\\')-1);
					// ��������ƥ�� ���� ����������ͬʱ ��Ϊ�ǿ����ļ�
					if( !strcmp(Buffer,(char *)da.VirusData ) || FileName==(CString)(*it).VirusName )
					{
						// ��������ļ��б�
						::EnterCriticalSection(&g_VirusCs);
						m_VirusList.push_back(strFilePath);
						GetDlgItem(IDC_EDIT_SAFE)->SetWindowText("\t\t\t���ļ��ѹ�����в����ʼ�ڴ�ɨ��...");
						::LeaveCriticalSection(&g_VirusCs);
						ScanProcess(strFilePath);// ɨ�����
					}
					else
					{
						GetDlgItem(IDC_EDIT_SAFE)->SetWindowText("\t\t\t���ļ���ȫ��");
					}
					
				}//for
				
				
			}//else
			
		}//if dos
		::CloseHandle(hFile);
	}// if
	else if(MyExt==".txt")
	{
		CString   strSum=_T("");
		CString   strLine;
		CStdioFile   fpTxtFile; 
		
		//ֻ����ʽ���ļ� 
		if(   !fpTxtFile.Open( strFilePath, CFile::modeRead)) 
		{ 
			return   FALSE; 
		} 
		//ѭ����ȡ�� 
		while(fpTxtFile.ReadString(strLine)) 
		{ 
			strSum+=strLine;
		} 
		fpTxtFile.Close();
		//������������������
		for (VirusList::iterator it = m_VirusLibLIst.begin(); it != m_VirusLibLIst.end(); ++it)
		{
			VIRUS_DATA da=(*it);
			// ��������ƥ�� ��Ϊ�ǿ����ļ�
			if(-1!=strSum.Find(CString(da.VirusData)))
			{
				// ��������ļ��б�
				::EnterCriticalSection(&g_VirusCs);
				m_VirusList.push_back(strFilePath);
				GetDlgItem(IDC_EDIT_SAFE)->SetWindowText("\t\t\t���ļ��ѹ�����в��");
				::LeaveCriticalSection(&g_VirusCs);
			}
			else
			{
				GetDlgItem(IDC_EDIT_SAFE)->SetWindowText("\t\t\t���ļ���ȫ��");
			}
		}
	}

	return true;
}

void CKillerDlg::SaveUserSet()
{
	char appPath[256];
	GetCurrentDirectory(256,appPath);	//ȡ��Ӧ�ó���ǰ·��
	CString  filePath;					//����Ini�ļ���
	filePath.Format("%s",appPath);
	filePath +="\\";
	filePath += "virus.ini";	
	
	if(m_VirusLibLIst.empty())// �������б�Ϊ��ʱ
		return;

	//ɾ��ԭ�����ļ�
	DeleteFile(filePath);
	int		nIndex = 0;
	
	//������������
	for (VirusList::iterator it = m_VirusLibLIst.begin(); it != m_VirusLibLIst.end(); ++it)
	{	
		VIRUS_DATA da=(*it);
		CString strSection("section");							
		
		//section
		CString strIndex;
		strIndex.Format("%d",nIndex);
		strSection += strIndex;
		// �����볤��
		CString strdataLen;								
		strdataLen.Format("%ld", (long)da.dataLen);
		// ����������
		CString strOffset;
		strOffset.Format("%d",da.offset);
		CString strdata;
		strdata=(char *)(LPCTSTR)da.VirusData;
			//д��������48�ֽ�
		WritePrivateProfileString(strSection, _T("VirusName"), da.VirusName, filePath);	
			//�����볤��
		WritePrivateProfileString(strSection, _T("DataLen"), strdataLen, filePath);
		//�ļ�����
		WritePrivateProfileString(strSection, _T("ExtName"), da.ExtName,filePath);	
		// ƫ�Ƶ�ַ
		WritePrivateProfileString(strSection, _T("OffSet"),strOffset ,filePath);
		//����������
		WritePrivateProfileString(strSection, _T("VirusData"),strdata,filePath);	
		++nIndex;
	}

	//����������д��ini�ļ�
	CString strCount;
	strCount.Format("%d", nIndex);
	::WritePrivateProfileString("VirusCount", "Count", strCount, filePath);	
	
	
	m_VirusLibLIst.clear();//ɾ���������нڵ�
}

void CKillerDlg::LoadUserSet()
{
	#define  TEMP_BUF_SIZE 125//����������
	char appPath[256];
	GetCurrentDirectory(256,appPath);	//ȡ��Ӧ�ó���ǰ·��
	CString  filePath;					//����Ini�ļ���
	filePath.Format("%s",appPath);
	filePath += "\\";
	filePath += "virus.ini";
	
	//��ini�ļ��л�ò�����������
 	int count =::GetPrivateProfileInt("VirusCount", "Count", 0, filePath);
	if(count<=0)
		return;
	int nIndex = 0;
	while (TRUE)
	{
		CString strSection("section");
		CString strIndex;
		strIndex.Format("%d",nIndex);
		strSection += strIndex;
		
		//��������
		CString strSectionKey = "VirusName";					
		CString strValue = _T("");
		char cBuf[TEMP_BUF_SIZE];
		memset(cBuf, 0, TEMP_BUF_SIZE);
		VIRUS_DATA *pVirusInfo = new VIRUS_DATA;//�½�VIRUS_DATA����
		memset(pVirusInfo,0,sizeof(VIRUS_DATA));
		if(GetPrivateProfileString (strSection, strSectionKey, NULL, cBuf, TEMP_BUF_SIZE, filePath))
		{
			strcpy(pVirusInfo->VirusName,cBuf);
		}
		//  �����볤��
		strSectionKey = "DataLen";
		memset(cBuf, 0, TEMP_BUF_SIZE);
		if(GetPrivateProfileString (strSection, strSectionKey, NULL, cBuf, TEMP_BUF_SIZE, filePath))
		{
			pVirusInfo->dataLen = (LONG)atoi(cBuf);
		}
		
		//�ļ���չ��
		strSectionKey = "ExtName";
		memset(cBuf, 0, TEMP_BUF_SIZE);
		if(GetPrivateProfileString (strSection, strSectionKey, NULL, cBuf, TEMP_BUF_SIZE, filePath))
		{
			strcpy(pVirusInfo->ExtName,cBuf);
		}
		
		//ƫ����
		strSectionKey = "OffSet";
		memset(cBuf, 0, TEMP_BUF_SIZE);
		if(GetPrivateProfileString (strSection, strSectionKey, NULL, cBuf, TEMP_BUF_SIZE, filePath))
		{
			pVirusInfo->offset = (LONG)atoi(cBuf);
		}

		//��ȡ������
		strSectionKey = "VirusData";
		memset(cBuf, 0, TEMP_BUF_SIZE);
		if(GetPrivateProfileString (strSection, strSectionKey, NULL,  cBuf,TEMP_BUF_SIZE, filePath))
		{
			strcpy(pVirusInfo->VirusData,cBuf);
		}
		m_VirusLibLIst.push_back(*pVirusInfo);		//��������
		delete pVirusInfo;
		if(++nIndex>=count) // �жϲ�����������
			break;
	}	
}

CKillerDlg * CKillerDlg::GetCKillerDlg()
{
	if(NULL==m_pCKillerDlg)
	{
		return NULL;
	}
	else
	{
		return m_pCKillerDlg;
	}
}


CKillerDlg * GetCKillerDlg()
{
	return CKillerDlg::GetCKillerDlg();
}


BOOL CKillerDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_pCKillerDlg=this;
	    		//��ʼ���ٽ���
	::InitializeCriticalSection(&g_FileCs);
	::InitializeCriticalSection(&g_VirusCs);
	//�б��ʼ���б�
	m_listCtrl.ModifyStyle(LVS_TYPEMASK,LVS_REPORT);
	m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_FLATSB|LVS_EX_TWOCLICKACTIVATE |LVS_EX_GRIDLINES);
	m_listCtrl.SetBkColor(RGB(247,247,255));
	m_listCtrl.SetTextColor(RGB(0,100,155));
	m_listCtrl.SetTextBkColor(RGB(247,247,255));

	m_listCtrl.InsertColumn(0,_T("���"),LVCFMT_LEFT,60);
	m_listCtrl.InsertColumn(1,_T("�ļ�·��"),LVCFMT_LEFT,420);
	if(!InitVirusData())
	{
		AfxMessageBox(_T("�������ļ�����ʧ�ܣ��޷����в�����ɱ����!"));
		return 0;
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CKillerDlg::ScanProcess(CString ModuleName)
{
	// ɨ����̼�ģ����Ϣ
    //StaBar->SimpleText = " ����ɨ�����...";

	PROCESSENTRY32 ProcessEntry = { 0 };
    MODULEENTRY32  ModuleEntry  = { 0 };
	HANDLE hProcessSnap;
    HANDLE hModuleSnap;
    HANDLE hVirus;
	ProcessEntry.dwSize = sizeof(PROCESSENTRY32);
    ModuleEntry.dwSize  = sizeof(MODULEENTRY32);
	
    // �������� ��������
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	BOOL bRet = Process32First(hProcessSnap,&ProcessEntry);
	while(bRet)
	{
        // ��������ģ��
		CString st=_T(" ����ɨ�����");
       GetDlgItem(IDC_EDIT_SAFE)->SetWindowText(st + CString(ProcessEntry.szExeFile));
        hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,ProcessEntry.th32ProcessID);
        bRet = Module32First(hModuleSnap,&ModuleEntry);
        while(bRet)
        {
            if(strstr(ModuleEntry.szExePath,ModuleName.Right(ModuleName.GetLength()-ModuleName.ReverseFind('\\')-1) ) )
            {
				ModuleName.ReleaseBuffer();
                hVirus = OpenProcess(PROCESS_ALL_ACCESS,FALSE,ProcessEntry.th32ProcessID);
                TerminateProcess(hVirus,0);
                CloseHandle(hVirus);
				st=_T(" �������ɽ���");
                GetDlgItem(IDC_EDIT_SAFE)->SetWindowText(st+ModuleEntry.szExePath );
            }
            bRet = Module32Next(hModuleSnap,&ModuleEntry);
        }
		bRet = Process32Next(hProcessSnap,&ProcessEntry);
	}
    return;
}

// ɨ�����д��� Ŀ¼
void CKillerDlg::ScanDisk()
{
	char Disk = NULL;
    UINT Type = 0;
    char strFileName[MAX_PATH] = {0};
    char szDriveName[4] = {0};
	wsprintf(szDriveName,"C:\0");

	// �������п��ܴ��ڵķ���
	for(szDriveName[0] = 'C';szDriveName[0] < 'Z';szDriveName[0]++)
	{	// �õ��÷�������
        Type = GetDriveType(szDriveName);
        // ����Ϊ1�����÷�����Ŀ¼������
	    if(Type != 1)
		{
            Disk = szDriveName[0];

            sprintf(strFileName,"%c:\\autorun.inf",Disk);
            SetFileAttributes(strFileName,~FILE_ATTRIBUTE_HIDDEN);
            if(DeleteFile(strFileName))
			{
				GetDlgItem(IDC_EDIT_SAFE)->SetWindowText((CString)strFileName+"�ɹ�ɾ��..");
			}
            sprintf(strFileName,"%c:\\setup.exe",Disk);
            SetFileAttributes(strFileName,~FILE_ATTRIBUTE_HIDDEN);
            if(DeleteFile(strFileName))
			{
				GetDlgItem(IDC_EDIT_SAFE)->SetWindowText((CString)strFileName+"�ɹ�ɾ��..");
			}
        }
    }
    return;
}
// ����ֹͣ��ť��Ϣ

void CKillerDlg::OnButtonStop() //Terminate
{
	m_bStop=true; //����ֹͣΪ��
	m_bStartScan=false;// ��ʼɨ��Ϊ��
	for(int i=0;i<MAX_THREAD_NUM;i++)
	{
		if(INVALID_HANDLE_VALUE!=m_hThread[i])// �ж��Ƿ�Ϊ��Ч���
		{
			TerminateThread(m_hThread[i],0);// ����������Ա��߳�
			CloseHandle(m_hThread[i]);
			m_hThread[i]=INVALID_HANDLE_VALUE;
		}
	}
	if(INVALID_HANDLE_VALUE!=m_hMainrds)
	{
		TerminateThread(m_hMainrds,0);// �������߳�
		CloseHandle(m_hMainrds);
		m_hMainrds=INVALID_HANDLE_VALUE;
	}
	m_FileList.clear();// ���Ҫɨ����ļ��б�
	m_VirusList.clear();// �����ļ��б�
	GetDlgItem(IDC_STATIC_TOTAL)->SetWindowText("���ļ�����0");
	GetDlgItem(IDC_STATIC_VIR_NUM)->SetWindowText("�����ļ���:0");
    GetDlgItem(IDC_EDIT_SCAN)->SetWindowText("\tֹͣɨ��");
	GetDlgItem(IDC_EDIT_SAFE)->SetWindowText("\t\t.....");
	GetDlgItem(IDC_BUTTON_START )->EnableWindow(TRUE);               
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
	UpdateData(FALSE);
}

// �����б���Ҽ���Ϣ ���������˵�
void CKillerDlg::OnRclickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
    if(pNMListView->iItem != -1)
	{
	  DWORD dwPos = GetMessagePos();// �õ��������
      CPoint point( LOWORD(dwPos), HIWORD(dwPos) );
      CMenu menu;
      VERIFY( menu.LoadMenu(IDR_MENU1));
      CMenu* popup = menu.GetSubMenu(0);
      ASSERT( popup != NULL );
	  //ȷ������ʽ�˵���λ��
      popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	  //��Դ����
		HMENU hmenu=menu.Detach();
		menu.DestroyMenu();
   }
	*pResult = 0;
	*pResult = 0;
}

void CKillerDlg::OnMenueOpen() 
{
	// TODO: Add your command handler code here
	CString tem = _T("");
	CString str = _T("Explorer /select,");
	int nItem = m_listCtrl.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
	tem = m_listCtrl.GetItemText(nItem, 1);
	SetCurrentDirectory(tem);
	str +=tem;
	WinExec(str,SW_SHOWNORMAL);//��ʾ�ļ���λ��
}

void CKillerDlg::OnMenueDel() 
{
	// TODO: Add your command handler code here
	CString tem = _T("");
	int nItem = m_listCtrl.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
	tem = m_listCtrl.GetItemText(nItem, 1);
	DeleteFile(tem);
}

void CKillerDlg::OnButtonClear() 
{
	// TODO: Add your control notification handler code here
	m_listCtrl.DeleteAllItems(); // ��տ����ļ��б�
}
