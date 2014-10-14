#pragma once


// CBTL_HMDialog dialog

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
};
