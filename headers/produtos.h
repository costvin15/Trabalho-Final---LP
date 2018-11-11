//  ;=====================================
//  ;  Title: produtos.h
//  ;  Author: Vinicius Costa Castro
//  ;  Date: 10/13/18
//  ;=====================================

#ifndef PRODUTOS_H
#define PRODUTOS_H

#include "util.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct produto {
    unsigned int index;
    char nome[100];
    int EmEstoque;
    double preco;
};

typedef struct {
    int tamanho;
    struct produto *produtos;
} Produtos;

Produtos *reutilizar_produtos();
Produtos *criar_produtos();
void apagar_produtos(Produtos *);

void popular_produtos(Produtos *);
void salvar_produtos(Produtos *);

int adicionar_produto(Produtos *, struct produto);
int remover_produto(Produtos *, int);
int modificar_produto(Produtos *, int, struct produto *);
struct produto **buscar_produto(Produtos *, char *, int *);

#endif