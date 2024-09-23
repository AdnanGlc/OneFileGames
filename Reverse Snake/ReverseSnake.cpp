#include<iostream>
#include<ctime>
#include<windows.h>
#include<list>
#include<map>

using namespace std;

const int visina=20,sirina=20;
const char cHrana=char(254),cZmija=char(178),zid=char(219);
const int plava=9,crvena=4,bijela=7,roza=12,zelena=10;

const int smjerx[4]={0,0,1,-1};
const int smjery[4]={-1,1,0,0};
const char Csmjer[4]={'w','s','d','a'};

char ploca[visina][sirina];
int IgracX=visina/2+2,IgracY=sirina/2;
bool gameover=false;

HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

struct Zmija
{
    list<pair<int,int> >rep;
    int boja=crvena;
    bool obrisi;
    float brzina=108.0;
    void Zpomjeri(char smjer);
}zmija;

void PostaviBoju(int boja){
    SetConsoleTextAttribute(handle,boja);
}
void font(int Font,bool Vidljivost)
{
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize=sizeof(cfi);
    cfi.dwFontSize.X=Font;
    cfi.dwFontSize.Y=Font;
    cfi.FontFamily=FF_DONTCARE;
    cfi.FontWeight=FW_NORMAL;

    CONSOLE_CURSOR_INFO cci;
    cci.bVisible=Vidljivost;
    cci.dwSize=100;

    SetCurrentConsoleFontEx(handle,false,&cfi);
    SetConsoleCursorInfo(handle,&cci);
}

void crtaj(char znak,int X,int Y)
{
    COORD Poz={X,Y};
    SetConsoleCursorPosition(handle,Poz);
    cout<<znak;
}

void stvori_hranu()
{
    while(true){
        int r1=rand()%(visina-2)+1;
        int r2=rand()%(sirina-2)+1;

        if(ploca[r1][r2]!=cHrana && ploca[r1][r2]!=cZmija){
            ploca[r1][r2]=cHrana;
            PostaviBoju(plava);
            crtaj(cHrana,r1,r2);
            break;
        }
    }
}
void PojeoHranu(int pozX,int pozY,char igrac)
{
    if(pozX==IgracX && pozY==IgracY)gameover=true;
    if(ploca[pozX][pozY]==cHrana && igrac==cHrana){
        crtaj(' ',zmija.rep.back().first,zmija.rep.back().second);
        ploca[zmija.rep.back().first][zmija.rep.back().second]=' ';
        zmija.rep.pop_back();
        stvori_hranu();
    }
    if(ploca[pozX][pozY]==cHrana && igrac==cZmija){
       zmija.obrisi=false;
       ploca[pozX][pozY]=' ';
       stvori_hranu();
    }
}
char bfs(int glavaX,int glavaY){
    bool bio[visina][sirina]={0};
    //inicijalizacija
    list<pair< pair<int,int>, string > >put;
    put.push_back(make_pair(make_pair(glavaX,glavaY),"x"));
    string staza;
    while(put.size())
    {
        int x=put.front().first.first;
        int y=put.front().first.second;
        staza=put.front().second;
        put.pop_front();
        if(ploca[x][y]==cHrana)break;//nasli smo najkraci put do hrane
        for(int i=0;i<4;i++){
            int novix=x+smjerx[i];
            int noviy=y+smjery[i];
            if(ploca[novix][noviy]!=zid && ploca[novix][noviy]!=cZmija && !bio[novix][noviy]){
              bio[novix][noviy]=true;
              put.push_back(make_pair(make_pair(novix,noviy),staza+Csmjer[i]));
            }
        }

    }
    return staza[1];
}
void Zmija::Zpomjeri(char smjer)
{
    map<char,pair<int,int> > pomak;
    pomak['w']=make_pair(0,-1);//gore
    pomak['s']=make_pair(0,1);//dolje
    pomak['d']=make_pair(-1,0);//lijevo
    pomak['a']=make_pair(1,0);//desno

    int noviX=zmija.rep.front().first-pomak[smjer].first;
        int noviY=zmija.rep.front().second+pomak[smjer].second;
        PojeoHranu(noviX,noviY,cZmija);
        PostaviBoju(zmija.boja);
        crtaj(cZmija,noviX,noviY);
        ploca[noviX][noviY]=cZmija;
        zmija.rep.push_front(make_pair(noviX,noviY));
        if(zmija.obrisi){
                crtaj(' ',zmija.rep.back().first,zmija.rep.back().second);
        ploca[ zmija.rep.back().first ][zmija.rep.back().second ]=' ';
        zmija.rep.pop_back();
                }
        else zmija.obrisi=true;
}
void pomjeri()
{
    if(GetAsyncKeyState(VK_UP)&& IgracY!=1 && ploca[IgracX][IgracY-1]!=cZmija){ ///pomjeranje gore
        PojeoHranu(IgracX,IgracY-1,cHrana);
        PostaviBoju(zelena);
        ploca[IgracX][IgracY]=' ';
        crtaj(' ',IgracX,IgracY);
        ploca[IgracX][--IgracY]=cHrana;
        crtaj(cHrana,IgracX,IgracY);
        crtaj(' ',0,visina);
    }
    if(GetAsyncKeyState(VK_DOWN)&& IgracY!=visina-2 && ploca[IgracX][IgracY+1]!=cZmija){ ///pomjeranje dolje
        PojeoHranu(IgracX,IgracY+1,cHrana);
        PostaviBoju(zelena);
        ploca[IgracX][IgracY]=' ';
        crtaj(' ',IgracX,IgracY);
        ploca[IgracX][++IgracY]=cHrana;
        crtaj(cHrana,IgracX,IgracY);
        crtaj(' ',0,visina);
    }
    if(GetAsyncKeyState(VK_RIGHT)&& IgracX!=sirina-2 && ploca[IgracX+1][IgracY]!=cZmija){ ///pomjeranje desno
        PojeoHranu(IgracX+1,IgracY,cHrana);
        PostaviBoju(zelena);
        ploca[IgracX][IgracY]=' ';
        crtaj(' ',IgracX,IgracY);
        ploca[++IgracX][IgracY]=cHrana;
        crtaj(cHrana,IgracX,IgracY);
        crtaj(' ',0,visina);
    }
     if(GetAsyncKeyState(VK_LEFT)&& IgracX!=1 && ploca[IgracX-1][IgracY]!=cZmija){ ///pomjeranje lijevo
        PojeoHranu(IgracX-1,IgracY,cHrana);
        PostaviBoju(zelena);
        ploca[IgracX][IgracY]=' ';
        crtaj(' ',IgracX,IgracY);
        ploca[--IgracX][IgracY]=cHrana;
        crtaj(cHrana,IgracX,IgracY);
        crtaj(' ',0,visina);
    }
}

