//调整一些参数。√
//登记空间也没有必要搞那么大。√
//吸引区间似乎有点小了。√
//有效空间也有一点小了。√

#include<graphics.h>
#include<conio.h>
//改动：image改为共有的静态量。
//支持多个体，涉及移动模式的改动。
//为了使代码更可读，将数组列为数字串。这不会影响执行。
//实现了登记空间。与点阵不同，这个非常不好办。使用了宽松的处理方法。//2020/6/12

//2020/6/13
//改进了驱赶逻辑和捕捉机制。driveaway变成了循环外部参数。
//调了一点参数，使控制更细腻了。在感应上引入了梯度。
//俭省了多余的登记空间。
#define up 0
#define down 1
#define left 2
#define right 3
#define upright 4
#define upleft 5
#define downright 6
#define downleft 7
#define still 8
char images_move[8][4][5][5];//有32种imagestate。
char image_still[5][5];//既然类的语法要求那么混账，就定义在类外边好了。
void imageinit(void)
{
	static const char upimg[4][5][5] = {
	0,1,1,1,0,0,1,0,0,1,0,1,0,0,0,0,1,0,0,0,0,0,1,0,1,0,0,1,0,0,0,1,1,1,0,1,1,0,1,0,1,1,1,0,0,0,1,1,0,0,0,1,1,1,0,1,0,0,1,0,0,0,0,1,0,0,0,0,1,0,1,0,1,0,0,0,0,1,0,0,0,1,1,1,0,0,1,0,1,1,0,0,1,1,1,0,0,1,1,0
	};
	static const char uprightimg[4][5][5] = {
	0,0,0,0,0,0,1,1,1,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,1,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,1,0,0,0,0,0,0,0,0
	};
	static const char stillimg[5][5] = {
	0,0,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0
	};
	//...利用对称性进行初始化。
	//y==列标，x==行标。
	//：
	for (int i = 0; i < 4; i++)
		for (int y = 0; y < 5; y++)
			for (int x = 0; x < 5; x++)
				images_move[up][i][y][x] = upimg[i][y][x];
	for (int i = 0; i < 4; i++)
		for (int y = 0; y < 5; y++)
			for (int x = 0; x < 5; x++)
				images_move[down][i][y][x] = images_move[up][i][4 - y][x];
	for (int i = 0; i < 4; i++)
		for (int y = 0; y < 5; y++)
			for (int x = 0; x < 5; x++)
				images_move[left][i][y][x] = images_move[up][i][x][4 - y];
	for (int i = 0; i < 4; i++)
		for (int y = 0; y < 5; y++)
			for (int x = 0; x < 5; x++)
				images_move[right][i][y][x] = images_move[left][i][y][4 - x];
	//：
	for (int i = 0; i < 4; i++)
		for (int y = 0; y < 5; y++)
			for (int x = 0; x < 5; x++)
				images_move[upright][i][y][x] = uprightimg[i][y][x];
	for (int i = 0; i < 4; i++)
		for (int y = 0; y < 5; y++)
			for (int x = 0; x < 5; x++)
				images_move[upleft][i][y][x] = images_move[upright][i][y][4 - x];
	for (int i = 0; i < 4; i++)
		for (int y = 0; y < 5; y++)
			for (int x = 0; x < 5; x++)
				images_move[downleft][i][y][x] = images_move[upleft][i][4 - y][x];
	for (int i = 0; i < 4; i++)
		for (int y = 0; y < 5; y++)
			for (int x = 0; x < 5; x++)
				images_move[downright][i][y][x] = images_move[downleft][i][y][4 - x];
	//：
	for (int y = 0; y < 5; y++)
		for (int x = 0; x < 5; x++)
			image_still[y][x] = stillimg[y][x];
}
//关键函数。
bool legaldirection(int x, int y, int direction);
void fsregister(int x, int y);
void reregister(int x, int y, int direction);
struct mover {
	int state;
	int substate;
	int centerx;
	int centery;
	void moverinit(int cx, int cy)
	{
		state = still;
		substate = 0;
		centerx = cx;
		centery = cy;
		fsregister(centerx, centery);
	}
	char(*move(int direction))[5]//返回[5][5]。
	{
		switch (direction)
		{
		case up:
			centery -= 1; break;
		case down:
			centery += 1; break;
		case left:
			centerx -= 1; break;
		case right:
			centerx += 1; break;
		case upleft:
			centerx -= 1; centery -= 1; break;
		case upright:
			centerx += 1; centery -= 1; break;
		case downleft:
			centerx -= 1; centery += 1; break;
		case downright:
			centerx += 1; centery += 1; break;
		}
		if (direction == state)
		{
			if (state == still)return image_still;
			else {
				substate = substate == 3 ? 0 : substate + 1;
				return images_move[state][substate];
			}
		}
		else {
			state = direction;
			substate = 0;
			if (state == still)return image_still;
			else return images_move[state][substate];
		}
	}
	char(*aimove(int tx, int ty, int tag = false))[5]//转化为center坐标。
	{
		int distance = (tx - centerx) * (tx - centerx) + (ty - centery) * (ty - centery);
		if (distance == 0)
		{
			return move(still);//完善了捕捉模式。
		}
		else if (distance < 25)//随机移动。
		{
			int dir = rand() % 8;
			if (legaldirection(centerx, centery, dir) == true)
				reregister(centerx, centery, dir), move(dir);
			return move(still);
		}
		else if (distance < 5000 || distance>62500)//朝向移动。
		{//分象限计算效率较高，但是这点计算量对于单点来说是微不足道的。
			if (rand() % 10 < 7) {
				int diroutcome[8];
				diroutcome[up] = (tx - centerx) * (tx - centerx) + (ty - centery + 1) * (ty - centery + 1);
				diroutcome[down] = (tx - centerx) * (tx - centerx) + (ty - centery - 1) * (ty - centery - 1);
				diroutcome[left] = (tx - centerx + 1) * (tx - centerx + 1) + (ty - centery) * (ty - centery);
				diroutcome[right] = (tx - centerx - 1) * (tx - centerx - 1) + (ty - centery) * (ty - centery);
				diroutcome[upleft] = (tx - centerx + 1) * (tx - centerx + 1) + (ty - centery + 1) * (ty - centery + 1);
				diroutcome[upright] = (tx - centerx - 1) * (tx - centerx - 1) + (ty - centery + 1) * (ty - centery + 1);
				diroutcome[downleft] = (tx - centerx + 1) * (tx - centerx + 1) + (ty - centery - 1) * (ty - centery - 1);
				diroutcome[downright] = (tx - centerx - 1) * (tx - centerx - 1) + (ty - centery - 1) * (ty - centery - 1);
				if (tag == false || distance > 62500) {
					int mindir = -1;
					int mindist = INT_MAX;
					for (int i = 0; i < 8; i++)
					{
						if (mindist > diroutcome[i])
						{
							mindist = diroutcome[i];
							mindir = i;
						}
					}
					if (legaldirection(centerx, centery, mindir) == true)
					{
						reregister(centerx, centery, mindir); return move(mindir);
					}
					else return move(still);
				}
				else {
					int maxdir = -1;
					int maxdist = INT_MIN;
					for (int i = 0; i < 8; i++)
					{
						if (maxdist < diroutcome[i])
						{
							maxdist = diroutcome[i];
							maxdir = i;
						}
					}
					if (legaldirection(centerx, centery, maxdir) == true)
					{
						reregister(centerx, centery, maxdir); return move(maxdir);
					}
					else return move(still);
				}
			}
			else {
				int dir = rand() % 8;
				if (legaldirection(centerx, centery, dir) == true)
					reregister(centerx, centery, dir), move(dir);
				return move(still);
			}
		}
		else if (distance < 15000)
		{
			int dir = rand() % 8;
			if (rand() % 10 != 0) {
				if (legaldirection(centerx, centery, dir) == true)
					reregister(centerx, centery, dir), move(dir);//翻筋斗。
				return move(still);
			}
			else {
				if (legaldirection(centerx, centery, dir) == true) {
					reregister(centerx, centery, dir);
					return move(dir);
				}
				else return move(still);
			}
		}
		else {
			if (distance<45000&&rand() % 100 < 31-(distance-15000)/1000) {//改为梯度模式更好。
				int dir = rand() % 8;
				if (legaldirection(centerx, centery, dir) == true)
					reregister(centerx, centery, dir), move(dir);
			}//添加不活跃运动。
			return move(still);
		}
	}
};

