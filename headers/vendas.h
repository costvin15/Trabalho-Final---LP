//  ;=====================================
//  ;  Title: vendas.h
//  ;  Author: Vinicius Costa Castro
//  ;  Date: 10/13/18
//  ;=====================================

#ifndef VENDAS_H
#define VENDAS_H

#include <time.h>
#include <stdlib.h>

struct venda {
    int id;
    unsigned int produtoId;
    char produto[100];
    unsigned int clienteId;
    char cliente[100];
    unsigned int horario;
};

typedef struct {
    int tamanho;
    struct venda *vendas;
} Vendas;

Vendas *criar_vendas();
void apagar_vendas(Vendas *);

#endif