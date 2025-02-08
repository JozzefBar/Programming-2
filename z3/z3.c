#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#define dlzka_slova 21
#define dlzka_riadka 1001

char* vypis(int *velkost_pola)
{
    char riadok[dlzka_riadka];
    char *riadky = (char *)malloc(dlzka_riadka * sizeof(char));
    while ((fgets(riadok, dlzka_riadka, stdin)) != NULL && riadok[0] != '\n')
    {
        strcat((char *) riadky, riadok);
        (*velkost_pola) += (int)strlen(riadok);
    }
    (*velkost_pola)++;
    riadky[*velkost_pola] = '\0';

    return riadky;
}

char* nahradenie(char* pole, const char* nahradzovacie_slovo, const char* indikator, const int moznost, const int podmienka)
{
    size_t dlzka_i = strlen(indikator);
    size_t dlzka_p = strlen(pole);
    size_t dlzka_n = strlen(nahradzovacie_slovo);
    if (moznost == 'r' && podmienka == 1)
    {
        for (int i = 0; i < dlzka_p; i++)
        {
            if ((pole[i] == indikator[0]) && (i == 0 || !isalpha(pole[i - 1])))
            {
                int spravne = 1;
                for (int k = 1; k < dlzka_i; k++)
                {
                    if (pole[i+k] == indikator[k])
                        spravne++;
                    else
                        break;
                }
                if (spravne == dlzka_i)
                {
                    int j, l = 0;
                    for (j = i; j < dlzka_p && isalpha(pole[j]) && pole[j] != '\0' && l < dlzka_n; j++, l++)
                        pole[j] = nahradzovacie_slovo[l];
                    i = j;
                }
            }
        }
    }
    else if (moznost == 'R' && podmienka == 1)
    {
        for (int i = 0; i < dlzka_p; i++)
        {
            if ((pole[i] == tolower(indikator[0]) || pole[i] == toupper(indikator[0])) && (i == 0 || !isalpha(pole[i - 1])))
            {
                int spravne = 1;
                for (int k = 1; k < dlzka_i; k++)
                {
                    if (tolower(indikator[k]) == pole[i+k] || toupper(indikator[k]) == pole[i+k])
                        spravne++;
                    else
                        break;
                }
                if (spravne == dlzka_i)
                {
                    int j, l = 0;
                    for (j = i; j < dlzka_p && isalpha(pole[j]) && pole[j] != '\0' && l < dlzka_n; j++, l++)
                        pole[j] = nahradzovacie_slovo[l];
                    i = j;
                }
            }
        }
    }
    else if (podmienka == 0)
    {
        for (int i = 0; i < dlzka_p; i++)
        {
            int j, l = 0;
            if (i == 0 || !isalpha(pole[i - 1]))
            {
                for (j = i; j < dlzka_p && isalpha(pole[j]) && pole[j] != '\0' && l < dlzka_n; j++, l++)
                    pole[j] = nahradzovacie_slovo[l];
                i = j;
            }
        }
    }
    return pole;
}

char* replace(const int main_parameter, const int moznost, char* pole, char **argv, const int argc)
{
    int k = 1;
    int podmienka = 0;
    char *nahradzovacie_slovo = argv[main_parameter + 1];
    while (main_parameter + 1 + k < argc)
    {
        podmienka = 1;
        char *indikator = argv[main_parameter + 1 + k];
        pole = nahradenie(pole, nahradzovacie_slovo, indikator, moznost, podmienka);
        k++;
    }
    if (podmienka == 0)
    {
        char indikator[2] = {"a"};
        pole = nahradenie(pole, nahradzovacie_slovo, indikator, moznost, podmienka);
    }
    return pole;
}

char* identifikacia_slova(const char *pole, int *p, unsigned long dlzka)
{
    int z = 0;
    char* slovo = (char *)malloc(dlzka_slova * sizeof(char));
    memset(slovo, '\0', dlzka_slova);
    while ((isalpha(pole[*p]) || pole[*p] == ' ') && z < dlzka)
    {
        slovo[z] = pole[*p];
        z++;
        (*p)++;
    }
    return slovo;
}

