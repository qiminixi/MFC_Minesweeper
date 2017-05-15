// GameSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Minesweeper.h"
#include "GameSettingDlg.h"


// CGameSettingDlg dialog

IMPLEMENT_DYNAMIC(CGameSettingDlg, CDialog)

CGameSettingDlg::CGameSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGameSettingDlg::IDD, pParent)
	, m_nRow(0)
	, m_nCol(0)
	, m_nMines(0)
{
	
}

CGameSettingDlg::~CGameSettingDlg()
{
}

void CGameSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ROW,m_nRow);
    DDV_MinMaxInt(pDX, m_nRow,1,50);
	DDX_Text(pDX, IDC_EDIT_COL,m_nCol);
    DDV_MinMaxInt(pDX, m_nCol, 1, 50);
	DDX_Text(pDX, IDC_EDIT_MINE,m_nMines);
    DDV_MinMaxInt(pDX, m_nMines, 1, 50);
}


BEGIN_MESSAGE_MAP(CGameSettingDlg, CDialog)
END_MESSAGE_MAP()


// CGameSettingDlg message handlers

BOOL CGameSettingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	CPoint point;
	::GetCursorPos(&point);
	WINDOWPLACEMENT wp;
	GetWindowPlacement(&wp);
	int x=point.x-wp.rcNormalPosition.left;
	int y=point.y-wp.rcNormalPosition.top;
	wp.rcNormalPosition.left+=x;
	wp.rcNormalPosition.top+=y;
	wp.rcNormalPosition.right+=x;
	wp.rcNormalPosition.bottom+=y;
	SetWindowPlacement(&wp);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
