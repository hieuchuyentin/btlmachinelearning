// BTL_HMDialog.cpp : implementation file
//

#include "stdafx.h"
#include "BTL_HM.h"
#include "BTL_HMDialog.h"

#include <math.h>

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
	m_edtResult = (CEdit*)this->GetDlgItem(IDC_EDIT3);
	m_lvTokens = (CListCtrl*)this->GetDlgItem(IDC_LIST1);

	m_lvTokens->SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_lvTokens->InsertColumn(0, _T("Token"), LVCFMT_LEFT, 120, -1);
	m_lvTokens->InsertColumn(1, _T("Ham score"), LVCFMT_LEFT, 120, -1);
	m_lvTokens->InsertColumn(2, _T("Spam score"), LVCFMT_LEFT, 120, -1);

}


BEGIN_MESSAGE_MAP(CBTL_HMDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CBTL_HMDialog::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CBTL_HMDialog::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CBTL_HMDialog::OnBnClickedButton3)
END_MESSAGE_MAP()


// CBTL_HMDialog message handlers
void CBTL_HMDialog::Train()
{
	// Tiền xử lý bỏ dẫu, chữ hoa
	stopword.StemMailList(m_listMailTrainNormal);
	stopword.StemMailList(m_listMailTrainSpam);

	// Chọn ngẫu nhiên mail để phân vào làm 2 loại

	// Đếm số lần xuất hiện của các từ
	CountWord(m_listMailTrainNormal, m_ListTokenNormal);
	CountWord(m_listMailTrainSpam, m_ListTokenSpam);

	// Loại bỏ stopword
	stopword.DeleteStopWord(m_ListTokenNormal, HASH_COUNT);
	stopword.DeleteStopWord(m_ListTokenSpam, HASH_COUNT);

	// Tìm từ khóa quan trọng
  	//GetKey(m_ListTokenNormal, m_ListTokenSpam, m_listKey);
	GetKeyNew(m_ListTokenNormal, m_ListTokenSpam, m_listKey);

	VerifyKey(m_listKey);

	m_nKeyNumber = m_listKey.GetCount();
 
	
	// Tính xác suất của mỗi từ khóa với mỗi nhãn lớp spam/normal
	//CalculateProbability(m_listMailTrainNormal, m_listMailTrainSpam, m_listKey, m_listTokenScore);
	CalculateProbabilityNew(m_ListTokenNormal, m_ListTokenSpam, m_listKey, m_listTokenScore);

	// Hiển thị ra listview tokens và điểm số từng tokens
	CString cszTemp;
	int		nIndex = 0;
	POSITION pos = m_listTokenScore.GetHeadPosition();
	TOKEN_SCORE tcTemp;

	m_lvTokens->DeleteAllItems();
	while(pos != NULL)
	{
		tcTemp = m_listTokenScore.GetNext(pos);

		m_lvTokens->InsertItem(nIndex, tcTemp.sToken);

		cszTemp.Format(_T("%f"), tcTemp.dbHamScore);
		m_lvTokens->SetItemText(nIndex, 1, cszTemp);

		cszTemp.Format(_T("%f"), tcTemp.dbSpamScore);
		m_lvTokens->SetItemText(nIndex, 2, cszTemp);

		nIndex ++;
	}

	cszTemp.Format(_T("Total tokens: %d\r\n"), m_listTokenScore.GetCount());
	
	m_edtResult->SetWindowText(cszTemp);
}
void CBTL_HMDialog::OnBnClickedButton1()//Bắt đầu huấn luyện
{
	// TODO: Add your control notification handler code here

	
//	Đọc danh sách thư từ tệp
	textFolderTrain.GetWindowText(m_sFolderTrain);

	GetMail(m_sFolderTrain + _T("\\") + SZ_NAME_FILE_NORMAL_TRAIN, m_listMailTrainNormal);
	GetMail(m_sFolderTrain + _T("\\") + SZ_NAME_FILE_SPAM_TRAIN, m_listMailTrainSpam);
 
	Train();
	
}

