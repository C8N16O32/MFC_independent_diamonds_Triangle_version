#include "stdafx.h"

//������
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

//��Ϣ������
BEGIN_MESSAGE_MAP(CMyFrameWnd,CFrameWnd)
ON_WM_CREATE()
ON_WM_MOUSEMOVE()
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONUP()
ON_WM_TIMER()
ON_WM_SIZE()
ON_WM_PAINT()
END_MESSAGE_MAP()

//��Ϣ�������Ӧ�Ĵ����� ����û��ON_WM_PAINT��Ӧ����Ŀ
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


//������
void mutidotexample(HDC hdc, CPoint pt, char*c, int clen, CRect rlimit, int ot, int num = 30, int pms = 700000, int speed = 1) {
	struct DEST { double x, y;int t, k = 0, v = 0; };
	static DEST *p = NULL, p0; static int size = 0;
	static CRect rboard0 = { -50,-50,50,50 }, rboard;
	rboard = rlimit + rboard0;
	int i, scale = 20;
	//new
	if (p == NULL) { 
		p0.x = 0; p0.y = 0; p0.t = -1; p0.v = 0; p0.k = 0;
		size = num; p = new DEST[size]; 
		for (i = 0; i < size; i++) {
			p[i].x = rand() % (rboard.right - rboard.left) + rboard.left;
			p[i].y = rand() % (rboard.bottom - rboard.top) + rboard.top;
			p[i].t = pms;
			p[i].v = (rand() % 4 + 3) * scale;
			p[i].k = rand() % 64 + 192;
		}
	}
	//fun
	switch (2) {
	case 1://��ɢЧ��
		for (i = 0; i < size; i++) {
			p[i].x += (-1 * ot)*(pt.x - p[i].x) / (10000.0 / speed);
			p[i].y += (-1 * ot)*(pt.y - p[i].y) / (10000.0 / speed);

		}
		break;
	case 2://����Ч��
		for (i = 0; i < size; i++) {
			p[i].y += (-1 * ot)*(p[i].v) / (1000.0 / speed);
			p[i].t += (-1 * ot);
		}
		break;
	}
	//check
	for (i = 0; i < size; i++) {
		p[i].t += (-1 * ot);
		if (p[i].y > rboard.bottom || p[i].y < rboard.top-p[i].v)p[i].t = -1;
		if (p[i].x > rboard.right || p[i].x < rboard.left-p[i].v)p[i].t = -1;
		if (p[i].t <= 0) {
			p[i].x = rand() % (rboard.right - rboard.left) + rboard.left;
			p[i].y = rboard.bottom;
			p[i].t = pms;
			p[i].v = (rand() % 4 + 3) * scale;
			p[i].k = rand() % 64 + 192;
		}
	}
	//����
	LOGFONT logfont;
	ZeroMemory(&logfont, sizeof(LOGFONT));
	logfont.lfCharSet = GB2312_CHARSET;
	SetBkMode(hdc, TRANSPARENT);

	for (i = 0; i < size; i++){
		logfont.lfHeight = -p[i].v; //��������Ĵ�С
		HFONT hFont = CreateFontIndirect(&logfont);
		SelectObject(hdc, hFont);
		DeleteObject(hFont);
	::SetTextColor(hdc, RGB(p[i].k, 102, 170));
	switch (2) {
	case 1:
		::TextOutA(hdc, (int)p[i].x, (int)p[i].y, c, clen);
		break;
	case 2:
		::TextOutA(hdc, (int)(p[i].x + (pt.x - (rlimit.right + rlimit.left) / 2)*rboard0.right * 2 / rlimit.right)
			, (int)(p[i].y + (pt.y - (rlimit.bottom + rlimit.top) / 2)*rboard0.bottom * 2 / rlimit.bottom)
			, c, clen);
		break;
	}
	}
	return;
}

//�������ζ�����ʯ��Ϸ
class CHESSMFC :public CHESS {
private:
	int i, j, k;
public:
	CRect ������, ����, ��ť��, �ı���;
	CRect *������, *����ť;
	int ��ť����;

