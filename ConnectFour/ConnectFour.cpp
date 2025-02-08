#include<iostream>
#include<windows.h>
#include<iomanip>

using namespace std;

const int visina = 6, sirina = 7;
const char coin = char(254), blok = char(219);
const int zuta = 14, crvena = 12, siva = 8, bijela = 15;

int ploca[visina + 1][sirina + 1], igrac = 12, x = 1, y = 2;
bool gameover = false;

HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

void PostaviBoju(int boja)
{
    SetConsoleTextAttribute(handle, boja);
}

void font(int VelicinaFonta, bool Vidljivost)
{
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    cfi.dwFontSize.X = VelicinaFonta;
    cfi.dwFontSize.Y = VelicinaFonta;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    SetCurrentConsoleFontEx(handle, false, &cfi);

    CONSOLE_CURSOR_INFO cci;
    cci.bVisible = Vidljivost;
    cci.dwSize = 100;
    SetConsoleCursorInfo(handle, &cci);
}
void crtaj(char znak, int pozX, int pozY)
{
    COORD poz = { pozX,pozY };
    SetConsoleCursorPosition(handle, poz);
    cout << znak;
}
void crtaj(string znak, int pozX, int pozY)
{
    COORD poz = { pozX,pozY };
    SetConsoleCursorPosition(handle, poz);
    cout << znak;
}

void setup()
{
    system("cls");
    crtaj("Arrows left,right to move,",sirina+8, 5);
    crtaj("SPACE or ARROW DOWN to select a spot", sirina + 8, 6);
    PostaviBoju(siva);
    crtaj(' ', 0, 0);
    cout << endl << endl << endl << endl;
    for (int i = 0; i < visina * 2; i++) {
        for (int j = 0; j < sirina * 2 + 1; j++) {
            if (i % 2 || !(j % 2))cout << blok;
            else cout << " ";
        }
        cout << endl;
    }

    for (int i = 0; i <= visina; i++)
        for (int j = 0; j <= sirina; j++)
            ploca[i][j] = 0;
    gameover = false;
}

bool pobjeda()
{
    bool Pobjeda = false;
    int dijBrojac1 = 0, horBrojac = 0, verBrojac, nulaBrojac = 0, dijBrojac2 = 0;;
    for (int i = 0; i < visina; i++)
    {
        for (int j = 0; j < sirina; j++)
        {
            verBrojac = 0; horBrojac = 0, dijBrojac1 = 0, dijBrojac2 = 0;
            for (int k = 0; k < 4; k++) {
                if (i + k < visina && ploca[i + k][j] == igrac)verBrojac++;
                if (j + k < sirina && ploca[i][j + k] == igrac)horBrojac++;
                if (ploca[i - k][j + k] == igrac)dijBrojac1++;
                if (ploca[i - k][j - k] == igrac)dijBrojac2++;
                if (ploca[i][j] == 0)nulaBrojac++;
            }
            if (dijBrojac1 == 4)Pobjeda = true;
            if (verBrojac == 4)Pobjeda = true;
            if (horBrojac == 4)Pobjeda = true;
            if (dijBrojac2 == 4)Pobjeda = true;
        }
    }
    if (nulaBrojac == 0) {
        crtaj('\n', 0, visina * 2 + 5);
        cout << "REMI" << endl;
        igrac = 0;
        return true;
    }
    return Pobjeda;

}
void pomjeri(int i)
{
    PostaviBoju(igrac);
    while (true) {
        if (GetAsyncKeyState(VK_LEFT) && x != 1) {
            crtaj(' ', x, y);
            x -= 2;
            crtaj(coin, x, y);
            return;
        }
        if (GetAsyncKeyState(VK_RIGHT) && x < sirina * 2 - 1) {
            crtaj(' ', x, y);
            x += 2;
            crtaj(coin, x, y);
            return;
        }
        if ((GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState(VK_SPACE) && ploca[0][x / 2] == 0))
        {
            crtaj(' ', x, y);
            i = visina - 1;
            while (ploca[i][x / 2] != 0)i--;
            ploca[i][x / 2] = igrac;
            crtaj(coin, x, i * 2 + 4);

            if (pobjeda()) { gameover = true; return; }
            else {
                if (igrac == crvena)igrac = zuta;
                else igrac = crvena;
                x = 1; y = 2;
                PostaviBoju(igrac);
                crtaj(coin, x, y);
                Sleep(200);
                return;
            }
        }
    }
}

int main()
{
    system("mode 45,30");
    system("title Connect Four");
    font(25, false);
    while (true)
    {

        setup();
        PostaviBoju(crvena);
        crtaj(coin, 1, 2);
        while (!gameover) {
            pomjeri(igrac);
            Sleep(200);
        }
        crtaj('\n', 0, visina * 2 + 5);
        if (igrac != 0)
            cout << "Pobjednik je ";
        if (igrac == crvena)cout << "CRVENI" << endl;
        else if (igrac == zuta)cout << "ZUTI" << endl;

        bool playAgain = true;
        cout << "Play again - ENTER" << endl;
        cout << "Exit- Esc" << endl;
        while (true)
        {
            if (GetAsyncKeyState(VK_RETURN))break;
            if (GetAsyncKeyState(VK_ESCAPE)) { playAgain = false; break; }
        }

        if (!playAgain)break;
    }
    getchar();
    return 0;
}
