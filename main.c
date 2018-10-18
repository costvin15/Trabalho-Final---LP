#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "headers/produtos.h"
#include "headers/clientes.h"

/*void adicionando(Produtos *);
 *void lendo(Produtos *);
 *void buscando(Produtos *);

 *int main(void){
 *   Produtos *lista;
 *   lista = criar_produtos();
 *
 *   popular_produtos(lista);
 *   
 *   adicionando(lista);
 *   lendo(lista);
 *
 *   buscando(lista);
 *   salvar_produtos(lista);
 *
 *   apagar_produtos(lista);
 *   return 0;
 *}
 */

static Clientes *listaClientes();

void aguardar();
void limparTela();
void menu();
void novocliente();
void listarclientes();

int main(void){
    Clientes *c = listaClientes();
    popular_clientes(c);

    menu();

    salvar_clientes(c);
}

void aguardar(){
    char c;
    while(((c=getchar()) != '\n') && c != EOF);

    printf("Pressione alguma tecla para continuar.");
    getchar();
}

void limparTela(){
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

Clientes *listaClientes(){
    static Clientes *lista = NULL;
    if (lista == NULL)
        lista = criar_clientes();

    return lista;
}

void menu(){
    int cancontinue = true;
    while (cancontinue){
        limparTela();
        printf("***********************************\n");
        printf("***** SISTEMA DE LOJA - NERDZ *****\n");
        printf("***********************************\n\n");
        printf("( 1 ) Novo cliente\n");
        printf("( 2 ) Listar clientes\n");
        printf("( 0 ) Sair\n");

        char c;
        fseek(stdin,0,SEEK_END);
        scanf("%c", &c);

        switch (c){
            case '1':
                novocliente();
                break;
            case '2':
                listarclientes();
                break;
            case '0':
                limparTela();
                printf("Saindo...\n");
                cancontinue = false;
                break;
            default:
                break;
        }
    }
}

void novocliente(){
    limparTela();
    printf("***********************************\n");
    printf("***** SISTEMA DE LOJA - NERDZ *****\n");
    printf("*****       NOVO USUARIO      *****\n");
    printf("***********************************\n\n");
    
    Clientes *c = listaClientes();
    struct cliente novo_cliente;

    printf("Nome: ");
    getchar();
    scanf("%[^\n]s", &novo_cliente.nome);

    printf("Telefone - DDD (Dois dígitos): ");
    getchar();
    scanf("%u", &novo_cliente.telefone.ddd);

    printf("Telefone - Número: ");
    getchar();
    scanf("%[^\n]s", &novo_cliente.telefone.telefone);

    printf("Endereço - Logradouro: ");
    getchar();
    scanf("%[^\n]s", &novo_cliente.endereco.logradouro);

    printf("Endereço - Nome: ");
    getchar();
    scanf("%[^\n]s", &novo_cliente.endereco.endereco);

    printf("Endereço - Número da casa ou apartamento: ");
    getchar();
    scanf("%u", &novo_cliente.endereco.casa);

    printf("Endereço - Cidade: ");
    getchar();
    scanf("%[^\n]s", &novo_cliente.endereco.cidade);

    printf("Endereço - Estado: ");
    getchar();
    scanf("%[^\n]s", &novo_cliente.endereco.estado);

    printf("Endereço - País: ");
    getchar();
    scanf("%[^\n]s", &novo_cliente.endereco.pais);

    adicionar_cliente(c, novo_cliente);
    aguardar();
}

void listarclientes(){
    limparTela();
    printf("***********************************\n");
    printf("***** SISTEMA DE LOJA - NERDZ *****\n");
    printf("*****     LISTAR USUARIOS     *****\n");
    printf("***********************************\n\n");
    
    int i;
    Clientes *c = listaClientes();
    for (i = 0; i < c->tamanho; i++){
        printf("***** Cliente %d *****\n", i + 1);
        printf("Nome: %s\n", c->clientes[i].nome);
        printf("Telefone: (%u) %s\n", c->clientes[i].telefone.ddd, c->clientes[i].telefone.telefone);
        printf("Endereço: %s %s - %u, %s - %s, %s\n\n", c->clientes[i].endereco.logradouro, c->clientes[i].endereco.endereco, c->clientes[i].endereco.casa, c->clientes[i].endereco.cidade, c->clientes[i].endereco.cidade, c->clientes[i].endereco.estado, c->clientes[i].endereco.pais);
    }
    
    aguardar();
}

/*void adicionando(Produtos *lista){
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
}*/