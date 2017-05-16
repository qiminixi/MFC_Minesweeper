// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "Minesweeper.h"
#include "ChildView.h"
#include "GameSettingDlg.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MARGIN 20

BOOL g_bGoOn=FALSE;
int g_nMessage=0;

HANDLE g_hEvent=NULL;
BOOL g_bLBtnDown=FALSE;
BOOL g_bRBtnDown=FALSE;
CPoint g_point;
// CChildView

UINT RelayMessage(LPVOID pParam)
{
	TRACE(_T("RealyMessage: Thread started!\n"));
	
	CChildView* pView=(CChildView*)pParam;
	while(1)
	{
		
		while(1)
		{
			Sleep(10);
			if(g_bGoOn)
			{
				g_bGoOn=FALSE;
				break;
			}
		}
		Sleep(100);
		if(g_nMessage==1)
			pView->PostMessageW(UM_GAMEOVER);
		if(g_nMessage==2)
			pView->PostMessageW(UM_TACHMINE);
		g_nMessage=0;
	}
	return 0;
}
UINT MouseClick(LPVOID pParam)
{
	TRACE(_T("MouseClick: Thread started!\n"));
	
	CChildView* pView=(CChildView*)pParam;
	while(1)
	{
		::WaitForSingleObject(g_hEvent,INFINITE);
		if(g_bLBtnDown)
		{
			g_bLBtnDown=FALSE;
			Sleep(500);
			if(g_bLBtnDown)//如果一定时间内还单击了一次鼠标，判断为双击
				g_bLBtnDown=FALSE;
			else if(g_bRBtnDown)//左右键都点击了
			{
				g_bRBtnDown=FALSE;
				pView->PostMessageW(UM_LRBUTTONDOWN);
			}
			else //单击
				pView->PostMessageW(UM_LBUTTONDOWN);
			continue;
		}
		if(g_bRBtnDown)
		{
			g_bRBtnDown=FALSE;
			pView->PostMessageW(UM_RBUTTONDOWN);
		}
		
	}
	return 0;
}
CChildView::CChildView()
{
	m_nSideLength=30;
	m_nRow=16;
	m_nCol=13;
	m_bGameOver=FALSE;
	m_nTimer=0;
	m_bTimerStarted=FALSE;
	
	for(int i=0;i<ROW;i++)
	{
		for(int j=0;j<COL;j++)
		{
			m_Board[i][j].nValue=-1;
			m_Board[i][j].nStatus=0;
		}
	}
	m_nMines=15;
	m_nRemainMines=m_nMines;
	InitBoard();
	
	//外观
	m_bmpEmpty.LoadBitmapW(IDB_BITMAP_EMPTY1);//
	for(int i=0;i<8;i++)//
	{
		m_bmpNum[i].LoadBitmapW(IDB_BITMAP_NUM_A1+i);
	}
	m_bmpMine.LoadBitmapW(IDB_BITMAP_MINE1);//
	m_bmpMineTach.LoadBitmapW(IDB_BITMAP_MINE_TACH1);//
	//
	m_bmpUnknown.LoadBitmapW(IDB_BITMAP_UNKNOWN1);//
	m_bmpSure.LoadBitmapW(IDB_BITMAP_SURE1);//
	m_bmpPend.LoadBitmapW(IDB_BITMAP_PEND1);//	
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_COMMAND(ID_NEWGAME, &CChildView::OnNewgame)
//	ON_WM_LBUTTONUP()
//	ON_WM_RBUTTONUP()
	ON_MESSAGE(UM_GAMEOVER,OnGameOver)
	ON_MESSAGE(UM_TACHMINE,OnTachMine)
	ON_MESSAGE(UM_LBUTTONDOWN,OnUMLButtonDown)
	ON_MESSAGE(UM_RBUTTONDOWN,OnUMRButtonDown)
	ON_MESSAGE(UM_LRBUTTONDOWN,OnUMLRButtonDown)
	ON_COMMAND(ID_GAMESETTING, &CChildView::OnGamesetting)
	ON_WM_TIMER()
	ON_COMMAND(ID_RESUMEGAME, &CChildView::OnResumegame)
	ON_WM_ERASEBKGND()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDBLCLK()
END_MESSAGE_MAP()



// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);
	return TRUE;
}