	void cnew(int Y); void cdelete(); void creset();

	void dsetmain(CRect rlimit); //���¸�ͼ���CRect(�������������ڴ�С�ı�)

	void inputcheck(CPoint pt);//����������(��������������)

	void dc(CDC *pDC); void dbutton(CDC *pDC); void dp(CDC *pDC);//��ͼ(��ͼ������ͼ�����仯)
	void dhighlight(CDC *pDC);
	void dmain(CDC *pDC);
	void startgame_MFC();
};
void CHESSMFC::cnew(int Y){
	this->CHESS::cnew(Y);
	��ť���� = 3;
	������ = new CRect[size]; ����ť = new CRect[��ť����];
	return;
}
void CHESSMFC::cdelete(){
	delete[]������; delete[]����ť;
	return CHESS::cdelete();
}
void CHESSMFC::creset(){
	return CHESS::creset();
}
//ȷ������
void CHESSMFC::dsetmain(CRect rlimit){
	//�������1.0
	������ = { 0,0,1469,827 };
	���� = { 656,120,1338,708 };
	��ť�� = { 133,103,545,203 };
	�ı��� = { 133,295,545,708 };
	//����ϵ��
	CSize r1, r2; r1 = rlimit.Size(); r2 = ������.Size();
	if (r1.cx < 640 || r1.cy < 480)r1.SetSize(640, 480);
	double fx = r1.cx*1.0 / r2.cx, fy = r1.cy*1.0 / r2.cy;
	if (fx > fy) { r1.cx = r1.cy; r2.cx = r2.cy; }
	//����
	������.MulDiv(r1.cx, r2.cx);
	����.MulDiv(r1.cx, r2.cx);
	��ť��.MulDiv(r1.cx, r2.cx);
	�ı���.MulDiv(r1.cx, r2.cx);
	//������
	int x, y, f, dx = ����.Width() / ymax, dy = ����.Height() / ymax;
	CRect ����; ����.SetRect(0, 0, dx, dy); ����.OffsetRect(����.TopLeft()); ����.DeflateRect(10, 10, 10, 10);
	for (i = 0; i < size; i++) {
		fdir(i + 1, &x, &y, &f);
		������[i] = ����;
		������[i].OffsetRect((int)(((ymax - y)*1.0 / 2 + x - 1)*dx), (y - 1)*dy);
	}
	//����ť
	dx = ��ť��.Width() / ��ť����;
	CRect һ����ť; һ����ť.SetRect(0, 0, dx, ��ť��.Height()); һ����ť.OffsetRect(��ť��.TopLeft());
	for (i = 0; i < ��ť����; i++) {
		����ť[i] = һ����ť;
		����ť[i].OffsetRect(i*dx, 0);
	}
	return;
}
//ȷ������
void CHESSMFC::inputcheck(CPoint pt){
	//int������������input Ĭ��ֵΪ0����0ֵ�����������ӻ�ť
	//��������������������롣������Ч������ı�input��ֵ��
	//����fmain������input��ֵ�Ļ�0�����Ҹ���ִ�ж�Ӧ�Ķ�����
	//��� �����ӵ�����
	for (i = 0; i < size; i++) 
		if(������[i].PtInRect(pt)){
			//TODO:���õ��ж� ����������ɫ�ж�
			input = i + 1;
		}
	if (input != 0)return;
	//��� ����ť������
	for (i = 0; i < ��ť����; i++)
		if (����ť[i].PtInRect(pt)) {
			input = -(i + 1);
		}
	//restart,undo,redo
	return;
}
//��ͼ
void CHESSMFC::dc(CDC * pDC) {
	for (i = 0; i < size; i++) {
		if (chess[i] == ��)pDC->FillSolidRect(������ + i, RGB(66, 66, 66));
		if (chess[i] == ��)pDC->FillSolidRect(������ + i, RGB(128, 128, 190));
		if (chess[i] == ��ѡ��)pDC->FillSolidRect(������ + i, RGB(128, 190, 128));
	}
	return;
}
void CHESSMFC::dbutton(CDC * pDC){
	int buttom[3]; buttom[0] = restart; buttom[1] = undo; buttom[2] = redo;
	for (i = 0; i < ��ť����; i++){
		if (buttom[i] == ��ť��)pDC->FillSolidRect(����ť + i, RGB(128, 160 + i % 3 * 5, 128));
		if (buttom[i] == ��ť��)pDC->FillSolidRect(����ť + i, RGB(128 / 2, 160 / 2 + i % 3 * 5, 128 / 2));
	}
	return;
}
void CHESSMFC::dp(CDC * pDC){
	pDC->FillSolidRect(&�ı���, RGB(190, 150, 128));
	return;
}
void CHESSMFC::dmain(CDC * pDC){
	dc(pDC);
	dbutton(pDC);
	dp(pDC);
	return;
}

