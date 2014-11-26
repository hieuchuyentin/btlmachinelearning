// BTL_HMDialog.cpp : implementation file
//

#include "stdafx.h"
#include "BTL_HM.h"
#include "BTL_HMDialog.h"


// CBTL_HMDialog dialog

IMPLEMENT_DYNAMIC(CBTL_HMDialog, CDialog)

CBTL_HMDialog::CBTL_HMDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CBTL_HMDialog::IDD, pParent)
{
}

CBTL_HMDialog::~CBTL_HMDialog()
{
}

void CBTL_HMDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, textFolderTrain);
	DDX_Control(pDX, IDC_EDIT2, textFolderTest);
	DDX_Control(pDX, IDC_BUTTON1, btTrain);
	DDX_Control(pDX, IDC_BUTTON2, btTest);
	DDX_Control(pDX, IDC_BUTTON3, btEvaluate);
}


BEGIN_MESSAGE_MAP(CBTL_HMDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CBTL_HMDialog::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CBTL_HMDialog::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CBTL_HMDialog::OnBnClickedButton3)
END_MESSAGE_MAP()


// CBTL_HMDialog message handlers
void CBTL_HMDialog::OnBnClickedButton1()//Bắt đầu huấn luyện
{
	// TODO: Add your control notification handler code here

// 	TOKEN_INFO tokenInfo;
// 	tokenInfo = m_ListTokenNormal.GetAt(m_ListTokenNormal.FindIndex(1));
// 	tokenInfo.dPercent = 6;
//	Đoạn code test việc thay đổi phần tử của list:
//	Sau khi getAt, đối tượng trả về là hoàn toàn mới, không nằm trong list, phải dùng hàm setAt thì mới thay đổi
//	được phần tử của list.

// 	CString s = _T("aaaaaa aaaaaa bcbc");
// 	int p = 0;
// 	CString resToken = s.Tokenize(_T(" "), p);
// 	s.Replace(resToken, _T("e"));
// 	resToken = s.Tokenize(_T(" "), p);
//	Đoạn code thử tokenize: Nếu thay đổi xâu trong khi tokenize thì kết quả sẽ sai. Cần điều chỉnh p để có kết quả đúng.

//	Đọc danh sách thư từ tệp
	textFolderTrain.GetWindowText(m_sFolderTrain);

	GetMail(m_sFolderTrain + _T("\\") + SZ_NAME_FILE_NORMAL_TRAIN, 
		m_listMailTrainNormal);
	GetMail(m_sFolderTrain + _T("\\") + SZ_NAME_FILE_SPAM_TRAIN, 
		m_listMailTrainSpam);
// 
// 	//Tiền xử lý bỏ dẫu, chữ hoa
	stopword.PreProcessListMail(m_listMailTrainNormal);
	stopword.PreProcessListMail(m_listMailTrainSpam);

	//Đếm số lần xuất hiện của các từ
	CountWord(m_listMailTrainNormal, m_ListTokenNormal);
	CountWord(m_listMailTrainSpam, m_ListTokenSpam);

	//Stem các từ tìm được
	StemWord(m_ListTokenNormal);
	StemWord(m_ListTokenSpam);

	//Tìm từ khóa quan trọng
  	GetKey(m_ListTokenNormal, m_ListTokenSpam, m_listKey);
	m_nKeyNumber = m_listKey.GetCount();

// 
// 	Tính xác suất của mỗi từ khóa với mỗi nhãn lớp spam/normal
	CalculateProbability(m_listMailTrainNormal, m_listKey, m_listProbabilityKeyOnNormal);
 	CalculateProbability(m_listMailTrainSpam, m_listKey, m_listProbabilityKeyOnSpam);

	//Đã xong giai đoạn huấn luyện
}

void CBTL_HMDialog::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	textFolderTest.GetWindowText(m_sFolderTest);

	GetMail(m_sFolderTest + _T("\\") + SZ_NAME_FILE_NORMAL_TEST, 
		m_listMailTestNormal);
	GetMail(m_sFolderTest + _T("\\") + SZ_NAME_FILE_SPAM_TEST, 
		m_listMailTestSpam);

}

void CBTL_HMDialog::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here
}

