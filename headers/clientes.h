//  ;=====================================
//  ;  Title: clientes.h
//  ;  Author: Vinicius Costa Castro
//  ;  Date: 10/13/18
//  ;=====================================

#ifndef CLIENTES_H
#define CLIENTES_H

#include "util.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

struct endereco {
    char logradouro[20];
    char endereco[50];
    unsigned short casa;
    char cidade[50];
    char estado[25];
};

struct telefone {
    unsigned short ddd;
    char telefone[12];
};

struct cliente {
    char nome[100];
    struct telefone telefone;
    struct endereco endereco;
};

typedef struct {
    int tamanho;
    struct cliente *clientes;
} Clientes;

Clientes *criar_clientes();
void apagar_clientes(Clientes *);

void popular_clientes(Clientes *);
void salvar_clientes(Clientes *);

int adicionar_cliente(Clientes *, struct cliente);
int remover_cliente(Clientes *, char *);
struct cliente **buscar_cliente(Clientes *, char *, int *);

#endif