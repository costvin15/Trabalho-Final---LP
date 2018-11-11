//  ;=====================================
//  ;  Title: produtos.c
//  ;  Author: Vinicius Costa Castro
//  ;  Date: 13/10/18
//  ;=====================================

#include "../headers/produtos.h"

Produtos *reutilizar_produtos(){
    static Produtos *lista = NULL;
    if (lista)
        return lista;
    lista = criar_produtos();
    popular_produtos(lista);
    return lista;
}

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
        fscanf(dados, "%[^\n]%*c\n", &p.nome);
        fscanf(dados, "%d\n", &p.EmEstoque);
        fscanf(dados, "%lf\n", &p.preco);
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

int adicionar_produto(Produtos *lista, struct produto novo_produto){
    lista->produtos = (struct produto *) realloc(lista->produtos, (lista->tamanho + 1) * sizeof(struct produto));
    int i, pos = BuscaBinaria(lista, novo_produto.nome, 0, lista->tamanho - 1);

    if (strcmp(lista->produtos[pos].nome, novo_produto.nome) == 0)
        return false;
    

    for (i = lista->tamanho - 1; i >= pos; i--){
        lista->produtos[i].index = i + 1;
        lista->produtos[i + 1] = lista->produtos[i];
    }    
    
    novo_produto.index = pos;
    lista->produtos[pos] = novo_produto;
    lista->tamanho++;
    return true;
}

int remover_produto(Produtos *lista, int index){
    for (; index < lista->tamanho; index++)
        lista->produtos[index] = lista->produtos[index + 1];
    lista->produtos = (struct produto *) realloc(lista->produtos, --(lista->tamanho) * sizeof(struct produto));
    return true;
}

int modificar_produto(Produtos *lista, int index, struct produto *produto){
    lista->produtos[index] = *produto;
    return true;
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