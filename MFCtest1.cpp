#include "stdafx.h"

//程序框架
class CMyFrameWnd : public CFrameWnd {
	DECLARE_MESSAGE_MAP()
public:
	int OnCreate(LPCREATESTRUCT p);
	void OnMouseMove(UINT nKey, CPoint pt);
	void OnLButtonUp(UINT nKey, CPoint pt);
	void OnLButtonDown(UINT nKey, CPoint pt);
	void OnTimer(UINT_PTR ptr);
	void OnSize(UINT uint, int x, int y);
	void OnPaint();
	void doublebuffer(CDC *pDC);
	UINT nKeyold; CPoint ptold;
	CRect rlimit = { 0,0,1280,720 };
	time_t timenow, timeold;
};

//消息反射宏表
BEGIN_MESSAGE_MAP(CMyFrameWnd,CFrameWnd)
ON_WM_CREATE()
ON_WM_MOUSEMOVE()
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONUP()
ON_WM_TIMER()
ON_WM_SIZE()
ON_WM_PAINT()
END_MESSAGE_MAP()

//消息反射宏表对应的处理函数
int CMyFrameWnd::OnCreate(LPCREATESTRUCT p) {
	SetTimer(1, 13, NULL);
	timenow=clock();
	timeold = timenow;
	return CFrameWnd::OnCreate(p);
}
void CMyFrameWnd::OnMouseMove(UINT nKey, CPoint pt) {
	ptold = pt;
	return CFrameWnd::OnMouseMove(nKey, pt);
}
void CMyFrameWnd::OnLButtonUp(UINT nKey, CPoint pt){
	nKeyold = nKey;
	::InvalidateRect(this->m_hWnd, &rlimit, FALSE);
	return CFrameWnd::OnLButtonUp(nKey, pt);
}
void CMyFrameWnd::OnLButtonDown(UINT nKey, CPoint pt) {
	nKeyold = nKey;
	::InvalidateRect(this->m_hWnd, &rlimit, FALSE);
	return CFrameWnd::OnLButtonDown(nKey, pt);
}
void CMyFrameWnd::OnTimer(UINT_PTR ptr) {
	::InvalidateRect(this->m_hWnd, &rlimit, FALSE);
	return CFrameWnd::OnTimer(ptr);
}
void CMyFrameWnd::OnSize(UINT uint, int x, int y) {
	rlimit.right = x; rlimit.bottom = y;
	::InvalidateRect(this->m_hWnd, &rlimit, FALSE);
	return CFrameWnd::OnSize(uint, x, y);
}


//背景
void mutidotexample(CDC *pDC, CPoint pt, char*c, int clen, CRect rlimit, int ot, int num = 30, int pms = 700000, int speed = 1) {
	struct DEST { double x, y; int t, k, v; }p0 = { 0,0,-1,0,0 };
	DEST static *p = NULL; static int size = 0;
	CRect rboard0 = { -50,-50,50,50 }, rboard = rlimit + rboard0;//区域大小
	int i, scale = 20;//文本大小
	//new
	if (p == NULL) {
		size = num; p = new DEST[size]; 
		for (i = 0; i < size; i++) {
			p[i].x = rand() % (rboard.Width()) + rboard.left;
			p[i].y = rand() % (rboard.Height()) + rboard.top;
			p[i].t = pms;
			p[i].v = (rand() % 4 + 3) * scale;
			p[i].k = rand() % 64 + 192;
		}
	}
	//fun
	switch (2) {
	case 1://扩散效果
		for (i = 0; i < size; i++) {
			p[i].x += (-1 * ot)*(pt.x - p[i].x) / (10000.0 / speed);
			p[i].y += (-1 * ot)*(pt.y - p[i].y) / (10000.0 / speed);

		}
		break;
	case 2://上升效果
		for (i = 0; i < size; i++) {
			p[i].y += (-1 * ot)*(p[i].v) / (1000.0 / speed);
			p[i].t += (-1 * ot);
		}
		break;
	}
	//check
	for (i = 0; i < size; i++) {
		p[i].t += (-1 * ot);
		if (p[i].y > rboard.bottom || p[i].y < rboard.top - p[i].v)p[i].t = -1;
		if (p[i].x > rboard.right || p[i].x < rboard.left - p[i].v)p[i].t = -1; 
		if (p[i].t <= 0) {
			p[i].x = rand() % (rboard.Width()) + rboard.left;
			p[i].y = rboard.bottom;
			p[i].t = pms;
			p[i].v = (rand() % 4 + 3) * scale;
			p[i].k = rand() % 64 + 192;
		}
	}
	//绘制
	CFont cfbg;//字体
	pDC->SetBkMode(TRANSPARENT);//透明背景色
	for (i = 0; i < size; i++) {
		cfbg.CreatePointFont(6*p[i].v, (LPCTSTR)"黑体");//字体大小与格式
		pDC->SelectObject(cfbg);
		pDC->SetTextColor(RGB(p[i].k, 102, 170));//字体颜色
		switch (2) {
		case 1:
			pDC->TextOut((int)p[i].x, (int)p[i].y, c);
			break;
		case 2:
			pDC->TextOut((int)(p[i].x + (pt.x - (rlimit.right + rlimit.left) / 2)*rboard0.right * 2 / rlimit.right)
				, (int)(p[i].y + (pt.y - (rlimit.bottom + rlimit.top) / 2)*rboard0.bottom * 2 / rlimit.bottom)
				, c);
			break;
		}
		cfbg.DeleteObject();//释放字体
	}
	return;
}

