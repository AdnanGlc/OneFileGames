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
map<UINT, COORD> directions;
vector<string> _gameMap;
//
HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
const char cWall = 219, cFood = 254, cBody = 178;
enum Color { BLUE = 1, GREEN, CYAN, RED, PURPLE, YELLOW, WHITE, GRAY, DARK_CYAN, LIGHT_GREEN, LIGHT_BLUE, PINK, MAGENTA, BEIGE, WHITE_N_BLACK = 240 };
const int dirX[4] = { 0,0,1,-1 };
const int dirY[4] = { -1,1,0,0 };
const char CDir[4] = { 'w','s','d','a' };
//
template<class T>
void advCout(int x, int y, T msg, Color color = WHITE) {
	SetConsoleTextAttribute(h, color);
	SetConsoleCursorPosition(h, { (short)x,(short)y });
	cout << msg;
}
//
class Food
{
	COORD _position;
	int _points;
	bool _isBad;
public:
	Food(bool isBad = false) {
		while (true) {
			int tempX = rand() % (gameWidth - 1) + 1;
			int tempY = rand() % (gameHeight - 1) + 1;
			//
			if (_gameMap[tempX][tempY] == cBody || _gameMap[tempX][tempY] == cWall || _gameMap[tempX][tempY] == cFood)continue;
			_position.X = tempX;
			_position.Y = tempY;
			_points = rand() % 3 + 1;
			_gameMap[tempX][tempY] = cFood;
			advCout(tempX, tempY, cFood, Color(_points));
			_isBad = isBad;
			break;
		}
	}
	Food(const Food& obj) {
		if (&obj == nullptr)return;
		_position = obj._position;
		_points = obj._points;
		_isBad = obj._isBad;
	}
	~Food() { ; }
	int getPoints() { return _points; }
	COORD getPosition() { return _position; }
	void DecreasePoints() { _points--; }
	bool IsBad() { return _isBad; }
	void DeclareBad() { _isBad = true; }
};
list<Food> _allFood;
list<Food> _foodToConsume;
Food FindFood(int x, int y)
{
	for (auto it : _allFood)
		if (it.getPosition().X == x && it.getPosition().Y == y)
			return it;
}
class Entity
{
protected:
	clock_t _lastMoved;
	float _moveSpeed;
	bool _alive;
	Color _color = WHITE;
	virtual void ConsumeFood() = 0;
public:
	Entity() { _lastMoved = clock(); _alive = true; }
	virtual void Move() = 0;
	~Entity() { ; }
};
//
class Player : public Entity {
	COORD _position;
	float  _moveSpeed;
	void ConsumeFood() {
		if (!_allFood.size())return;
		Food f = _allFood.back();
		f.DeclareBad();
		_foodToConsume.push_back(f);
		_allFood.pop_back();
	}
	//
	void AsyncMove(COORD dir)
	{
		_lastMoved = clock();
		COORD tempPos = _position;
		_gameMap[_position.X][_position.Y] = ' ';
		tempPos.X += dir.X;
		tempPos.Y += dir.Y;
		if (tempPos.X > 0 && tempPos.X < gameWidth - 1 && tempPos.Y>0 && tempPos.Y < gameHeight - 1) {
			if (_gameMap[tempPos.X][tempPos.Y] == cBody) { _alive = false; return; }
			else if (_gameMap[tempPos.X][tempPos.Y] == cFood){  ConsumeFood(); }
			advCout(_position.X, _position.Y, ' ');
			advCout(tempPos.X, tempPos.Y, cFood, _color);
			_gameMap[tempPos.X][tempPos.Y] = cFood;
			_position = tempPos;
		}

	}
public:
	Player() : Entity() {
		_moveSpeed = 150.0f;
		_color = LIGHT_BLUE;
		_position.X = gameWidth / 2 + gameWidth / 4;
		_position.Y = gameHeight / 2 - 1;
		_gameMap[_position.X][_position.Y] = cFood;
		advCout(_position.X, _position.Y, cFood, _color);
	}
	void Move() {
		if (float(clock() - _lastMoved) / float(CLOCKS_PER_SEC / 1000) < _moveSpeed)return;
		if (GetAsyncKeyState(VK_UP) || GetAsyncKeyState('W') || GetAsyncKeyState('w'))
			AsyncMove(directions[VK_UP]);
		if (GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState('S') || GetAsyncKeyState('s'))
			AsyncMove(directions[VK_DOWN]);
		if (GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState('A') || GetAsyncKeyState('a'))
			AsyncMove(directions[VK_LEFT]);
		if (GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState('D') || GetAsyncKeyState('d'))
			AsyncMove(directions[VK_RIGHT]);
	}
	COORD getPosition() { return _position; }
	bool IsAlive() { return _alive; }
	void Kill() { _alive = false; }
	~Player() { ; }
};
//
class Snake : public Entity
{
	list<COORD> _body;
	Color _headColor;
	void ConsumeFood() { 
		if (_foodToConsume.size()) {
			if (_foodToConsume.back().IsBad()) { ShrinkBody(); ShrinkBody(); return; }

			_foodToConsume.back().DecreasePoints();
			if (_foodToConsume.back().getPoints() <= 0)
				_foodToConsume.pop_back();
			return;
		}
		else ShrinkBody();
	}
	void ShrinkBody() {
		advCout(_body.back().X, _body.back().Y, ' ');
		_gameMap[_body.back().X][_body.back().Y] = ' ';
		_body.pop_back();
		if (_foodToConsume.size())_foodToConsume.pop_back();
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
			if (_gameMap[x][y] == cFood)break;
			for (size_t i = 0; i < 4; i++) {
				int newX = x + dirX[i];
				int newY = y + dirY[i];
				if (_gameMap[newX][newY] != cWall && _gameMap[newX][newY] != cBody && !visited[newX][newY]) {
					visited[newX][newY] = 1;
					paths.push_back(make_pair(COORD{ (short)newX,(short)newY }, tempPath + CDir[i]));
				}
			}
		}
		return tempPath[1];
	}
	void ExtendBody_b(int x, int y)
	{
		_body.push_back(COORD{ (short)(x),(short)(y) });
		advCout(x, y, cBody, _color);
		_gameMap[x][y] = cBody;
	}
	void ExtendBody_f(int x, int y)
	{
		if (_gameMap[x][y] == cFood)_foodToConsume.push_back(FindFood(x, y));
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
		for (size_t i = 0; i < 5; i++)
			ExtendBody_b(gameWidth / 2 - i, gameHeight / 2);
		advCout(_body.front().X, _body.front().Y, cBody, _headColor);
	}
	void Move() { ; }
	void Move(Player* player) {

		if (float(clock() - _lastMoved) / float(CLOCKS_PER_SEC / 1000) < _moveSpeed)return;
		_lastMoved = clock();
		//bfs -> food/player
		char dir = FindDirection();
		COORD c{ 0,0 };
		if		(dir == 'w')c = directions[VK_UP];
		else if (dir == 's')c = directions[VK_DOWN];
		else if (dir == 'a')c = directions[VK_LEFT];
		else if (dir == 'd')c = directions[VK_RIGHT];
		else return;
		//move
		ExtendBody_f(_body.front().X + c.X, _body.front().Y + c.Y);
		if (player->getPosition().X == _body.front().X && player->getPosition().Y == _body.front().Y) {
			player->Kill();
		}
		ConsumeFood();
	}
	list<COORD>& getBody() { return _body; }
	~Snake() { ; }
};
class Game
{
	Player* _player;
	Snake* _snake;
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
		//setup empty map
		_gameMap.clear();
		string s = "";
		for (size_t i = 0; i < gameWidth; i++)s += " ";
		s[0] = s[gameWidth - 1] = cWall;
		_gameMap.insert(_gameMap.begin(), gameHeight, s);
		s = "";
		for (size_t i = 0; i < gameWidth; i++)s += cWall;
		_gameMap[0] = _gameMap[gameHeight - 1] = s;
		//
		/*for (auto it : _gameMap)
			cout << it << endl;*/
	}
	void UpdateTitle()
	{
		string title = "title Reverse Snake ";
		system(title.c_str());
	}
	void playSound()
	{
		Beep(1200, 50);
	}
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
		_player = new Player();
		_snake = new Snake();
		while (_player->IsAlive() && _snake->getBody().size())
		{
			_player->Move();
			_snake->Move(_player);
			if (float(clock() - _lastGenerated) / float(CLOCKS_PER_SEC / 1000) > _frequency)
			{
				_allFood.push_back(Food());
				_lastGenerated = clock();
			}
		}
		system("cls");
		if(_player->IsAlive())	advCout(gameWidth/2-3, gameHeight/2, "YOU WIN");
		else advCout(gameWidth / 2 - 5, gameHeight / 2, "SNAKE WINS");
	}
};
int main()
{
	//
	directions[VK_UP] = { 0,-1 };//VK_UP
	directions[VK_DOWN] = { 0,1 };//VK_DOWN
	directions[VK_LEFT] = { -1,0 };//VK_LEFT
	directions[VK_RIGHT] = { 1,0 };//VK_RIGHT	
	//
	Game *game;
	while (true) {
		game = new Game();
		game->Play();
		delete game;

		bool playAgain = true;
		advCout(1, gameHeight - 3, "Play again - ENTER");
		advCout(1, gameHeight - 2, "Exit- ESC");

		while (true)
		{
			if (GetAsyncKeyState(VK_RETURN))break;
			if (GetAsyncKeyState(VK_ESCAPE)) { playAgain = false; break; }
		}
		if (!playAgain)break;
	}

	system("pause>null");
	return 0;
}
