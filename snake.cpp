// A Snake Program

#include <graphics.h>
#include <conio.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

const int radius = 9;
const int SCREENX = 640;
const int SCREENY = 480;
const int speed = 100;
const int MINX = 2;
const int MINY = 2;
const int MAXX = 70;
const int MAXY = 50;
// const int MAXX = (SCREENX - 30 - radius)/(2*radius);
// const int MAXY = (SCREENY - 30 - radius)/(2*radius);
const COLORREF COLORS[] = {BLUE, GREEN, WHITE, YELLOW, RED};

enum SnakeType {human, AI};
enum MODE {NOFILL, FILL};
bool noborder, nobody, noself;

struct Position
{
	int x;
	int y;
};

// make a position
Position Pos(int xx, int yy)
{
	Position p;
	
	p.x = xx;
	p.y = yy;
	
	return p;
}


//

void CIRCLE(Position & p, MODE mode, Position & center)
{

	int xx = SCREENX/2 + (p.x - center.x)*2*radius;
	int yy = SCREENY/2 + (p.y - center.y)*2*radius;
	
	if ((xx <= radius) || (xx >= SCREENX - radius) ||
	    (yy <= radius) || (yy >= SCREENY - radius) )
		return;
	
	if (mode == FILL)
	{
		fillcircle(xx, yy, radius);
		
	}
	else if (mode == NOFILL)
	{
		circle(xx, yy, radius);
	}
		
}

//

void foodshow(Position & food, Position & center)
{
	setfillcolor(COLORS[0]);
	CIRCLE(food, FILL, center);
}

//


class Snake
{
	
public:
	
	//Position pos[50];
	
	vector <Position> pos; // 0 is head of snake
	
	bool IsAI;
	// bool noborder, nobody, noself;
	
	COLORREF color;

	Position direction; // Right(1,0), Up(0,-1), Left(-1,0), Down(0,1)

	bool alive;
	
	Snake(COLORREF, SnakeType);

	bool testoccupied(Position &);
	
	void show(Position & center);	
	void move(vector <Snake> & snakes, vector <Position> & foods);
	void chooseDir();
	~Snake();

};

Snake::Snake(COLORREF scolor, SnakeType s = human)
{
	alive = true;
	// noborder = nobody = noself = true;
	color = scolor;
	
	IsAI = (s == AI);

	int sx = MAXX/2;
	int sy = MAXY/2;

if (!IsAI)
{

	pos.push_back(Pos(sx,sy));
	pos.push_back(Pos(sx,sy+1));
	pos.push_back(Pos(sx,sy+2));
	pos.push_back(Pos(sx,sy+3));
	pos.push_back(Pos(sx,sy+4));

}
else
{
	pos.push_back(Pos(sx+5,sy));
	pos.push_back(Pos(sx+5,sy+1));
	pos.push_back(Pos(sx+5,sy+2));
	pos.push_back(Pos(sx+5,sy+3));
	pos.push_back(Pos(sx+5,sy+4));
	
}

	direction = Pos(0, -1);
}


Position getNextHead(Position & pos, Position & direction)
{
	Position next;
	
	next.x = pos.x + direction.x;
	next.y = pos.y + direction.y;
	
	return next;
}

bool Snake::testoccupied(Position & position)
{
	for (auto & p : pos)
	{
		if ((p.x == position.x) && (p.y == position.y)) 
		{
			return true;
		}
	}
		
	return false;
	
}

bool testifoccupied(vector <Snake> & snakes, Position & position)
{
	// whether it is occupied by one of snakes' body

	for (auto & sn : snakes)
	{
		if (sn.testoccupied(position)) return true;
	}

	return false;
		
}

bool testiftoWall(Position & next)
{
	return ((next.x < MINX) || (next.x > MAXX) || (next.y < MINY) || (next.y > MAXY));
}

void createnew(Position & food, vector <Snake> & snakes)
{
	do
	{
		food.x = rand() % (MAXX - MINX - 1) + MINX + 1;
		food.y = rand() % (MAXY - MINY - 1) + MINY + 1;
	} while (testifoccupied(snakes, food));
	
}

void Snake::move(vector <Snake> & snakes, vector <Position> & foods)
{
	Position next = getNextHead(pos[0], direction);
	
	bool intoWall = testiftoWall(next);
	bool intoBody = testifoccupied(snakes, next);
	bool intoSelf = testoccupied(next);	
	
	Position food;
	
	if ((!noborder && intoWall) || (!nobody && intoBody) || (!noself && intoSelf)) 
	{
		alive = false;
		pos.clear();
	}
	else if (!intoWall && !intoBody && !intoSelf) // 
	{			
		// push next position to the front
		pos.insert(pos.begin(), next);
		
		// pop out the position of the tail if it does not run into food
		bool runintofoods = false;
		
		for (auto & food : foods)
		{		
			if ((food.x == next.x) && (food.y == next.y)) 
			{
				createnew(food, snakes);
				foodshow(food, snakes[0].pos[0]);
				runintofoods = true;
			}
		}
			
		if (!runintofoods)	
		{
			pos.pop_back();
		}
	}
	
}