void CChildView::OnPaint() 
{
	static BOOL bDoOnce=TRUE;
	if(bDoOnce)
	{
		g_hEvent=CreateEvent(NULL,FALSE,FALSE,NULL);
		AfxBeginThread(RelayMessage,this);
		AfxBeginThread(MouseClick,this);
		UpdateStatusBar();
		bDoOnce=FALSE;
	}
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	CRect rectClient;
	GetClientRect(rectClient);
	int n;
	n=(rectClient.Width()-MARGIN*2)/m_nCol;//水平方向的边长
	m_nSideLength=(rectClient.Height()-MARGIN*2)/m_nRow;//竖直方向的边长
	//TRACE("OnPaint: n=%d,m_nSideLength=%d\n",n,m_nSideLength);
	if(n<m_nSideLength)
		m_nSideLength=n;
	//TRACE("OnPaint: m_nSideLength=%d\n",m_nSideLength);
	
	
	m_ptTopleft.x=MARGIN+(rectClient.Width()-MARGIN*2-m_nCol*m_nSideLength)/2;
	m_ptTopleft.y=MARGIN+(rectClient.Height()-MARGIN*2-m_nRow*m_nSideLength)/2;
	
	CDC memDC;
	CBitmap memBmp;
	memDC.CreateCompatibleDC(NULL);
	memBmp.CreateCompatibleBitmap(&dc,rectClient.Width(),rectClient.Height());
	CBitmap* pMemBmp=memDC.SelectObject(&memBmp);
	CBrush brBg(RGB(183,187,236));
	memDC.FillRect(rectClient,&brBg);
	memDC.SetBkMode(TRANSPARENT);
	//memDC.SetStretchBltMode(COLORONCOLOR);
	
	for(int i=0;i<=m_nRow;i++)//绘制行
	{
		memDC.MoveTo(m_ptTopleft.x,m_ptTopleft.y+i*m_nSideLength);
		memDC.LineTo(m_ptTopleft.x+m_nCol*m_nSideLength,m_ptTopleft.y+i*m_nSideLength);
	}
	for(int i=0;i<=m_nCol;i++)//绘制列
	{
		memDC.MoveTo(m_ptTopleft.x+i*m_nSideLength,m_ptTopleft.y);
		memDC.LineTo(m_ptTopleft.x+i*m_nSideLength,m_ptTopleft.y+m_nRow*m_nSideLength);
	}
	
	CPoint point;//小方块的左上角坐标
	CBrush brBlue(RGB(0,162,232));
	CBrush* pOldBrush=NULL;
	CBrush brRed(RGB(255,127,39));
	CBrush brGreen(RGB(181,230,29));
	CString str;
	CDC BmpDC;
	BmpDC.CreateCompatibleDC(&dc);
	CBitmap* pOldBitmap=BmpDC.SelectObject(&m_bmpUnknown);
	for(int i=0;i<m_nRow;i++)
	{
		for(int j=0;j<m_nCol;j++)
		{
			point.x=m_ptTopleft.x+j*m_nSideLength+1;
			point.y=m_ptTopleft.y+i*m_nSideLength+1;
			if(m_Board[i+1][j+1].nStatus==0)//隐藏的格子
			{
				BmpDC.SelectObject(&m_bmpUnknown);
				memDC.StretchBlt(point.x,point.y,m_nSideLength-1,m_nSideLength-1,&BmpDC,0,0,100,100,SRCCOPY);
			}
			else if(m_Board[i+1][j+1].nStatus==1)
			{
				if(m_Board[i+1][j+1].nValue==9)//踩中雷
				{
					BmpDC.SelectObject(&m_bmpMineTach);
					memDC.StretchBlt(point.x,point.y,m_nSideLength-1,m_nSideLength-1,&BmpDC,0,0,100,100,SRCCOPY);
				}
				else if(m_Board[i+1][j+1].nValue<9&&m_Board[i+1][j+1].nValue>0)//翻开的数字格子
				{
					BmpDC.SelectObject(&m_bmpNum[m_Board[i+1][j+1].nValue-1]);
					memDC.StretchBlt(point.x,point.y,m_nSideLength-1,m_nSideLength-1,&BmpDC,0,0,100,100,SRCCOPY);
				}
				else//空格
				{
					BmpDC.SelectObject(&m_bmpEmpty);
					memDC.StretchBlt(point.x,point.y,m_nSideLength-1,m_nSideLength-1,&BmpDC,0,0,100,100,SRCCOPY);
				}
			}
			else if(m_Board[i+1][j+1].nStatus==2)//标记雷
			{
				BmpDC.SelectObject(&m_bmpSure);
				memDC.StretchBlt(point.x,point.y,m_nSideLength-1,m_nSideLength-1,&BmpDC,0,0,100,100,SRCCOPY);
			}
			else if(m_Board[i+1][j+1].nStatus==3)//待定雷
			{
				BmpDC.SelectObject(&m_bmpPend);
				memDC.StretchBlt(point.x,point.y,m_nSideLength-1,m_nSideLength-1,&BmpDC,0,0,100,100,SRCCOPY);
			}
			else if(m_Board[i+1][j+1].nStatus==4)//赢了，将它们都显示出来
			{
				BmpDC.SelectObject(&m_bmpMine);
				memDC.StretchBlt(point.x,point.y,m_nSideLength-1,m_nSideLength-1,&BmpDC,0,0,100,100,SRCCOPY);
			}
			//if(m_Board[i+1][j+1].nValue==9)
			//	memDC.FillRect(CRect(point,CSize(m_nSideLength-1,m_nSideLength-1)),&brRed);
			//str.Format(_T("%d"),m_Board[i+1][j+1].nValue);
			//memDC.TextOutW(point.x,point.y,str);
		}
	}
	
	dc.BitBlt(0,0,rectClient.Width(),rectClient.Height(),&memDC,0,0,SRCCOPY);
	
	BmpDC.SelectObject(pOldBitmap);
	memDC.SelectObject(pMemBmp);
	// Do not call CWnd::OnPaint() for painting messages
}
BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return true;
	return CWnd::OnEraseBkgnd(pDC);
}
void CChildView::InitBoard()
{
	srand((unsigned int)time(NULL));
	
	int nCount=0;
	int n;
	int r,c;
	
	for(int i=1;i<=m_nRow;i++)//初始化
	{
		for(int j=1;j<=m_nCol;j++)
		{
			m_Board[i][j].nValue=0;
			m_Board[i][j].nStatus=0;
		}
	}
	
	for(int i=0;i<m_nMines;i++)//生成地雷
	{
		do{
			n=int((double)rand()/RAND_MAX*m_nRow*m_nCol);
			r=n/m_nCol;
			c=n-r*m_nCol;
			r++;c++;
		}while(m_Board[r][c].nValue==9);
		m_Board[r][c].nValue=9;
	}
	
	for(int i=1;i<=m_nRow;i++)//计算其他格子的值
	{
		for(int j=1;j<=m_nCol;j++)
		{
			if(m_Board[i][j].nValue==9) continue;
			n=0;
			if(m_Board[i+1][j].nValue==9) n++;//东
			if(m_Board[i+1][j+1].nValue==9) n++;//东南
			if(m_Board[i][j+1].nValue==9) n++;//南
			if(m_Board[i-1][j+1].nValue==9) n++;//西南
			if(m_Board[i-1][j].nValue==9) n++;//西
			if(m_Board[i-1][j-1].nValue==9) n++;//西北
			if(m_Board[i][j-1].nValue==9) n++;//北
			if(m_Board[i+1][j-1].nValue==9) n++;//东北
			m_Board[i][j].nValue=n;
		}
	}
	
	for(int i=0;i<=m_nRow+1;i++)
	{
		for(int j=0;j<=m_nCol+1;j++)
		{
			TRACE("%-3d",m_Board[i][j]);
		}
		TRACE("\n");
	}
}
void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetEvent(g_hEvent);
	g_bLBtnDown=TRUE;
	g_point=point;
	return;
	CWnd::OnLButtonDown(nFlags, point);
}
void CChildView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetEvent(g_hEvent);
	g_bRBtnDown=TRUE;
	g_point=point;
	//OnUMRButtonDown(0,0);
	return;
	CWnd::OnRButtonDown(nFlags, point);
}

