//Pred spustením si zmenši termínal čo najviac!!!
//Je nutné pri spustení dať spíňač vo forme čísla od 2-4 (počet hráčov)!!!      forma: z6.exe 2
//číslovanie pre hráčov je vo forme: 1. hráč = A, 2. hráč = B atď...
//dizajn je veľmi originálny (rovnaký ako v demo 2 xd) + aktuálny vypis hráčovych vlastností (pozíciu, cash atď ako bolo v zadaní 4)
//Pravidlá rovnaké ako v zadaní 4, hra končí ked niekto skrachuje, vyhrá ten čo ma najviac penazi, v prípade remízy sa posudzuje sucet cien nehnutelnosti
//polia sú číslované od 1(start) - 24(posledná nehnutelnosť) --- tak ako v z4.
//Kod je mozno trochu zlozitejsi a zbytočne komplikovaný, ale keď to funguje není treba to meniť :)

//Potrebné knižnice
#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

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

// Makra
#define MAX_NAME 100        // max. dlzka nazvu (bez znaku '\0')
#define NUM_PROPERTIES 16   // pocet nehnutelnosti na hracom plane
#define NUM_SPACES 24       // pocet poli na hracom plane
#define MAX_PLAYERS 4       // max. pocet hracov
#define NUM_SPACE_TYPES 6   // pocet typov poli na hracom plane
#define NUM_COLORS 8        // pocet farieb nehnutelnosti

// Typy poli na hracom plane
typedef enum _space_type {
    Property,       // nehnutelnost
    Start,          // startovacie pole
    Free_parking,   // specialne pole bez akcie
    In_jail,        // specialne pole, predstavuje vazenie
    Go_to_jail,     // specialne pole, ktore presunie hraca do vazenia
    Jail_pass       // specialne pole s priepustkou
} SPACE_TYPE;

// Farba nehnutelnosti
typedef enum _color {
    Brown,
    Skyblue,
    Purple,
    Orange,
    Red,
    Yellow,
    Green,
    Blue,
} COLOR;

// Struktura pre nehnutelnost
typedef struct _property {
    char name[MAX_NAME + 1];    // nazov
    int price;                  // cena nehnutelnosti/vyska najomneho
    COLOR color;                // farba
} PROPERTY;

// Struktura pre pole na hracom plane
typedef struct _space {
    SPACE_TYPE type;        // typ pola na hracom plane
    PROPERTY *property;     // pointer na nehnutelnost ak je pole
    // typu Property, inak NULL
} SPACE;

// Struktura pre hraca
typedef struct _player {
    int number;             // cislo hraca (1, 2, 3 alebo 4)
    int space_number;       // cislo pola, na ktorom stoji (pole Start ma cislo 1)
    int cash;               // hotovost hraca
    int num_jail_pass;      // pocet nazbieranych priepustiek z vazenia
    int is_in_jail;         // info, ci je hrac vo vazeni: 0 - nie je; 1 - je
    PROPERTY *owned_properties[NUM_PROPERTIES]; // zoznam kupenych nehnutelnosti
    int num_properties;                         // pocet kupenych nehnutelnosti
} PLAYER;

// nazvy typov poli na hracom plane
const char *space_types[NUM_SPACE_TYPES] = {
        "PROPERTY",
        "START",
        "FREE PARKING",
        "IN JAIL",
        "GO TO JAIL",
        "JAIL PASS"
};

// nazvy farieb nehnutelnosti
const char *property_colors[NUM_COLORS] = {
        "Brown",
        "Skyblue",
        "Purple",
        "Orange",
        "Red",
        "Yellow",
        "Green",
        "Blue"
};

