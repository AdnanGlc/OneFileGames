#include<iostream>
#include<list>
#include<vector>
#include<Windows.h>
#include<string>
#include<thread>
#include<mutex>
#include<conio.h>

using namespace std;
//
const short gameHeight = 30, gameWidth = 50,fps=24;
HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
//
enum Direction { UP, DOWN, LEFT, RIGHT };
enum Color {BLUE=1,GREEN,CYAN,RED,PURPLE,YELLOW,WHITE,GRAY,DARK_CYAN,LIGHT_GREEN,LIGHT_BLUE,PINK,MAGENTA,BEIGE,WHITE_N_BLACK=240};
const char fBlock = 219, mBlock = 254, dBlock = 220, lBlock = 221, rBlock = 222, uBlock = 223;
const char lShade = 176, mShade = 177, sShade = 178;
const char rTriangle = 16, lTriangle = 17, uTriangle = 30, dTriangle = 31;
const float moveSpeeds[] = { 100.0f,150.0f,200.0f,250.0f,300.0f };
const float shootingSpeeds[] = { 200.0f,500.0f,700.0f,900.0f,1800.0f };
const float bulletSpeed = 10.0f;
vector<string> robotBodies;
mutex guard;
//
string reverseBody(string body)
{					
	string rBody = "";
	for (int i = body.length() - 2; i >= 0; i--)
		if ((i + 1) % 4 == 0)rBody += '\n';
		else if (body[i] == dBlock)rBody += uBlock;//down->up
		else if (body[i] == rBlock)rBody += lBlock;//right -> left
		else if (body[i] == uBlock)rBody += dBlock;//up -> down
		else if (body[i] == lBlock)rBody += rBlock;// left -> right
		else if (body[i] == uTriangle)rBody += dTriangle;
		else if (body[i] == dTriangle)rBody += uTriangle;
		else if (body[i] == lTriangle)rBody += rTriangle;
		else if (body[i] == rTriangle)rBody += lTriangle;
		else if (body[i] != '\n')rBody += body[i];
	return rBody;
}
void advCout(short x, short y, string text, Color color=WHITE)
{
	vector<string> textLines;
	string temp = "";
	for (size_t i = 0; i < text.length(); i++)
		if (text[i] != '\n')
			temp += text[i];
		else {
			textLines.push_back(temp);
			temp = "";
		}
	if (temp != "")textLines.push_back(temp);
	SetConsoleTextAttribute(h, color);
	for (short i = 0; i < textLines.size(); i++)
	{
		SetConsoleCursorPosition(h, { x,short(y + i) });
		cout << textLines[i];
	}
	SetConsoleTextAttribute(h, WHITE);
}
template<class T>
void advCout(short x, short y,T  sign, Color color=WHITE)
{
	SetConsoleTextAttribute(h, color);
	SetConsoleCursorPosition(h, { x,y });
	cout << sign;
	SetConsoleTextAttribute(h, WHITE);
}
template<class T1>
class MenuPicker
{
	int _x, _y;
	int _index = 0;
	vector<T1> _choices;
	string _message;
public:
	MenuPicker(string message,vector<T1>& choices,int x=0, int y=0)
	{
		_x = x;
		_y = y;
		_index = 0;
		_choices = choices;
		_message = message;
		advCout(_x,_y, message, WHITE_N_BLACK);
		advCout(_x - 2, _y, lTriangle, WHITE);
		advCout(_x + message.length()+1, _y, rTriangle, WHITE);
        advCout(_x + message.length() / 2 -3 , _y + 2, choices[_index]);
	}
	T1 Select()
	{
		while (true){
			if (_kbhit()) {

				if (GetAsyncKeyState(VK_RIGHT)){
					_index++;
					if (_index >= _choices.size())_index = 0;
				}
				if (GetAsyncKeyState(VK_LEFT)){
					_index--;
					if (_index < 0)_index = _choices.size() - 1;
				}
				if (GetAsyncKeyState(VK_RETURN))
				{
					system("cls");
					return _choices[_index];
				}
				Sleep(300);
				advCout(_x + _message.length()/2-3, _y + 2, _choices[_index], WHITE);
			}
		}
	}
};
//
class Bullet
{
public:
	short _x, _y;
	Direction _direnciton;
	clock_t _lastMoved;
	Bullet(int x, int y, Direction direction) { _x = x, _y = y; _direnciton = direction; _lastMoved = clock(); }
	~Bullet() { ; }
	void moveBullet(Color color = GRAY)
	{
		_lastMoved = clock();
		advCout(_x, _y, ' ');
		if (_direnciton == RIGHT)_x++;
		else _x--;
		if (_x != gameWidth - 3 && _x != 2)
			advCout(_x, _y, _direnciton==RIGHT?rTriangle:lTriangle, color);
	}
};
class Robot
{
protected:
	short _x, _y;
	float _moveSpeed, _shootingSpeed;
	list<Bullet> _bullets;
	string _body;
	Color _color;
	clock_t _lastMoved, _lastShot;
	void moveBullets() {
		//move existing bullets
		if (_bullets.size())
			if (_bullets.back()._x >= gameWidth - 3 && _bullets.back()._direnciton == RIGHT)
				_bullets.pop_back();
			else if (_bullets.back()._x <= 2 && _bullets.back()._direnciton == LEFT)
				_bullets.pop_back();

		for (auto& it : _bullets)
			if (float(clock() - it._lastMoved) > 100.0f)
				it.moveBullet(_color);
	}
	void asyncMove(Direction direction)
	{
		if (direction == UP && _y > 1)
		{
			advCout(_x, _y + 2, "    ");
			_y--;
			advCout(_x, _y, _body, _color);
			_lastMoved = clock();
			return;
		}
		else if (direction == DOWN && _y < gameHeight - 4)
		{
			advCout(_x, _y, "    ");
			_y++;
			advCout(_x, _y, _body, _color);
			_lastMoved = clock();
			return;
		}
	}
	void asyncShoot(int x,int y,Direction direction)
	{
		_lastShot = clock();
		_bullets.push_front(Bullet(x, y, direction));
		_bullets.front()._lastMoved = clock();
	}
public:
	int _lives;
	Robot(int diff = 3)
	{
		_moveSpeed = moveSpeeds[diff];
		_shootingSpeed = shootingSpeeds[diff];
		_lastMoved = clock();
		_lastShot = clock();
		_color = Color(rand() % 15 + 1);
		_lives = 1;
	}
	~Robot()
	{

	}
	virtual void Move() = 0;
	virtual void Shoot() = 0;
	int getLives() { return _lives; }
	void reduceLife() { cout << '\7'; _lives--; }
	int getX() { return _x; }
	int getY() { return _y; }
	list<Bullet>& getBullets() { return _bullets; }
	bool IsHit(Bullet bullet) { return _x<bullet._x&& _x + 3>bullet._x && _y < bullet._y&& _y + 3 >= bullet._y; }
};
class Player : public Robot
{
public:
	Player(int difficulty = 3) : Robot(difficulty)
	{
		_x = 1;
		_y = gameHeight / 2 - 3;
		advCout(_x, _y, _body, _color);
		_lives = 3;
		MenuPicker<string> mp("Chose a body", robotBodies, gameWidth/2-6, gameHeight/2-5);
		_body = mp.Select();
		advCout(_x, _y, _body, _color);
	}
	virtual void Move()
	{
		if (float(clock() - _lastMoved) > _moveSpeed){
			Direction dir = Direction(-1);
			if (GetAsyncKeyState(VK_UP))
				dir = UP;
			else if (GetAsyncKeyState(VK_DOWN))
				dir = DOWN;
			if (dir != -1){
				thread tMOve(&Player::asyncMove, this,dir);
				tMOve.join();
			} 
		}
	}
	virtual void Shoot()
	{
		moveBullets();
		//shoot new bullet
		if (float(clock() - _lastShot) > _shootingSpeed)
			if(GetAsyncKeyState(VK_SPACE)){
				thread tShoot(&Player::asyncShoot, this,_x+4,_y+1,RIGHT);
				tShoot.join();
			}
	}
	
};
class Enemy : public Robot
{
	Direction _movingDireciton;
public:
	Enemy(int x,int y,int difficulty) : Robot(5-difficulty)
	{
		_x = x;
		_y = y;
		_movingDireciton = Direction(rand()%2);//up or down
		_body = reverseBody(robotBodies[rand() % robotBodies.size()]);
		advCout(_x, _y, _body, _color);
	}
	~Enemy()
	{
		for (short i = -1; i < 4; i++)
			advCout(_x, _y + i, "   ");
		while (_bullets.size()){
			if (_bullets.back()._x >= 1)
				advCout(_bullets.back()._x, _bullets.back()._y, ' ');
			_bullets.pop_back();
		}
	}
	virtual void Move()
	{ 
		if (_y == 1)_movingDireciton = DOWN;
		if (_y >= gameHeight - 4)_movingDireciton = UP;
		if (float(clock() - _lastMoved) > _moveSpeed){
			thread tMove(&Enemy::asyncMove, this, _movingDireciton);
			tMove.join();
		}
	}
	virtual void Shoot() 
	{ 
		moveBullets();
		if (float(clock() - _lastShot) > _shootingSpeed){
			thread tShoot(&Enemy::asyncShoot, this, _x - 1, _y + 1, LEFT);
			tShoot.join();
		}
	}
	bool CheckHit(int x,int y)
	{
		if (_bullets.size() == 0)return false;
		int bulletX = _bullets.back()._x;
		int bulletY = _bullets.back()._y;
		bool hit = x < bulletX&& x + 3 >= bulletX && y < bulletY&& y + 3 >= bulletY;
		if (hit){
			advCout(_bullets.back()._x, _bullets.back()._y, ' ');
			_bullets.pop_back();
		}
		return hit;
	}
};
class Game
{
	Player *_player = nullptr;
	vector<Enemy*> _enemies;
	int _difficulty,_score = 0;
	void SetFont(int fontSize=15)
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
		memset(border, fBlock, gameWidth * sizeof(char));
		border[gameWidth] = '\0';
		advCout(0, 0, border, RED);
		advCout(0, gameHeight - 1, border, RED);
		for (size_t i = 0; i < gameHeight - 1; i++)
		{
			advCout(0, i + 1, fBlock, RED);
			advCout(gameWidth - 1, i + 1, fBlock, RED);
		}
	}
	void UpdateTitle()
	{
		string title = "title 2dShooter  lives " + to_string(_player->getLives()) + "  score " + to_string(_score);
		system(title.c_str());
	}
	void playSound()
	{
		Beep(1200, 50);
	}
