//  ;=====================================
//  ;  Title: clientes.c
//  ;  Author: Vinicius Costa Castro
//  ;  Date: 10/13/18
//  ;=====================================

#include "../headers/clientes.h"

Clientes *criar_clientes(){
    Clientes *nova_lista = (Clientes *) malloc(sizeof(Clientes));
    if (nova_lista == NULL)
        return NULL;
    nova_lista->tamanho = 0;
    nova_lista->clientes = NULL;
    return nova_lista;
}

void apagar_clientes(Clientes *nova_lista){
    free(nova_lista->clientes);
    free(nova_lista);
}

void popular_clientes(Clientes *lista){
    FILE *dados;
    struct cliente c;
    int quantidade;
    
    dados = fopen("data/clientes.data", "r");
    if (dados == NULL)
        return;
    
    fscanf(dados, "%d ", &quantidade);

    for (; quantidade > 0; quantidade--){
        fscanf(dados, "%[^\n]%*c", &c.nome);
        fscanf(dados, "%d ", &c.telefone.ddd);
        fscanf(dados, "%[^\n]%*c", &c.telefone.telefone);
        fscanf(dados, "%[^\n]%*c", &c.endereco.logradouro);
        fscanf(dados, "%[^\n]%*c", &c.endereco.endereco);
        fscanf(dados, "%d ", &c.endereco.casa);
        fscanf(dados, "%[^\n]%*c", &c.endereco.cidade);
        fscanf(dados, "%[^\n]%*c", &c.endereco.estado);
        fscanf(dados, "%[^\n]%*c", &c.endereco.pais);

        adicionar_cliente(lista, c);
    }
}

void salvar_clientes(Clientes *lista){
    FILE *dados;
    dados = fopen("data/clientes.data", "w");
    
    if (dados == NULL)
        return;
    
    fprintf(dados, "%d\n", lista->tamanho);

    int i;
    for (i = 0; i < lista->tamanho; i++){
        fprintf(dados, "%s\n", lista->clientes[i].nome);
        fprintf(dados, "%d\n", lista->clientes[i].telefone.ddd);
        fprintf(dados, "%s\n", lista->clientes[i].telefone.telefone);
        fprintf(dados, "%s\n", lista->clientes[i].endereco.logradouro);
        fprintf(dados, "%s\n", lista->clientes[i].endereco.endereco);
        fprintf(dados, "%d\n", lista->clientes[i].endereco.casa);
        fprintf(dados, "%s\n", lista->clientes[i].endereco.cidade);
        fprintf(dados, "%s\n", lista->clientes[i].endereco.estado);
        fprintf(dados, "%s\n", lista->clientes[i].endereco.pais);
    }
}

static int BuscaBinaria(Clientes *lista, char *termo, int inicio, int fim){
    int retorno, meio = (inicio + fim) / 2;
    
    char *palavra1, *palavra2;
    palavra1 = palavra_para_palavra_minuscula(lista->clientes[meio].nome);
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

void adicionar_cliente(Clientes *lista, struct cliente novo_cliente){
    lista->clientes = (struct cliente *) realloc(lista->clientes, (lista->tamanho + 1) * sizeof(struct cliente));
    int i, pos = BuscaBinaria(lista, novo_cliente.nome, 0, lista->tamanho - 1);

    for (i = lista->tamanho - 1; i >= pos; i--)
        lista->clientes[i + 1] = lista->clientes[i];
    lista->clientes[pos] = novo_cliente;
    lista->tamanho++;
}

struct cliente **buscar_cliente(Clientes *lista, char *termo, int *ocorrencias){
    int pos = BuscaBinaria(lista, termo, 0, lista->tamanho - 1);
    struct cliente **resultados = (struct cliente **) malloc(sizeof(struct cliente *));
    char *termo1, *termo2;

    *ocorrencias = 0;

    while (strstr((termo1 = palavra_para_palavra_minuscula(lista->clientes[pos].nome)), (termo2 = palavra_para_palavra_minuscula(termo))) != NULL){
        printf("-> %s\n", termo1);
        
        resultados[(*ocorrencias)++] = &(lista->clientes[pos++]);
        resultados = (struct cliente **) realloc(resultados, (*ocorrencias + 1) * sizeof(struct cliente *));

        free(termo1);
        free(termo2);
    }

    return resultados;
}