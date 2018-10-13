#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers/produtos.h"
#include "headers/clientes.h"

void adicionando(Clientes *);
void lendo(Clientes *);
void buscando(Clientes *);

int main(void){
    Clientes *lista;
    lista = criar_clientes();

    popular_clientes(lista);
    
    adicionando(lista);
    lendo(lista);

    buscando(lista);
    salvar_clientes(lista);

    apagar_clientes(lista);
    return 0;
}

void adicionando(Clientes *lista){
    printf("***** ADICIONAR *****\n");

    struct cliente c;
    while (1){
        printf("Nome: ");
        gets(c.nome);

        printf("Telefone\n");
        printf("DDD: ");
        scanf("%d", &c.telefone.ddd);
        fflush(stdin);
        printf("Telefone: ");
        gets(c.telefone.telefone);

        printf("Endereco\n");
        printf("Logradouro: ");
        gets(c.endereco.logradouro);
        printf("Endereco: ");
        gets(c.endereco.endereco);
        fflush(stdin);
        printf("Numero da casa/apt: ");
        scanf("%d", &c.endereco.casa);
        fflush(stdin);
        printf("Cidade: ");
        gets(c.endereco.cidade);
        printf("Estado: ");
        gets(c.endereco.estado);
        printf("Pais: ");
        gets(c.endereco.pais);

        adicionar_cliente(lista, c);

        char cancontinue;
        printf("Continuar? (S/N): ");
        fflush(stdin);
        scanf("%c", &cancontinue);

        if (cancontinue == 'N')
            break;
    }
}

void lendo(Clientes *lista){
    int i;
    printf("Tam: %d\n", lista->tamanho);
    for (i = 0; i < lista->tamanho; i++){
        printf("***** CLIENTE *****\n");
        printf("Nome: %s\n", lista->clientes[i].nome);
        printf("Telefone: %d %s\n", lista->clientes[i].telefone.ddd, lista->clientes[i].telefone.telefone);
        printf("Endereco: %s %s - %d, %s, %s, %s\n", lista->clientes[i].endereco.logradouro, lista->clientes[i].endereco.endereco, lista->clientes[i].endereco.casa, lista->clientes[i].endereco.cidade, lista->clientes[i].endereco.estado, lista->clientes[i].endereco.pais);
    }
}

void buscando(Clientes *lista){
    printf("***** BUSCAR *****\n");
    printf("Pesquisar: ");
    
    int ocorrencias;
    char termo[50];
    fflush(stdin);
    gets(termo);

    struct cliente **resultados = buscar_cliente(lista, termo, &ocorrencias);

    printf("Encontramos %d resultados.\n", ocorrencias);
    int i;
    for (i = 0; i < ocorrencias; i++){
        printf("***** CLIENTE *****\n");
        printf("Nome: %s\n", resultados[i]->nome);
        printf("Telefone: %d %s\n", resultados[i]->telefone.ddd, resultados[i]->telefone.telefone);
        printf("Endereco: %s %s - %d, %s, %s, %s\n", resultados[i]->endereco.logradouro, resultados[i]->endereco.endereco, resultados[i]->endereco.casa, resultados[i]->endereco.cidade, resultados[i]->endereco.estado, resultados[i]->endereco.pais);
    }
}