void CBTL_HMDialog::Test()
{
	POSITION posMail, posToken;
	TOKEN_SCORE tcTemp;
	CString cszMail, cszResult, cszResult_;
	int nReallyHam, nDetectNormal, nErrorHam, nReallySpam, nDetectSpam, nErrorSpam;
	double dbProbMailHam, dbProbMailSpam, dbScoreHam, dbScoreSpam;
	CTime timeBegin;
	CTime timeEnd;

	CStringList listOneMail;
	TOKEN_LIST listTokenMail[HASH_COUNT];
	TOKEN_INFO tokenKey;
	int nHashValue;
	POSITION posTokenMail;

	nReallyHam  = m_listMailTestNormal.GetCount();
	nReallySpam = m_listMailTestSpam.GetCount();

	dbProbMailHam  = log((double)m_listMailTrainNormal.GetCount() / (m_listMailTrainNormal.GetCount() + m_listMailTrainSpam.GetCount()));
	dbProbMailSpam = log((double)m_listMailTrainSpam.GetCount() / (m_listMailTrainNormal.GetCount() + m_listMailTrainSpam.GetCount()));

	timeBegin = CTime::GetCurrentTime();

	stopword.StemMailList(m_listMailTestNormal);
	stopword.StemMailList(m_listMailTestSpam);

	// classify ham mails
	posMail = m_listMailTestNormal.GetHeadPosition();

	nDetectNormal = 0;

	while (posMail != NULL)
	{
		dbScoreHam  = dbProbMailHam;
		dbScoreSpam = dbProbMailSpam;

		cszMail = m_listMailTestNormal.GetNext(posMail);

		listOneMail.RemoveAll();
		listOneMail.AddTail(cszMail);
		CountWord(listOneMail, listTokenMail);

		posToken = m_listTokenScore.GetHeadPosition();

		while (posToken != NULL)
		{
			tcTemp = m_listTokenScore.GetNext(posToken);

			nHashValue = HashString(tcTemp.sToken);
			tokenKey.sTokenName = tcTemp.sToken;

			posTokenMail = listTokenMail[nHashValue].Find(tokenKey);
			if (posTokenMail != NULL)
			{
				tokenKey = listTokenMail[nHashValue].GetAt(posTokenMail);
				dbScoreHam  += tcTemp.dbHamScore * tokenKey.dPercent;
				dbScoreSpam += tcTemp.dbSpamScore * tokenKey.dPercent;
			}
		}		
		if (dbScoreHam >= dbScoreSpam) nDetectNormal ++;
	}

	nErrorHam = nReallyHam - nDetectNormal;

	// classify spam mails
	posMail = m_listMailTestSpam.GetHeadPosition();

	nDetectSpam = 0;

	while (posMail != NULL)
	{
		dbScoreHam  = dbProbMailHam;
		dbScoreSpam = dbProbMailSpam;

		cszMail = m_listMailTestSpam.GetNext(posMail);

		listOneMail.RemoveAll();
		listOneMail.AddTail(cszMail);
		CountWord(listOneMail, listTokenMail);

		posToken = m_listTokenScore.GetHeadPosition();

		while (posToken != NULL)
		{
			tcTemp = m_listTokenScore.GetNext(posToken);

			nHashValue = HashString(tcTemp.sToken);
			tokenKey.sTokenName = tcTemp.sToken;

			posTokenMail = listTokenMail[nHashValue].Find(tokenKey);
			if (posTokenMail != NULL)
			{
				tokenKey = listTokenMail[nHashValue].GetAt(posTokenMail);
				dbScoreHam  += tcTemp.dbHamScore * tokenKey.dPercent;
				dbScoreSpam += tcTemp.dbSpamScore * tokenKey.dPercent;
			}
		}
		if (dbScoreHam < dbScoreSpam) nDetectSpam ++;
	}

	nErrorSpam = nReallySpam - nDetectSpam;

	timeEnd = CTime::GetCurrentTime();

	int nTime = (timeEnd.GetMinute() - timeBegin.GetMinute()) * 60 + timeEnd.GetSecond() - timeBegin.GetSecond();

	CString sResultCalculate;
	double dAccuracy, dPrecisionNormal, dPrecisionSpam, dRecallNormal, dRecallSpam;
	double dPrecisionAverage, dRecallAverage, dF1Average;

	int nFailDetectNormal, nFailDetectSpam;

	m_edtResult->GetWindowText(cszResult_);
	cszResult.Format(_T("%sTotal mail: %d\r\nTotal ham: %d ---------- Detected: %d ---------- Error: %d\r\n\Total spam: %d ---------- Detected: %d ----------Error: %d\r\n"),
		cszResult_, nReallyHam + nReallySpam, nReallyHam, nDetectNormal, nErrorHam, nReallySpam, nDetectSpam, nErrorSpam);

	nFailDetectNormal = nReallySpam - nDetectSpam;//số mail là mail rác nhưng bị phân thành mail thường
	nFailDetectSpam = nReallyHam - nDetectNormal;//số mail là mail thường nhưng bị phân thành mail rác

	//Tính độ chính xác
	dAccuracy = (double) (nDetectNormal + nDetectSpam) / (nReallyHam + nReallySpam);
	sResultCalculate.Format(_T("Tỉ lệ chính xác %f\r\n"), dAccuracy); 
	cszResult += sResultCalculate;

	//Tính Precision
	dPrecisionNormal = (double)nDetectNormal / (nDetectNormal + nFailDetectNormal);
	dPrecisionSpam = (double)nDetectSpam / (nDetectSpam + nFailDetectSpam);
	sResultCalculate.Format(_T("Precision mail thường: %f\r\n"), dPrecisionNormal);
	cszResult += sResultCalculate;
	sResultCalculate.Format(_T("Precision mail rác: %f\r\n"), dPrecisionSpam); 
	cszResult += sResultCalculate;

	//Tính Recall
	dRecallNormal = (double)nDetectNormal / nReallyHam;
	dRecallSpam = (double)nDetectSpam / nReallySpam;
	sResultCalculate.Format(_T("Recall mail thường: %f\r\n"), dRecallNormal);
	cszResult += sResultCalculate;
	sResultCalculate.Format(_T("Recall mail rác: %f\r\n"), dRecallSpam); 
	cszResult += sResultCalculate;

	//Tính F1
	dPrecisionAverage = (dPrecisionNormal + dPrecisionSpam) / 2;
	dRecallAverage = (dRecallNormal + dRecallSpam) / 2;
	dF1Average = 2 / (1 / dPrecisionAverage + 1 / dRecallAverage);
	sResultCalculate.Format(_T("Precision trung bình vĩ mô: %f\r\n"), dPrecisionAverage);
	cszResult += sResultCalculate;
	sResultCalculate.Format(_T("Recall trung bình vĩ mô: %f\r\n"), dRecallAverage);
	cszResult += sResultCalculate;
	sResultCalculate.Format(_T("F1: %f\r\n"), dF1Average);
	cszResult += sResultCalculate;

	sResultCalculate.Format(_T("Tổng thời gian: %d giây\r\nTrung bình một mail: %f\r\n"), nTime, (double)nTime / (m_listMailTestNormal.GetCount() + m_listMailTestSpam.GetCount()));
	cszResult += sResultCalculate;

	cszResult += _T("================\r\n\r\n");

	m_edtResult->SetWindowText(cszResult);
}
void CBTL_HMDialog::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here

	textFolderTest.GetWindowText(m_sFolderTest);

	GetMail(m_sFolderTest + _T("\\") + SZ_NAME_FILE_NORMAL_TEST, m_listMailTestNormal);
	GetMail(m_sFolderTest + _T("\\") + SZ_NAME_FILE_SPAM_TEST, m_listMailTestSpam);

	Test();
	
}

