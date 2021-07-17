#pragma once

#include "afxwin.h"

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
