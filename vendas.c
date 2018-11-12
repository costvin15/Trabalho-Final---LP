//  ;=====================================
//  ;  Title: vendas.c
//  ;  Author: Vinicius Costa Castro
//  ;  Date: 11/11/18
//  ;=====================================


#include <gtk/gtk.h>
#include <stdbool.h>
#include "headers/vendas.h"

static Vendas *VENDAS_LISTA = NULL;
static int dialogOpened = false;

void vendas_gui(GtkBuilder *, Vendas *);
void vendas_aba(GtkBuilder *, Vendas *);
void vendas_botao(GtkBuilder *);

void vendas_gui(GtkBuilder *interface, Vendas *vendas){
    VENDAS_LISTA = vendas;
}

void vendas_aba(GtkBuilder * interface, Vendas * vendas){
}