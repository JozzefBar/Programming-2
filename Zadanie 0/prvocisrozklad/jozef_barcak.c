#include <stdio.h>

// hlavicka funkcie
void primeFactors(int a, int m)
{
    int delitel = 2, vysledok = a, tmp, pocet = 0, rovnake = 0;

// Miesto pre definiciu funkcie 'primeFactors'.

    while (vysledok != 1 && pocet < m)
    {
        while (vysledok%delitel == 0)
        {
            if (rovnake != delitel)
            {
                printf("%d\n", delitel);
                rovnake = delitel;
                pocet += 1;
            }
            tmp = vysledok;
            vysledok = tmp/delitel;

        }
        delitel += 1;
    }
}

int main()
{
    int a, m;
    // 1. Nacitanie vstupov 'a' a 'm'.
    scanf("%d", &a);

    scanf("%d", &m);
    if (a <= 1)
    {
        printf("ERROR");
    }
    else
    {
    // 2. Zavolanie funkcie 'primeFactors'
    primeFactors(a, m);
    }
    return 0;
}