void CChildView::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//将双击当成单击处理
	SetEvent(g_hEvent);
	g_bRBtnDown=TRUE;
	g_point=point;
	//OnUMRButtonDown(0,0);
	return;
	CWnd::OnRButtonDblClk(nFlags, point);
}

LRESULT CChildView::OnUMLButtonDown(WPARAM wParam, LPARAM lParam)
{
	if(m_bGameOver) return 0;
	
	CPoint& point=g_point;
	if(point.x<m_ptTopleft.x||point.y<m_ptTopleft.y)
		return 0;
	
	if(!m_bTimerStarted)//启动计时器
	{
		SetTimer(1,1000,NULL);
		m_bTimerStarted=TRUE;
	}
	
	int r=(point.y-m_ptTopleft.y)/m_nSideLength;
	int c=(point.x-m_ptTopleft.x)/m_nSideLength;
	r++;c++;
	if(r>m_nRow||c>m_nCol) return 0;
	TRACE("OnLButtonUp: (%d,%d)\n",r,c);
	
	if(m_Board[r][c].nStatus!=0)//不是隐藏的格子
		return 0;
	
	m_Board[r][c].nStatus=1;
	if(m_Board[r][c].nValue==9)//踩中地雷
	{
		g_bGoOn=TRUE;
		g_nMessage=2;
		
		for(int i=1;i<=m_nRow;i++)//将所有的雷都显示出来
		{
			for(int j=1;j<=m_nCol;j++)
			{
				if(m_Board[i][j].nValue==9)
					m_Board[i][j].nStatus=1;
			}
		}
		
		Invalidate();
		return 0;
	}
	else if(m_Board[r][c].nValue==0)//踩中空地
	{
		DealWithEmptyCell(r,c);
	}
	
	g_bGoOn=TRUE;
	g_nMessage=1;
	
	if(IsGameOver())//如果赢了，将所有的地雷标示出来
	{
		for(int i=1;i<=m_nRow;i++)//将所有的雷都显示出来
		{
			for(int j=1;j<=m_nCol;j++)
			{
				if(m_Board[i][j].nValue==9)
					m_Board[i][j].nStatus=4;
			}
		}
	}
	Invalidate();
	
	return 0;
}
LRESULT CChildView::OnUMRButtonDown(WPARAM wParam, LPARAM lParam)
{
	if(m_bGameOver) return 0;
	
	CPoint& point=g_point;
	if(point.x<m_ptTopleft.x||point.y<m_ptTopleft.y)
		return 0;
	
	if(!m_bTimerStarted)//启动计时器
	{
		SetTimer(1,1000,NULL);
		m_bTimerStarted=TRUE;
	}
	
	int r=(point.y-m_ptTopleft.y)/m_nSideLength;
	int c=(point.x-m_ptTopleft.x)/m_nSideLength;
	r++;c++;
	if(r>m_nRow||c>m_nCol) return 0;
	TRACE("OnRButtonDown: (%d,%d)\n",r,c);
	
	if(m_Board[r][c].nStatus==1)//不对已翻开的格子进行操作
		return 0;
	else if(m_Board[r][c].nStatus==0)
	{
		m_Board[r][c].nStatus=2;
		m_nRemainMines--;
		UpdateStatusBar();
	}
	else if(m_Board[r][c].nStatus==2)
	{
		m_Board[r][c].nStatus=3;
		m_nRemainMines++;
		UpdateStatusBar();
	}
	else if(m_Board[r][c].nStatus==3)
		m_Board[r][c].nStatus=0;
	Invalidate();
	return 0;
}
LRESULT CChildView::OnUMLRButtonDown(WPARAM wParam, LPARAM lParam)
{
	if(m_bGameOver) return 0;
	
	CPoint& point=g_point;
	if(point.x<m_ptTopleft.x||point.y<m_ptTopleft.y)
		return 0;
	
	if(!m_bTimerStarted)//启动计时器
	{
		SetTimer(1,1000,NULL);
		m_bTimerStarted=TRUE;
	}
	
	int r=(point.y-m_ptTopleft.y)/m_nSideLength;
	int c=(point.x-m_ptTopleft.x)/m_nSideLength;
	r++;c++;
	if(r>m_nRow||c>m_nCol) return 0;
	TRACE("OnLRButtonUp: (%d,%d)\n",r,c);
	if(m_Board[r][c].nValue>0&&m_Board[r][c].nValue<9&&m_Board[r][c].nStatus==1)
	{
		int i=r;
		int j=c;
		int n=0,m=0;
		i=r+1;j=c;//东
		if(m_Board[i][j].nValue==9&&m_Board[i][j].nStatus==0) n++;
		if(m_Board[i][j].nStatus==2) m++;
		i=r+1;j=c+1;//东南
		if(m_Board[i][j].nValue==9&&m_Board[i][j].nStatus==0) n++;
		if(m_Board[i][j].nStatus==2) m++;
		i=r;j=c+1;//南
		if(m_Board[i][j].nValue==9&&m_Board[i][j].nStatus==0) n++;
		if(m_Board[i][j].nStatus==2) m++;
		i=r-1;j=c+1;//西南
		if(m_Board[i][j].nValue==9&&m_Board[i][j].nStatus==0) n++;
		if(m_Board[i][j].nStatus==2) m++;
		i=r-1;j=c;//西
		if(m_Board[i][j].nValue==9&&m_Board[i][j].nStatus==0) n++;
		if(m_Board[i][j].nStatus==2) m++;
		i=r-1;j=c-1;//西北
		if(m_Board[i][j].nValue==9&&m_Board[i][j].nStatus==0) n++;
		if(m_Board[i][j].nStatus==2) m++;
		i=r;j=c-1;//北
		if(m_Board[i][j].nValue==9&&m_Board[i][j].nStatus==0) n++;
		if(m_Board[i][j].nStatus==2) m++;
		i=r+1;j=c-1;//东北
		if(m_Board[i][j].nValue==9&&m_Board[i][j].nStatus==0) n++;
		if(m_Board[i][j].nStatus==2) m++;
		
		if(m==0)//周围没有一个标识为地雷，不翻开
			return 0;
		else if(n!=0)//踩到雷了
		{
			g_bGoOn=TRUE;
			g_nMessage=2;

			for(int i=1;i<=m_nRow;i++)//将所有的雷都显示出来
			{
				for(int j=1;j<=m_nCol;j++)
				{
					if(m_Board[i][j].nValue==9)
						m_Board[i][j].nStatus=1;
				}
			}
			Invalidate();
			return 0;
		}
		else//将周围8个都显示出来
		{
			int i=r;
			int j=c;
			i=r+1;j=c;//东
			if(m_Board[i][j].nStatus==0) m_Board[i][j].nStatus=1;
			if(m_Board[i][j].nValue==0) DealWithEmptyCell(i,j);
			i=r+1;j=c+1;//东南
			if(m_Board[i][j].nStatus==0) m_Board[i][j].nStatus=1;
			if(m_Board[i][j].nValue==0) DealWithEmptyCell(i,j);
			i=r;j=c+1;//南
			if(m_Board[i][j].nStatus==0) m_Board[i][j].nStatus=1;
			if(m_Board[i][j].nValue==0) DealWithEmptyCell(i,j);
			i=r-1;j=c+1;//西南
			if(m_Board[i][j].nStatus==0) m_Board[i][j].nStatus=1;
			if(m_Board[i][j].nValue==0) DealWithEmptyCell(i,j);
			i=r-1;j=c;//西
			if(m_Board[i][j].nStatus==0) m_Board[i][j].nStatus=1;
			if(m_Board[i][j].nValue==0) DealWithEmptyCell(i,j);
			i=r-1;j=c-1;//西北
			if(m_Board[i][j].nStatus==0) m_Board[i][j].nStatus=1;
			if(m_Board[i][j].nValue==0) DealWithEmptyCell(i,j);
			i=r;j=c-1;//北
			if(m_Board[i][j].nStatus==0) m_Board[i][j].nStatus=1;
			if(m_Board[i][j].nValue==0) DealWithEmptyCell(i,j);
			i=r+1;j=c-1;//东北
			if(m_Board[i][j].nStatus==0) m_Board[i][j].nStatus=1;
			if(m_Board[i][j].nValue==0) DealWithEmptyCell(i,j);
			
			g_bGoOn=TRUE;
			g_nMessage=1;
			
			if(IsGameOver())
			{
				for(int i=1;i<=m_nRow;i++)//将所有的雷都显示出来
				{
					for(int j=1;j<=m_nCol;j++)
					{
						if(m_Board[i][j].nValue==9)
							m_Board[i][j].nStatus=4;
					}
				}
			}
			
			Invalidate();
			return 0;
		}
	}	
	return 0;
}
void CChildView::DealWithEmptyCell(int r, int c)
{
	int i,j;
	
	i=r+1;j=c;//东
	DealWithEmptyCellAssistant(i,j);
	i=r+1;j=c+1;//东南
	DealWithEmptyCellAssistant(i,j);
	i=r;j=c+1;//南
	DealWithEmptyCellAssistant(i,j);
	i=r-1;j=c+1;//西南
	DealWithEmptyCellAssistant(i,j);
	i=r-1;j=c;//西
	DealWithEmptyCellAssistant(i,j);
	i=r-1;j=c-1;//西北
	DealWithEmptyCellAssistant(i,j);
	i=r;j=c-1;//北
	DealWithEmptyCellAssistant(i,j);
	i=r+1;j=c-1;//东北
	DealWithEmptyCellAssistant(i,j);
}
void CChildView::DealWithEmptyCellAssistant(int r, int c)
{
	//不清除玩家对雷的错误标识（即空格或数字格标为雷）
	if(m_Board[r][c].nValue==-1)
		return;
	else if(m_Board[r][c].nValue==0&&m_Board[r][c].nStatus==0)//隐藏的空格
	{
		m_Board[r][c].nStatus=1;
		DealWithEmptyCell(r,c);
	}
	else if(m_Board[r][c].nValue>0&&m_Board[r][c].nValue<9&&m_Board[r][c].nStatus==0)//隐藏的数字格子
	{
		m_Board[r][c].nStatus=1;
	}
	//由于都是通过空格周围的格子，所以不可能探索到雷
}
BOOL CChildView::IsGameOver()
{
	//所有的非雷格都被翻开，扫雷成功
	int i,j;
	for(i=1;i<=m_nRow;i++)
	{
		for(j=1;j<=m_nCol;j++)
		{
			if(m_Board[i][j].nValue!=9&&m_Board[i][j].nStatus==0)
				return FALSE;
		}
	}
	return TRUE;
}
LRESULT CChildView::OnGameOver(WPARAM wParam, LPARAM lParam)
{
	if(IsGameOver())
	{//赢 了
		KillTimer(1);
		m_bTimerStarted=FALSE;
		m_bGameOver=TRUE;
		MessageBox(_T("成功"));
	}
	return 0;
}
LRESULT CChildView::OnTachMine(WPARAM wParam, LPARAM lParam)
{
	m_bGameOver=TRUE;
	KillTimer(1);
	m_bTimerStarted=FALSE;
	MessageBox(_T("抱歉，你输了"));
	return 0;
}
void CChildView::OnNewgame()
{
	// TODO: Add your command handler code here
	InitBoard();
	m_bGameOver=FALSE;
	m_bTimerStarted=FALSE;
	KillTimer(1);
	m_nRemainMines=m_nMines;
	m_nTimer=0;
	UpdateStatusBar();
	Invalidate();
}

