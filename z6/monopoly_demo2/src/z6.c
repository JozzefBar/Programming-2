//#include <ncurses/curses.h> //pre mingw
#include <curses.h>           //pre cygwin a linux
#include <string.h>
#include <stdlib.h>
#include <time.h>             //pre nahodny generator
#include "monopoly.h"

#define WIDTH_CARD 20         //sirka jednej karty
#define HEIGHT_CARD 7         //vyska jednej karty
#define HEIGHT_LABEL_CARD 2   //vyska (z hora) predelovacej ciary na farbu
#define NUM_CARD_IN_ROW (NUM_SPACES/4) //pocet kariet v jednom riadku

// definovanie nazvov pre farebne objekty - musi zacinat od 1
#define COL_LABEL_BROWN 1
#define COL_LABEL_SKYBLUE 2
#define COL_LABEL_PURPLE 3
#define COL_LABEL_ORANGE 4
#define COL_LABEL_RED 5
#define COL_LABEL_YELLOW 6
#define COL_LABEL_GREEN 7
#define COL_LABEL_BLUE 8
#define COL_LABEL_NOCOLOR 9
#define COL_NOPLAYER 10
#define COL_PLAYER1 11

// definovanie nazvov pre farby
#define COL_BROWN 17
#define COL_SKYBLUE 18
#define COL_PURPLE 19
#define COL_ORANGE 20
#define COL_RED 21
#define COL_YELLOW 22
#define COL_GREEN 23
#define COL_BLUE 24



struct _win_st *create_property_card(int starty, int startx, SPACE game_space);
void drawGameBoard(struct _win_st **win, SPACE *gameboard, int maxy, int maxx);
void drawPlayer(struct _win_st **win, int act_location, int new_location);
void init_custom_color_pair();
int getCubeNumber(int lower, int upper);
int genNumDigits(int number);
int get_new_location(int actual_location, int dice_roll);

int main(void){
    struct _win_st *win[NUM_SPACES];
    int maxy, maxx;
    int c=0;
    int location=0, new_location=0;
    int dice_roll=0;

    //srand(time(0));   //inicializacia nahodneho generatora cisel
    srand(0);   //inicializacia nahodneho generatora cisel = vzdy rovnake nahodne c.
    initscr();			// Start curses mode
    noecho();
    cbreak();
    keypad(stdscr,TRUE);
    getmaxyx(stdscr,maxy,maxx);   //vrati maximalny pocet riadkov a stlpcov konzoly
    if(has_colors() == FALSE)     //test, ci konzola podporuje farby
    {	endwin();
        printf("Your terminal does not support color\n");
        exit(1);
    }

    //definovanie vlastnych farieb a farebnych parov (text, pozadie)
    init_custom_color_pair();
    mvprintw(0,0,"Number of supported colors: %d", COLORS);
    mvprintw(1,0,"hod kockou: 'c'  Quit: 'Esc'");
    refresh();

    //nakreslenie hracieho planu monopolov - pre 24 policok
    drawGameBoard(win, game_board, maxy, maxx);
    //umiestnenie hraca na START
    drawPlayer(win, location, new_location);

    //hra moze zacat...
    while (1) {
        c = getch();
        // hod kockou
        if (c == 'c') {
            dice_roll = getCubeNumber(1,6);  //nahodne cislo od 1 - 6
            new_location = get_new_location(location, dice_roll); //vypocet novej polohy hraca (osetrenie prejdenia cez START)
            drawPlayer(win,location, new_location);   //prekreslenie polohy hraca
            //debug vypisy
            mvprintw(maxy-1, 0,"old_loc: %2d  new_loc: %2d", location, new_location);
            mvprintw(maxy-2, 0,"hod kockou: %d", dice_roll);
            location = new_location;
        }
            // koniec hry
        else if (c == 27){
            mvprintw(maxy, 0,"exiting...");
            refresh();
            //napms(500);
            break;
        }
    }
    endwin();			// End curses modeeturn 0;
    return 0;
}

