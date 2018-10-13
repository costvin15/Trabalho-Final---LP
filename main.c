#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers/produtos.h"
#include "headers/clientes.h"

void adicionando(Produtos *);
void lendo(Produtos *);
void buscando(Produtos *);

int main(void){
    Produtos *lista;
    lista = criar_produtos();

    popular_produtos(lista);
    
    adicionando(lista);
    lendo(lista);

    buscando(lista);
    salvar_produtos(lista);

    apagar_produtos(lista);
    return 0;
}

void adicionando(Produtos *lista){
    printf("***** ADICIONAR *****\n");
    
    struct produto p;
    while (1){
        printf("Nome: ");
        gets(p.nome);
        printf("Em estoque: ");
        scanf("%d", &p.EmEstoque);
        printf("Preco: ");
        scanf("%lf", &p.preco);

        adicionar_produto(lista, p);

        char cancontinue;
        fflush(stdin);
        printf("Continuar? (S/N): ");
        scanf("%c", &cancontinue);
        if (cancontinue == 'N')
            break;
    }
}

void lendo(Produtos *lista){
    int i;
    printf("Tam: %d\n", lista->tamanho);
    for (i = 0; i < lista->tamanho; i++){
        printf("***** PRODUTO *****\n");
        printf("Nome: %s\n", lista->produtos[i].nome);
        printf("Em estoque: %d\n", lista->produtos[i].EmEstoque);
        printf("Preco: %lf\n", lista->produtos[i].preco);
    }
}

void buscando(Produtos *lista){
    printf("***** BUSCAR *****\n");
    printf("Pesquisar: ");
    
    int ocorrencias;
    char termo[50];
    fflush(stdin);
    gets(termo);

    struct produto **resultados = buscar_produto(lista, termo, &ocorrencias);

    printf("Encontramos %d resultados.\n", ocorrencias);
    int i;
    for (i = 0; i < ocorrencias; i++){
        printf("***** PRODUTO *****\n");
        printf("Nome: %s\n", resultados[i]->nome);
        printf("Em estoque: %d\n", resultados[i]->EmEstoque);
        printf("Preco: %lf\n", resultados[i]->preco);    
    }
}