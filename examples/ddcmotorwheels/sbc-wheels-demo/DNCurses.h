#ifndef DNCURSES_H
#define DNCURSES_H

#include <ncurses.h>
#include <string>

#define KEY_ESC 27

class DNCurses {
    public:
        enum KB_TIMEOUT {KB_TIMEOUT_INFINITE = -1, KB_NO_TIMEOUT = 0};
        class DWindow {
            public:
                DWindow(short int Line, short int Col, short int Height, short int Width);
                ~DWindow();
                void AddBox(chtype vLine=ACS_VLINE, chtype hLine=ACS_HLINE);
                void SetBackground(chtype ColorPair);
                void Update(void);
                void Hide(void);
                void Printf(short int Line, short int Col, const std::string fmt, ...);
                void Printf(bool DoUpdate, short int Line, short int Col, std::string fmt, ...);
                void SetKbTimeout(short int Millis);
                chtype Getch(void);
                void Clean(void);
                WINDOW* pWindow;
        };

        DNCurses();
        ~DNCurses();
        void Init(void);
        DWindow* CreateWindow(short int Line, short int Col, short int Height, short int Width);


    protected:
    private:
};

#endif // DNCURSES_H
