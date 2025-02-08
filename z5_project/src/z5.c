#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "monopoly.h"

#define MAX_DLZKA 101

int najst_cenu(char* riadok)
{
    int cena = 0;
    for (int i = 0; i < (int)strlen(riadok) - 1; i++)
        if (riadok[i] ==  ':')
        {
            cena = atoi(&riadok[i + 1]);
            break;
        }

    return cena;
}

char* najst_nehnutelnost(const char* riadok)
{
    char* pozemok = (char*)malloc(MAX_DLZKA * sizeof(char));
    int i = 1, j = 0;
    while(i < (int)strlen(riadok) && riadok[i] != '"')
    {
        pozemok[j] = riadok[i];
        i++; j++;
    }
    pozemok[j] = '\0';
    return pozemok;
}

char * najst_farbu(char* riadok)
{
    char *farba = (char*)malloc(MAX_DLZKA * sizeof(char));
    int i;
    for  (i = 1 ; i < (int)strlen(riadok) - 2; i++)
        farba[i - 1] = riadok[i];

    farba[i - 1] = '\0';
    return farba;
}

void vypis_nehnutelnosti(FILE* nehnutelnosti, FILE* hracia_doska, int aktualny_riadok, int *i, int poradie)
{
    char* farba = NULL, *nehnutelnost;
    char riadok[MAX_DLZKA + 2];
    while((fgets(riadok, MAX_DLZKA + 2, nehnutelnosti)) != NULL)
    {
        if (riadok[0] == '[')
        {
            free(farba);
            farba = najst_farbu(riadok);
        }
        else
        {
            nehnutelnost = najst_nehnutelnost(riadok);
            int cena = najst_cenu(riadok);
            if (hracia_doska == NULL)
            {
                printf("%-20s %-2d %-10s\n", nehnutelnost, cena, farba);
                free(nehnutelnost);
            }
            else if (aktualny_riadok > *i)
            {
                printf("%-d. %s %-20s %-2d %-10s\n", poradie, poradie < 10 ? " ": "", nehnutelnost, cena, farba);
                free(nehnutelnost);
                break;
            }
        }
        aktualny_riadok++;
    }
    free(farba);
    if (hracia_doska != NULL)
        *i = aktualny_riadok;
}

void vypis_dosky(FILE* hracia_doska, FILE* nehnutelnosti)
{
    char riadok_doska[MAX_DLZKA];
    int poradie = 0, i = 0;
    fseek(hracia_doska, 0, SEEK_SET);
    while((fgets(riadok_doska, MAX_DLZKA, hracia_doska)) != NULL)
    {
        if (nehnutelnosti == NULL && poradie >= 1)
        {
            riadok_doska[strlen(riadok_doska) - 1] = '\0';
            if ( riadok_doska[0] == 'P')
            {
                printf("%-d. %s %-20s %-2d %-10s\n", poradie,  poradie < 10 ? " ": "", properties[i].name, properties[i].price,
                       properties[i].color == Brown ? property_colors[0]:
                       properties[i].color == Skyblue ? property_colors[1]:
                       properties[i].color == Purple ? property_colors[2]:
                       properties[i].color == Orange ? property_colors[3]:
                       properties[i].color == Red ? property_colors[4]:
                       properties[i].color == Yellow ? property_colors[5]:
                       properties[i].color == Green ? property_colors[6]: property_colors[7]);
                i++;
            }
            else
                printf("%-d. %s %-18s\n", poradie, poradie < 10 ? " ": "", riadok_doska);

            poradie++;
        }
        else if (nehnutelnosti != NULL && poradie >= 1)
        {
            riadok_doska[strlen(riadok_doska) - 1] = '\0';
            if (riadok_doska[0] == 'P')
            {
                fseek(nehnutelnosti, 0, SEEK_SET);
                vypis_nehnutelnosti(nehnutelnosti, hracia_doska, 0, &i, poradie);
            }

            else
                printf("%-d. %s %-18s\n", poradie, poradie < 10 ? " ": "", riadok_doska);

            poradie++;
        }
        else
            poradie = 1;
    }
}

int zistit_pocet_nehnutelnosti(FILE* hracia_doska, FILE* nehnutelnosti)
{
    int pocet = 0;
    char riadok[MAX_DLZKA];
    if (hracia_doska != NULL)
    {
        while((fgets(riadok, MAX_DLZKA, hracia_doska)) != NULL)
        {
            if (riadok[0] == 'P')
                pocet++;
        }
    }
    else
        while((fgets(riadok, MAX_DLZKA, nehnutelnosti)) != NULL)
        {
            if (riadok[0] == '"')
                pocet++;
        }

    return pocet;
}

int main(int argc, char *argv[])
{
    FILE *game_s, *properties_s;
    int prepinac, prepinac_b = 'x', prepinac_t = 'x';
    char *parameter_b = "0", *parameter_t = "0";
    while ((prepinac = getopt(argc, argv, "b:t:")) != -1)
    {
        if (prepinac == 'b')
        {
            prepinac_b = 'b';
            parameter_b = optarg;
        }
        else
        {
            prepinac_t = 't';
            parameter_t = optarg;
        }
    }
    if (prepinac_t == 't' && prepinac_b == 'x')
    {
        properties_s = fopen(parameter_t, "r");
        if (properties_s == NULL)
        {
            printf("E1");
            return 0;
        }
        else
        {
            vypis_nehnutelnosti(properties_s, NULL, 0, 0, 0);
            fclose(properties_s);
        }
    }
    else if (prepinac_b == 'b')
    {
        game_s = fopen(parameter_b, "r");

        if (strcmp(parameter_t, "0") != 0)
            properties_s = fopen(parameter_t, "r");
        else
            properties_s = NULL;

        if (game_s == NULL || (strcmp(parameter_t, "0") != 0 && properties_s == NULL))
        {
            printf("E1");
            if (properties_s != NULL)
                fclose(properties_s);
            else if (game_s != NULL)
                fclose(game_s);
            return 0;
        }

        int pocet_nehnutelnosti_game = zistit_pocet_nehnutelnosti(game_s, NULL), pocet_nehnutelnosti_properties = 16;

        if (properties_s != NULL)
            pocet_nehnutelnosti_properties = zistit_pocet_nehnutelnosti(NULL, properties_s);
        if (pocet_nehnutelnosti_game > pocet_nehnutelnosti_properties)
        {
            printf("E2");
            if (properties_s != NULL)
                fclose(properties_s);
            fclose(game_s);
            return 0;
        }
        vypis_dosky(game_s, properties_s);

        if (properties_s != NULL)
            fclose(properties_s);
        fclose(game_s);
    }
    return 0;
}