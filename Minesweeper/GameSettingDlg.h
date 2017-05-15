#pragma once


// CGameSettingDlg dialog

class CGameSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CGameSettingDlg)

public:
	CGameSettingDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGameSettingDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_nRow;
public:
	int m_nCol;
public:
	int m_nMines;
public:
	virtual BOOL OnInitDialog();
};