//******************************************************************************************
//Phân tích một file *.ems đầu vào thành các email riên biệt với kiểu CString
//Input:
// - CString  cFilePath: đường dẫn đến file email muốn phân tích
//Output:
// - CstringList &csContent: list các CString chứa nội dung email, mỗi phần tử là một email
//Example:
// - AnalysisInput(_T("C:\\train_SPAM.ems"), g_clTrainSpam);
//******************************************************************************************

void CBTL_HMDialog::GetMail(__in CString sPath, __out CStringList& sListMail)
{
	FILE* file;
	CString cContent;
	CString cTemp;
	TCHAR szBuffer[MY_MAX_PATH];

	sListMail.RemoveAll();

	if (_wfopen_s(&file, sPath, _T("r")) != 0)
	{
		MessageBox(_T("Open file error"), _T("Error"), MB_OK + MB_ICONERROR);
		return;
	}

	while (true)
	{
		if (fgetws(szBuffer, MY_MAX_PATH, file) == NULL) break;
		if (_tcsncmp(szBuffer, TEXT_BEGIN_TAG, LEN_BEGIN_TAG) != 0) continue;

		fgetws(szBuffer, MY_MAX_PATH, file);										// escape blank line

		cContent = _T("");
		while(true)
		{
			fgetws(szBuffer, MY_MAX_PATH, file);
			if (_tcsncmp(szBuffer, TEXT_END_TAG, LEN_END_TAG) == 0) break;

			cTemp = (CString)(szBuffer + 2);
			cContent += cTemp;
		}

		sListMail.AddTail(cContent);
	}

	fclose(file);
}
// void CBTL_HMDialog::GetFrequency(CStringList& listMail, __out TOKEN_LIST& TOKEN_INFO)
// {
// 	TOKEN_INFO.RemoveAll();
// 	return;
// }

void CBTL_HMDialog::GetKey(TOKEN_LIST listNormal[], TOKEN_LIST listSpam[], __out CStringList& listKey)
{
// 	int nIndexSpam, nIndexNormal;
// 	POSITION posAppear;
// 	TOKEN_INFO infoTokenNormal, infoTokenSpam;
// 	
// 	listKey.RemoveAll();
// 
// 	//Tìm các token chỉ xuất hiện trong tập thường
// 	for (nIndexNormal = 0; nIndexNormal < listNormal.GetCount(); nIndexNormal++)
// 	{
// 		infoTokenNormal = listNormal.GetAt(listNormal.FindIndex(nIndexNormal));
// 		posAppear = listSpam.Find(infoTokenNormal);
// 		if (posAppear == NULL)
// 			listKey.AddTail(infoTokenNormal.sTokenName);
// 	}
// 
// 	//Tìm các token chỉ xuất hiện trong tập rác
// 	for (nIndexSpam = 0; nIndexSpam < listSpam.GetCount(); nIndexSpam++)
// 	{
// 		infoTokenSpam = listSpam.GetAt(listSpam.FindIndex(nIndexSpam));
// 		posAppear = listNormal.Find(infoTokenSpam);
// 		if (posAppear == NULL)
// 			listKey.AddTail(infoTokenSpam.sTokenName);
// 	}
// 
// 	//Tìm các token xuất hiện trong cả 2 tập, so sánh
// 	for (nIndexNormal = 0; nIndexNormal < listNormal.GetCount(); nIndexNormal++)
// 	{
// 		infoTokenNormal = listNormal.GetAt(listNormal.FindIndex(nIndexNormal));
// 		posAppear = listSpam.Find(infoTokenNormal);
// 		if (posAppear != NULL)
// 		{
// 			infoTokenSpam = listSpam.GetAt(posAppear);
// 			double dPorportion;
// 			dPorportion = ((double)infoTokenNormal.dPercent) / (infoTokenNormal.dPercent + infoTokenSpam.dPercent);
// 			if ((dPorportion < 0.2) || (dPorportion > 0.8))
// 				listKey.AddTail(infoTokenNormal.sTokenName);
// 		}
// 	}

	return;
}
void CBTL_HMDialog::GetAppearOneMail(CStringList& listKey, CString sMail, __out NUMBER_LIST& listFrequency)
{
	listFrequency.RemoveAll();
	return;
}
void CBTL_HMDialog::CalculateProbability(CStringList& listMail, CStringList& listKey, __out TOKEN_LIST listProbability[])
{
	//listProbability.RemoveAll();
	return;
}

