// ChildView.h : interface of the CChildView class
//
#pragma once
#define ROW 52 //在外围增加一圈格子
#define COL 52 //在外围增加一圈格子
#define UM_GAMEOVER			WM_USER+101
#define UM_TACHMINE			WM_USER+102
#define UM_LBUTTONDOWN		WM_USER+103
#define UM_RBUTTONDOWN		WM_USER+104
#define UM_LRBUTTONDOWN		WM_USER+105

struct SDia
{
	int nValue;//值，0表示空，1-8表示周围的雷数，9表示雷
	int nStatus;//状态，0表示未知，1表示翻开，2表示标识为雷，3表示待定
};

// CChildView window

class CChildView : public CWnd
{
public:
	int m_nSideLength;//边长
	int m_nRow;//行数
	int m_nCol;//列数
	CPoint m_ptTopleft;//棋盘左上角坐标
	SDia m_Board[ROW][COL];
	int m_nMines;//雷数
	BOOL m_bGameOver;//游戏是否结束
	int m_nTimer;//计时器
	BOOL m_bTimerStarted;//是否启动了计时器
	int m_nRemainMines;//剩余地雷数
	
	//外观
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

