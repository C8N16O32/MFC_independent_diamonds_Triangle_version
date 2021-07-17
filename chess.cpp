#include "stdafx.h"

#ifndef ���restart
#define �� 0
#define �� 1
#define ��ѡ�� 2
#define ��ť�� 0
#define ��ť�� 1
#define ���� 0
#define ������ 1
#define ���� 2
#define ͨ�� 3
#define ���restart -1
#define ���undo -2
#define ���redo -3
#endif
#ifdef CHESS
class CHESS {
private:
	int i, j, k;
public:
	int undo, redo, restart, state;
	int ymax, size, *chess = NULL;
	int *pa = NULL, *pb = NULL, plen, plenmax;
	int input = 0;

	void cnew(int size); void cdelete(); void creset();//����ռ�

	void fdir(int in, int*outx, int*outy, int*isexist);//��Ϸ
	void ford(int *ord, int x, int y, int*isexist);
	void fmid(int a, int b, int *mid);
	void fchoose(int input, int*o);
	void fmain();

	void pnormal(int a, int b); void pundo(); void predo();//���ع���

	void bundo(); void bredo(); void brestart();//��ť
};
#endif

//����ռ䡢�ͷſռ䡢��ʼ�������ϲ����Ϊ�˿ɶ��Բ�ʹ�ù��������������
void CHESS::cnew(int Y) {
	ymax = Y; size = 0;
	for (i = 1; i <= ymax; i++)size += i;
	chess = new int[size];
	pa = new int[size]; pb = new int[size];
	return;
}
void CHESS::cdelete() {
	delete[]chess; delete[]pa; delete[]pb; return;
}
void CHESS::creset() {
	state = ����;
	undo = ��ť��; redo = ��ť��; restart = ��ť��;
	plen = 0; plenmax = plen;
	for (i = 0; i < size; i++) {
		chess[i] = ��;
		pa[i] = 0; pb[i] = 0;
	}
	return;
}

//��Ϸ���Ĺ���
void CHESS::fdir(int in, int*outx, int*outy, int*isexist) {
	*isexist = 1;
	int s = 1, x, y = 1, s2 = 2;
	while (in >= s2) { s += y; y++; s2 += y; }
	x = in - s + 1;
	if (y<1 || y > ymax) { *isexist = 0; return; }
	*outy = y; *outx = x; return;
}
void CHESS::ford(int *ord, int x, int y, int*isexist) {
	*isexist = 1;
	if (x<1 || x>y) {
		*isexist = 0; return;
	}
	int s = 0;
	for (i = 1; i < y; i++) s += i;
	s += x;
	*ord = s; 
	return;
}
void CHESS::fchoose(int input, int*o) {
	int ord, x, y, f;
	for (i = 0; i < 6; i++) o[i] = 0;
	fdir(input, &x, &y, &f);
	ford(&ord, x - 2, y, &f); if (f)o[0] = ord;
	ford(&ord, x + 2, y, &f); if (f)o[1] = ord;
	ford(&ord, x, y - 2, &f); if (f)o[2] = ord;
	ford(&ord, x, y + 2, &f); if (f)o[3] = ord;
	ford(&ord, x - 2, y - 2, &f); if (f)o[4] = ord;
	ford(&ord, x + 2, y + 2, &f); if (f)o[5] = ord;
	return;
}
void CHESS::fmid(int a, int b, int * mid) {
	int x, xc, y, yc, f;
	fdir(a, &xc, &yc, &f);
	fdir(b, &x, &y, &f);
	ford(mid, (xc + x) / 2, (yc + y) / 2, &f);
	return;
}
void CHESS::fmain() {
	static int c1 = 0, o[6];
	int inp = input; input = 0;
	if (inp == 0)return;
	switch (inp)
	{
	case ���undo:if (c1 != 0) { chess[c1 - 1] = ��; c1 = 0; };
				bundo(); return;
	case ���redo:if (c1 != 0) { chess[c1 - 1] = ��; c1 = 0; }
				bredo(); return;
	case ���restart:if (c1 != 0) { chess[c1 - 1] = ��; c1 = 0; }
				   brestart(); return;
	default:break;
	}
	if (state == ����) {
		chess[inp-1] = ��;
		pnormal(0, c1);
		restart = ��ť��;
		undo = ��ť��;//��һ�����ܳ���
		state = ������;
		return;
	}
	if (state == ���� || state == ͨ��)return;

	//����1
	if (c1 == 0 && chess[inp - 1] == ��) {
		c1 = inp;
		chess[c1 - 1] = ��ѡ��;
		fchoose(c1, o);
		return;
	}
	if (c1 == 0)return;
	//����2
	if (chess[inp - 1] == ��|| chess[inp - 1] == ��ѡ��) {
		chess[c1 - 1] = ��; c1 = 0; return;
	}
	int mid;
	if (chess[inp - 1] == ��)
		for (i = 0; i < 6; i++)
			if (inp == o[i]) {
				fmid(c1, inp, &mid);
				if (chess[mid - 1] == ��) {
					chess[c1 - 1] = ��; c1 = 0; return;
				}
				pnormal(c1, inp);
				chess[inp - 1] = ��;
				chess[mid - 1] = ��;
				chess[c1 - 1] = ��;
				c1 = 0; return;
			}
}

//�ؿ������ء�����
void CHESS::pnormal(int a, int b) {
	pa[plen] = a; pb[plen] = b;
	plen++; plenmax = plen;
	undo = ��ť��; redo = ��ť��; return;
}
void CHESS::pundo() {
	plen--;
	state = ������;
	int mid; fmid(pa[plen], pb[plen], &mid);
	chess[pb[plen] - 1] = ��; chess[mid - 1] = ��; chess[pa[plen] - 1] = ��;
	if (plen == 1)undo = ��ť��;
	redo = ��ť��; return;
}
void CHESS::predo() {
	int mid; fmid(pa[plen], pb[plen], &mid);
	chess[pb[plen] - 1] = ��; chess[mid - 1] = ��; chess[pa[plen] - 1] = ��;
	plen++;
	if (plen == plenmax)redo = ��ť��;
	undo = ��ť��; return;
}
void CHESS::bundo() {
	if (undo == ��ť��)return;
	pundo(); return;
}
void CHESS::bredo() {
	if (redo == ��ť��)return;
	predo(); return;
}
void CHESS::brestart() {
	if (restart == ��ť��)return;
	creset(); return;
}