//��ͼ ������
void paintmain(CDC *pDC, CPoint pt, UINT nKeyold, CRect rlimit, int ot) {
	static CHESSMFC *pcm = NULL;
	if (pcm == NULL) { pcm = new CHESSMFC; pcm->cnew(6); pcm->creset(); }
	//����
	char t[] = "����", t2[] = "�ã�";
	int i, c = 0;
	for (i = 0; i < pcm->size; i++)if (pcm->chess[i] != ��)c++;
	if (c == 1)pcm->state = ͨ��;
	if (pcm->state != ͨ��)
		mutidotexample(pDC->m_hDC, pt, t, strlen(t), rlimit, ot);
	else mutidotexample(pDC->m_hDC, pt, t2, strlen(t2), rlimit, ot);
	//����
	static int nkold = 0;
	pcm->dsetmain(rlimit);
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

//˫����ˢ��
void CMyFrameWnd::doublebuffer(CDC *pDC) {
	//�������
	CDC MemDC; CBitmap MemBitmap;
	//ʹ�ú�����ʼ��1����������Ļ��ʾ���ݵ��ڴ���ʾ�豸
	MemDC.CreateCompatibleDC(NULL);
	//ʹ�ú�����ʼ��2����������Ļ��ʾ���ݵ�λͼ
	int nWidth = rlimit.right - rlimit.left, nHeight = rlimit.bottom - rlimit.top;
	MemBitmap.CreateCompatibleBitmap(pDC, nWidth, nHeight);
	//��λͼ���ڴ���ʾ�豸��������
	CBitmap *pOldBit = MemDC.SelectObject(&MemBitmap);
	//���ù��ܣ��ñ���ɫ��λͼ����ɾ�
	MemDC.FillSolidRect(0, 0, nWidth, nHeight, RGB(196, 196, 240));
	//��ͼ
	timenow = clock();
	paintmain(&MemDC, ptold, nKeyold, rlimit, (int)(timenow - timeold));
	timeold = timenow;
	//λͼ���ƣ�����ʾ�豸���Ƶ�����ʾ�豸PDC ��ʱˢ�»���
	pDC->BitBlt(0, 0, nWidth, nHeight, &MemDC, 0, 0, SRCCOPY);
	//ʹ�ú�������������������
	MemBitmap.DeleteObject();
	MemDC.DeleteDC();
	return;
}
//��ͼ��Ϣ����
void CMyFrameWnd::OnPaint() {
	PAINTSTRUCT ps = { 0 };
	HDC hdc = ::BeginPaint(this->m_hWnd, &ps);
	CDC dc; dc.Attach(hdc);
	doublebuffer(&dc);
	::EndPaint(m_hWnd, &ps);
	return CFrameWnd::OnPaint();
}

//������
class CMyWinApp : public CWinApp {
public: 
	CMyWinApp() {};
	virtual BOOL InitInstance() {
		CMyFrameWnd* pFrame = new CMyFrameWnd;
		pFrame->Create(NULL, (LPCTSTR)(L"MFC �����ζ�����ʯ"));
		m_pMainWnd = pFrame;
		pFrame->ShowWindow(SW_SHOW);
		pFrame->UpdateWindow();
		return TRUE; 
	}
};
//���ڶ��󴴽�
CMyWinApp theApp;