void drawPlayer(struct _win_st **win, int act_location, int new_location){
    wattron(win[act_location], COLOR_PAIR(COL_NOPLAYER));
    mvwprintw(win[act_location],HEIGHT_CARD-2,2,"  ");
    wattroff(win[act_location], COLOR_PAIR(COL_NOPLAYER));

    wattron(win[new_location], COLOR_PAIR(COL_PLAYER1));
    mvwprintw(win[new_location],HEIGHT_CARD-2,2,":)");
    wattroff(win[new_location], COLOR_PAIR(COL_PLAYER1));

    wrefresh(win[act_location]);
    wrefresh(win[new_location]);
}
// vykreslenie policka MONOPOLU cez strukturu WINDOW
struct _win_st *create_property_card(int starty, int startx, SPACE game_space){

    struct _win_st *local_win;

    local_win = newwin(HEIGHT_CARD, WIDTH_CARD, starty, startx);
    //vonkajsie ohranicenie
    box(local_win, ACS_VLINE , ACS_HLINE);

    //label oddelovac
    mvwhline(local_win, HEIGHT_LABEL_CARD, 1, ACS_HLINE, WIDTH_CARD-1);
    mvwaddch(local_win, HEIGHT_LABEL_CARD, 0, ACS_LTEE);
    mvwaddch(local_win, HEIGHT_LABEL_CARD, WIDTH_CARD-1, ACS_RTEE);

    //zafarbenie label-u
    if(game_space.type==Property){
        wattron(local_win, COLOR_PAIR(game_space.property->color+1));
        for (int y = 1; y < HEIGHT_LABEL_CARD; y++){
            for(int x = 1; x < WIDTH_CARD-1; x++){
                mvwprintw(local_win, y, x," ");
            }
        }
    }
    else{
        wattron(local_win, COLOR_PAIR(COL_LABEL_NOCOLOR));
        for (int y = 1; y < HEIGHT_LABEL_CARD; y++){
            for(int x = 1; x < WIDTH_CARD-1; x++){
                mvwprintw(local_win, y, x," ");
            }
        }
    }

    //nastavenie textu: farba
    if (game_space.type == Property) {
        mvwprintw(local_win, (HEIGHT_LABEL_CARD / 2),
                  (WIDTH_CARD - strlen(property_colors[game_space.property->color])) / 2, "%s",
                  property_colors[game_space.property->color]);

        //nastavenie textu: nazov, cena
        wattroff(local_win, COLOR_PAIR(game_space.property->color+1));
        mvwprintw(local_win, (HEIGHT_LABEL_CARD + 1), (WIDTH_CARD - strlen(game_space.property->name)) / 2, "%s",
                  game_space.property->name);
        mvwprintw(local_win, (HEIGHT_LABEL_CARD + 3), (WIDTH_CARD - genNumDigits(game_space.property->price)) / 2, "%d",
                  game_space.property->price);
    }
    else{
        mvwprintw(local_win, (HEIGHT_LABEL_CARD / 2),
                  (WIDTH_CARD - strlen(space_types[game_space.type])) / 2, "%s",
                  space_types[game_space.type]);
        wattroff(local_win, COLOR_PAIR(COL_LABEL_NOCOLOR));
    }
    wrefresh(local_win);		/* Show that box 		*/