BOOL operator == (TOKEN_INFO i1, TOKEN_INFO i2)
{
	return (i1.sTokenName.Compare(i2.sTokenName) == 0);
}

void CBTL_HMDialog::CountWord(CStringList& listMail, TOKEN_LIST listToken[])
{
	int nMailNumber;
	nMailNumber = listMail.GetCount();
	CString mail;
	CString sToken;
	TOKEN_INFO infoToken;
	POSITION posToken;
	int curPos;
	int nHashValue;

	for (int i = 0; i < HASH_COUNT; i++)
		listToken[i].RemoveAll();

	for (int i = 0; i < nMailNumber; i++)
	{
		mail = listMail.GetAt((listMail.FindIndex(i)));
		curPos = 0;
		sToken = mail.Tokenize(_T(" "), curPos);
		while (sToken != _T(""))
		{
			infoToken.sTokenName = sToken;
			infoToken.dPercent = 1;
			
			nHashValue = HashString(sToken);

			posToken = listToken[nHashValue].Find(infoToken);

			if (posToken == NULL)
			{
				listToken[nHashValue].AddTail(infoToken);
			}
			else
			{
				infoToken.dPercent = listToken[nHashValue].GetAt(posToken).dPercent + 1;
				listToken[nHashValue].SetAt(posToken, infoToken);
			}

			sToken = mail.Tokenize(_T(" "), curPos);
		}
		
	}
	
	double nTokenCount = 0;
	double nToken = 0;

	for (int i = 0; i < HASH_COUNT; i++)
	{
		nToken += listToken[i].GetCount();
		for (int j = 0; j < listToken[i].GetCount(); j++)
			nTokenCount += listToken[i].GetAt(listToken[i].FindIndex(j)).dPercent;
	}
}

int CBTL_HMDialog::HashString(CString sToken)
{
	int nLeng;
	int nSum;

	nSum = 0;
	nLeng = sToken.GetLength();
	for (int i = 0; i < nLeng; i++)
		nSum += sToken.GetAt(i);

	return nSum % HASH_COUNT;
}

void CBTL_HMDialog::StemWord(TOKEN_LIST listToken[])
{
	int nIndexHash, nIndexToken;
	TOKEN_INFO infoToken;
	CString sNewToken;
	Stemmer stem;
	POSITION posCurrent, posNew;
	int nHashValue;

	for (nIndexHash = 0; nIndexHash < HASH_COUNT; nIndexHash++)
	{
		nIndexToken = 0;
		while (nIndexToken < listToken[nIndexHash].GetCount())
		{
			posCurrent = listToken[nIndexHash].FindIndex(nIndexToken);
			infoToken = listToken[nIndexHash].GetAt(posCurrent);
			stem.resetIndex();
			stem.stemmer(infoToken.sTokenName);
			sNewToken = stem.toString();
			if (sNewToken.Compare(infoToken.sTokenName) != 0)//có sự thay đổi, cần loại bỏ token cũ, 
			{
				infoToken.sTokenName = sNewToken;
				listToken[nIndexHash].RemoveAt(posCurrent);
				nHashValue = HashString(infoToken.sTokenName);
				posNew = listToken[nHashValue].Find(infoToken);
				if (posNew == NULL)
				{
					listToken[nHashValue].AddTail(infoToken);
				}
				else
				{
					infoToken.dPercent = infoToken.dPercent + listToken[nHashValue].GetAt(posNew).dPercent;
					listToken[nHashValue].SetAt(posNew, infoToken);
				}
			}
			else
			{
				nIndexToken++;
			}
		}
	}

	double nTokenCount = 0;
	double nToken = 0;

	for (int i = 0; i < HASH_COUNT; i++)
	{
		nToken += listToken[i].GetCount();
		for (int j = 0; j < listToken[i].GetCount(); j++)
			nTokenCount += listToken[i].GetAt(listToken[i].FindIndex(j)).dPercent;
	}
}