int pocet_znakov(const char* pole, int y)
{
    int pocet_pismen = 0;
    while(isalpha(pole[y]))
    {
        pocet_pismen++;
        y++;
    }

    return pocet_pismen;
}

int najdi_najdlhsie_slovo(const char * pole)
{
    int pocet = 0;
    int naj_pocet = pocet;
    for (int i = 0; i < strlen(pole); i++)
    {
        while (isalpha(pole[i]))
        {
            pocet++;
            i++;
        }
        if (naj_pocet < pocet)
            naj_pocet = pocet;
        pocet = 0;
    }
    return naj_pocet;
}

int main (int argc, char *argv[])
{
    for (int x = 1; x < argc; x++)
    {
        if (argv[x][0] == '-')
        {
            if (!(argv[x][1] == 'd' || argv[x][1] == 'p' || argv[x][1] == 'r' || argv[x][1] == 'R' || argv[x][1] == 's'))
            {
                printf("E1");
                return 0;
            }
        }
        if (((strcmp(argv[x], "-r") == 0) || (strcmp(argv[x], "-R") == 0)) && (x == argc - 1))
        {
            printf("E2");
            return 0;
        }
        if ((strcmp(argv[x], "-r") == 0) || (strcmp(argv[x], "-R") == 0))
        {
            if (strlen(argv[x + 1]) >= dlzka_slova || (strlen(argv[x + 1])) < 1)
            {
                printf("E3");
                return 0;
            }
        }
    }

    int main_parameter = 1;
    int velkost_pola = 0;
    char* pole = vypis(&velkost_pola);
    char* povodne_pole = (char *)malloc((velkost_pola + 1) * sizeof(char));
    strcpy(povodne_pole, pole);
    int opakovanie = argc;

    int najdlhsie_slovo = najdi_najdlhsie_slovo(pole);

    while (opakovanie >= 1)
    {
        int moznost = getopt(argc, argv, ":dpsr:R:");


        if (moznost == 'd' || moznost == 'p')
        {
            for (int i = 0; i < velkost_pola; i++)
            {
                if ((isdigit(pole[i]) && (moznost == 'd')) || (ispunct(pole[i])) && (moznost == 'p'))
                {
                    for (int j = i; pole[j] != '\0'; j++)
                        pole[j] = pole[j + 1];
                    i--;
                    velkost_pola--;
                }
            }
            strcpy(povodne_pole, pole);
            najdlhsie_slovo = najdi_najdlhsie_slovo(povodne_pole);
        }

        else if (moznost == 'r' || moznost == 'R')
        {
            pole = replace(main_parameter, moznost, pole, argv, argc);
        }
        else if (moznost == 's')
        {
            int varianta;
            if (argc - main_parameter != 1)
            {
                if (strcmp(argv[main_parameter + 1], "-r") == 0)
                {
                    varianta = 'r';
                    pole = replace(main_parameter + 1, varianta, pole, argv, argc);
                }
                else if (strcmp(argv[main_parameter + 1], "-R") == 0)
                {
                    varianta = 'R';
                    pole = replace(main_parameter + 1, varianta, pole, argv, argc);
                }
            }

            int porad_cislo = 1;
            int p = 0, q = 0, y = 0;

            while (pole[p] != '\0')
            {
                size_t dlzka_povodne = pocet_znakov(povodne_pole, y);
                y += (int)dlzka_povodne;
                if (dlzka_povodne != 0)
                {
                    char* povodne_slovo = identifikacia_slova(povodne_pole, &p, dlzka_povodne);
                    char* aktualne_slovo = identifikacia_slova(pole, &q, dlzka_povodne);
                    printf("%i. %s", porad_cislo, povodne_slovo);
                    for (int f = 0; f < najdlhsie_slovo - dlzka_povodne; f++)
                        printf(" ");
                    printf(":%s\n", aktualne_slovo);
                    free(povodne_slovo);
                    free(aktualne_slovo);
                }

                if (povodne_pole[y] == '\n')
                    porad_cislo++;
                y++;
                p++;
                q++;
            }
            break;

        }
        else if (moznost == -1)
        {
            printf("%s", pole);
            break;
        }
        opakovanie--;
        main_parameter++;
    }
    free(pole);
    free(povodne_pole);
    return 0;
}