public:
	Game(int difficulty = 3,int fontSize = 20)
	{
		srand(time(NULL));
		_difficulty = difficulty;
		//setfont,cursor and window to height and width
		SetFont(fontSize);
		//set windows size
		SetWindow(fontSize);
	}
	void Setup()
	{
		//get player and enemies
		system("cls");
		_player = new Player(_difficulty);
		for (int i = 0; i < 3; i++)
			_enemies.push_back(new Enemy((gameWidth - 5 - i * 3), rand() % (gameHeight - 4) + 1, _difficulty));
		//set border
		Drawborder();
		//set title
		UpdateTitle();
	}
	void Play()
	{
		while (_player->getLives())
		{
			clock_t frameStart = clock();
			_player->Move();
			_player->Shoot();
			for (auto &it: _enemies)
			{
				it->Move();
				it->Shoot();
				if (it->getBullets().size() &&  _player->IsHit(it->getBullets().back()))
					_player->reduceLife();
				if(_player->getBullets().size())
					if (it->IsHit(_player->getBullets().back()))
					{
						_player->getBullets().pop_back();
						_score += 100;
						int x = it->getX();
						delete it;
						thread ts(&Game::playSound, this);
						ts.join();
						it = new Enemy(x, rand() % (gameHeight - 4) + 1, _difficulty);
					}
			}
			UpdateTitle();
			clock_t frameEnd = clock();
			if (double(frameEnd - frameStart) / CLOCKS_PER_SEC * 1000 < 1000 / fps)
				Sleep(1000 / fps - double(frameEnd - frameStart) / CLOCKS_PER_SEC * 1000);
		}
		system("cls");
		advCout(0, 0, "GAME OVER", RED);
		advCout(0, 1, "Final score: "+to_string(_score), RED);
	}
};
int main()
{
	//
	robotBodies.push_back("");
	robotBodies.back() += dBlock; robotBodies.back() += dBlock; robotBodies.back() += fBlock;		robotBodies.back() += "\n";
	robotBodies.back() += rBlock; robotBodies.back() += mBlock; robotBodies.back() += rTriangle; robotBodies.back() += "\n";
	robotBodies.back() += uBlock; robotBodies.back() += uBlock; robotBodies.back() += fBlock;		robotBodies.back() += "\n";
	//
	robotBodies.push_back("");
	robotBodies.back() += lTriangle; robotBodies.back() += fBlock; robotBodies.back() += rTriangle;		robotBodies.back() += "\n";
	robotBodies.back() += " "; robotBodies.back() += rBlock; robotBodies.back() += mBlock; robotBodies.back() += "\n";
	robotBodies.back() += lTriangle; robotBodies.back() += fBlock; robotBodies.back() += rTriangle;		robotBodies.back() += "\n";
	//
	robotBodies.push_back("");
	robotBodies.back() += dBlock; robotBodies.back() += fBlock; robotBodies.back() += fBlock; robotBodies.back() += "\n";
	robotBodies.back() += rBlock; robotBodies.back() += fBlock; robotBodies.back() += mBlock; robotBodies.back() += "\n";
	robotBodies.back() += uBlock; robotBodies.back() += fBlock; robotBodies.back() += fBlock; robotBodies.back() += "\n";
	//
	robotBodies.push_back("");
	robotBodies.back() += dBlock; robotBodies.back() += fBlock; robotBodies.back() += rTriangle; robotBodies.back() += "\n";
	robotBodies.back() += rBlock; robotBodies.back() += mBlock; robotBodies.back() += mBlock; robotBodies.back() += "\n";
	robotBodies.back() += uBlock; robotBodies.back() += fBlock; robotBodies.back() += rTriangle; robotBodies.back() += "\n";
	//
	robotBodies.push_back("");
	robotBodies.back() += rBlock; robotBodies.back() += dBlock; robotBodies.back() += " "; robotBodies.back() += "\n";
	robotBodies.back() += fBlock; robotBodies.back() += mBlock; robotBodies.back() += mBlock; robotBodies.back() += "\n";
	robotBodies.back() += rBlock; robotBodies.back() += uBlock; robotBodies.back() += " "; robotBodies.back() += "\n";
	//
	robotBodies.push_back("");
	robotBodies.back() += rBlock; robotBodies.back() += rTriangle; robotBodies.back() += " "; robotBodies.back() += "\n";
	robotBodies.back() += fBlock; robotBodies.back() += mBlock; robotBodies.back() += mBlock; robotBodies.back() += "\n";
	robotBodies.back() += rBlock; robotBodies.back() += rTriangle; robotBodies.back() += " "; robotBodies.back() += "\n";
	Game *game = new Game(3,20);
	vector<int> dif = { 1,2,3,4,5 };
	MenuPicker<int> mp("Chose difficulty", dif, gameWidth/2-8, gameHeight/2-2);
	int difficulty = mp.Select();

	while (true)
	{
		game->Setup();
		game->Play();
		advCout(0, 2, "PLAY AGAIN -> ENTER");
		advCout(0, 3, "EXIT -> ESC");
		bool flag = false;
		while (true)
		{
			if (GetAsyncKeyState(VK_ESCAPE)){
				flag = true;
				break;
			}
			if (GetAsyncKeyState(VK_RETURN))
				break;
		}
		if (flag)
			break;
		delete game;
		game = new Game(difficulty);
	}
	return 0;
}