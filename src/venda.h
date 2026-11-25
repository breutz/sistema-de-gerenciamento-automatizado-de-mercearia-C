#ifndef VENDA_H
#define VENDA_H

#include "cliente.h"
#include "produto.h"
#include "usuario.h"
#include "caixa.h"

typedef struct {
    int codigoProduto;
    int numeroVenda;
    char situacao; // 'a' = aberto | 'p' = pago
    char descricao[50];
    float precoVenda;
    int quantidade;
    float subtotal;
} ItemVenda;

typedef struct {
    int numeroVenda;
    int codigoCliente;
    char nomeCliente[60];
    char data[11];    // Formato DD/MM/AAAA
    ItemVenda *itens; // Vetor dinamico de itens da venda
    int totalItens;
    float valorTotal;
    float desconto;
    float valorFinal;
    char situacao; // 'a' = Aberta, 'p' = Paga
} Venda;


typedef struct {
    int numeroVenda;
    float valorPago;
    char tipo[3]; // "d" = Dinheiro, "c" = Cartão, "m" = Misto
    float parcelaDinheiro;
    float parcelaCartao;
} Pagamento;


typedef struct {
    Venda *dados;
    int total;
} ListaVendas;

typedef struct {
    Pagamento *dados;
    int total;
} ListaPagamentos;

/* --------------     Protótipos de Vendas e Pagamentos     -------------- */
void novaVenda(ListaVendas *vendas, ListaPagamentos *pagamentos, const ListaClientes *clientes, ListaProdutos *produtos, Caixa *caixa);
void processarPagamento(Venda *v, Caixa *caixa, ListaPagamentos *pagamentos);
void recuperarVendaAberta(ListaVendas *vendas, Caixa *caixa, ListaPagamentos *pagamentos);
int adicionarVenda(ListaVendas *lista, Venda nova);
int adicionarPagamento(ListaPagamentos *lista, Pagamento novo);

/* --------------     Protótipos de persistência de dados     -------------- */
int salvarVendas(const ListaVendas *lista);
int carregarVendas(ListaVendas *lista);
int salvarPagamentos(const ListaPagamentos *lista);
int carregarPagamentos(ListaPagamentos *lista);

#endif