//
void Snake::chooseDir()
{
	
	// choose next direction
	Position nextDir = direction, nextHead = pos[0];
		
	if (IsAI)	{
	
		if (rand() % 100 < 30) // with a lower probability
		{
			do
			{
				int nextRand = rand() % 4;
				nextDir = Pos((nextRand == 3)?0:1 - nextRand, (nextRand == 0)?0:nextRand - 2);
				nextHead = getNextHead(pos[0], nextDir);
			} while (testiftoWall(nextHead) || ((direction.x + nextDir.x == 0) && (direction.y + nextDir.y == 0)) );

			direction = nextDir;	
		}
	}
	else
	{

  		if (GetAsyncKeyState('A') & 0x8000) nextDir = Pos(-1, 0);
        if (GetAsyncKeyState('D') & 0x8000) nextDir = Pos(1, 0);
        if (GetAsyncKeyState('W') & 0x8000) nextDir = Pos(0, -1);
        if (GetAsyncKeyState('S') & 0x8000) nextDir = Pos(0, 1);
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) exit(0);
        
		if ((direction.x + nextDir.x == 0) && (direction.y + nextDir.y == 0)) ; else direction = nextDir;

	}

}


void Snake::show(Position & center)
{

	setfillcolor(color);
		
	if (nobody)
	{
		setcolor(noself? WHITE: color);
		
		for (auto & p : pos)
			CIRCLE(p, NOFILL, center);	
	}
	else
	{
		for (auto & p : pos)
			CIRCLE(p, FILL, center);
	}

	CIRCLE(pos[0], FILL, center);		

}


Snake::~Snake()
{
	pos.clear();
}



// main function of snake.cpp
int main()
{

	int snakenum = 2, foodnum = 3;
	
	bool pause = true;	

newgame:
	
	noborder = nobody = noself = true;
	vector <Snake> snakes;
	vector <Position> foods;
	Position food;
	
	for (int i = 1; i <= snakenum; i ++)
	{
		Snake sn(COLORS[i], (i == 1)?human:AI);			
		snakes.push_back(sn);
	}
	
	
	//
	srand(time(0));
	initgraph(SCREENX, SCREENY);
	//
	
	for (int i = 1; i <= foodnum; i ++)
	{
		createnew(food, snakes);
		foods.push_back(food);
	}
	
	// while (snake1.alive)
	while (true)	
	{
		cleardevice();
		
		int xx = SCREENX/2 + (MINX - snakes[0].pos[0].x)*2*radius - 1.2*radius;
		int yy = SCREENY/2 + (MINY - snakes[0].pos[0].y)*2*radius - 1.2*radius;
		int xx2 = SCREENX/2 + (MAXX - snakes[0].pos[0].x)*2*radius + 1.35*radius;
		int yy2 = SCREENY/2 + (MAXY - snakes[0].pos[0].y)*2*radius + 1.35*radius;
	
		setfillcolor(RGB(100, 102, 102));
		fillrectangle(xx, yy, xx2, yy2);	
		if (!noborder) rectangle(xx + 0.3*radius, yy - 0.3*radius, xx2 + 0.3*radius, yy2 - 0.3*radius);

		if (!pause)
		{
	    	for (auto & sn : snakes)
			{
				if (sn.alive)
				{
					sn.show(snakes[0].pos[0]);
				}
			}

	    	for (auto & sn : snakes)
			{
				if (sn.alive)
				{
					sn.move(snakes, foods);
					sn.chooseDir();
				}
			}

			for (auto & food : foods)
			{
				foodshow(food, snakes[0].pos[0]);				
			}
		}
		else
		{
			cleardevice();
			outtextxy(200, 140, "Press P to continue");
			outtextxy(200, 170, "Press R, 1, 2, 3, 4 to restart");

			if (noborder)
				outtextxy(200, 200, "Press N : no-border (ON)");
			else
				outtextxy(200, 200, "Press N : no-border (OFF)");
				
			if (nobody)
				outtextxy(200, 230, "Press B : no-body-collision (ON)");
			else
				outtextxy(200, 230, "Press B : no-body-collision (OFF)");
				
			if (noself)
				outtextxy(200, 260, "Press M : no-self-collision (ON)");
			else
				outtextxy(200, 260, "Press M : no-self-collision (OFF)");
	
			if (GetAsyncKeyState('1') & 0x8000) { snakenum = 1; pause = false; goto newgame; }
			if (GetAsyncKeyState('2') & 0x8000) { snakenum = 2; pause = false; goto newgame; }
			if (GetAsyncKeyState('3') & 0x8000) { snakenum = 3; pause = false; goto newgame; }
			if (GetAsyncKeyState('4') & 0x8000) { snakenum = 4; pause = false; goto newgame; }
			
		}

		if (GetAsyncKeyState('P') & 0x8000) pause = !pause;
		if (GetAsyncKeyState('R') & 0x8000) goto newgame;
		if (GetAsyncKeyState('N') & 0x8000) noborder = !noborder;
		if (GetAsyncKeyState('B') & 0x8000) nobody = !nobody;
		if (nobody) if (GetAsyncKeyState('M') & 0x8000) noself = !noself;
		
		Sleep(speed);
	}
	
	//
	getch();	
	//
	closegraph();
		
	return 0;
	
}


