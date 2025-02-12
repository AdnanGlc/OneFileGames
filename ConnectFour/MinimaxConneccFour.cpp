#include<iostream>
#include<vector>
#include<cstdlib>
#include<Windows.h>
#include<string>

using namespace std;

HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

const int gameWidth = 7, gameHeight = 6,fontSize = 25;
int playerX = 2, playerY = 0;
int currentPlayer = 1;

const char cBlock = 219,cCoin = 254;
int RED = 12, YELLOW = 6;

const int DEPTH = 5,WINNING_SCORE = 1000000;

vector<vector<int>> gameGrid;

//
void setFontSize(bool visibleCursor = false)
{
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    cfi.dwFontSize.X = cfi.dwFontSize.Y = fontSize;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    SetCurrentConsoleFontEx(h, false, &cfi);

    CONSOLE_CURSOR_INFO cci;
    cci.bVisible = visibleCursor;
    cci.dwSize = 100;
    SetConsoleCursorInfo(h, &cci);
}
void windowSetup() {
	system("title Connect Four");
    setFontSize();
	//set dimensions
	SetWindowPos(GetConsoleWindow(), nullptr, 0, 0, (gameWidth*2.5) * fontSize, (gameHeight*2.5) * fontSize, SWP_NOZORDER | SWP_NOMOVE);
	system(("mode con cols=" + to_string(int(gameWidth*2.5)) + " lines=" + to_string(int(gameHeight*2.5))).c_str());
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
//
template<class T>
void advCout(T msg, short x, short y, int color = 7)
{
	SetConsoleCursorPosition(h,{ x,y });
	SetConsoleTextAttribute(h, color);
	cout << msg;
	SetConsoleTextAttribute(h, 7);
}
//
bool playCol(vector<vector<int>>& tempGrid, int index, int player, bool draw = false)
{
	// Provera da li je kolona puna pre igranja poteza
	if (tempGrid[index][0] != 0)
		return false;

	for (int i = tempGrid[index].size() - 1; i >= 0; i--)
	{
		if (tempGrid[index][i] == 0)
		{
			tempGrid[index][i] = player;
			if (draw)
				advCout(cCoin, index * 2 + 2, i * 2 + 2, player == 1 ? YELLOW : RED);
			return true;
		}
	}
}

int checkWin(vector<vector<int>>& grid,int player) {//1 - last player wins, -1 - draw, 0 - still playing
	int counters[4];
	bool draw = true;

	for (int i = 0; i < grid.size(); i++)
		for (int j = 0; j < grid[i].size(); j++) {
			memset(counters, 0, sizeof(int) * 4);
			for (int k = 0; k < 4; k++) {
				if (i + k < gameWidth && grid[i + k][j] == player)counters[0]++;
				if (j + k < gameHeight && grid[i][j + k] == player)counters[1]++;
				if (i-k>= 0 && j-k>=0 && grid[i - k][j - k] == player)counters[2]++;
				if (i-k>=0 && j+k<gameHeight &&  grid[i - k][j + k] == currentPlayer)counters[3]++;
				if (grid[i][j] == 0)draw = false;
			}
			if (counters[0] & 4 | counters[1] & 4 | counters[2] & 4 | counters[3] & 4)return 1;
		}
	if (draw)return -1;
	return 0;
}
int nextMoveWins(vector<vector<int>>& grid, int player) {
	for (int col = 0; col < gameWidth; col++) {
		vector<vector<int>> tempGrid = grid;
		if (playCol(tempGrid, col, player)) {
			if (checkWin(tempGrid, player) == 1) {
				return col;
			}
		}
	}
	return -1;
}
//
void movePlayer(int dir)
{
	int tempX = playerX + dir;
	if (tempX <= 0 || tempX >= gameWidth*2+1)
		return;
	advCout(' ', playerX, playerY);
	advCout(cCoin, tempX, playerY,YELLOW);
	playerX = tempX;
	Sleep(200);
}
void playerInput()
{
	if (GetAsyncKeyState(VK_RIGHT))
		movePlayer(2);
	else if (GetAsyncKeyState(VK_LEFT))
		movePlayer(-2);
	else if (GetAsyncKeyState(VK_SPACE) || GetAsyncKeyState(VK_DOWN))
	{
		if (playCol(gameGrid, playerX / 2 - 1, 1, true))
			currentPlayer = 2;
		Sleep(200);
	}

}
//
void gameSetup()
{
	vector<int> temp(gameHeight, 0);
	gameGrid.clear();
	gameGrid.insert(gameGrid.begin(), gameWidth, temp);
	system("cls");
	advCout("", 0, 0);
	advCout(cCoin, playerX, playerY, YELLOW);
	cout << endl;
	cout << endl;
	for (int i = 1; i <= gameHeight * 2; i++) {
		cout << ' ';
		for (int j = 0; j <= gameWidth * 2; j++) {
			if (i % 2 == 0 || j % 2 == 0 || j == gameWidth * 2)
				cout << cBlock;
			else cout << ' ';
		}
		cout << endl;
	}
}
//minimax
int evaluteGrid(vector<vector<int>>& grid, int player)
{
	int opponent = (player == 1) ? 2 : 1;

	if (checkWin(grid, player)) return WINNING_SCORE;
	if (checkWin(grid, opponent)) return -WINNING_SCORE;

	return 0; 
}

int minimax(vector<vector<int>> grid, int depth, int alpha, int beta, bool maximizingPlayer, int player) 
{
	int oponent = (player == 1) ? 2 : 1;
	if (depth == 0 || checkWin(grid,player)==1 || checkWin(grid,oponent) ==1 || checkWin(grid,currentPlayer)!=0)
		return evaluteGrid(grid, player);
	if (maximizingPlayer){
		int maxEval = INT_MIN;
		for (size_t i = 0; i < grid.size(); i++)
		{
			vector<vector<int>> tempGrid = grid;
			playCol(tempGrid, i, player);

			int eval = minimax(tempGrid, depth-1, alpha, beta, false, player);

			maxEval = max(maxEval, eval);
			alpha = max(alpha, eval);

			if (beta <= alpha)
				break;
		}
		return maxEval;
	}
	else {
		int minEval = INT_MAX;
		for (size_t i = 0; i < grid.size(); i++)
		{
			vector<vector<int>> tempGrid = grid;
			if (playCol(tempGrid, i, oponent)) {

				int eval = minimax(tempGrid, depth - 1, alpha, beta, false, oponent);

				minEval = min(minEval, eval);
				alpha = min(beta, eval);

				if (beta <= alpha)
					break;
			}
		}
		return minEval;
	}

	return -1;
}
int getBestmove(vector<vector<int>>& grid, int player = 2)
{
	int bestMove = -1;
	int bestScore = INT_MIN;
	int oponent = player == 1 ? 2 : 1;
	int playerWins = nextMoveWins(grid, player), oponentWins = nextMoveWins(grid, oponent);

	if (playerWins != -1)	return playerWins;
	if (oponentWins != -1)	return oponentWins;
	
	for (int i = 0; i < grid.size(); i++) {
		vector<vector<int>> tempGrid = grid;
		if (playCol(tempGrid, i, player)) {
			int score = minimax(tempGrid, DEPTH, INT_MIN, INT_MAX, false, player);
			if (score > bestScore) {
				bestScore = score;
				bestMove = i;
			}
		}
	}

	return bestMove;
}

//
int main()
{
	windowSetup();
	gameSetup();
	int moveCounter = 0;
	while (true)
	{
		while (true)
		{
			if (checkWin(gameGrid, currentPlayer))
				break;
			if (currentPlayer == 1)
				playerInput();
			else
			{
				advCout("AI is calculating", 0, gameHeight * 2 + 2);
				int move = getBestmove(gameGrid, 2);
				playCol(gameGrid, move, 2, true);
				currentPlayer = 1;
				advCout("AI played       " + to_string(move + 1), 0, gameHeight * 2 + 2);

			}
			Sleep(150);
		}
		system("cls");
		advCout("Game Over", gameWidth - 3, gameHeight);
		advCout("Play again - Enter", 0, gameHeight * 2 - 3);
		advCout("Exit- Esc", 1, gameHeight * 2 - 2);
		bool playAgain = false;
		while (true) {
			if (GetAsyncKeyState(VK_RETURN)) { playAgain = true; break; }
			if (GetAsyncKeyState(VK_ESCAPE))break;
		}
		if (!playAgain)break;
	}
	return 0;
}