#include<iostream>
#include<windows.h>
#include<vector>
#include<algorithm>
#include<ctime>
#include<list>

using namespace std;

const int visina = 25, sirina = 25;
const char zid = (char)219, prolaz = ' ';
///desno, lijevo, dole, gore
const int smjerx[4] = { 0, 0,1,-1 };
const int smjery[4] = { 1,-1,0, 0 };

const int bijela = 7, crvena = 12, crvena2 = 68;

int a = 0, b = 1;
bool posjetio[visina][sirina] = { 0 };
vector<string> labirint;
vector<pair<int, int> >put;

void font(int x)
{
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    cfi.dwFontSize.X = x;
    cfi.dwFontSize.Y = x;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_LIGHT;
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), false, &cfi);
}
void postavi_boju(int boja)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), boja);
}
void ispis(int x, int y)
{
    LockWindowUpdate(GetConsoleWindow());
    system("cls");
    for (int i = 0; i < labirint.size(); i++) {
        for (int j = 0; j < labirint[i].size(); j++) {
            if ((i <= a + 3 && i >= a - 3) && (j <= b + 3 && j >= b - 3)) {
                if (i == a && j == b) {
                    postavi_boju(crvena); cout << char(254);
                }
                else {
                    postavi_boju(bijela); cout << labirint[i][j];
                }
            }
            else cout << char(176);
        }
        cout << endl;
    }
    LockWindowUpdate(NULL);
    Sleep(10);
}
void setup()
{
    string s = "";
    cout << "\tGet from top left corner to bottom\n\t right corner by moving with arrow keys\n\tBut your vission is limited, good luck" << endl;
    system("pause");
    for (int j = 0; j < sirina; j++)s += zid;
    labirint.insert(labirint.begin(), visina, s);
    labirint[visina - 2][sirina - 2] = prolaz;
    put.push_back(make_pair(visina - 2, sirina - 2));
}
bool ima_susjeda(int x, int y)
{
    ///novo mjesto ne smije imati susjeda osim iz smjera od kojeg dolazimo
    ///zbog toga je br_susjeda=-1
    int br_susjeda = -1;
    for (int i = 0; i < 4; i++)
    {
        int novix = x + smjerx[i];
        int noviy = y + smjery[i];
        if (labirint[novix][noviy] == prolaz)br_susjeda++;
    }
    return br_susjeda;
}
bool nema_dijagonala(int x, int y, int i)
{
    ///radi estetike provjeravamo ima li dijagonala
    if (i == 0 && labirint[x + 1][y + 1] == zid && labirint[x - 1][y + 1] == zid)return true;
    if (i == 1 && labirint[x + 1][y - 1] == zid && labirint[x - 1][y - 1] == zid)return true;
    if (i == 2 && labirint[x + 1][y + 1] == zid && labirint[x + 1][y - 1] == zid)return true;
    if (i == 3 && labirint[x - 1][y + 1] == zid && labirint[x - 1][y - 1] == zid)return true;
    else return false;
}
bool moze_se_povezati(int x, int y)
{
    for (int i = 0; i < 4; i++)
    {
        int novix = x + smjerx[i];
        int noviy = y + smjery[i];
        if (novix && noviy && novix < visina - 1 && noviy < sirina - 1 &&
            labirint[novix][noviy] == zid && !ima_susjeda(novix, noviy) && nema_dijagonala(novix, noviy, i))return true;
    }
    return false;
}
void povezi(int x, int y)
{
    vector<vector<int> >smjer = { {0,1,0},{0,-1,1},{1,0,2},{-1,0,3} };
    random_shuffle(smjer.begin(), smjer.end());
    while (true) {
        int sx = smjer.back()[0];
        int sy = smjer.back()[1];
        int novix = x + sx, noviy = y + sy;
        if (novix && noviy && novix < visina - 1 && noviy < sirina - 1 && labirint[novix][noviy] == zid &&
            !ima_susjeda(novix, noviy) && nema_dijagonala(novix, noviy, smjer.back()[2])) {

            labirint[novix][noviy] = prolaz;
            put.push_back(make_pair(novix, noviy));
            ///radi estetike povezujemo drugi put u istom smjeru ako je moguce
            if (novix + sx && noviy + sy && novix + sx < visina - 1 && noviy + sy < sirina - 1 && labirint[novix + sx][noviy + sy] == zid &&
                !ima_susjeda(novix + sx, noviy + sy) && nema_dijagonala(novix + sx, noviy + sy, smjer.back()[2])) {

                labirint[novix + sx][noviy + sy] = prolaz;
                put.push_back(make_pair(novix + sx, noviy + sy));
            }
            break;
        }
        smjer.pop_back();
    }
}
void rjesavanje_labirinta()
{
    ///bfs
    list<pair<pair<int, int>, string> > staza;
    string s = "";
    char smjer[4] = { 'd','a','s','w' };
    staza.push_back(make_pair(make_pair(1, 1), s));
    labirint[0][1] = '.';
    labirint[1][1] = '.';
    bool bio[visina][sirina] = { false };
    bio[1][1] = true;
    while (staza.size()) {
        int x = staza.back().first.first;
        int y = staza.back().first.second;
        s = staza.back().second;
        staza.pop_back();
        if (x == visina - 2 && y == sirina - 2)break;//nasli smo rjesenje
        for (int i = 0; i < 4; i++) {
            int novix = x + smjerx[i];
            int noviy = y + smjery[i];
            if (!bio[novix][noviy] && labirint[novix][noviy] == prolaz)
            {
                staza.push_front(make_pair(make_pair(novix, noviy), s + smjer[i]));
                bio[novix][noviy] = true;
            }
        }
    }
    labirint[visina - 2][sirina - 2] = '.';
    labirint[visina - 1][sirina - 2] = '.';
    int x = 1, y = 1;
    for (int i = 0; i < s.length(); i++)
    {
        labirint[x][y] = '.';
        if (s[i] == 'd')y++;
        if (s[i] == 'a')y--;
        if (s[i] == 's')x++;
        if (s[i] == 'w')x--;
    }
    cout << endl << "Najkraci put: " << endl;
    for (int i = 0; i < visina; i++)
    {
        for (int j = 0; j < sirina; j++)
        {
            if (labirint[i][j] == '.') { postavi_boju(crvena2); cout << labirint[i][j]; }
            else { postavi_boju(bijela); cout << labirint[i][j]; }
        }
        cout << endl;
        Sleep(10);
    }
    postavi_boju(bijela);
    cout << "Broj koraka u najkracem putu: " << s.size() << endl;
}
void unos()
{
    if (GetAsyncKeyState(VK_UP)) {
        if (a != 0 && labirint[a - 1][b] == prolaz) {
            posjetio[--a][b] = true;
            ispis(a, b);
        }
    }
    if (GetAsyncKeyState(VK_DOWN)) {
        if (labirint[a + 1][b] == prolaz) {
            posjetio[++a][b] = true;
            ispis(a, b);
        }
    }
    if (GetAsyncKeyState(VK_LEFT)) {
        if (labirint[a][b - 1] == prolaz) {
            posjetio[a][--b] = true;
            ispis(a, b);
        }
    }
    if (GetAsyncKeyState(VK_RIGHT)) {
        if (labirint[a][b + 1] == prolaz) {
            posjetio[a][++b] = true;
            ispis(a, b);
        }
    }
    if (GetAsyncKeyState('x')) {
        a = visina - 1; b = sirina - 1;
        return;
    }
}
int main()
{
    setup();
    system("title Labirint");
    srand(time(NULL));
    font(17);
    MoveWindow(GetConsoleWindow(), (GetSystemMetrics(SM_CXSCREEN) - sirina * 12) / 2, (GetSystemMetrics(SM_CYSCREEN) - visina * 12 - 400) / 2,
        sirina * 15, visina * 2 * 15, TRUE);
    while (put.size()) {
        int x = put.back().first;
        int y = put.back().second;
        if (moze_se_povezati(x, y))povezi(x, y);
        else {
            put.pop_back();
        }
    }
    labirint[0][1] = prolaz;
    labirint[1][1] = prolaz;
    labirint[visina - 1][sirina - 2] = prolaz;

    posjetio[0][1];
    ispis(a, b);
    while (a != visina - 1 || b != sirina - 2)
    {
        unos();
    }
    system("cls");
    int brkoraka = 0;
    for (int i = 0; i < labirint.size(); i++) {
        for (int j = 0; j < labirint[i].size(); j++)
        {
            if (posjetio[i][j]) { cout << '.'; brkoraka++; }
            else cout << labirint[i][j];
        }
        cout << endl;
    }
    rjesavanje_labirinta();
    cout << endl << "Broj vasih koraka: " << brkoraka << endl;
    system("pause");
    return 0;
}
