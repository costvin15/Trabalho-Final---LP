//  ;=====================================
//  ;  Title: clientes.c
//  ;  Author: Vinicius Costa Castro
//  ;  Date: 10/13/18
//  ;=====================================

#include "../headers/clientes.h"

Clientes *reutilizar_clientes(){
    static Clientes *lista = NULL;
    if (lista)
        return lista;
    lista = criar_clientes();
    popular_clientes(lista);
    return lista;
}

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
        fscanf(dados, "%[^\n]%*c\n", &c.nome);
        fscanf(dados, "%d\n", &c.telefone.ddd);
        fscanf(dados, "%[^\n]%*c\n", &c.telefone.telefone);
        fscanf(dados, "%[^\n]%*c\n", &c.endereco.logradouro);
        fscanf(dados, "%[^\n]%*c\n", &c.endereco.endereco);
        fscanf(dados, "%d\n", &c.endereco.casa);
        fscanf(dados, "%[^\n]%*c\n", &c.endereco.bairro);
        fscanf(dados, "%[^\n]%*c\n", &c.endereco.cidade);
        fscanf(dados, "%[^\n]%*c\n", &c.endereco.estado);

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
        fprintf(dados, "%s\n", lista->clientes[i].endereco.bairro);
        fprintf(dados, "%s\n", lista->clientes[i].endereco.cidade);
        fprintf(dados, "%s\n", lista->clientes[i].endereco.estado);
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

int adicionar_cliente(Clientes *lista, struct cliente novo_cliente){
    lista->clientes = (struct cliente *) realloc(lista->clientes, (lista->tamanho + 1) * sizeof(struct cliente));
    int i, pos = BuscaBinaria(lista, novo_cliente.nome, 0, lista->tamanho - 1);

    if (strcmp(lista->clientes[pos].nome, novo_cliente.nome) == 0)
        return false;

    for (i = lista->tamanho - 1; i >= pos; i--){
        lista->clientes[i].index = i + 1;
        lista->clientes[i + 1] = lista->clientes[i];
    }

    novo_cliente.index = pos;
    lista->clientes[pos] = novo_cliente;
    lista->tamanho++;
    return true;
}

int remover_cliente(Clientes *lista, int index){
    for (; index < lista->tamanho; index++){
        lista->clientes[index] = lista->clientes[index + 1];
    }

    printf("-> %d\n", sizeof(struct cliente));
    printf("-> %d\n", (lista->tamanho - 1) * sizeof(struct cliente));
    lista->clientes = (struct cliente *) realloc(lista->clientes, --(lista->tamanho) * sizeof(struct cliente));
}

int modificar_cliente(Clientes *lista, int index, struct cliente *cliente){
    lista->clientes[index] = *cliente;
    return true;
}

struct cliente **buscar_cliente(Clientes *lista, char *termo, int *ocorrencias){
    int pos = BuscaBinaria(lista, termo, 0, lista->tamanho - 1);
    struct cliente **resultados = (struct cliente **) malloc(sizeof(struct cliente *));
    char *termo1, *termo2;

    *ocorrencias = 0;

    while (strstr((termo1 = palavra_para_palavra_minuscula(lista->clientes[pos].nome)), (termo2 = palavra_para_palavra_minuscula(termo))) != NULL){
        resultados[(*ocorrencias)++] = &(lista->clientes[pos++]);
        resultados = (struct cliente **) realloc(resultados, (*ocorrencias + 1) * sizeof(struct cliente *));

        free(termo1);
        free(termo2);
    }

    return resultados;
}