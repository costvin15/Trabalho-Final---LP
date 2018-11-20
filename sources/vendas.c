#include "../headers/vendas.h"

Vendas *reutilizar_vendas(){
    static Vendas *lista = NULL;
    if (lista)
        return lista;
    lista = criar_vendas();
    popular_vendas(lista);
    return lista;
}

Vendas *criar_vendas(){
    Vendas *lista = (Vendas *) malloc(sizeof(Vendas));
    lista->tamanho = 0;
    lista->vendas = NULL;
    return lista;
}

void apagar_vendas(Vendas *lista){
    if (!lista){
        fprintf(stderr, "As vendas precisam ser inicializadas primeiro.\n");
        return;
    }

    free(lista->vendas);
    free(lista);
}

void popular_vendas(Vendas *lista){
    FILE *arquivo;
    Carrinho *carrinho;
    struct comprador cliente;
    struct produto_vendido produto;
    long int horario;
    int i, j, quant;
    double desconto;
    
    carrinho = criar_carrinho();
    arquivo = fopen("data/vendas.data", "r");

    if (!arquivo)
        return;

    fscanf(arquivo, "%d ", &quant);

    for (i = 0; i < quant; i++){
        fscanf(arquivo, "%ld\n", &horario);
        fscanf(arquivo, "%[^\n]%*c\n", &cliente.nome);
        fscanf(arquivo, "%[^\n]%*c\n", &cliente.endereco);
        fscanf(arquivo, "%[^\n]%*c\n", &cliente.telefone);

        fscanf(arquivo, "%d\n", &j);
        for (; j > 0; j--){
            fscanf(arquivo, "%[^\n]%*c\n", &produto.nome);
            fscanf(arquivo, "%lf\n", &produto.preco);
            fscanf(arquivo, "%d\n", &produto.quantidade);
            adicionar_produto_vendido_carrinho(carrinho, produto);
        }

        fscanf(arquivo, "%lf\n", &carrinho->desconto);
        fscanf(arquivo, "%d\n", &carrinho->descontopercento);
        fscanf(arquivo, "%lf\n", &carrinho->preco);
        fscanf(arquivo, "%lf\n", &carrinho->preco_final);

        adicionar_venda(lista, cliente, carrinho, (time_t) horario);
        carrinho = criar_carrinho();
    }

    fclose(arquivo);
}

void salvar_vendas(Vendas *lista){
    int i, j;
    char horario[21];
    struct tm *shorario;
    FILE *arquivo;
    arquivo = fopen("data/vendas.data", "w");

    fprintf(arquivo, "%d\n", lista->tamanho);
    for (i = 0; i < lista->tamanho; i++){
        fprintf(arquivo, "%ld\n", lista->vendas[i].horario);
        fprintf(arquivo, "%s\n", lista->vendas[i].cliente.nome);
        fprintf(arquivo, "%s\n", lista->vendas[i].cliente.endereco);
        fprintf(arquivo, "%s\n", lista->vendas[i].cliente.telefone);
        fprintf(arquivo, "%d\n", lista->vendas[i].carrinho.tamanho);
        
        for (j = 0; j < lista->vendas[i].carrinho.tamanho; j++){
            fprintf(arquivo, "%s\n", lista->vendas[i].carrinho.produtos[j].nome);
            fprintf(arquivo, "%lf\n", lista->vendas[i].carrinho.produtos[j].preco);
            fprintf(arquivo, "%d\n", lista->vendas[i].carrinho.produtos[j].quantidade);
        }

        fprintf(arquivo, "%lf\n", lista->vendas[i].carrinho.desconto);
        fprintf(arquivo, "%d\n", lista->vendas[i].carrinho.descontopercento);
        fprintf(arquivo, "%lf\n", lista->vendas[i].carrinho.preco);
        fprintf(arquivo, "%lf\n", lista->vendas[i].carrinho.preco_final);
    }

    fclose(arquivo);
}

int buscabinaria(Vendas *lista, long int busca, int inicio, int fim){
    int meio = (inicio + fim) / 2;
    if (lista->vendas[meio].horario == busca)
        return meio;
    if (inicio > fim)
        return inicio;
    else
        if (lista->vendas[meio].horario > busca)
            return buscabinaria(lista, busca, inicio, meio - 1);
        else
            return buscabinaria(lista, busca, meio + 1, fim);
}

void adicionar_venda(Vendas *lista, struct comprador cliente, Carrinho *carrinho, time_t horario){
    if (!lista){
        fprintf(stderr, "As vendas precisam ser inicializadas primeiro.\n");
        return;
    }

    lista->vendas = (struct venda *) realloc(lista->vendas, (lista->tamanho + 1) * sizeof(struct venda));

    int index = buscabinaria(lista, horario, 0, lista->tamanho - 1);

    lista->vendas[index].index = index;
    lista->vendas[index].cliente = cliente;
    lista->vendas[index].horario = (time_t) horario;
    lista->vendas[index].carrinho = *carrinho;
    lista->tamanho++;
}

void nova_venda(Vendas *lista, struct cliente cliente, Carrinho *carrinho){
    if (!lista){
        fprintf(stderr, "As vendas precisam ser inicializadas primeiro.\n");
        return;
    }

    lista->vendas = (struct venda *) realloc(lista->vendas, (lista->tamanho + 1) * sizeof(struct venda));
    
    time_t horario;
    time(&horario);

    int index = buscabinaria(lista, horario, 0, lista->tamanho - 1);
    
    lista->vendas[index].index = index;
    sprintf(lista->vendas[index].cliente.nome, "%s", cliente.nome);
    sprintf(lista->vendas[index].cliente.endereco, "%s %s - %u, %s, %s - %s", cliente.endereco.logradouro, cliente.endereco.endereco, cliente.endereco.casa, cliente.endereco.bairro, cliente.endereco.cidade, cliente.endereco.estado);
    sprintf(lista->vendas[index].cliente.telefone, "(%u) %s", cliente.telefone.ddd, cliente.telefone.telefone);
    time(&(lista->vendas[index].horario));
    lista->vendas[index].carrinho = *carrinho;
    lista->tamanho++;
}

void remover_venda(Vendas *lista, int index){
    for (; index < lista->tamanho - 1; index++)
        lista->vendas[index] = lista->vendas[index + 1];
    lista->vendas = (struct venda *) realloc(lista->vendas, (lista->tamanho - 1) * sizeof(struct venda));
    lista->tamanho--;
}