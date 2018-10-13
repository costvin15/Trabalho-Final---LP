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
        fscanf(dados, "%[^\n]%*c", &p.nome);
        fscanf(dados, "%d", &p.EmEstoque);
        fscanf(dados, "%lf", &p.preco);
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
        fprintf(dados, "%s\n", lista->produtos[i].nome);
        fprintf(dados, "%d\n", lista->produtos[i].EmEstoque);
        fprintf(dados, "%lf\n", lista->produtos[i].preco);
    }
}

static int BuscaBinaria(Produtos *lista, char *termo, int inicio, int fim){
    int meio = (inicio + fim) / 2;

    if (strcmp(palavra_para_palavra_minuscula(lista->produtos[meio].nome), termo) == 0)
        return meio;
    if (inicio > fim)
        return inicio;
    else
        if (strcmp(termo, palavra_para_palavra_minuscula(lista->produtos[meio].nome)) > 0)
            return BuscaBinaria(lista, termo, meio + 1, fim);
        else
            return BuscaBinaria(lista, termo, inicio, meio - 1);
}

void adicionar_produto(Produtos *lista, struct produto novo_produto){
    //lista->produtos = (struct produto *) realloc(lista->produtos, (lista->tamanho + 1) * sizeof(struct produto));
    
    lista->tamanho++;
    lista->produtos = (struct produto *) realloc(lista->produtos, lista->tamanho * sizeof(struct produto));
    lista->produtos[lista->tamanho - 1] = novo_produto;

    int i, j;
    char *p1, *p2;
    for (i = 0; i < lista->tamanho - 1; i++)
        for (j = i + 1; j < lista->tamanho; j++){
            p1 = palavra_para_palavra_minuscula(lista->produtos[i].nome);
            p2 = palavra_para_palavra_minuscula(lista->produtos[j].nome);

            if (strcmp(p1, p2) > 0){
                struct produto aux;
                aux = lista->produtos[i];
                lista->produtos[i] = lista->produtos[j];
                lista->produtos[j] = aux;
            }

            free(p1);
            free(p2);
        }
}

struct produto **busca(Produtos *lista, char *produto, int *quantidade){
    int resultado = BuscaBinaria(lista, palavra_para_palavra_minuscula(produto), 0, lista->tamanho - 1);

    *quantidade = 0;
    struct produto **vetor = (struct produto **) malloc(sizeof(struct produto *));

    while (strstr(palavra_para_palavra_minuscula(lista->produtos[resultado].nome), palavra_para_palavra_minuscula(produto)) != NULL){
        vetor[(*quantidade)++] = &(lista->produtos[resultado++]);
        vetor = (struct produto **) realloc(vetor, (*quantidade + 1) * sizeof(struct produto *));
    }

    return vetor;
}