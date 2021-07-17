#pragma once

#include "afxwin.h"

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