//******************************************************************************************
//Phân tích một file *.ems đầu vào thành các email riêng biệt với kiểu CString
//Input:
// - CString  cFilePath: đường dẫn đến file email muốn phân tích
//Output:
// - CstringList &csContent: list các CString chứa nội dung email, mỗi phần tử là một email
//Example:
// - AnalysisInput(_T("C:\\train_SPAM.ems"), g_clTrainSpam);
//******************************************************************************************

void CBTL_HMDialog::GetMail(__in CString sPath, __out CStringList& sListMail, int nStatus)
{
	FILE* file;
	CString cContent;
	CString cTemp;
	TCHAR szBuffer[MY_MAX_PATH];

	if (nStatus == 0)
	{
		sListMail.RemoveAll();
	}

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

void CBTL_HMDialog::GetKey(TOKEN_LIST listNormal[], TOKEN_LIST listSpam[], __out CStringList& listKey)
{
	double nTokenCountNormal = 0;
	double nTokenNormal = 0;
	double nTokenCountSpam = 0;
	double nTokenSpam = 0;
	double dAverageNormal;
	double dAverageSpam;
	double dPorportion;

	int nIndexHash, nIndexToken;
	TOKEN_INFO infoToken;
	POSITION pos;

	int nIndexNormal;
	int nIndexSpam;
	POSITION posAppear;
	TOKEN_INFO infoTokenNormal, infoTokenSpam;
	TOKEN_SCORE tcTemp;

	for (int i = 0; i < HASH_COUNT; i++)
	{
		nTokenNormal += listNormal[i].GetCount();
		for (int j = 0; j < listNormal[i].GetCount(); j++)
			nTokenCountNormal += listNormal[i].GetAt(listNormal[i].FindIndex(j)).dPercent;
	}
	dAverageNormal = 1.0 / nTokenNormal;

	for (nIndexHash = 0; nIndexHash < HASH_COUNT; nIndexHash++)
	{
		nIndexToken = 0;
		while (nIndexToken < listNormal[nIndexHash].GetCount())
		{
			pos = listNormal[nIndexHash].FindIndex(nIndexToken);
			infoToken = listNormal[nIndexHash].GetAt(pos);

			//infoToken.dPercent = infoToken.dPercent / nTokenCountNormal;
			if (infoToken.dPercent / nTokenCountNormal < dAverageNormal)
			{
				listNormal[nIndexHash].RemoveAt(pos);
			}
			else
			{
				//listNormal[nIndexHash].SetAt(pos, infoToken);//Đã thay số lần xuất hiện bằng tỉ lệ
				nIndexToken++;
			}
		}
	}


	for (int i = 0; i < HASH_COUNT; i++)
	{
		nTokenSpam += listSpam[i].GetCount();
		for (int j = 0; j < listSpam[i].GetCount(); j++)
			nTokenCountSpam += listSpam[i].GetAt(listSpam[i].FindIndex(j)).dPercent;
	}
	dAverageSpam = 1.0 / nTokenSpam;

	for (nIndexHash = 0; nIndexHash < HASH_COUNT; nIndexHash++)
	{
		nIndexToken = 0;
		while (nIndexToken < listSpam[nIndexHash].GetCount())
		{
			pos = listSpam[nIndexHash].FindIndex(nIndexToken);
			infoToken = listSpam[nIndexHash].GetAt(pos);

			//infoToken.dPercent = infoToken.dPercent / nTokenCountSpam;
			if (infoToken.dPercent / nTokenCountSpam < dAverageSpam)
			{
				listSpam[nIndexHash].RemoveAt(pos);
			}
			else
			{
				//listSpam[nIndexHash].SetAt(pos, infoToken);
				nIndexToken++;
			}
		}
	}
	
	listKey.RemoveAll();

	//Tìm các token xuất hiện trong cả 2 tập, so sánh
	for (nIndexHash = 0; nIndexHash < HASH_COUNT; nIndexHash++)
	{
		for (nIndexNormal = 0; nIndexNormal < listNormal[nIndexHash].GetCount(); nIndexNormal++)
		{
			infoTokenNormal = listNormal[nIndexHash].GetAt(listNormal[nIndexHash].FindIndex(nIndexNormal));
			
			posAppear = listSpam[nIndexHash].Find(infoTokenNormal);
			if (posAppear != NULL)
			{
				infoTokenSpam = listSpam[nIndexHash].GetAt(posAppear);
				dPorportion = ((double)infoTokenNormal.dPercent / nTokenCountNormal) 
					/ (infoTokenNormal.dPercent / nTokenCountNormal + infoTokenSpam.dPercent / nTokenCountSpam);
				if ((dPorportion < 0.1) || (dPorportion > 0.9))
				{
					listKey.AddTail(infoTokenNormal.sTokenName);
				}
			}
		}
	}

	//Tìm các từ chỉ có trong tập thường
	for (nIndexHash = 0; nIndexHash < HASH_COUNT; nIndexHash++)
	{
		for (nIndexNormal = 0; nIndexNormal < listNormal[nIndexHash].GetCount(); nIndexNormal++)
		{
			infoTokenNormal = listNormal[nIndexHash].GetAt(listNormal[nIndexHash].FindIndex(nIndexNormal));

			posAppear = listSpam[nIndexHash].Find(infoTokenNormal);
			if (posAppear == NULL)
				listKey.AddTail(infoTokenNormal.sTokenName);
		}
	}
	
	//Tìm các từ chỉ có trong tập rác
	for (nIndexHash = 0; nIndexHash < HASH_COUNT; nIndexHash++)
	{
		for (nIndexSpam = 0; nIndexSpam < listSpam[nIndexHash].GetCount(); nIndexSpam++)
		{
			infoTokenSpam = listSpam[nIndexHash].GetAt(listSpam[nIndexHash].FindIndex(nIndexSpam));

			posAppear = listNormal[nIndexHash].Find(infoTokenSpam);
			if (posAppear == NULL)
				listKey.AddTail(infoTokenSpam.sTokenName);
		}
	}

	return;
}

void CBTL_HMDialog::GetKeyNew(TOKEN_LIST listNormal[], TOKEN_LIST listSpam[], __out CStringList& listKey)
{
	TOKEN_LIST listTokenTotal[HASH_COUNT];
	int nIndexHash;
	POSITION posToken, posTokenTotal;
	TOKEN_INFO infoToken;
	double dCountToken, nTokenCountNormal, nTokenCountSpam;

	nTokenCountNormal = 0;
	for (nIndexHash = 0; nIndexHash < HASH_COUNT; nIndexHash++)
	{
		posToken = listNormal[nIndexHash].GetHeadPosition();
		while (posToken != NULL)
		{
			infoToken = listNormal[nIndexHash].GetNext(posToken);
			nTokenCountNormal += infoToken.dPercent;
		}
	}

	nTokenCountSpam = 0;
	for (nIndexHash = 0; nIndexHash < HASH_COUNT; nIndexHash++)
	{
		posToken = listSpam[nIndexHash].GetHeadPosition();
		while (posToken != NULL)
		{
			infoToken = listSpam[nIndexHash].GetNext(posToken);
			nTokenCountSpam += infoToken.dPercent;
		}
	}

	dCountToken = nTokenCountNormal * 2;

	for (nIndexHash = 0; nIndexHash < HASH_COUNT; nIndexHash++)
	{
		posToken = listNormal[nIndexHash].GetHeadPosition();
		while (posToken != NULL)
		{
			infoToken = listNormal[nIndexHash].GetNext(posToken);
			listTokenTotal[nIndexHash].AddTail(infoToken);		
		}
	}

	for (nIndexHash = 0; nIndexHash < HASH_COUNT; nIndexHash++)
	{
		posToken = listSpam[nIndexHash].GetHeadPosition();
		while (posToken != NULL)
		{
			infoToken = listSpam[nIndexHash].GetNext(posToken);
			infoToken.dPercent = infoToken.dPercent * nTokenCountNormal / nTokenCountSpam;

			posTokenTotal = listTokenTotal[nIndexHash].Find(infoToken);
			if (posTokenTotal == NULL)
			{
				listTokenTotal[nIndexHash].AddTail(infoToken);
			}
			else
			{
				infoToken.dPercent += listTokenTotal[nIndexHash].GetAt(posTokenTotal).dPercent;
				listTokenTotal[nIndexHash].SetAt(posTokenTotal, infoToken);
			}
		}
	}

	listKey.RemoveAll();
	for (nIndexHash = 0; nIndexHash < HASH_COUNT; nIndexHash++)
	{
		posTokenTotal = listTokenTotal[nIndexHash].GetHeadPosition();
		while (posTokenTotal != NULL)
		{
			infoToken = listTokenTotal[nIndexHash].GetNext(posTokenTotal);
			if (infoToken.dPercent / dCountToken > 0.0001)
				listKey.AddTail(infoToken.sTokenName);
		}
	}
}
void CBTL_HMDialog::GetAppearOneMail(CStringList& listKey, CString sMail, __out NUMBER_LIST& listFrequency)
{
	listFrequency.RemoveAll();
	return;
}


void CBTL_HMDialog::CalculateProbability(CStringList& listMailHam, CStringList& listMailSpam, 
	/*TOKEN_LIST listTokenNormal[], TOKEN_LIST listTokenSpam[],*/ CStringList& listKey, __out TOKEN_SCORE_LIST& listProbability)
{
	TOKEN_SCORE tcTemp;
	POSITION	posKey, posMail;
	int nCount, nTotalMailHam, nTotalMailSpam;
	CString cszToken, cszMail;

	nTotalMailHam = listMailHam.GetCount();
	nTotalMailSpam = listMailSpam.GetCount();

	posKey = listKey.GetHeadPosition();

	while(posKey != NULL)
	{
		cszToken = listKey.GetNext(posKey);

		tcTemp.sToken = cszToken;

		posMail = listMailHam.GetHeadPosition();
		nCount = 0;

		while(posMail != NULL)
		{
			cszMail = listMailHam.GetNext(posMail);
			if (cszMail.Find(cszToken) != -1)
			{
				nCount ++;
			}
		}

		tcTemp.dbHamScore = log((double)(nCount + 1)/ nTotalMailHam);

		posMail = listMailSpam.GetHeadPosition();
		nCount = 0;

		while(posMail != NULL)
		{
			cszMail = listMailSpam.GetNext(posMail);
			if (cszMail.Find(cszToken) != -1)
			{
				nCount ++;
			}
		}

		tcTemp.dbSpamScore = log((double)(nCount + 1) / nTotalMailSpam);

		listProbability.AddTail(tcTemp);
	}


	return;
}

void CBTL_HMDialog::CalculateProbabilityNew(TOKEN_LIST listTokenNormal[], TOKEN_LIST listTokenSpam[], CStringList& listKey, __out TOKEN_SCORE_LIST& listProbability)
{
	TOKEN_SCORE tcTemp;
	TOKEN_INFO infoToken;
	POSITION	posKey, posNormal, posSpam;
	CString sToken;
	int nHashValue;
	double dCountNormal, dCountSpam;

	listProbability.RemoveAll();
	dCountNormal = listKey.GetCount();
	dCountSpam = listKey.GetCount();

	posKey = listKey.GetHeadPosition();
	while (posKey != NULL)
	{
		sToken = listKey.GetNext(posKey);
		nHashValue = HashString(sToken);
		infoToken.sTokenName = sToken;
		posNormal = listTokenNormal[nHashValue].Find(infoToken);
		posSpam = listTokenSpam[nHashValue].Find(infoToken);

		if (posNormal != NULL)
			dCountNormal = dCountNormal + listTokenNormal[nHashValue].GetAt(posNormal).dPercent;
		if (posSpam != NULL)
			dCountSpam = dCountSpam + listTokenSpam[nHashValue].GetAt(posSpam).dPercent;
	}

	posKey = listKey.GetHeadPosition();
	while (posKey != NULL)
	{
		sToken = listKey.GetNext(posKey);
		nHashValue = HashString(sToken);
		infoToken.sTokenName = sToken;
		posNormal = listTokenNormal[nHashValue].Find(infoToken);
		posSpam = listTokenSpam[nHashValue].Find(infoToken);

		tcTemp.sToken = sToken;

		if (posNormal != NULL)
			tcTemp.dbHamScore = log((listTokenNormal[nHashValue].GetAt(posNormal).dPercent + 1) / dCountNormal);
		else
			tcTemp.dbHamScore = log((double)1 / dCountNormal);

		if (posSpam != NULL)
			tcTemp.dbSpamScore = log((listTokenSpam[nHashValue].GetAt(posSpam).dPercent + 1) / dCountSpam);
		else
			tcTemp.dbSpamScore = log((double)1 /dCountSpam);

		listProbability.AddTail(tcTemp);
	}
	
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
	POSITION posToken, posMail;
	int curPos;
	int nHashValue;

	for (int i = 0; i < HASH_COUNT; i++)
		listToken[i].RemoveAll();

	posMail = listMail.GetHeadPosition();
	while (posMail != NULL)
	{
		mail = listMail.GetNext((posMail));
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

void CBTL_HMDialog::GroupMail(CStringList& listMail, CStringList& listMailTrain, CStringList& listMailTest)
{
	int nCount = 0;
	int nSize = listMail.GetCount();
	int nTest = nSize * 3 / 10;
	int nIndex;
	POSITION pos;
	CString cszMail;

	listMailTrain.RemoveAll();
	listMailTest.RemoveAll();

	srand(time(NULL));

	pos = listMail.GetHeadPosition();
	while (pos != NULL)
	{
		cszMail = listMail.GetNext(pos);
		listMailTrain.AddTail(cszMail);
	}

	while (nCount < nTest)
	{
		nIndex = rand() % nSize;

		pos = listMailTrain.FindIndex(nIndex);
		cszMail = listMailTrain.GetAt(pos);

		listMailTest.AddTail(cszMail);

		listMailTrain.RemoveAt(pos);

		nSize = listMailTrain.GetCount();
		nCount ++;
	}
}

void CBTL_HMDialog::VerifyKey(CStringList& listKey)
{
	POSITION pos, posOld;
	CString sKey;

	pos = listKey.GetHeadPosition();

	while (pos != NULL)
	{
		posOld = pos;
		sKey = listKey.GetNext(pos);
		if ((sKey.GetLength() <= 2)
			|| (sKey.Find(_T('0')) != -1)
			|| (sKey.Find(_T('1')) != -1)
			|| (sKey.Find(_T('2')) != -1)
			|| (sKey.Find(_T('3')) != -1)
			|| (sKey.Find(_T('4')) != -1)
			|| (sKey.Find(_T('5')) != -1)
			|| (sKey.Find(_T('6')) != -1)
			|| (sKey.Find(_T('7')) != -1)
			|| (sKey.Find(_T('8')) != -1)
			|| (sKey.Find(_T('9')) != -1)
			|| (sKey.Find(_T('!')) != -1)
			|| (sKey.Find(_T('$')) != -1)
			|| (sKey.Find(_T('#')) != -1)
			|| (sKey.Find(_T('/')) != -1)
			|| (sKey.Find(_T('\\')) != -1)
			|| (sKey.Find(_T('-')) != -1)
			|| (sKey.Find(_T('+')) != -1)
			|| (sKey.Find(_T('~')) != -1)
			|| (sKey.Find(_T('\'')) != -1)
			|| (sKey.Find(_T('&')) != -1)
			|| (sKey.Find(_T('_')) != -1)
			)
			listKey.RemoveAt(posOld);
	}
}
void CBTL_HMDialog::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here
	textFolderTrain.GetWindowText(m_sFolderTrain);

	GetMail(m_sFolderTrain + _T("\\") + SZ_NAME_FILE_NORMAL_TRAIN, m_listMailNormal);
	GetMail(m_sFolderTrain + _T("\\") + SZ_NAME_FILE_SPAM_TRAIN, m_listMailSpam);

	textFolderTest.GetWindowText(m_sFolderTest);

	GetMail(m_sFolderTest + _T("\\") + SZ_NAME_FILE_NORMAL_TEST, m_listMailNormal, 1);
	GetMail(m_sFolderTest + _T("\\") + SZ_NAME_FILE_SPAM_TEST, m_listMailSpam, 1);

	GroupMail(m_listMailNormal, m_listMailTrainNormal, m_listMailTestNormal);
	GroupMail(m_listMailSpam, m_listMailTrainSpam, m_listMailTestSpam);

	Train();
	Test();
}
