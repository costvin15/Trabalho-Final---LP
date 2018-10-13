#include "../headers/util.h"

char *palavra_para_palavra_minuscula(char *palavra){
    int i;
    char *resultante = (char *) malloc((strlen(palavra) + 1) * sizeof(char));

    if (palavra == NULL)
        return NULL;

    for (i = 0; palavra[i] != '\0'; i++)
        if (isupper(palavra[i]))
            resultante[i] = tolower(palavra[i]);
        else
            resultante[i] = palavra[i];
    resultante[i] = '\0';
    return resultante;
}