#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "headers/produtos.h"
#include "headers/clientes.h"

static Clientes *listaClientes();

void aguardar();
void limparTela();
void menu();
void novocliente();
void listarclientes();
void buscarcliente();
void apagarcliente();

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
        printf("( 3 ) Buscar cliente\n");
        printf("( 4 ) Remover cliente\n");
        printf("( 0 ) Sair\n");

        char c;
        scanf("%c", &c);

        switch (c){
            case '1':
                novocliente();
                break;
            case '2':
                listarclientes();
                break;
            case '3':
                buscarcliente();
                break;
            case '4':
                apagarcliente();
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
    
    Clientes *lista = listaClientes();
    struct cliente novo_cliente;

    printf("Nome: ");
    getchar();
    scanf("%[^\n]s", &novo_cliente.nome);

    printf("Telefone - DDD (Dois digitos): ");
    getchar();
    scanf("%u", &novo_cliente.telefone.ddd);

    printf("Telefone - Numero: ");
    getchar();
    scanf("%[^\n]s", &novo_cliente.telefone.telefone);

    printf("Endereco - Logradouro: ");
    getchar();
    scanf("%[^\n]s", &novo_cliente.endereco.logradouro);

    printf("Endereco - Nome: ");
    getchar();
    scanf("%[^\n]s", &novo_cliente.endereco.endereco);

    printf("Endereco - Numero da casa ou apartamento: ");
    getchar();
    scanf("%u", &novo_cliente.endereco.casa);

    printf("Endereco - Cidade: ");
    getchar();
    scanf("%[^\n]s", &novo_cliente.endereco.cidade);

    printf("Endereco - Estado: ");
    getchar();
    scanf("%[^\n]s", &novo_cliente.endereco.estado);

    printf("Endereco - Pais: ");
    getchar();
    scanf("%[^\n]s", &novo_cliente.endereco.pais);

    adicionar_cliente(lista, novo_cliente);
    aguardar();
}

void listarclientes(){
    limparTela();
    printf("***********************************\n");
    printf("***** SISTEMA DE LOJA - NERDZ *****\n");
    printf("*****     LISTAR USUARIOS     *****\n");
    printf("***********************************\n\n");
    
    int i;
    Clientes *lista = listaClientes();
    for (i = 0; i < lista->tamanho; i++){
        printf("***** Cliente %d *****\n", i + 1);
        printf("Nome: %s\n", lista->clientes[i].nome);
        printf("Telefone: (%u) %s\n", lista->clientes[i].telefone.ddd, lista->clientes[i].telefone.telefone);
        printf("Endereco: %s %s - %u, %s - %s, %s\n\n", lista->clientes[i].endereco.logradouro, lista->clientes[i].endereco.endereco, lista->clientes[i].endereco.casa, lista->clientes[i].endereco.cidade, lista->clientes[i].endereco.estado, lista->clientes[i].endereco.pais);
    }
    
    aguardar();
}

void buscarcliente(){
    limparTela();
    printf("***********************************\n");
    printf("***** SISTEMA DE LOJA - NERDZ *****\n");
    printf("*****     BUSCAR USUARIOS     *****\n");
    printf("***********************************\n\n");
    
    int i, ocorrencias;
    Clientes *lista;
    struct cliente **resultados;

    char pesquisar[100];
    printf("Digite o nome: ");
    getchar();
    scanf("%[^\n]s", &pesquisar);

    lista = listaClientes();
    ocorrencias = 0;
    resultados = buscar_cliente(lista, pesquisar, &ocorrencias);
    
    printf("\nEncontramos %d correspondencias.\n\n", ocorrencias);

    for (i = 0; i < ocorrencias; i++){
        printf("***** Resultado %d *****\n", i + 1);
        printf("Nome: %s\n", resultados[i]->nome);
        printf("Telefone: (%u) %s\n", resultados[i]->telefone.ddd, resultados[i]->telefone.telefone);
        printf("Endereco: %s %s - %u, %s - %s, %s\n\n", resultados[i]->endereco.logradouro, resultados[i]->endereco.endereco, resultados[i]->endereco.casa, resultados[i]->endereco.cidade, resultados[i]->endereco.estado, resultados[i]->endereco.pais);
    }

    aguardar();
}

void apagarcliente(){
    limparTela();
    printf("***********************************\n");
    printf("***** SISTEMA DE LOJA - NERDZ *****\n");
    printf("*****     REMOVER USUARIOS    *****\n");
    printf("***********************************\n\n");

    int retorno, tipoDeBusca = 0;
    Clientes *lista = listaClientes();
    struct cliente *ContatoAApagar;
    char pesquisar[100], escolha;
    
    printf("Digite o nome: ");
    getchar();
    scanf("%[^\n]s", &pesquisar);

    retorno = remover_cliente(lista, pesquisar, tipoDeBusca);

    if (retorno == -1 && tipoDeBusca == 0){
        printf("Nao encontramos correspondencias para esse nome. Deseja que busquemos nomes parecidos? (S/N) ");
        getchar();
        scanf("%c", &escolha);

        if (escolha == 'S' || escolha == 's'){
            ContatoAApagar = buscar_simples_cliente(lista, pesquisar);

            printf("\n***** RESULTADO ENCONTRADO *****\n");
            printf("Nome: %s\n", ContatoAApagar->nome);
            printf("Telefone: (%u) %s\n", ContatoAApagar->telefone.ddd, ContatoAApagar->telefone.telefone);
            printf("Endereco: %s %s - %u, %s - %s, %s\n\n", ContatoAApagar->endereco.logradouro, ContatoAApagar->endereco.endereco, ContatoAApagar->endereco.casa, ContatoAApagar->endereco.cidade, ContatoAApagar->endereco.estado, ContatoAApagar->endereco.pais);
            
            printf("Tem certeza que deseja apagar? (S/N) ");
            getchar();
            scanf("%c", &escolha);            
                
            if (escolha == 'S' || escolha == 's'){
                tipoDeBusca = 1;
                retorno = remover_cliente(lista, pesquisar, tipoDeBusca);
            } else 
                printf("\nNada foi apagado.\n");
        } else
            printf("\nNada foi apagado.\n");
    }

    if (retorno != -1){
        printf("\n***** REMOVIDO COM SUCESSO *****\n");
    }

    aguardar();
}