void CChildView::OnGamesetting()
{
	CGameSettingDlg dlg(AfxGetApp()->m_pMainWnd);
	dlg.m_nRow=m_nRow;
	dlg.m_nCol=m_nCol;
	dlg.m_nMines=m_nMines;
	if(dlg.DoModal()==IDOK)
	{
		//计算窗口大小并更改
		int x=(dlg.m_nCol-m_nCol)*m_nSideLength;
		int y=(dlg.m_nRow-m_nRow)*m_nSideLength;
		WINDOWPLACEMENT wp;
		AfxGetApp()->m_pMainWnd->GetWindowPlacement(&wp);
		wp.rcNormalPosition.right+=x;
		wp.rcNormalPosition.bottom+=y;
		AfxGetApp()->m_pMainWnd->SetWindowPlacement(&wp);
		//----------------------
		
		m_nRow=dlg.m_nRow;
		m_nCol=dlg.m_nCol;
		m_nMines=dlg.m_nMines;
		
		
		
		OnNewgame();
		return;
	}
	Invalidate();
}

void CChildView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	m_nTimer++;
	UpdateStatusBar();
	CWnd::OnTimer(nIDEvent);
}
void CChildView::UpdateStatusBar()
{
	CMainFrame* pMainFrame=(CMainFrame*)AfxGetApp()->m_pMainWnd;
	//更新时间
	CString strTime(_T("已用时间: "));
	CString str;
	int h=m_nTimer/3600;
	int m=m_nTimer/60-h*60;
	int s=m_nTimer%60;
	if(h<10) str.Format(_T("0%d:"),h);
	else str.Format(_T("%d:"),h);
	strTime+=str;
	if(m<10) str.Format(_T("0%d:"),m);
	else str.Format(_T("%d:"),m);
	strTime+=str;
	if(s<10) str.Format(_T("0%d"),s);
	else str.Format(_T("%d"),s);
	strTime+=str;
	pMainFrame->m_wndStatusBar.SetPaneText(0,strTime);
	//-----------------------------------------------------
	
	//更新剩余地雷数
	str.Format(_T("剩余地雷数：%d"),m_nRemainMines);
	pMainFrame->m_wndStatusBar.SetPaneText(1,str);
	//-----------------------------------------------------
}
void CChildView::OnResumegame()
{
	for(int i=0;i<=m_nRow+1;i++)
	{
		for(int j=0;j<=m_nCol+1;j++)
		{
			m_Board[i][j].nStatus=0;
		}
	}
	m_bGameOver=FALSE;
	m_bTimerStarted=FALSE;
	KillTimer(1);
	m_nRemainMines=m_nMines;
	m_nTimer=0;
	UpdateStatusBar();
	Invalidate();
}


