//2020/6/7/2:45
//2020/6/7/5:00unfinished。
//2020/6/9改进。
#include<graphics.h>
#include <conio.h>
#include <stdio.h>

typedef
struct coodnode
{
	int x, y;
	coodnode* last;
}*&coodstack;
inline bool isempty(coodstack stack)
{
	return stack == nullptr;
}
inline void coodpop(coodstack top, int& x, int& y)
{
	if (top == nullptr)throw(-1);

	x = top->x, y = top->y;
	coodnode* med(top->last);
	delete top;
	top = med;
}
inline void coodpush(coodstack top, int x, int y)
{
	top = new coodnode{ x,y,top };
}

class oscillator
{
private:
	int stage;
	DWORD col;
	void nextcol(void) {
		switch (stage) {
		case 0:
			if (col == 0xffffff)col = 0xfefefe, stage = 1; else col += 0x010101; break;
		case 1:
			if (col == 0x000000)col = 0x010101, stage = 0; else col -= 0x010101; break;
		default:throw(-1);
		}
	}
public:
	oscillator(void) {
		stage = 0, col = rand() % 256 * 0x010101;
	}
	inline DWORD now(void) { return col; }
	inline DWORD operator++(void){
		nextcol();
		return col;
	}
	inline DWORD operator++(int nul){
		DWORD color(col);
		nextcol();
		return color;
	}
};


void fill(char* tar, int size, double density, unsigned rseed)
{
	if (tar == nullptr || size <= 0 || density < 0 || density>1)throw(-1);

	memset(tar, 0, size);
	int total((int)(density * size));
	if (total > size)total = size;//：：：。。。
	for (int i(0); i < total; tar[i++] = 1);

	srand(rseed);
	for (int i(0); i < size; i++)
	{
		int r(((rand() * 12321 + rand() % 32123) % (i + 1) + i + 1) % (i + 1));
		char med(tar[i]);
		tar[i] = tar[r];
		tar[r] = med;
	}
}
int acirc(char* tar, int x, int y)
{
	if (tar == nullptr || x <= 0 || y <= 0)throw(-1);

	int tot(0);

	for (int dx(0); dx < x; dx++)
		for (int dy(0); dy < y; dy++)
		{
			int sum(0);
			for (int d2x(-1); d2x <= 1; d2x++)
				for (int d2y(-1); d2y <= 1; d2y++)
				{
					sum += (tar[((x + (dx + d2x)) % x) * y + (y + dy + d2y) % y] & 1);
				}
			tar[dx * y + dy] |= ((sum > 4) << 1);
		}
	int total = x * y;
	for (int i(0); i < total; tot += ((tar[i] & 1) != ((tar[i] & 2) >> 1)), tar[i++] >>= 1);

	return tot;
}

#define width 1536
#define height 864
#define X height
#define Y width
char space[X][Y];

struct patchnode
{
	oscillator osca;
	int x,y;
	patchnode* next;
	patchnode(int px, int py):x(px),y(py),next(nullptr){}
};

//顺便实现【函数板技术。】【floodfill只有在初始化时采用得到。】
oscillator* pointerspace[X][Y]{};//nullptr初始化。

//：：：用过之后，就space不用了。
//不再追究通用性。
patchnode* getpatch(char color, int fx, int fy)
{
	if (space[fx][fy] != color)return nullptr;
	else {
		patchnode* patch = new patchnode(fx,fy);//创建patch。

		coodnode* stack(nullptr);
		coodpush(stack, fx, fy);

		int cx, cy;
		while (isempty(stack) == false)
		{
			coodpop(stack, cx, cy);
			if (space[cx][cy] == color) {
				space[cx][cy] |= (1 << 4);
				pointerspace[cx][cy] = &patch->osca;
				int xto, yto;

				xto = cx == 0 ? X - 1 : cx - 1, yto = cy;
				if (space[xto][yto] == color)coodpush(stack, xto, yto);

				xto = cx == X - 1 ? 0 : cx + 1, yto = cy;
				if (space[xto][yto] == color)coodpush(stack, xto, yto);

				xto = cx, yto = cy == 0 ? Y - 1 : cy - 1;
				if (space[xto][yto] == color)coodpush(stack, xto, yto);

				xto = cx, yto = cy == Y - 1 ? 0 : cy + 1;
				if (space[xto][yto] == color)coodpush(stack, xto, yto);
			}
		}
		return patch;
	}
}
int main()
{
	//printf("%d", GetSystemMetrics(SM_CYSCREEN));
	HWND wd=initgraph(width, height,6);
	SetWindowPos(wd, HWND_TOPMOST, 0, 0, 0, 0, 0);
	SetWindowLong(wd, GWL_STYLE, GetWindowLong(wd, GWL_STYLE) & ~WS_CAPTION);

	ShowWindow(wd, SW_MAXIMIZE);
	DWORD* p = GetImageBuffer();
	for (int i(0); i < X * Y; i++)p[i] = WHITE;

	fill((char*)space, X * Y, 0.42, 1);
	for (int i = 0; i < 64; i++)
		acirc((char*)space, X, Y);

	patchnode head(0, 0);
	patchnode* end = &head;
	for (int x(0); x < X; x++)
		for (int y = 0; y < Y; y++)
		{
			patchnode* med = getpatch(1,x, y);
			if (med != nullptr)
			{
				end->next = med;
				end = end->next;
			}
		}

	for (;;)
	{
		for(int x=0;x<X;x++)
			for (int y = 0; y < Y; y++)
			{
				if (pointerspace[x][y] != nullptr)p[x * Y + y] = pointerspace[x][y]->now();
			}
		patchnode* cur = head.next;

		while (cur != nullptr)
		{
			for (int i = 0; i < 5; i++)
				if (rand() % 2 == 0)++cur->osca;
				else break;
			++cur->osca;
			cur = cur->next;
		}
		Sleep(16);
		if (_kbhit() && _getch() =='0')break;
	}
}
