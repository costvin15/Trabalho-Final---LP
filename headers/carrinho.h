#ifndef CARRINHO_H
#define CARRINHO_H

#include "produtos.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct produto_vendido {
    unsigned int index;
    char nome[100];
    double preco;
    int quantidade;
};

typedef struct {
    unsigned int tamanho;
    struct produto_vendido *produtos;
    double desconto;
    int descontopercento;
    double preco;
    double preco_final;
} Carrinho;

Carrinho *criar_carrinho();
void apagar_carrinho(Carrinho *);

void adicionar_produto_vendido_carrinho(Carrinho *, struct produto_vendido);

void adicionar_produto_carrinho(Carrinho *, struct produto, int);
void remover_produto_carrinho(Carrinho *, int);

void definir_desconto(Carrinho *, int);

#endif