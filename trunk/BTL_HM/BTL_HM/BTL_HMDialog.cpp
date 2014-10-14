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
}


BEGIN_MESSAGE_MAP(CBTL_HMDialog, CDialog)
END_MESSAGE_MAP()


// CBTL_HMDialog message handlers
