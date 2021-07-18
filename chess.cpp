#include "stdafx.h"

#ifndef 点击restart
#define 空 0
#define 满 1
#define 被选中 2
#define 按钮灭 0
#define 按钮亮 1
#define 开局 0
#define 进行中 1
#define 死亡 2
#define 通关 3
#define 点击restart -1
#define 点击undo -2
#define 点击redo -3
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

	void cnew(int size); void cdelete(); void creset();//分配空间

	void fdir(int in, int*outx, int*outy, int*isexist);//游戏
	void ford(int *ord, int x, int y, int*isexist);
	void fmid(int a, int b, int *mid);
	void fchoose(int input, int*o);
	void fmain();

	void pnormal(int a, int b); void pundo(); void predo();//撤回功能

	void bundo(); void bredo(); void brestart();//按钮
};
#endif

//申请空间、释放空间、初始化（是上层对象，为了可读性不使用构造和析构函数）
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
	state = 开局;
	undo = 按钮灭; redo = 按钮灭; restart = 按钮灭;
	plen = 0; plenmax = plen;
	for (i = 0; i < size; i++) {
		chess[i] = 满;
		pa[i] = 0; pb[i] = 0;
	}
	return;
}

//游戏核心功能
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
	case 点击undo:if (c1 != 0) { chess[c1 - 1] = 满; c1 = 0; };
				bundo(); return;
	case 点击redo:if (c1 != 0) { chess[c1 - 1] = 满; c1 = 0; }
				bredo(); return;
	case 点击restart:if (c1 != 0) { chess[c1 - 1] = 满; c1 = 0; }
				   brestart(); return;
	default:break;
	}
	if (state == 开局) {
		chess[inp - 1] = 空;
		pnormal(0, inp);
		restart = 按钮亮;
		undo = 按钮灭;//第一步不能撤回
		state = 进行中;
		return;
	}
	if (state == 死亡 || state == 通关)return;

	//操作1
	if (c1 == 0 && chess[inp - 1] == 满) {
		c1 = inp;
		chess[c1 - 1] = 被选中;
		fchoose(c1, o);
		return;
	}
	if (c1 == 0)return;
	//操作2
	if (chess[inp - 1] == 满 || chess[inp - 1] == 被选中) {
		chess[c1 - 1] = 满; c1 = 0; return;
	}
	int mid;
	if (chess[inp - 1] == 空)
		for (i = 0; i < 6; i++)
			if (inp == o[i]) {
				fmid(c1, inp, &mid);
				if (chess[mid - 1] == 空) {
					chess[c1 - 1] = 满; c1 = 0; return;
				}
				pnormal(c1, inp);
				chess[inp - 1] = 满;
				chess[mid - 1] = 空;
				chess[c1 - 1] = 空;
				c1 = 0; return;
			}
}

//重开、撤回、重做
void CHESS::pnormal(int a, int b) {
	pa[plen] = a; pb[plen] = b;
	plen++; plenmax = plen;
	undo = 按钮亮; redo = 按钮灭; return;
}
void CHESS::pundo() {
	plen--;
	state = 进行中;
	int mid; fmid(pa[plen], pb[plen], &mid);
	chess[pb[plen] - 1] = 空; chess[mid - 1] = 满; chess[pa[plen] - 1] = 满;
	if (plen == 1)undo = 按钮灭;
	redo = 按钮亮; return;
}
void CHESS::predo() {
	int mid; fmid(pa[plen], pb[plen], &mid);
	chess[pb[plen] - 1] = 满; chess[mid - 1] = 空; chess[pa[plen] - 1] = 空;
	plen++;
	if (plen == plenmax)redo = 按钮灭;
	undo = 按钮亮; return;
}
void CHESS::bundo() {
	if (undo == 按钮灭)return;
	pundo(); return;
}
void CHESS::bredo() {
	if (redo == 按钮灭)return;
	predo(); return;
}
void CHESS::brestart() {
	if (restart == 按钮灭)return;
	creset(); return;
}
