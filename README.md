# Sistema de Loja - NerdZ

## Descrição

Trabalho Final da Disciplina "Linguagem de Programação" ministrada pelo Prof. Me. Francisco G. N. Clímaco.

### Sobre a loja

Loja de seguimento de varejo focada em vendas de produtos do mercado nerd, geek, pop, decoração criativa e produtos divertidos.

### O sistema é capaz de

Registrar, editar e remover clientes, produtos; realizar vendas, salvando todas as informações no computador que o está executando.

### Como compilar

Utilizando o gcc e a biblioteca GTK+.

```
gcc `pkg-config --cflags gtk+-3.0` -o main sources/util.c sources/clientes.c sources/produtos.c sources/vendas.c sources/carrinho.c main.c `pkg-config --libs gtk+-3.0`
```