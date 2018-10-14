#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers/produtos.h"
#include "headers/clientes.h"

void menu();

int main(void){
    return 0;
}

void menu(){
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    time_t currentimespan = time(NULL);
    struct tm *currentime = localtime(&currentime);

    printf("***** MENU *****\n");
    printf("%d/%d/%d - %d - %d - %d");
    printf("( 1 ) Nova venda");
}