void setup()
{
    system("mode 25,25");
    system("title Reverse Snake");
    gameover=false;
    IgracX=visina/2+2;IgracY=sirina/2;
    system("cls");
    font(20,false);
    for(int i=0;i<visina;i++){
        for(int j=0;j<sirina;j++){
            if(!i || !j || i==visina-1 || j==sirina-1){cout<<zid; ploca[i][j]=zid;}
            else {cout<<" ";ploca[i][j]=' ';}
        }
        cout<<endl;
    }
    PostaviBoju(zelena);
    crtaj(cHrana,IgracX,IgracY);
    PostaviBoju(zmija.boja);
    while(zmija.rep.size())zmija.rep.pop_back();
    for(int i=0;i<4;i++){
            zmija.rep.push_back(make_pair(visina/2-1-i,sirina/2));
            ploca[visina/2-1-i][sirina/2]=cZmija;
            crtaj(cZmija,visina/2-1-i,sirina/2);
    }
    zmija.obrisi=true;
    crtaj(' ',0,visina);

    stvori_hranu();
}
int main()
{
    srand(time(NULL));
    pocetak:
    setup();

    crtaj('\r',0,visina);
    PostaviBoju(0);
    system("pause");
    clock_t start=clock()/2,kraj;
    while(zmija.rep.size() && !gameover){
        pomjeri();
        kraj=clock();
        if( float(kraj-start)/float(CLOCKS_PER_SEC/1000)>zmija.brzina && zmija.rep.size()){
        zmija.Zpomjeri(bfs(zmija.rep.front().first,zmija.rep.front().second));
        start=clock();
        if(zmija.rep.size()==1)zmija.brzina=109.0;
        else zmija.brzina=99.0;
        }
        Sleep(100);
    }
    crtaj('\r',0,visina);
    PostaviBoju(7);
    if(gameover)cout<<"GAMEOVER"<<endl;
    else cout<<"POBIJEDILI STE"<<endl;
    cout<<"Play again- ENTER"<<endl;
    cout<<"Exit game- ESC"<<endl;
    while(true)
    {
      if(GetAsyncKeyState('\r'))goto pocetak;
      if(GetAsyncKeyState(VK_ESCAPE))break;
    }
    return 0;
}
