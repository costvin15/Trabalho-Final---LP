//  ;=====================================
//  ;  Title: produtos.c
//  ;  Author: Vinicius Costa Castro
//  ;  Date: 10/13/18
//  ;=====================================

#include "../headers/produtos.h"

Produtos *criar_produtos(){
    Produtos *nova_lista = (Produtos *) malloc(sizeof(Produtos));
    if (nova_lista == NULL)
        return NULL;

    nova_lista->tamanho = 0;
    nova_lista->produtos = NULL;
    return nova_lista;
}

void apagar_produtos(Produtos *lista){
    free(lista->produtos);
    free(lista);
}

void popular_produtos(Produtos *lista){
    FILE *dados;
    struct produto p;    
    int quantidade;

    dados = fopen("data/produtos.data", "r");
    if (dados == NULL)
        return;
    
    fscanf(dados, "%d ", &quantidade);

    for (; quantidade > 0; quantidade--){
        fscanf(dados, "%d ", &p.id);
        fscanf(dados, "%[^\n]%*c", &p.nome);
        fscanf(dados, "%d ", &p.EmEstoque);
        fscanf(dados, "%lf ", &p.preco);
        adicionar_produto(lista, p);
    }
}

void salvar_produtos(Produtos *lista){
    FILE *dados = fopen("data/produtos.data", "w");
    if (dados == NULL)
        return;

    fprintf(dados, "%d\n", lista->tamanho);
    int i;
    for (i = 0; i < lista->tamanho; i++){
        fprintf(dados, "%d\n", lista->produtos[i].id);
        fprintf(dados, "%s\n", lista->produtos[i].nome);
        fprintf(dados, "%d\n", lista->produtos[i].EmEstoque);
        fprintf(dados, "%lf\n", lista->produtos[i].preco);
    }
}

static int BuscaBinaria(Produtos *lista, char *termo, int inicio, int fim){
    int retorno, meio = (inicio + fim) / 2;
    
    char *palavra1, *palavra2;
    palavra1 = palavra_para_palavra_minuscula(lista->produtos[meio].nome);
    palavra2 = palavra_para_palavra_minuscula(termo);

    if (strcmp(palavra1, palavra2) == 0)
        retorno = meio;
    else
        if (inicio > fim)
            retorno = inicio;
        else
            if (strcmp(palavra1, palavra2) > 0)
                retorno = BuscaBinaria(lista, termo, inicio, meio - 1);
            else
                retorno = BuscaBinaria(lista, termo, meio + 1, fim);
    
    free(palavra1);
    free(palavra2);
    return retorno;
}

void adicionar_produto(Produtos *lista, struct produto novo_produto){
    lista->produtos = (struct produto *) realloc(lista->produtos, (lista->tamanho + 1) * sizeof(struct produto));
    int i, pos = BuscaBinaria(lista, novo_produto.nome, 0, lista->tamanho - 1);

    for (i = lista->tamanho - 1; i >= pos; i--)
        lista->produtos[i + 1] = lista->produtos[i];
    lista->produtos[pos] = novo_produto;
    lista->produtos[pos].id = lista->tamanho;
    lista->tamanho++;
}

struct produto **buscar_produto(Produtos *lista, char *termo, int *ocorrencias){
    int pos = BuscaBinaria(lista, termo, 0, lista->tamanho - 1);
    struct produto **resultados = (struct produto **) malloc(sizeof(struct produto *));
    char *termo1, *termo2;

    *ocorrencias = 0;

    while (strstr((termo1 = palavra_para_palavra_minuscula(lista->produtos[pos].nome)), (termo2 = palavra_para_palavra_minuscula(termo))) != NULL){
        resultados[(*ocorrencias)++] = &(lista->produtos[pos++]);
        resultados = (struct produto **) realloc(resultados, (*ocorrencias + 1) * sizeof(struct produto *));

        free(termo1);
        free(termo2);
    }

    return resultados;
}