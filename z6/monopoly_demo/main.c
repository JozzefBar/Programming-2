#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h> // upravit podla toho, akym sposobom je kniznica ncurses nainstalovana

// Rozmery pola na hracom plane
#define SPACE_WIDTH 18 // (16 vyuzitelne, 2 okraje)
#define SPACE_HEIGHT 7 // (5 vyuzitelne, 2 okraje)

#define JAIL_COLOR 10 // farebny kod pre pole IN JAIL a GO TO JAIL

#define MAX_NAME 100 // max. dlzka nazvu (bez znaku '\0')
#define NUM_SPACE_TYPES 6   // pocet typov poli na hracom plane
#define NUM_SPACES 20       // pocet poli na hracom plane
#define NUM_PROPERTIES 12   // pocet nehnutelnosti na hracom plane

// Nazvy typov poli na hracom plane
const char *space_types[NUM_SPACE_TYPES] = {
        "PROPERTY",
        "START",
        "FREE PARKING",
        "IN JAIL",
        "GO TO JAIL",
        "JAIL PASS"
};

// Typy poli na hracom plane
typedef enum _space_type {
    Property,       // nehnutelnost
    Start,          // startovacie pole
    Free_parking,   // specialne pole bez akcie
    In_jail,        // specialne pole, predstavuje vazenie
    Go_to_jail,     // specialne pole, ktore presunie hraca do vazenia
    Jail_pass       // specialne pole s priepustkou
} SPACE_TYPE;

// Struktura pre nehnutelnost
typedef struct _property {
    char name[MAX_NAME + 1];    // nazov
    int price;                  // cena nehnutelnosti/vyska najomneho
    short color;                // farba
} PROPERTY;

// Struktura pre pole na hracom plane
typedef struct _space {
    SPACE_TYPE type;        // typ pola na hracom plane
    PROPERTY *property;     // pointer na nehnutelnost ak je pole
    // typu Property, inak NULL
} SPACE;

// Struktura reprezentujuca parametre okna
typedef struct window_params {
    int win_height;
    int win_width;
    int offset_y;
    int offset_x;
} WINDOW_PARAMS;

// Zoznam nehnutelnosti na hracom plane
PROPERTY properties[NUM_PROPERTIES] = {
        {.name = "FOOD TRUCK", .price = 1, .color = COLOR_RED},
        {.name = "PIZZA RESTAURANT", .price = 1, .color = COLOR_RED},
        {.name = "DOUGHNUT SHOP", .price = 1, .color = COLOR_GREEN},
        {.name = "ICE CREAM SHOP", .price = 1, .color = COLOR_GREEN},
        {.name = "MUSEUM", .price = 2, .color = COLOR_YELLOW},
        {.name = "LIBRARY", .price = 2, .color = COLOR_YELLOW},
        {.name = "THE PARK", .price = 2, .color = COLOR_BLUE},
        {.name = "THE BEACH", .price = 2, .color = COLOR_BLUE},
        {.name = "POST OFFICE", .price = 3, .color = COLOR_MAGENTA},
        {.name = "TRAIN STATION", .price = 3, .color = COLOR_MAGENTA},
        {.name = "COMMUNITY GARDEN", .price = 3, .color = COLOR_CYAN},
        {.name = "PET RESCUE", .price = 3, .color = COLOR_CYAN},
};

// Hraci plan
SPACE game_board[NUM_SPACES] = {
        {.type = Start, .property = NULL},
        {.type = Property, .property = &properties[0]},
        {.type = Property, .property = &properties[1]},
        {.type = Jail_pass, .property = NULL},
        {.type = Property, .property =  &properties[2]},
        {.type = In_jail, .property =  NULL},
        {.type = Property, .property =  &properties[3]},
        {.type = Property, .property =  &properties[4]},
        {.type = Jail_pass, .property =  NULL},
        {.type = Property, .property =  &properties[5]},
        {.type = Free_parking, .property =  NULL},
        {.type = Property, .property =  &properties[6]},
        {.type = Property, .property =  &properties[7]},
        {.type = Jail_pass, .property =  NULL},
        {.type = Property, .property =  &properties[8]},
        {.type = Go_to_jail, .property =  NULL},
        {.type = Property, .property =  &properties[9]},
        {.type = Property, .property =  &properties[10]},
        {.type = Jail_pass, .property =  NULL},
        {.type = Property, .property =  &properties[11]}
};


//////////////////////   FUNKCIE //////////////////////

// Funkcia, ktora vyfarbi zahlavie nehnutelnosti podla priradenej farby
void print_color_header(WINDOW *win, const short color_id) {
    wattron(win, COLOR_PAIR(color_id));
    mvwprintw(win, 1, 1, "%*.s", SPACE_WIDTH - 2, " ");
    wattroff(win, COLOR_PAIR(color_id));
    wrefresh(win);
}

// Funkcia, ktora vyfarbi cele pole
void print_color_space(WINDOW *win, const short color_id) {
    wattron(win, COLOR_PAIR(color_id));
    for(int i=0; i< SPACE_HEIGHT-2; i++){
    	mvwprintw(win, i+1, 1, "%*.s", SPACE_WIDTH - 2, " ");
    }
    wattroff(win, COLOR_PAIR(color_id));
    wrefresh(win);
}