//棋盘
class CHESSMFC :public CHESS {
private:
	int i, j, k;
public:
	CRect 主界面, 棋盘, 按钮区, 文本区;
	CRect *各棋子, *各按钮;
	int 按钮数量;

	void cnew(int Y); void cdelete(); void creset();

	void dset(CRect rlimit); //更新CRect(更新条件：窗口大小改变)

	void inputcheck(CPoint pt);//检查鼠标输入(检查条件：鼠标点击)

	void dc(CDC *pDC); 
	void dbutton(CDC *pDC); void dbutton_text(CDC *pDC, int ord, COLORREF textrgb, char*text); 
	void dp(CDC *pDC);//绘图(绘图条件：图像发生变化)
	//void dhighlight(CDC *pDC);
	void dmain(CDC *pDC);
};
void CHESSMFC::cnew(int Y){
	this->CHESS::cnew(Y);
	按钮数量 = 3;
	各棋子 = new CRect[size]; 各按钮 = new CRect[按钮数量];
	return;
}
void CHESSMFC::cdelete(){
	delete[]各棋子; delete[]各按钮;
	return CHESS::cdelete();
}
void CHESSMFC::creset(){
	return CHESS::creset();
}
//确定区域
void CHESSMFC::dset(CRect rlimit){
	//界面设计1.0
	主界面 = { 0,0,1469,827 };
	棋盘 = { 656,120,1338,708 };
	按钮区 = { 133,103,545,203 };
	文本区 = { 133,295,545,708 };
	//缩放系数
	CSize r1, r2; r1 = rlimit.Size(); r2 = 主界面.Size();
	if (r1.cx < 640 || r1.cy < 480)r1.SetSize(640, 480);
	double fx = r1.cx*1.0 / r2.cx, fy = r1.cy*1.0 / r2.cy;
	if (fx > fy) { r1.cx = r1.cy; r2.cx = r2.cy; }
	//缩放
	主界面 = 主界面.MulDiv(r1.cx, r2.cx);
	棋盘 = 棋盘.MulDiv(r1.cx, r2.cx);
	按钮区 = 按钮区.MulDiv(r1.cx, r2.cx);
	文本区 = 文本区.MulDiv(r1.cx, r2.cx);
	//各棋子
	int x, y, f, dx = 棋盘.Width() / ymax, dy = 棋盘.Height() / ymax;
	CRect 棋子; 棋子.SetRect(0, 0, dx, dy); 棋子.OffsetRect(棋盘.TopLeft()); 棋子.DeflateRect(棋子.MulDiv(1, 8).Size());
	for (i = 0; i < size; i++) {
		fdir(i + 1, &x, &y, &f);
		各棋子[i] = 棋子;
		各棋子[i].OffsetRect((int)(((ymax - y)*1.0 / 2 + x - 1)*dx), (y - 1)*dy);
	}
	//各按钮
	dx = 按钮区.Width() / 按钮数量;
	CRect 一个按钮; 一个按钮.SetRect(0, 0, dx, 按钮区.Height()); 一个按钮.OffsetRect(按钮区.TopLeft());
	for (i = 0; i < 按钮数量; i++) {
		各按钮[i] = 一个按钮;
		各按钮[i].OffsetRect(i*dx, 0);
	}
	return;
}
//确定输入
void CHESSMFC::inputcheck(CPoint pt){
	//int变量：变量名input 默认值为0，非0值代表点击了棋子或按钮
	//本函数：负责检查鼠标输入。若有有效输入则改变input的值。
	//函数fmain：负责将input的值改回0。并且负责执行对应的动作。
	//检查 各棋子的区域
	for (i = 0; i < size; i++) 
		if(各棋子[i].PtInRect(pt)){
			//TODO:更好地判断 根据像素颜色判断
			input = i + 1;
		}
	if (input != 0)return;
	//检查 各按钮的区域
	for (i = 0; i < 按钮数量; i++)
		if (各按钮[i].PtInRect(pt)) {
			input = -(i + 1);
		}
	//restart,undo,redo
	return;
}
//绘图
void CHESSMFC::dc(CDC * pDC) {
	for (i = 0; i < size; i++) {
		if (chess[i] == 空)pDC->FillSolidRect(各棋子 + i, RGB(66, 66, 66));
		if (chess[i] == 满)pDC->FillSolidRect(各棋子 + i, RGB(128, 128, 190));
		if (chess[i] == 被选中)pDC->FillSolidRect(各棋子 + i, RGB(128, 190, 128));
	}
	return;
}
void CHESSMFC::dbutton_text(CDC * pDC,int ord, COLORREF textrgb, char * text){
	CFont cfdb; cfdb.CreatePointFont(各按钮[ord].Height() * 10/4, (LPCTSTR)"黑体");
	pDC->SelectObject(cfdb); pDC->SetTextColor(textrgb);
	pDC->TextOut(各按钮[ord].CenterPoint().x - pDC->GetTextExtent(text).cx / 2
		, 各按钮[ord].CenterPoint().y - pDC->GetTextExtent(text).cy / 2, text);
	cfdb.DeleteObject();
	return;
}
void CHESSMFC::dbutton(CDC * pDC){
	int buttom[3]; buttom[0] = restart; buttom[1] = undo; buttom[2] = redo;
	char btext[][16] = { "restart","undo","redo" }; COLORREF btextrgb = RGB(197, 224, 179);
	for (i = 0; i < 按钮数量; i++){
		if (buttom[i] == 按钮亮)pDC->FillSolidRect(各按钮 + i, RGB(128, 160 + i % 3 * 5, 128));
		if (buttom[i] == 按钮灭)pDC->FillSolidRect(各按钮 + i, RGB(128 / 2, 160 / 2 + i % 3 * 5, 128 / 2));
		dbutton_text(pDC, i, btextrgb, btext[i]);
	}
	return;
}
void CHESSMFC::dp(CDC * pDC) {
	//背景
	pDC->FillSolidRect(&文本区, RGB(190, 150, 128));
	//MFC方法设置字体
	int dx = 14, dy = 30;
	CFont cfdp; 
	cfdp.CreateFont(
		dy,                       // nHeight
		dx,                        // nWidth
		0,                        // nEscapement
		0,                        // nOrientation
		FW_NORMAL,                // nWeight
		FALSE,                    // bItalic
		FALSE,                    // bUnderline
		0,                        // cStrikeOut
		ANSI_CHARSET,             // nCharSet
		OUT_DEFAULT_PRECIS,       // nOutPrecision
		CLIP_DEFAULT_PRECIS,      // nClipPrecision
		DEFAULT_QUALITY,          // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		(LPCTSTR)"黑体");                // lpszFacename

	pDC->SelectObject(cfdp);
	pDC->SetTextColor(RGB(183, 71, 42));
	pDC->SetBkMode(TRANSPARENT);
	//输出文本
	CRect 文本区0 = 文本区;
	文本区0.DeflateRect(15, 15, 15, 15);
	int x = 文本区0.left, y = 文本区0.top;
	char c[32] = ""; int clen = 0;
	for (i = 0; i < plen; i++) {
		for (j = 0; j < 32; j++)c[j] = 0;
		clen = sprintf(c, "%d->%d ", pa[i], pb[i]);
		if (x + clen*dx > 文本区0.right) {
			x = 文本区0.left; y += dy;
		}
		pDC->TextOut(x, y, c);
		x += clen*dx;
	}
	cfdp.DeleteObject();
	return;
}
void CHESSMFC::dmain(CDC * pDC){
	dc(pDC);
	dbutton(pDC);
	dp(pDC);
	return;
}

//绘图 主流程
void paintmain(CDC *pDC, CPoint pt, UINT nKeyold, CRect rlimit, int ot) {
	static CHESSMFC *pcm = NULL;
	if (pcm == NULL) { pcm = new CHESSMFC; pcm->cnew(6); pcm->creset(); }
	//背景
	char t[] = "背景", t2[] = "好！";
	int i, c = 0;
	for (i = 0; i < pcm->size; i++)if (pcm->chess[i] != 空)c++;
	if (c == 1)pcm->state = 通关;
	if (pcm->state != 通关)
		mutidotexample(pDC, pt, t, strlen(t), rlimit, ot);
	else mutidotexample(pDC, pt, t2, strlen(t2), rlimit, ot);
	//棋盘
	static int nkold = 0;
	pcm->dset(rlimit);
	if (nkold != nKeyold) {
		nkold = nKeyold;
		if (nKeyold&MK_LBUTTON)
			pcm->inputcheck(pt);
	}
	if (pcm->input)
		pcm->fmain();
	pcm->dmain(pDC);
	return;
}

//双缓冲刷新
void CMyFrameWnd::doublebuffer(CDC *pDC) {
	//定义对象
	CDC MemDC; CBitmap MemBitmap;
	//使用函数初始化1：建立与屏幕显示兼容的内存显示设备
	MemDC.CreateCompatibleDC(NULL);
	//使用函数初始化2：建立与屏幕显示兼容的位图
	MemBitmap.CreateCompatibleBitmap(pDC, rlimit.Width(), rlimit.Height());
	//将位图与内存显示设备进行链接
	CBitmap *pOldBit = MemDC.SelectObject(&MemBitmap);
	//调用功能：用背景色将位图清除干净
	MemDC.FillSolidRect(0, 0, rlimit.Width(), rlimit.Height(), RGB(196, 196, 240));
	//绘图
	timenow = clock();
	paintmain(&MemDC, ptold, nKeyold, rlimit, (int)(timenow - timeold));
	timeold = timenow;
	//位图复制：从显示设备复制到主显示设备PDC 此时刷新画面
	pDC->BitBlt(0, 0, rlimit.Width(), rlimit.Height(), &MemDC, 0, 0, SRCCOPY);
	//使用函数清理：清理两个对象
	MemBitmap.DeleteObject();
	MemDC.DeleteDC();
	return;
}
//绘图消息反射
void CMyFrameWnd::OnPaint() {
	CDC *pDC = GetDC();
	doublebuffer(pDC);
	this->ReleaseDC(pDC);
	return CFrameWnd::OnPaint();
}

//窗口类
class CMyWinApp : public CWinApp {
public: 
	CMyWinApp() {};
	virtual BOOL InitInstance() {
		CMyFrameWnd* pFrame = new CMyFrameWnd;
		pFrame->Create(NULL, (LPCTSTR)("MFC 三角形独立钻石"));
		m_pMainWnd = pFrame;
		pFrame->ShowWindow(SW_SHOW);
		pFrame->UpdateWindow();
		return TRUE; 
	}
};
//窗口对象创建
CMyWinApp theApp;
