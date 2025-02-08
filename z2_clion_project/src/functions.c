/*
 *  ReLU funkcia a pomocne funkcie
 */

#include <stdio.h>
#include "functions.h"

// ReLU funkcia
double relu(const double x){
    if(x>0){
        return x;
    }
    return 0;
}

// funkcia na nacitanie hodnot pola 'data' s dlzkou 'len'
// zo standardneho vstupu (stdin)
void load_data(double *data, const unsigned int len){
    for(unsigned int i = 0; i< len;i++){
        scanf("%lf", &data[i]);
    }
}

// funkcia na pomocny vypis obrazku 'img' (ocakavaju sa pixely v rozsahu <0,1>)
// obrazok 'img' ma sirku 'img_width' a vysku 'img_height'
void print_image(const double* img, unsigned int img_width, unsigned int img_height){
    char white[3] = "WW";
    char black[3] = "..";
    for(unsigned int i=0; i<img_height;i++){
        for(unsigned int j=0; j<img_width;j++){
            if(img[i*img_height+j]>=0.10){
                printf("%s", white);
            }
            else{
                printf("%s", black);
            }
        }
        printf("\n");
    }
    printf("\n");
}