//为了省略精密估算，直接给够。
#define registerXsize 1024
#define registerYsize 1024
bool registerspace[registerYsize][registerXsize]{};
//不进行检测。需要外部检测。
void fsregister(int x, int y)
{
	x += 256;
	y += 256;
	registerspace[y][x] = true;
}
//不进行检测。
//移动和映射是可交换的。
void reregister(int x, int y, int dir)
{
	x += 256;
	y += 256;
	registerspace[y][x] = false;
	switch (dir)
	{
	case up:
		y -= 1; break;
	case down:
		y += 1; break;
	case left:
		x -= 1; break;
	case right:
		x += 1; break;
	case upleft:
		x -= 1; y -= 1; break;
	case upright:
		x += 1; y -= 1; break;
	case downleft:
		x -= 1; y += 1; break;
	case downright:
		x += 1; y += 1; break;
	}
	registerspace[y][x] = true;
}
bool legalposition(int x, int y)
{
	x += 256;
	y += 256;
	for (int dy = -4; dy <= 4; dy++)
		for (int dx = -4; dx <= 4; dx++)
			if ((dy != 0 && dx != 0) && registerspace[y + dy][x + dx] == true)
				return false;
	return true;
}
bool legaldirection(int x, int y, int dir)
{
	int vx = x, vy = y;
	switch (dir)
	{
	case up:
		vy -= 1; break;
	case down:
		vy += 1; break;
	case left:
		vx -= 1; break;
	case right:
		vx += 1; break;
	case upleft:
		vx -= 1; vy -= 1; break;
	case upright:
		vx += 1; vy -= 1; break;
	case downleft:
		vx -= 1; vy += 1; break;
	case downright:
		vx += 1; vy += 1; break;
	}
	x += 256, y += 256;
	vx += 256, vy += 256;
	for (int dy = -4; dy <= 4; dy++)
		for (int dx = -4; dx <= 4; dx++)
			if (registerspace[vy + dy][vx + dx] == true && !(vy + dy == y && vx + dx == x))//错了很久。
				return false;
	return true;
}
#define movernum 64
mover mvr[movernum];