// zoznam nehnutelnosti na hracom plane
PROPERTY properties[NUM_PROPERTIES] = {
        {.name = "FOOD TRUCK", .price = 1, .color = Brown},
        {.name = "PIZZA RESTAURANT", .price = 1, .color = Brown},
        {.name = "DOUGHNUT SHOP", .price = 1, .color = Skyblue},
        {.name = "ICE CREAM SHOP", .price = 1, .color = Skyblue},
        {.name = "MUSEUM", .price = 2, .color = Purple},
        {.name = "LIBRARY", .price = 2, .color = Purple},
        {.name = "THE PARK", .price = 2, .color = Orange},
        {.name = "THE BEACH", .price = 2, .color = Orange},
        {.name = "POST OFFICE", .price = 3, .color = Red},
        {.name = "TRAIN STATION", .price = 3, .color = Red},
        {.name = "COMMUNITY GARDEN", .price = 3, .color = Yellow},
        {.name = "PET RESCUE", .price = 3, .color = Yellow},
        {.name = "AQUARIUM", .price = 4, .color = Green},
        {.name = "THE ZOO", .price = 4, .color = Green},
        {.name = "WATER PARK", .price = 5, .color = Blue},
        {.name = "AMUSEMENT PARK", .price = 5, .color = Blue}
};

