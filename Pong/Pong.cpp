#include<iostream>
#include<windows.h>
#include<ctime>
#include<list>
#include<iomanip>
#include<conio.h>

using namespace std;

const int visina = 30, sirina = 45;///max visina=40,sirina=52
const int bijela = 7, crvena = 12;
const char zid = char(219), lopta = char(254), b1 = char(222), b2 = char(221);

list<int> p1, p2;
int x, y, smjerx = 1, smjery = 1;
bool gameover;

void crtaj(int x, int y, char c)
{
    LockWindowUpdate(GetConsoleWindow());
    COORD poz;
    poz.X = x;
    poz.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), poz);
    if (c == lopta) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
        cout << lopta;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    }
    else cout << c;
    poz.X = 0;
    poz.Y = visina + 1;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), poz);
    LockWindowUpdate(NULL);
}
void font(int VelicinaFonta)
{
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    cfi.dwFontSize.X = VelicinaFonta;
    cfi.dwFontSize.Y = VelicinaFonta;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), false, &cfi);

    CONSOLE_CURSOR_INFO cci;
    cci.bVisible = false;
    cci.dwSize = 100;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cci);
}
void setup()
{
    system("cls");
    for (int i = 0; i < visina; i++) {
        for (int j = 0; j < sirina; j++) {
            if (!i || !j || i == visina - 1 || j == sirina - 1)cout << zid;
            else cout << ' ';
        }
        cout << endl;
    }
    cout << "Igrac 1 w,s" << setw(sirina - 11) << "Igrac 2 UP,DOWN" << endl;
    p1.clear();
    p2.clear();
    for (int i = visina / 2 - 1; i < visina / 2 + 3; i++) {
        p1.push_back(i);
        p2.push_back(i);
        crtaj(1, i, b1);
        crtaj(sirina - 2, i, b2);
    }
    x = visina / 2; y = sirina / 2;
    crtaj(y, x, lopta);
    gameover = false;
}
void pomjeri(int igrac, int smjer)
{
    int igrac1 = 1, igrac2 = 2, gore = 1, dolje = 2;
    if (igrac == igrac1) {
        int x = p1.front();
        int y = p1.back();
        if (smjer == gore && x > 1) {
            crtaj(1, y, ' ');
            p1.pop_back();
            crtaj(1, x - 1, b1);
            p1.push_front(x - 1);
        }
        if (smjer == dolje && y < visina - 2) {
            crtaj(1, x, ' ');
            p1.pop_front();
            crtaj(1, y + 1, b1);
            p1.push_back(y + 1);
        }
    }
    if (igrac == igrac2) {
        int x = p2.front();
        int y = p2.back();
        if (smjer == gore && x > 1) {
            crtaj(sirina - 2, y, ' ');
            p2.pop_back();
            crtaj(sirina - 2, x - 1, b2);
            p2.push_front(x - 1);
        }
        if (smjer == dolje && y < visina - 2) {
            crtaj(sirina - 2, x, ' ');
            p2.pop_front();
            crtaj(sirina - 2, y + 1, b2);
            p2.push_back(y + 1);
        }
    }
}
void unos()
{
    int igrac1 = 1, igrac2 = 2, gore = 1, dolje = 2;
    if (GetAsyncKeyState('w') || GetAsyncKeyState('W'))pomjeri(igrac1, gore);
    if (GetAsyncKeyState('s') || GetAsyncKeyState('S'))pomjeri(igrac1, dolje);
    if (GetAsyncKeyState(VK_UP))pomjeri(igrac2, gore);
    if (GetAsyncKeyState(VK_DOWN))pomjeri(igrac2, dolje);
}
void pomjeri_loptu()
{
    LockWindowUpdate(GetConsoleWindow());
    crtaj(y, x, ' ');
    if (x + smjerx == 0)smjerx = 1;
    else if (x + smjerx == visina - 3)smjerx = -1;
    if (y + smjery == 1) {
        if (x<p1.front() || x>p1.back()) { gameover = true; return; }
        smjery = 1;
    }
    else if (y + smjery == sirina - 2) {
        if (x<p2.front() || x>p2.back()) { gameover = true; return; }
        smjery = -1;
    }
    x += smjerx;
    y += smjery;
    crtaj(y, x, lopta);
    LockWindowUpdate(NULL);
}
int main()
{
    srand(time(NULL));
    font(15);
    system("Mode 45,33");
ponovo:
    setup();
    while (!gameover) {
        unos();
        pomjeri_loptu();
        Sleep(40);
    }
    if (gameover) {
        system("cls");
        for (int i = 0; i < visina / 2; i++)cout << endl;
        if (y > 5)cout << setw(8) << "     POBJEDINK JE IGRAC 1" << endl;
        else   cout << setw(8) << "     POBJEDINK JE IGRAC 2" << endl;
    }
    cout << endl << "     PONOVO-enter" << endl << "\tKRAJ-esc" << endl;
    while (true) {
        LockWindowUpdate(NULL);
        if (GetAsyncKeyState('\r'))goto ponovo;
        if (GetAsyncKeyState(VK_ESCAPE))break;
    }
    return 0;
}
