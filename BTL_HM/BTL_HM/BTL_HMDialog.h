#pragma once
#include "afxwin.h"
#include "Stemmer.h"

typedef struct TOKEN_INFO
{
	CString sTokenName;
	double dPercent;
} TOKEN_INFO;
typedef CList<TOKEN_INFO, TOKEN_INFO&> TOKEN_LIST;

typedef CList<int, int&> NUMBER_LIST;

// CBTL_HMDialog dialog

#define SZ_NAME_FILE_SPAM_TRAIN		_T("train_SPAM.ems")
#define SZ_NAME_FILE_NORMAL_TRAIN	_T("train_GEN.ems")
#define SZ_NAME_FILE_SPAM_TEST		_T("MailTestSpam.txt")
#define SZ_NAME_FILE_NORMAL_TEST	_T("MailTestNormal.txt")

#define MY_MAX_PATH	1024
#define TEXT_BEGIN_TAG	_T("<TEXT_NORMAL>")
#define LEN_BEGIN_TAG	13
#define TEXT_END_TAG	_T("</TEXT_NORMAL>")
#define LEN_END_TAG		14

class StopWord
{
public:
	StopWord();
	~StopWord();
	void readStopWords(CList<CString>& listStopWord);
	void GetFrequency(CStringList& listMail, TOKEN_LIST& buffer );
private:
};


class CBTL_HMDialog : public CDialog
{
	DECLARE_DYNAMIC(CBTL_HMDialog)

public:
	CBTL_HMDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBTL_HMDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_MAIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	CString m_sFolderTrain;
	CString m_sFolderTest;
	CStringList m_listMailTrainSpam;
	CStringList m_listMailTrainNormal;
	CStringList m_listMailTestSpam;
	CStringList m_listMailTestNormal;
	CStringList m_listMailTestAll;
	TOKEN_LIST m_listTokenSpam;
	TOKEN_LIST m_listTokenNormal;
	CStringList m_listKey;
	TOKEN_LIST m_listProbabilityKeyOnSpam;
	TOKEN_LIST m_listProbabilityKeyOnNormal;
	int m_nKeyNumber;
	StopWord stopword;

public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();

	void GetMail(__in CString sPath, __out CStringList& sListMail);
	//void GetFrequency(CStringList& listMail, __out TOKEN_LIST& listToken);
	void GetKey(TOKEN_LIST& listNormal, TOKEN_LIST& listSpam, __out CStringList& listKey);
	void GetAppearOneMail(CStringList& listKey, CString sMail, __out NUMBER_LIST& listFrequency);
	void CalculateProbability(CStringList& listMail, CStringList& listKey, __out TOKEN_LIST& listProbability);

	CEdit textFolderTrain;
	CEdit textFolderTest;
	CButton btTrain;
	CButton btTest;
	CButton btEvaluate;
};