#define width 1536
#define height 864
int main()
{
	imageinit();
	HWND wd = initgraph(width, height,6);
	SetWindowPos(wd, HWND_TOPMOST, 0, 0, 0, 0, 0);
	SetWindowLong(wd, GWL_STYLE, GetWindowLong(wd, GWL_STYLE) & ~WS_CAPTION);

	ShowWindow(wd, SW_MAXIMIZE);
	WCHAR place[64];
	DWORD(*p)[width] = (DWORD(*)[width])GetImageBuffer();
	for (int i = 0; i < movernum; i++)
	{
		for (;;) {
			int x = rand() % (width / 5);
			int y = rand() % (height / 5);
			if (legalposition(x, y)) {
				mvr[i].moverinit(x, y);
				break;
			}
		}
	}
	for (int i = 0; i < movernum; i++) {
		char(*img)[5] = mvr[i].move(still);
		if (mvr[i].centerx >= 4 && mvr[i].centery >= 4 && mvr[i].centerx <= (width / 5) - 4 && mvr[i].centery <= (height / 5) - 4)
			for (int y = 0; y < 5; y++)
				for (int x = 0; x < 5; x++)
					for (int dy = 0; dy <= 5; dy++)
						for (int dx = 0; dx <= 5; dx++)
							p[(mvr[i].centery - 2 + y) * 5 + dy][(mvr[i].centerx - 2 + x) * 5 + dx] = img[y][x] * WHITE;
	}
	int tox = 100, toy = 50;
	wsprintf(place, L"tox:%-10d  toy:%-10d",tox, toy);
	outtextxy(0, 0, place);
	int drivaaway = false;//调到外边就好了。
	for (;;)
	{
		if (MouseHit())
		{
			MOUSEMSG m = GetMouseMsg();
			tox = m.x / 5, toy = m.y / 5;
			drivaaway = (m.mkLButton==true);//左键驱赶。
			FlushMouseMsgBuffer();
		}
		for (int i = 0; i < movernum; i++) {
			int ocx = mvr[i].centerx, ocy = mvr[i].centery;
			char(*img)[5] = mvr[i].aimove(tox, toy,drivaaway);
			if (ocx >= 4 && ocy >= 4 && ocx <= (width / 5) - 4 && ocy <= (height / 5) - 4)
				for (int y = 0; y < 5; y++)
					for (int x = 0; x < 5; x++)
						for (int dy = 0; dy <= 5; dy++)
							for (int dx = 0; dx <= 5; dx++)
								p[(ocy - 2 + y) * 5 + dy][(ocx - 2 + x) * 5 + dx] = BLACK;
			if (mvr[i].centerx >= 4 && mvr[i].centery >= 4 && mvr[i].centerx <= (width / 5) - 4 && mvr[i].centery <= (height / 5) - 4)
				for (int y = 0; y < 5; y++)
					for (int x = 0; x < 5; x++)
						for (int dy = 0; dy <= 5; dy++)
							for (int dx = 0; dx <= 5; dx++)
								p[(mvr[i].centery - 2 + y) * 5 + dy][(mvr[i].centerx - 2 + x) * 5 + dx] = img[y][x] * WHITE;
		}
		wsprintf(place, L"tox:%-10d  toy:%-10d", tox, toy);
		outtextxy(0, 0, place);
		if(_kbhit()&&_getch()=='0')break;
		Sleep(20);
	}
}
