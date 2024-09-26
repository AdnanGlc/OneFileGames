#include<iostream>
#include<list>
#include<Windows.h>
#include<time.h>
#include<map>
#include<thread>
#include<conio.h>
#include<vector>
#include<string>

using namespace std;

const int gameWidth = 20, gameHeight = 20;
map<char, COORD> directions;
vector<string> _gameMap;
//
HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
const char cWall = 219, cFood = 15, cBody = 178,cPlayer=2;
enum Color { BLUE = 1, GREEN, CYAN, RED, PURPLE, YELLOW, WHITE, GRAY, DARK_CYAN, LIGHT_GREEN, LIGHT_BLUE, PINK, MAGENTA, BEIGE, WHITE_N_BLACK = 240 };
const int dirX[4] = { 0,0,1,-1 };
const int dirY[4] = { -1,1,0,0 };
const char CDir[4] = { 'w','s','d','a' };
//const char CDir[4] = { 'w','s','d','a'};
//
template<class T>
void advCout(int x, int y, T msg, Color color = WHITE){
	SetConsoleTextAttribute(h, color);
	SetConsoleCursorPosition(h, { (short)x,(short)y });
	cout << msg;
}
//
class Food
{
	COORD _position;
	int _points;
public:
	Food() {
		while (true)
		{
			int tempX = rand() % (gameWidth - 1) + 1;
			int tempY = rand() % (gameHeight - 1) + 1;
			//
			if (_gameMap[tempX][tempY] == cBody || _gameMap[tempX][tempY] == cWall || _gameMap[tempX][tempY] == cFood)continue;
			_position.X = tempX;
			_position.Y = tempY;
			_points = rand() % 3 + 1;
			_gameMap[tempX][tempY] = cFood;
			advCout(tempX, tempY, cFood, Color(_points));
			break;
		}
	}
	Food(const Food& obj){
		_position = obj._position;
		_points = obj._points;
	}
	~Food() { ; }
	int getPoints() { return _points; }
	COORD getPosition() { return _position; }
	void DecreasePoints() { _points--; }
};
//
list<Food> _food;
Food FindFood(int x, int y){
	for (auto it : _food)
		if (it.getPosition().X == x && it.getPosition().Y == y)
			return it;
}
//
class Entity
{
protected:
	clock_t _lastMoved;
	float _moveSpeed;
	bool _alive;
	Color _color = WHITE;
public:
	Entity() { _lastMoved = clock(); _alive = true; }
	virtual void Move() = 0;
	~Entity() { ; }
};
class Snake : public Entity
{
private:
	list<Food> _foodToConsume;//push_front, pop_back
	list<COORD> _body;
	Color _headColor;
	unsigned int _negativePoints=0;
	void ConsumeFood()
	{
		if (_foodToConsume.size()) {
			_foodToConsume.back().DecreasePoints();
			if (_foodToConsume.back().getPoints() <= 0)
				_foodToConsume.pop_back();
		}
		else {
			advCout(_body.back().X, _body.back().Y, ' ');
			_gameMap[_body.back().X][_body.back().Y] = ' ';
			_body.pop_back();
		}
		while (_negativePoints)
		{
			advCout(_body.back().X, _body.back().Y, ' ');
			_gameMap[_body.back().X][_body.back().Y] = ' ';
			_body.pop_back();
			_negativePoints--;
		}
	}
	char FindDirection()
	{
		bool visited[gameHeight][gameWidth] = { 0 };
		list<pair<COORD, string> > paths;
		paths.push_back(make_pair(_body.front(), "x"));
		string tempPath;
		while (!paths.empty())
		{
			int x = paths.front().first.X;
			int y = paths.front().first.Y;
			tempPath = paths.front().second;
			paths.pop_front();
			if (_gameMap[x][y] == cFood || _gameMap[x][y]==cPlayer)break;
			for (size_t i = 0; i < 4; i++){
				int newX = x + dirX[i];
				int newY = y + dirY[i];
				if (_gameMap[newX][newY] != cWall && _gameMap[newX][newY] != cBody && !visited[newX][newY]){
					visited[newX][newY] = 1;
					paths.push_back(make_pair(COORD{ (short)newX,(short)newY }, tempPath + CDir[i]));
				}
			}
		}
		tempPath += 'x';
		return tempPath[1];
	}
	void ExtendBody_f(int x, int y)
	{
		if (_food.size() && _gameMap[x][y] == cFood)_foodToConsume.push_back(FindFood(x, y));
		if(_body.size())
			advCout(_body.front().X, _body.front().Y, cBody, _color);
		_body.push_front(COORD{ (short)(x),(short)(y) });
		advCout(x, y, cBody, _headColor);
		_gameMap[x][y] = cBody;
	}
public:
	Snake() : Entity() { 
		_moveSpeed = 160.0f; 
		_color = RED;
		_headColor = PINK;
		for (int i = -5; i < 0; i++)
			ExtendBody_f(gameWidth / 2 + i, gameHeight / 2);
	}
	void Move() { 
		if (float(clock() - _lastMoved) / float(CLOCKS_PER_SEC / 1000) < _moveSpeed)return;
		_lastMoved = clock();
		//bfs -> food/player
		char dir = FindDirection();
		//move
		ExtendBody_f(_body.front().X + directions[dir].X, _body.front().Y + directions[dir].Y);
		//delete tail?
		ConsumeFood();
	}
	list<COORD>& getBody() { return _body; }
	list<Food>& getSnakeFood() { return _foodToConsume; }
	unsigned int& getNegativePoints() { return _negativePoints; }
	void increseNegativePoints() { _negativePoints++; }
	~Snake() { ; }
};
class Player : public Entity
{
	COORD _position;
	float  _moveSpeed;
	//
	void AsyncMove(COORD dir,unsigned int& negativePoints)
	{
		_lastMoved = clock();
		COORD tempPos = _position;
		tempPos.X += dir.X;
		tempPos.Y += dir.Y;
		if (tempPos.X > 0 && tempPos.X < gameWidth - 1 && tempPos.Y>0 && tempPos.Y < gameHeight - 1){
			if (_gameMap[tempPos.X][tempPos.Y] == cBody) { _alive = false; return; }
			if (_gameMap[tempPos.X][tempPos.Y] == cFood)negativePoints++;
			advCout(_position.X, _position.Y, ' ');
			_gameMap[_position.X][_position.Y] = ' ';
			advCout(tempPos.X, tempPos.Y, cPlayer,_color);
			_gameMap[tempPos.X][tempPos.Y] = cPlayer;
			_position = tempPos;
		}
	}
public:
	Player() : Entity() {
		_moveSpeed = 150.0f;
		_color = LIGHT_BLUE;
		_position.X = gameWidth / 2 + gameWidth / 4;
		_position.Y = gameHeight / 2-1;
		_gameMap[_position.X][_position.Y] = cFood;
		advCout(_position.X, _position.Y, cPlayer, _color);
	}
	void Move() override { ; }
	void Move(unsigned int& negativePoints){
		if (float(clock() - _lastMoved) / float(CLOCKS_PER_SEC / 1000) < _moveSpeed)return;
		if (GetAsyncKeyState(VK_UP) || GetAsyncKeyState('W') || GetAsyncKeyState('w'))
			AsyncMove(directions['w'], negativePoints);
		else if (GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState('S') || GetAsyncKeyState('s'))
			AsyncMove(directions['s'], negativePoints);
		else if (GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState('A') || GetAsyncKeyState('a'))
			AsyncMove(directions['a'], negativePoints);
		else if (GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState('D') || GetAsyncKeyState('d'))
			AsyncMove(directions['d'], negativePoints);
	}
	COORD getPosition() { return _position; }
	bool IsAlive() { return _alive; }
	~Player() { ; }
};
class Game
{
	Player *_player;
	Snake *_snake;
	clock_t _lastGenerated;
	float _frequency = 2000.0f;
	void SetFont(int fontSize = 15)
	{
		CONSOLE_FONT_INFOEX cfi;
		cfi.cbSize = sizeof(cfi);
		cfi.dwFontSize.X = fontSize;
		cfi.dwFontSize.Y = fontSize;
		cfi.FontFamily = FF_DONTCARE;
		cfi.FontWeight = FW_NORMAL;
		SetCurrentConsoleFontEx(h, true, &cfi);
		CONSOLE_CURSOR_INFO cci;
		//if (cci.bVisible == false)return;
		cci.bVisible = false;
		cci.dwSize = 100;
		SetConsoleCursorInfo(h, &cci);
	}
	void SetWindow(int fontSize)
	{
		//set dimensions
		SetWindowPos(GetConsoleWindow(), nullptr, 0, 0, (gameWidth + 2) * fontSize, (gameHeight + 2) * fontSize, SWP_NOZORDER | SWP_NOMOVE);
		system(("mode con cols=" + to_string(gameWidth) + " lines=" + to_string(gameHeight)).c_str());
		//set position
		HWND consoleWindow = GetConsoleWindow();
		RECT screenRect;
		GetWindowRect(GetDesktopWindow(), &screenRect);
		int screenWidth = screenRect.right;
		int screenHeight = screenRect.bottom;
		RECT consoleRect;
		GetWindowRect(consoleWindow, &consoleRect);
		int consoleWidth = consoleRect.right - consoleRect.left;
		int consoleHeight = consoleRect.bottom - consoleRect.top;
		int posX = (screenWidth - consoleWidth) / 2;
		int posY = (screenHeight - consoleHeight) / 2;
		MoveWindow(consoleWindow, posX, posY, consoleWidth, consoleHeight, TRUE);
	}
	void Drawborder()
	{
		char* border = new char[gameWidth + 1];
		memset(border, cWall, gameWidth * sizeof(char));
		border[gameWidth] = '\0';
		advCout(0, 0, border, LIGHT_GREEN);
		advCout(0, gameHeight - 1, border, LIGHT_GREEN);
		for (size_t i = 0; i < gameHeight - 1; i++)
		{
			advCout(0, i + 1, cWall, LIGHT_GREEN);
			advCout(gameWidth - 1, i + 1, cWall, LIGHT_GREEN);
		}
	}
	void UpdateTitle(string msg="")
	{
		string title = "title Reverse Snake " + msg;
		system(title.c_str());
	}
	void playSound()
	{
		Beep(1200, 50);
	}
	//
	int min=0,sec=0, ms = 0;
	bool running = false;
	void clockThread()
	{
		while (running) {
			this_thread::sleep_for(chrono::milliseconds(100));
			ms += 100;
			if (ms > 1000) { ms -= 1000; sec += 1; }
			if (sec > 60) { sec -= 60; min += 1; }
		}
	}
	void startClock() { running = true; }
	void stopClock() { running = false; }
	//
public:
	Game(int fontSize = 20)
	{
		srand(time(NULL));
		//setfont,cursor and window to height and width
		SetFont(fontSize);
		//set windows size
		SetWindow(fontSize);
		system("cls");
		//set title
		UpdateTitle();
		//set border
		Drawborder();
		_lastGenerated = clock();
	}
	void Play()
	{
		time_t t = time(nullptr);
		tm startTime,now;
		localtime_s(&startTime,&t);
		_player = new Player();
		_snake = new Snake();
		thread tClock(&Game::clockThread, this);
		tClock.join();
		startClock();
		while (_snake->getBody().size() && _player->IsAlive())
		{
			_player->Move(_snake->getNegativePoints());
			_snake->Move();
			if (float(clock() - _lastGenerated) / float(CLOCKS_PER_SEC / 1000) > _frequency)
			{
				_food.push_back(Food());
				_lastGenerated = clock();
			}
			localtime_s(&now, &t);

			UpdateTitle(" \\\\ lenght: " + to_string(_snake->getBody().size()) + " \\\\ time: "+to_string(min)+":"+(sec<10?"0":"")+ to_string(sec) + ":" + to_string(ms / 100));
		}
		system("cls");
		string msg = _player->IsAlive() ? "YOU WIN": "YOU LOSE";
		advCout(gameWidth / 2 - msg.length() / 2, gameHeight / 2, msg, _player->IsAlive() ? GREEN : RED);
		msg = "PLAY AGAIN -> enter";
		advCout(gameWidth / 2 - msg.length() / 2, gameHeight / 2, msg, WHITE);
		msg = "EXIT -> esc";
		advCout(gameWidth / 2 - msg.length() / 2, gameHeight / 2, msg, WHITE);
		while (true)
		{
			if (GetAsyncKeyState(VK_ESCAPE))break;
			//finish
		}
	}
};
int main()
{
	/*for (int i = 0; i < 256; i++)
		cout << i << " " << char(i) << endl;
	return 0;*/
	//
	directions['w'] = {0,-1};//VK_UP
	directions['s'] = {0,1};//VK_DOWN
	directions['a'] = {-1,0};//VK_LEFT
	directions['d'] = {1,0};//VK_RIGHT	
	//setup empty map
	string s = "";
	for (size_t i = 0; i < gameWidth; i++)s += " ";
	s[0] = s[gameWidth - 1] = cWall;
	_gameMap.insert(_gameMap.begin(), gameHeight, s);
	s = "";
	for (size_t i = 0; i < gameWidth; i++)s += cWall;
	_gameMap[0] = _gameMap[gameHeight - 1] = s;
	//
	for (auto it : _gameMap)
		cout << it << endl;

	Game game;
	game.Play();

	system("pause>null");
	return 0;
}