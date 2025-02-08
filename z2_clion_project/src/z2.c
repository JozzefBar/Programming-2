#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "functions.h" // ReLU funkcia a pomocne funkcie
#include "data.h" // Makra, vahy a bias hodnoty

void rezim1(int index)
{
    for (int i = 0; i < IMAGE_SIZE; i++)
    {
        if ((i%10 == 0) && (i != 0))
        {
            printf("\n");
        }
        printf("%8.2lf ", weights[(index*IMAGE_SIZE) + i]);
    }
}

double rezim2(int index, double const obrazok[])
{
    double sucet = bias[index];
    for (int i = 0; i < IMAGE_SIZE; i++)
        sucet += obrazok[i]*weights[i + index*IMAGE_SIZE];

    return sucet;
}

void rezim3()
{
    double x = 0.0;
    scanf("%lf", &x);
    printf("%.2lf", relu(x));
}

double Findmax(double const pole[], int dlzka)
{
    double max = pole[0];
    for (int i = 1; i < dlzka; i++)
    {
        if (pole[i] > max)
        max = pole[i];
    }
    return max;
}

int Findmax_index(double const pole[], int dlzka)
{
    double max = pole[0];
    int index = 0;
    for (int i = 1; i < dlzka; i++)
    {
        if (pole[i] > max)
        {
            max = pole[i];
            index = i;
        }
    }
    return index;
}

double *softmax(double pole[], double maxz)
{
    double suma = 0.0;
    for (int j = 0; j < NUMBER_OF_NEURONS; j++)
        suma += exp(pole[j] - maxz);

    for (int i = 0; i < NUMBER_OF_NEURONS; i++)
        pole[i] = exp(pole[i] - maxz)/suma;

    return pole;
}

void rezim4()
{
    double cisla[NUMBER_OF_NEURONS];
    load_data(cisla, NUMBER_OF_NEURONS);

    for (int i = 0; i < NUMBER_OF_NEURONS; i++)
        cisla[i] = relu(cisla[i]);

    double maxz = Findmax(cisla,NUMBER_OF_NEURONS);
    double *softmax_pole = softmax(cisla, maxz);

    for (int i = 0; i < NUMBER_OF_NEURONS; i++)
        printf("%.2lf ", softmax_pole[i]);
}

void rezim5()
{
    double cisla[NUMBER_OF_NEURONS];
    load_data(cisla, NUMBER_OF_NEURONS);
    int dlzka = sizeof(cisla) / sizeof(cisla[0]);

    printf("%d", Findmax_index(cisla,dlzka));
}

void rezim6()
{
    double obrazok[IMAGE_SIZE];
    load_data(obrazok, IMAGE_SIZE);
    print_image(obrazok, IMAGE_WIDTH, IMAGE_HEIGHT);
}

void rezim7()
{
    double obrazok[IMAGE_SIZE], neurony[NUMBER_OF_NEURONS];
    load_data(obrazok,IMAGE_SIZE);
    for (int k = 0; k < NUMBER_OF_NEURONS; k++)
    {
        neurony[k] = rezim2(k, obrazok);
        neurony[k] = relu(neurony[k]);
    }
    double max = Findmax(neurony, NUMBER_OF_NEURONS);
    double *softmax_pole = softmax(neurony, max);
    printf("%d", Findmax_index(softmax_pole, NUMBER_OF_NEURONS));
}

int main(void)
{
    int rezim = 0, index = 0;
    scanf("%d", &rezim);
    if (rezim == 1)
    {
        scanf("%d", &index);
        rezim1(index);
    }
    else if (rezim == 2)
    {
        scanf("%d", &index);
        double obrazok[IMAGE_SIZE];
        load_data(obrazok,IMAGE_SIZE);
        printf("%.2lf", rezim2(index, obrazok));
    }
    else if (rezim == 3)
        rezim3();
    else if (rezim == 4)
        rezim4();
    else if (rezim == 5)
        rezim5();
    else if (rezim == 6)
        rezim6();
    else if (rezim == 7)
        rezim7();
    return 0;
}
