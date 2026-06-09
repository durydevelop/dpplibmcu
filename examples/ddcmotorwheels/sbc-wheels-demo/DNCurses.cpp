#include "DNCurses.h"
#include <memory>

DNCurses::DNCurses()
{
    Init();
}

DNCurses::~DNCurses()
{
    echo();
    curs_set(1);
    nocbreak();
    keypad(stdscr, false);
    nl();
    endwin();
}

void DNCurses::Init(void)
{
    initscr();      // inizializza la libreria curses
    nonl();         // non convertire NL->CR/NL in output
    cbreak();       // prende i caratteri in input uno alla volta, senza attendere il \n
    curs_set(0);    // Cursore OFF
    noecho();       // nessuna echo dell'input
    keypad(stdscr,true);  // Mappa i tasti KEY_...
    intrflush(stdscr,true);

    if (has_colors())
    {
        start_color();

        // Semplici colori, di solito tutto ciò che serve.
        init_pair(COLOR_BLACK, COLOR_BLACK, COLOR_BLACK);
        init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
        init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
        init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
        init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
        init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
        init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
    }

}

DNCurses::DWindow* DNCurses::CreateWindow(short int Line, short int Col, short int Height, short int Width)
{
    DWindow *win=new DWindow(Line,Col,Height,Width);
    return win;
}

DNCurses::DWindow::~DWindow()
{
    wborder(pWindow,' ',' ',' ',' ',' ',' ',' ',' ');
    wclear(pWindow);
    wrefresh(pWindow);
    if (pWindow != nullptr) {
        delwin(pWindow);
    }
}

DNCurses::DWindow::DWindow(short int Line, short int Col, short int Height, short int Width)
{
    pWindow=newwin(Height,Width,Line,Col);
    keypad(pWindow,true);
}

void DNCurses::DWindow::AddBox(chtype vLine, chtype hLine)
{
    box(pWindow,vLine,hLine);
}

void DNCurses::DWindow::SetBackground(chtype colorPair)
{
    //init_pair(COLOR_BLUE, COLOR_WHITE,COLOR_BLUE);
    //wbkgd(wDiag,COLOR_PAIR(COLOR_BLUE));
    wbkgd(pWindow,colorPair);
}

void DNCurses::DWindow::Update(void)
{
    //wrefresh(pWindow);
    touchwin(pWindow);
}

void DNCurses::DWindow::Printf(short int Line, short int Col, const std::string fmt, ...)
{
    va_list args;
    va_start(args,fmt);
    size_t len=vsnprintf(nullptr,0,fmt.c_str(),args);
    std::unique_ptr<char[]> msg(new char[len+1]);
    vsprintf(msg.get(),fmt.c_str(),args);
    mvwprintw(pWindow,Line,Col,"%s",msg.get());
    va_end(args);
}

void DNCurses::DWindow::Printf(bool DoUpdate, short int Line, short int Col, const std::string fmt, ...)
{
    va_list args;
    va_start(args,fmt);
    size_t len=vsnprintf(nullptr,0,fmt.c_str(),args);
    std::unique_ptr<char[]> msg(new char[len+1]);
    vsprintf(msg.get(),fmt.c_str(),args);
    mvwprintw(pWindow,Line,Col,"%s",msg.get());
    va_end(args);
    if (DoUpdate) {
        wrefresh(pWindow);
    }
}

void DNCurses::DWindow::SetKbTimeout(short int Millis)
{
    wtimeout(pWindow,Millis);
}

chtype DNCurses::DWindow::Getch(void)
{
    return(wgetch(pWindow));
}

void DNCurses::DWindow::Clean(void)
{
    wclear(pWindow);
    wrefresh(pWindow);
}
