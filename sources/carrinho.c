#include "../headers/carrinho.h"

Carrinho *criar_carrinho(){
    Carrinho *lista = (Carrinho *) malloc(sizeof(Carrinho));
    lista->tamanho = 0;
    lista->produtos = NULL;
    lista->desconto = 0.00;
    lista->preco_final = 0.00;
}

void apagar_carrinho(Carrinho *lista){
    if (!lista){
        fprintf(stderr, "O carrinho precisa ser inicializado primeiro.\n");
        return;
    }

    free(lista->produtos);
    free(lista);
}

void redefinir_carrinho(Carrinho *lista){
    apagar_carrinho(lista);
    lista = criar_carrinho();
}

void adicionar_produto_carrinho(Carrinho *lista, struct produto produto, int quantidade){
    if (!lista){
        fprintf(stderr, "O carrinho precisa ser inicializado primeiro.\n");
        return;
    }
    
    lista->produtos = (struct produto_vendido *) realloc(lista->produtos, (lista->tamanho + 1) * sizeof(struct produto_vendido));
    lista->produtos[lista->tamanho].index = lista->tamanho;
    strcpy(lista->produtos[lista->tamanho].nome, produto.nome);
    lista->produtos[lista->tamanho].preco = produto.preco;
    lista->preco += produto.preco * quantidade;
    lista->preco_final = lista->preco - ((lista->preco * lista->desconto) / 100);
    lista->tamanho++;
}

void remover_produto_carrinho(Carrinho *lista, int index){
    if (!lista){
        fprintf(stderr, "O carrinho precisa ser inicializado primeiro.\n");
        return;
    }

    lista->preco -= lista->produtos[index].preco;
    lista->preco_final = lista->preco - ((lista->preco * lista->desconto) / 100);
    
    for (; index < lista->tamanho - 1; index++)
        lista->produtos[index] = lista->produtos[index + 1];
    lista->produtos = (struct produto_vendido *) realloc(lista->produtos, (lista->tamanho - 1) * sizeof(struct produto_vendido));
    lista->tamanho--;
}

void definir_desconto(Carrinho *lista, double desconto){
    if (!lista){
        fprintf(stderr, "O carrinho precisa ser inicializado primeiro.\n");
        return;
    }

    lista->desconto = desconto;
    lista->preco_final = lista->preco - ((lista->preco * lista->desconto) / 100);
}