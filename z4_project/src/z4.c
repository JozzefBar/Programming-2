#include <stdio.h>
#include <unistd.h>
#include "monopoly.h" // NEMENIT !!!

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

void vypis_hraci(PLAYER *hraci, int pocet_hracov)
{
    printf("Players:\n");
    for (int i = 0; i < pocet_hracov; i++)
    {
        printf("%d. S: %d, C: %d, JP: %d, IJ: %s\n", hraci[i].number, hraci[i].space_number, hraci[i].cash, hraci[i].num_jail_pass, hraci[i].is_in_jail == 0 ? "no" : "yes");
        for (int j = 0; j < hraci[i].num_properties; j++)
            printf("      %-18s %-2d %-9s S%d\n", hraci[i].owned_properties[j]->name, hraci[i].owned_properties[j]->price, zistenie_farby(&hraci[i].owned_properties[j]->color), zistenie_cislo_pola_nehnutelnosti(&hraci[i], j));
    }
}

void vypis_hracej_plochy(PLAYER *hraci, int pocet_hracov)
{
    printf("Game board:\n");
    for (int k = 0; k < NUM_SPACES; k++)
    {
        if (game_board[k].type == Property)
        {
            printf("%-d. %s %-18s %-2d %-10s", k + 1,  k < 9 ? " ": "", game_board[k].property ->name,game_board[k].property ->price, zistenie_farby(&game_board[k].property->color));
            SPACE *pole = &game_board[k];
            PROPERTY *majetok = pole->property;
            int vlastnik = obsadenost_nehnutelnosti(majetok, hraci, pocet_hracov);
            if (vlastnik != -1)
            {
                int je_tam_monopol = monopol(majetok, hraci, pocet_hracov);
                printf("P%d %s\n", vlastnik, je_tam_monopol == 1 ? "yes" : "no");
            }
            else
                printf("\n");
        }
        else
            printf("%-d. %s %-18s\n", k + 1, k < 9 ? " ": "",
                   game_board[k].type == Start ? "START":
                   game_board[k].type ==  Free_parking ? "FREE PARKING":
                   game_board[k].type == In_jail ? "IN JAIL":
                   game_board[k].type == Go_to_jail ? "GO TO JAIL": "JAIL PASS");
    }
}

void vypis_koniec(int vitaz)
{
    printf("WINNER: ");
    if (vitaz == -1)
        printf("-\n");
    else if (vitaz == 0)
        printf("?\n");
    else
        printf("P%d\n", vitaz);
}

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

    SPACE *policko = &game_board[aktualny_hrac->space_number - 1];

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

int najdenie_vitaza(PLAYER *hraci, const int pocet_hracov, int skrachovany_hrac)
{
    int najviac_penazi = najdenie_najviac_penazi(hraci, pocet_hracov, skrachovany_hrac);
    int hraci_rovnako = najdenie_pocet_najviac_penazi(hraci, pocet_hracov, najviac_penazi);
    if (hraci_rovnako == 1)
    {
        return najdenie_hraca_najviac_penazi(hraci, pocet_hracov, najviac_penazi);
    }
    else
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

int main(int argc, char *argv[])
{
    int pocet_hracov = 2, prepinac_n, prepinac_spg = 0, vitaz = -1;
    while ((prepinac_n = getopt(argc, argv, "n:spg")) != -1)
    {
        if (prepinac_n == 'n')
            sscanf(optarg, "%d", &pocet_hracov);
        else
            prepinac_spg = prepinac_n;
    }
    PLAYER hraci[pocet_hracov];
    nastavenie_hraci(hraci, pocet_hracov);
    vypis_hraci(hraci, pocet_hracov);
    vypis_hracej_plochy(hraci, pocet_hracov);
    vypis_koniec(vitaz);

    int kolo = 1, koniec = 0;
    int skrachovany_hrac;
    while (koniec != 1)
    {
        for (int x = 0; x < pocet_hracov; x++)
        {
            PLAYER *aktualny_hrac = &hraci[x];
            int kocka;
            if (scanf("%d", &kocka) == 1)
            {
                int koniec_hry = zmena_udajov(aktualny_hrac, kocka, hraci, pocet_hracov);
                printf("R: %d\n", kocka);
                printf("Turn: %d\n", kolo);
                printf("Player on turn: P%i\n", aktualny_hrac->number);
                printf("\n");

                if (koniec_hry == 0)
                {
                    koniec = 1;
                    skrachovany_hrac = aktualny_hrac->number;
                    break;
                }

                if (prepinac_spg == 's')
                {
                    vypis_hracej_plochy(hraci, pocet_hracov);
                    printf("\n");
                }
                else if (prepinac_spg == 'p')
                {
                    vypis_hraci(hraci, pocet_hracov);
                    printf("\n");
                }
                else if (prepinac_spg == 'g')
                {
                    vypis_hraci(hraci, pocet_hracov);
                    vypis_hracej_plochy(hraci, pocet_hracov);
                    vypis_koniec(vitaz);
                    printf("\n");
                }
                kolo++;

            }
            else
            {
                koniec = 1;
                break;
            }
        }
    }
    vitaz = najdenie_vitaza(hraci, pocet_hracov, skrachovany_hrac);
    vypis_hraci(hraci, pocet_hracov);
    vypis_hracej_plochy(hraci, pocet_hracov);
    vypis_koniec(vitaz);

    return 0;
}