// inicializacia hracieho pola
SPACE game_board[NUM_SPACES] = {
        {.type = Start, .property = NULL},
        {.type = Property, .property = &properties[0]},
        {.type = Property, .property = &properties[1]},
        {.type = Jail_pass, .property = NULL},
        {.type = Property, .property =  &properties[2]},
        {.type = Property, .property =  &properties[3]},
        {.type = In_jail, .property =  NULL},
        {.type = Property, .property =  &properties[4]},
        {.type = Property, .property =  &properties[5]},
        {.type = Jail_pass, .property =  NULL},
        {.type = Property, .property =  &properties[6]},
        {.type = Property, .property =  &properties[7]},
        {.type = Free_parking, .property =  NULL},
        {.type = Property, .property =  &properties[8]},
        {.type = Property, .property =  &properties[9]},
        {.type = Jail_pass, .property =  NULL},
        {.type = Property, .property =  &properties[10]},
        {.type = Property, .property =  &properties[11]},
        {.type = Go_to_jail, .property =  NULL},
        {.type = Property, .property =  &properties[12]},
        {.type = Property, .property =  &properties[13]},
        {.type = Jail_pass, .property =  NULL},
        {.type = Property, .property =  &properties[14]},
        {.type = Property, .property =  &properties[15]}
};
//celé monopoly (dizajn, kod ako v demo 2 len par rozdielov)
int get_new_location(int actual_location, int dice_roll, int pocet_priepustiek){
    if (actual_location + dice_roll > NUM_SPACES){
        return actual_location + dice_roll - NUM_SPACES;
    }
    if (actual_location + dice_roll == 19 && pocet_priepustiek == 0)    //Treba podmienku aby pokiaľ hráč skočil na pole Go to Jail a nemá priepustku aby sa vykreslis na pole in Jail
    {
        actual_location = 7;
        return actual_location;
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
void drawPlayer(struct _win_st **win, int act_location, int new_location, int hrac){
    wattron(win[act_location], COLOR_PAIR(COL_NOPLAYER));
    mvwprintw(win[act_location],HEIGHT_CARD-2,2+ hrac*2,"  ");
    wattroff(win[act_location], COLOR_PAIR(COL_NOPLAYER));

    wattron(win[new_location], COLOR_PAIR(COL_PLAYER1));
    mvwprintw(win[new_location],HEIGHT_CARD-2, 2 + hrac*2, "%s", hrac == 0 ? "A": hrac == 1 ? "B": hrac == 2 ? "C": "D");
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
        mvwprintw(local_win, (HEIGHT_LABEL_CARD + 3), (WIDTH_CARD - genNumDigits(game_space.property->price)) / 2, "%2d",
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
        sprintf(cost, "%d", i + 1);

        // juh 1 - 6
        if (i >= 0 && i < NUM_CARD_IN_ROW) {
            win[i + 1] = create_property_card(maxy - HEIGHT_CARD, maxx - WIDTH_CARD * (n + 1), gameboard[i]);
            n++;
            if (n >= NUM_CARD_IN_ROW) {
                n = 0;
            }
        }
        // zapad 7 - 12
        if (i >= 1 * NUM_CARD_IN_ROW && i < 2 * NUM_CARD_IN_ROW) {
            win[i + 1] = create_property_card(maxy - HEIGHT_CARD * (n + 1), maxx - WIDTH_CARD * (NUM_CARD_IN_ROW + 1),gameboard[i]);
            n++;
            if (n >= NUM_CARD_IN_ROW) {
                n = NUM_CARD_IN_ROW;
            }
        }

        // sever 13 - 18
        if (i >= 2 * NUM_CARD_IN_ROW && i < 3 * NUM_CARD_IN_ROW) {
            win[i + 1] = create_property_card(maxy - HEIGHT_CARD * (NUM_CARD_IN_ROW + 1), maxx - WIDTH_CARD * (n + 1),gameboard[i]);
            n--;
            if (n <= 0) {
                n = NUM_CARD_IN_ROW;
            }
        }
        // vychod 19 - 24
        if (i >= 3 * NUM_CARD_IN_ROW && i < 4 * NUM_CARD_IN_ROW) {
            win[i + 1] = create_property_card(maxy - HEIGHT_CARD * (n + 1), maxx - WIDTH_CARD, gameboard[i]);
            n--;
            if (n <= 0) {
                n = NUM_CARD_IN_ROW;
            }
        }
    }
}
void init_custom_color_pair(){                      //toto skontrolovat mozno to ani neni treba mat v programe
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
    // inicalizacia zakladneho farebneho paru: B&W
    init_pair(COL_LABEL_NOCOLOR, COLOR_WHITE, COLOR_BLACK);
    init_pair(COL_PLAYER1, COLOR_WHITE, COLOR_MAGENTA);
    init_pair(COL_NOPLAYER, COLOR_BLACK, COLOR_BLACK);
}

//Vypis ako zo zadania 4
// nastavania pre každého hráča
PLAYER* nastavenie_hraci(PLAYER *hraci, int pocet_hracov)
{
    for (int i = 0; i < pocet_hracov; i++)
    {
        hraci[i].number = i + 1;
        hraci[i].space_number = 1;
        if (pocet_hracov == 2)
            hraci[i].cash = 20;
        else if (pocet_hracov == 3)
            hraci[i].cash = 18;
        else
            hraci[i].cash = 16;
        hraci[i].num_jail_pass = 0;
        hraci[i].is_in_jail = 0;
        hraci[i].num_properties = 0;
    }
    return hraci;
}
//zistenie cisla nehnutelnosti
int zistenie_cislo_pola_nehnutelnosti(PLAYER *hrac, int poradie)
{
    int pole = 0;
    for (int y = 0; y < NUM_SPACES; y++)
    {
        if (hrac->owned_properties[poradie] == game_board[y].property)
        {
            pole = y + 1;
            break;
        }
    }
    return pole;
}
//zistenie, či dané pole vlastní nejaký hráč
int obsadenost_nehnutelnosti(PROPERTY *nehnutelnost, PLAYER *hraci, int pocet_hracov)
{
    for (int i = 0; i < pocet_hracov; i++)
    {
        for (int j = 0; j < hraci[i].num_properties; j++)
        {
            if (hraci[i].owned_properties[j] == nehnutelnost)
                return hraci[i].number;
        }
    }
    return -1;
}
//zistenie, či má hrač monopol danej farby
int monopol(PROPERTY *nehnutelnost, PLAYER *hraci, int pocet_hracov)
{
    for (int i = 0; i < pocet_hracov; i++)
    {
        int farba = 0;
        for (int j = 0; j < hraci[i].num_properties; j++)
        {
            if (hraci[i].owned_properties[j]->color == nehnutelnost->color)
                farba++;
        }
        if (farba == 2)
            return 1;
    }
    return -1;
}
//funkcia na zistenie farby nehnutelnosti
const char* zistenie_farby(const COLOR *farba)
{
    if (*farba == Brown)
        return property_colors[0];
    else if (*farba == Skyblue)
        return property_colors[1];
    else if (*farba == Purple)
        return property_colors[2];
    else if (*farba == Orange)
        return property_colors[3];
    else if (*farba == Red)
        return property_colors[4];
    else if (*farba == Yellow)
        return property_colors[5];
    else if (*farba == Green)
        return property_colors[6];
    else
        return property_colors[7];
}
//vypis stavu hráčov, aktualizuje sa po každom kole
void vypis_hraci(PLAYER *hraci, int pocet_hracov)
{
    int y = 4;
    mvprintw(y, 0, "Players:");
    y += 2;
    for (int i = 0; i < pocet_hracov; i++)
    {
        mvprintw(y, 0, "                                                 ");    //Prekryje sa predtým napísaný text
        mvprintw(y, 0, "%d. S: %d, C: %d, JP: %d, IJ: %s", hraci[i].number, hraci[i].space_number, hraci[i].cash, hraci[i].num_jail_pass, hraci[i].is_in_jail == 0 ? "no" : "yes");

        for (int j = 0; j < hraci[i].num_properties; j++)
        {
            y++;
            mvprintw(y, 0, "                                                     ");
            mvprintw(y, 0, "      %-18s %-2d %-9s S%d", hraci[i].owned_properties[j]->name, hraci[i].owned_properties[j]->price, zistenie_farby(&hraci[i].owned_properties[j]->color), zistenie_cislo_pola_nehnutelnosti(&hraci[i], j));
        }
        y++;
    }
    refresh();
}
//záverečný výpis po ukončení, nachádza sa nad hracou doskou
void vypis_koniec(int vitaz, int maxx)
{
    mvprintw(5, maxx - 50,"WINNER:");
    if (vitaz == 0)
        mvprintw(5, maxx - 42, "Remíza!!!");
    else if (vitaz == -1)
        mvprintw(5, maxx - 42, "???????");
    else
        mvprintw(5, maxx - 42, "P%d!!!", vitaz);
    mvprintw(6, maxx - 54, "---------------------");
    refresh();
}
//funkcia na zmenenie údajov u hráča
int zmena_udajov(PLAYER *aktualny_hrac, int kocka, PLAYER *hraci, int pocet_hracov)
{
    aktualny_hrac->space_number += kocka;
    if (aktualny_hrac->is_in_jail == 1)
    {
        if (aktualny_hrac->cash - 1 < 0)
            return 0;
        aktualny_hrac->cash--;
        aktualny_hrac->is_in_jail = 0;
    }
    else if (aktualny_hrac->space_number > NUM_SPACES)
    {
        aktualny_hrac->space_number -= NUM_SPACES;
        aktualny_hrac->cash +=2;
    }
    //zistenie ci policko na ktorom je hráč, ci ide o klasicke pole alebo nejakú nehnutelnosť
    SPACE *policko = &game_board[aktualny_hrac->space_number - 1];
    if (aktualny_hrac->space_number > 24)
        return 0;

    if (policko->type == Jail_pass)
        aktualny_hrac->num_jail_pass++;
    else if (policko->type == Go_to_jail)
    {
        if (aktualny_hrac->num_jail_pass > 0)
            aktualny_hrac->num_jail_pass--;
        else
        {
            aktualny_hrac->is_in_jail = 1;
            aktualny_hrac->space_number = 7;
        }
    }
    //určenie, či kúpiť nehnuteľnosť alebo čí nejakému hráčovi zaplatiť za ňu a koľko (ak má monopol tak 2xcena)
    if (policko->type == Property)
    {
        PROPERTY *nehnutelnost = policko->property;
        int obsadenost_hrac = obsadenost_nehnutelnosti(nehnutelnost, hraci, pocet_hracov);
        int ma_monopol = monopol(nehnutelnost, hraci, pocet_hracov);
        if (obsadenost_hrac == aktualny_hrac->number)
            ;
        else if (((ma_monopol == -1 && (aktualny_hrac->cash - policko->property->price < 0)) || (ma_monopol == 1 && (aktualny_hrac->cash - policko->property->price*2 < 0) && obsadenost_hrac != aktualny_hrac->number)))
            return 0;
        else if(obsadenost_hrac == -1)
        {
            aktualny_hrac->owned_properties[aktualny_hrac->num_properties] = nehnutelnost;
            aktualny_hrac->num_properties++;
            aktualny_hrac->cash -= policko->property->price;
        }
        else if (obsadenost_hrac != aktualny_hrac->number)
        {
            if (ma_monopol == 1)
            {
                aktualny_hrac->cash-= policko->property->price*2;
                hraci[obsadenost_hrac - 1].cash += policko->property->price*2;
            }
            else
            {
                aktualny_hrac->cash -= policko->property->price;
                hraci[obsadenost_hrac - 1].cash += policko->property->price;
            }
        }
    }
    return 1;
}

//funkcie na zistenie, kto je víťaz
//najskôr najdeme najväčšiu sumu penazí, ktorú nejaký hráč má
int najdenie_najviac_penazi(PLAYER *hraci, int pocet_hracov, int skrachovany_hrac)
{
    int peniaze = 0;
    for (int i = 0; i < pocet_hracov; i++)
    {
        if (hraci[i].cash >= peniaze && hraci[i].number != skrachovany_hrac)
            peniaze = hraci[i].cash;
    }
    return peniaze;
}
//najdenie koľko hráčov má najviac penazi
int najdenie_pocet_najviac_penazi(PLAYER *hraci, int pocet_hracov, int najviac_penazi)
{
    int rovnako = 0;
    for (int j = 0 ; j < pocet_hracov; j++)
    {
        if (hraci[j].cash == najviac_penazi)
            rovnako++;
    }
    return rovnako;
}
//ak ma najviac penazi len jeden hráč, tak najdeme ktorý to je
int najdenie_hraca_najviac_penazi(PLAYER *hraci, int pocet_hracov, int najviac_penazi)
{
    int hrac;
    for (int k = 0; k < pocet_hracov; k++)
    {
        if (hraci[k].cash == najviac_penazi)
            hrac = hraci[k].number;
    }
    return hrac;
}
//celý algoritmus na zistenie, ktorý hráč vyhral
int najdenie_vitaza(PLAYER *hraci, const int pocet_hracov, int skrachovany_hrac)
{
    int najviac_penazi = najdenie_najviac_penazi(hraci, pocet_hracov, skrachovany_hrac);
    int hraci_rovnako = najdenie_pocet_najviac_penazi(hraci, pocet_hracov, najviac_penazi);
    if (hraci_rovnako == 1)
    {
        return najdenie_hraca_najviac_penazi(hraci, pocet_hracov, najviac_penazi);
    }
    else        //pri remize sa akokeby prechádzajú ceny nehnutelnosti hráčov, ktorý majú najviac penazi
    {
        int hraci_rovnako_po_remize = 0, najviac_penazi_po_remize = 0, vitaz_po_remize;
        int pole_penazi[pocet_hracov];
        for (int l = 0; l < pocet_hracov; l++)
        {
            pole_penazi[l] = 0;
            if (hraci[l].cash == najviac_penazi && hraci[l].number != skrachovany_hrac)
            {
                for (int p = 0; p < hraci[l].num_properties; p++)
                    pole_penazi[l]+= hraci[l].owned_properties[p]->price;
            }

            if (pole_penazi[l] > najviac_penazi_po_remize)
            {
                najviac_penazi_po_remize = pole_penazi[l];
                hraci_rovnako_po_remize = 1;
                vitaz_po_remize = hraci[l].number;
            }
            else if (pole_penazi[l] == najviac_penazi_po_remize)
                hraci_rovnako_po_remize++;
        }
        if (hraci_rovnako_po_remize > 1)
            return 0;
        return vitaz_po_remize;
    }
}

int main(int argc, char *argv[]){
    //spracovanie počtu hráčov
    int pocet_hracov = 0;
    if (argc == 2)
    {
        pocet_hracov = argv[1][0] - '0';
        if (pocet_hracov > MAX_PLAYERS || pocet_hracov < 2)
        {
            printf("\nNesprávne zadaný počet hráčov (2 - 4)");
            return 0;
        }
    }
    else
    {
        printf("\nNesprávne zadaný prepínač! (2 - 4)");
        return 0;
    }

    struct _win_st *win[NUM_SPACES];
    int maxy, maxx;
    int location=1, new_location=1;
    int dice_roll = 0;

    srand(time(NULL));   //inicializacia nahodneho generatora cisel = vždy rôzne cislo
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
    mvprintw(0,0,"hod kockou: 'c'  Quit: 'Esc'");
    mvprintw(3,0,"Turn: 0.");
    refresh();

    //nakreslenie hracieho planu monopolov - pre 24 policok
    drawGameBoard(win, game_board, maxy, maxx);
    //umiestnenie hracov na START
    for (int y = 0; y < pocet_hracov; y++)
        drawPlayer(win, location, new_location, y);
    //nastavenie údajov pre hráčov
    PLAYER hraci[pocet_hracov];
    nastavenie_hraci(hraci, pocet_hracov);
    vypis_hraci(hraci, pocet_hracov);
    refresh();

    int kolo = 1, koniec = 0, x = 0, skrachovany_hrac = -1;
    //Priebeh celej hry, pri stlačení klávesy c pokračuje pri Esc je koniec alebo až kým nie je nejaký víťaz
    while (1) {
        int c = getch();
        if (c == 'c' && koniec == 0) {
            //aktualizacia vypisu hracov
            if (x > pocet_hracov - 1)
                x = 0;
            PLAYER *aktualny_hrac = &hraci[x];
            // hod kockou
            dice_roll = getCubeNumber(1,6);  //nahodne cislo od 1 - 6
            new_location = get_new_location(aktualny_hrac->space_number, dice_roll, aktualny_hrac->num_jail_pass); //vypocet novej polohy hraca (osetrenie prejdenia cez START)
            drawPlayer(win,aktualny_hrac->space_number, new_location, x);   //prekreslenie polohy hraca
            //debug vypisy
            mvprintw(maxy-1, 0,"old_loc: %2d  new_loc: %2d", aktualny_hrac->space_number, new_location);
            mvprintw(maxy-2, 0,"hod kockou: %d", dice_roll);
            mvprintw(maxy-3, 0,"Hrac na tahu: %c", aktualny_hrac->number == 1 ? 'A' : aktualny_hrac->number == 2 ? 'B' : aktualny_hrac->number == 3 ? 'C' : 'D');

            int koniec_hry = zmena_udajov(aktualny_hrac, dice_roll, hraci, pocet_hracov);
            mvprintw(3,0,"Turn: %d", kolo);
            vypis_hraci(hraci, pocet_hracov);

            if (koniec_hry == 0)       //pokial sa najde ze nejaký hrač skrachoval, hra skončí
            {
                koniec = 1;
                skrachovany_hrac = aktualny_hrac->number;
            }
            kolo++;
            x++;
            refresh();
        }
        // koniec hry
        else if (c == 27 || koniec == 1){
            int vitaz;
            if (koniec == 0)
                vitaz = -1;
            else
                vitaz= najdenie_vitaza(hraci, pocet_hracov, skrachovany_hrac);
            vypis_koniec(vitaz, maxx);
            mvprintw(9, maxx - 42, "exiting...(12s)");           //po skonceni si mozes pozrieť stav hráčov, máš na to 12 sekúnd
            mvprintw(10, maxx - 42, "bankruptcy: %c", skrachovany_hrac== 1 ? 'A' : skrachovany_hrac == 2 ? 'B' : skrachovany_hrac == 3 ? 'C' : skrachovany_hrac == 4 ? 'D' : '?');
            refresh();
            napms(12000);
            break;
        }
    }
    endwin();			// End curses modeeturn 0;
    return 0;
}