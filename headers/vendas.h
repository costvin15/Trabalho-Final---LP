//  ;=====================================
//  ;  Title: vendas.h
//  ;  Author: Vinicius Costa Castro
//  ;  Date: 10/24/18
//  ;=====================================

#ifndef VENDAS_H
#define VENDAS_H

#include "clientes.h"
#include "produtos.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct venda {
    struct produto produto;
    struct cliente cliente;
    time_t horario;
};

typedef struct {
    int tamanho;
    struct venda *vendas;
} Vendas;

Vendas *criar_vendas();
void apagar_vendas(Vendas *);

int adicionar_venda(Vendas *, struct produto, struct cliente);

#endif