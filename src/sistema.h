#ifndef SISTEMA_H
#define SISTEMA_H

#include "usuario.h"
#include "cliente.h"
#include "produto.h"
#include "caixa.h"
#include "venda.h"

typedef struct Sistema {
    Caixa caixa;
    ListaUsuarios usuarios;
    ListaClientes clientes;
    ListaCategorias categorias;
    ListaProdutos produtos;
    ListaVendas vendas;
    ListaPagamentos pagamentos;
    Usuario usuarioLogado;
    int logado;
} Sistema;

#endif