// Funkcia na nakreslenie pola na hracom plane
WINDOW *show_space(const WINDOW_PARAMS *win_params, const SPACE *space) {
    WINDOW *local_win = newwin(win_params->win_height, win_params->win_width, win_params->offset_y,
                               win_params->offset_x);
    refresh();
    box(local_win, 0, 0); // okraje okna
    if (space->property) {
    	print_color_header(local_win, space->property->color);
    	mvwprintw(local_win, 3, 1, "%.*s", SPACE_WIDTH-2, space->property->name);
        mvwprintw(local_win, 4, 1, "%d", space->property->price);
    } else {
    	if(space->type == Go_to_jail || space->type == In_jail){
            print_color_space(local_win, JAIL_COLOR);
			wattron(local_win, COLOR_PAIR(JAIL_COLOR));
			mvwprintw(local_win, 3, 1, "%.*s", SPACE_WIDTH - 2, space_types[space->type]);
        	wattroff(local_win, COLOR_PAIR(JAIL_COLOR));
    	}
    	else{
    		mvwprintw(local_win, 3, 1, "%.*s", SPACE_WIDTH - 2, space_types[space->type]);
    	}
    	
    }
    wrefresh(local_win);
    return local_win;
}

// Funkcia na nakreslenie figurky na poli
void show_token(WINDOW *win, const int y, const int x, const short color_id) {
    wattron(win, COLOR_PAIR(color_id));
    mvwprintw(win, y, x, " ");
    wattroff(win, COLOR_PAIR(color_id));
    wrefresh(win); // aktualizacia okna
}

// Funkcia na vypis hracieho planu
WINDOW** show_gameboard() {
    WINDOW **visual_gameboard = (WINDOW**) malloc(NUM_SPACES*sizeof(WINDOW*));
    int space_counter = 0;

    WINDOW_PARAMS win_params = {SPACE_HEIGHT, SPACE_WIDTH, 1, 3};
    visual_gameboard[space_counter] = show_space(&win_params, &game_board[space_counter]);

    show_token(visual_gameboard[space_counter], 3, 7, COLOR_RED);
    show_token(visual_gameboard[space_counter], 3, 9, COLOR_GREEN);
    show_token(visual_gameboard[space_counter], 3, 11, COLOR_BLUE);
    space_counter++;

    for (int i = 0; i < 4; i++) {
        win_params.offset_x += SPACE_WIDTH;
        visual_gameboard[space_counter] = show_space(&win_params, &game_board[space_counter]);
        space_counter++;
    }

    win_params.offset_x += SPACE_WIDTH;
    visual_gameboard[space_counter] = show_space(&win_params, &game_board[space_counter]);
    space_counter++;

    for (int i = 0; i < 4; i++) {
        win_params.offset_y += SPACE_HEIGHT;
        visual_gameboard[space_counter] = show_space(&win_params, &game_board[space_counter]);
        space_counter++;
    }

    win_params.offset_y += SPACE_HEIGHT;
    visual_gameboard[space_counter] = show_space(&win_params, &game_board[space_counter]);
    space_counter++;

    for (int i = 0; i < 4; i++) {
        win_params.offset_x -= SPACE_WIDTH;
        visual_gameboard[space_counter] = show_space(&win_params, &game_board[space_counter]);
        space_counter++;
    }

    win_params.offset_x -= SPACE_WIDTH;
    visual_gameboard[space_counter] = show_space(&win_params, &game_board[space_counter]);
    space_counter++;

    for (int i = 0; i < 4; i++) {
        win_params.offset_y -= SPACE_HEIGHT;
        visual_gameboard[space_counter] = show_space(&win_params, &game_board[space_counter]);
        space_counter++;
    }
    return visual_gameboard;
}

int main() {
    initscr(); // inicializacia ncurses
    cbreak(); // deaktivujeme line buffering pre okamzitu dostupnost stlacenych klavesov
    noecho(); // deaktivujeme vypis stlacenych klavesov
    keypad(stdscr, TRUE); // aktivujeme odchytavanie specialnych klavesov (backspace, delete, sipky, ...)
    curs_set(FALSE); // deaktivacia kurzora
    if (has_colors() == FALSE) {
        endwin();
        printf("Your terminal does not support color.\n");
        return 1;
    }

	// inicializacia farieb
    start_color();
	init_pair(COLOR_RED, COLOR_RED, COLOR_RED);
	init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_GREEN);
	init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_YELLOW);
	init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLUE);
	init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_MAGENTA);
	init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_CYAN);
	init_pair(JAIL_COLOR, COLOR_BLACK, COLOR_WHITE);

    // nadpis
    mvprintw(21, 42, "Monopoly using");
    refresh();
    WINDOW *heading_win = newwin(3, 9, 20, 57);
    refresh();
    box(heading_win, 0, 0); // okraje okna
    mvwprintw(heading_win, 1, 1, "ncurses");
    wrefresh(heading_win);

    // gameboard
    WINDOW ** visual_gameboard = show_gameboard();
    free(visual_gameboard);

    // hraci - legenda
    mvprintw(20, 6*SPACE_WIDTH+10, "Player 1");
    mvprintw(22, 6*SPACE_WIDTH+10, "Player 2");
    mvprintw(24, 6*SPACE_WIDTH+10, "Player 3");
    show_token(stdscr, 20, 6 * SPACE_WIDTH + 8, COLOR_RED);
    show_token(stdscr, 22, 6 * SPACE_WIDTH + 8, COLOR_GREEN);
    show_token(stdscr, 24, 6 * SPACE_WIDTH + 8, COLOR_BLUE);
    refresh();

    getch(); // cakame na stlacenie klavesu
    endwin(); // ukoncenie prace s ncurses
    return 0;
}
