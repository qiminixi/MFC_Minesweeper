// ChildView.h : interface of the CChildView class
//
#pragma once
#define ROW 52 //����Χ����һȦ����
#define COL 52 //����Χ����һȦ����
#define UM_GAMEOVER			WM_USER+101
#define UM_TACHMINE			WM_USER+102
#define UM_LBUTTONDOWN		WM_USER+103
#define UM_RBUTTONDOWN		WM_USER+104
#define UM_LRBUTTONDOWN		WM_USER+105

struct SDia
{
	int nValue;//ֵ��0��ʾ�գ�1-8��ʾ��Χ��������9��ʾ��
	int nStatus;//״̬��0��ʾδ֪��1��ʾ������2��ʾ��ʶΪ�ף�3��ʾ����
};

// CChildView window

class CChildView : public CWnd
{
public:
	int m_nSideLength;//�߳�
	int m_nRow;//����
	int m_nCol;//����
	CPoint m_ptTopleft;//�������Ͻ�����
	SDia m_Board[ROW][COL];
	int m_nMines;//����
	BOOL m_bGameOver;//��Ϸ�Ƿ����
	int m_nTimer;//��ʱ��
	BOOL m_bTimerStarted;//�Ƿ������˼�ʱ��
	int m_nRemainMines;//ʣ�������
	
	//���
	CBitmap m_bmpEmpty;
	CBitmap m_bmpNum[8];
	CBitmap m_bmpMine;
	CBitmap m_bmpMineTach;
	//
	CBitmap m_bmpUnknown;
	CBitmap m_bmpSure;
	CBitmap m_bmpPend;
	
public:
	void InitBoard();
	void DealWithEmptyCell(int r,int c);
	void DealWithEmptyCellAssistant(int r,int c);
	BOOL IsGameOver();
	void UpdateStatusBar();
// Construction
public:
	CChildView();

// Attributes
public:

// Operations
public:

// Overrides
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CChildView();

	// Generated message map functions
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNewgame();
public:
//	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
public:
//	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
public:
	afx_msg LRESULT OnGameOver(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTachMine(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnUMLButtonDown(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnUMRButtonDown(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnUMLRButtonDown(WPARAM wParam,LPARAM lParam);
public:
	afx_msg void OnGamesetting();
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
	afx_msg void OnResumegame();
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
public:
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
};