    return local_win;
}
void drawGameBoard(struct _win_st **win, SPACE *gameboard, int maxy, int maxx) {
    char cost[3]={'\0'};
    int n = 0;
    for (int i = 0; i < NUM_SPACES; i++) {
        sprintf(cost, "%d", i);
        // juh 0 - 5
        if (i >= 0 && i < NUM_CARD_IN_ROW) {
            win[i] = create_property_card(maxy - HEIGHT_CARD, maxx - WIDTH_CARD * (n + 1), gameboard[i]);
            n++;
            if (n >= NUM_CARD_IN_ROW) {
                n = 0;
            }
        }
        // zapad 6 - 11
        if (i >= 1 * NUM_CARD_IN_ROW && i < 2 * NUM_CARD_IN_ROW) {
            //win[i] = create_property_card(maxy-HEIGHT_CARD*((i-NUM_CARD_IN_ROW+1)), maxx-WIDTH_CARD*(NUM_CARD_IN_ROW+1),"Cirkus", "red", cost );
            win[i] = create_property_card(maxy - HEIGHT_CARD * (n + 1), maxx - WIDTH_CARD * (NUM_CARD_IN_ROW + 1),gameboard[i]);
            n++;
            if (n >= NUM_CARD_IN_ROW) {
                n = NUM_CARD_IN_ROW;
            }
        }

        // sever 12 - 17
        if (i >= 2 * NUM_CARD_IN_ROW && i < 3 * NUM_CARD_IN_ROW) {
            //win[i] = create_property_card(maxy-HEIGHT_CARD*(NUM_CARD_IN_ROW+1), maxx-WIDTH_CARD*( NUM_CARD_IN_ROW*(NUM_CARD_IN_ROW-3)-i+1),"Cirkus", "red", cost );
            win[i] = create_property_card(maxy - HEIGHT_CARD * (NUM_CARD_IN_ROW + 1), maxx - WIDTH_CARD * (n + 1),gameboard[i]);
            n--;
            if (n <= 0) {
                n = NUM_CARD_IN_ROW;
            }
        }
        // vychod 18 - 23
        if (i >= 3 * NUM_CARD_IN_ROW && i < 4 * NUM_CARD_IN_ROW) {
            //win[i] = create_property_card(maxy-HEIGHT_CARD*(NUM_CARD_IN_ROW*(NUM_CARD_IN_ROW-2)-i+1), maxx-WIDTH_CARD,"Cirkus", "red", cost );
            win[i] = create_property_card(maxy - HEIGHT_CARD * (n + 1), maxx - WIDTH_CARD, gameboard[i]);
            n--;
            if (n <= 0) {
                n = NUM_CARD_IN_ROW;
            }
        }
    }
}
void init_custom_color_pair(){
    int r=4; //po spravnosti by malo byt r=1000/255 => 3.92
    start_color();
    //inicializacia vlastnych farebnych parov (text,pozadie)
    init_color(COL_BROWN, 139*r, 69*r, 19*r);
    init_color(COL_SKYBLUE, 65*r, 105*r, 225*r);
    init_color(COL_PURPLE, 128*r, 128*r, 0*r);
    init_color(COL_ORANGE, 250*r, 128*r, 0*r);
    init_color(COL_RED, 249*r, 0*r, 0*r);
    init_color(COL_YELLOW, 249*r, 249*r, 0*r);
    init_color(COL_GREEN, 0*r, 100*r, 0*r);
    init_color(COL_BLUE, 0*r, 0*r, 249*r);
    init_pair(COL_LABEL_BROWN, COLOR_WHITE, COL_BROWN);
    init_pair(COL_LABEL_SKYBLUE, COLOR_WHITE, COL_SKYBLUE);
    init_pair(COL_LABEL_PURPLE, COLOR_WHITE, COL_PURPLE);
    init_pair(COL_LABEL_ORANGE, COLOR_WHITE, COL_ORANGE);
    init_pair(COL_LABEL_RED, COLOR_WHITE, COL_RED);
    init_pair(COL_LABEL_YELLOW, COLOR_BLACK, COL_YELLOW);
    init_pair(COL_LABEL_GREEN, COLOR_WHITE, COL_GREEN);
    init_pair(COL_LABEL_BLUE, COLOR_WHITE, COL_BLUE);
    // iniicalizacia zakladneho farebneho paru: B&W
    init_pair(COL_LABEL_NOCOLOR, COLOR_WHITE, COLOR_BLACK);
    init_pair(COL_PLAYER1, COLOR_WHITE, COLOR_MAGENTA);
    init_pair(COL_NOPLAYER, COLOR_BLACK, COLOR_BLACK);
}
int get_new_location(int actual_location, int dice_roll){
    if (actual_location + dice_roll >= NUM_SPACES){
        return actual_location + dice_roll - NUM_SPACES;
    }
    return actual_location + dice_roll;
}
int genNumDigits(int number){
    int count=0;

    while(number!=0){
        number=number/10;
        count++;
    }
    return count;
}
int getCubeNumber(int lower, int upper){
    int num = (rand() % (upper - lower + 1)) + lower;
    return num;
}