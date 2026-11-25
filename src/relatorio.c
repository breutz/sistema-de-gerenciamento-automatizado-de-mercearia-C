#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "relatorio.h"
#include "auxiliar.h"

/* Indices da matriz consolidada [2][2] de pagamentos */
#define LIN_DINHEIRO 0
#define LIN_CARTAO   1
#define COL_PURO     0
#define COL_MISTO    1

/* --------------     Structs auxiliares internas dos relatórios     -------------- */
typedef struct {
    int codigoCliente;
    char nomeCliente[60];
    int qtdCompras;
    float totalGasto;
} ResumoClientePeriodo;

typedef struct {
    int codigoProduto;
    char descricao[50];
    int qtdVendida;
    float valorTotal;
} RankingProduto;

typedef struct {
    int qtdVendasPagas;
    int qtdVendasAbertas;
    float totalFaturado;
    float matrizPagamentos[2][2]; // [LIN_DINHEIRO/LIN_CARTAO][COL_PURO/COL_MISTO]
} ResumoFinanceiro;

/* --------------     Protótipos auxiliares privados     -------------- */
static void lerPeriodo(char *dataIni, char *dataFim, int *iniInt, int *fimInt);
static Produto *criarCopiaProdutosOrdenada(const ListaProdutos *lista);
static ResumoFinanceiro calcularResumoPeriodo(const Sistema *s, int iniInt, int fimInt);

/* --------------     Menu de Relatórios     -------------- */
void menuRelatorios(const Sistema *s) {
    int opcao = 0;

    do {
        printf("\n=============== MENU DE RELATORIOS ===============\n");
        printf("1. %47s\n", "Clientes em Ordem Alfabetica");
        printf("2. %47s\n", "Clientes que Compraram no Periodo");
        printf("3. %47s\n", "Produtos em Ordem Alfabetica");
        printf("4. %47s\n", "Produtos com Estoque Zero ou Minimo");
        printf("5. %47s\n", "Produtos Mais Vendidos no Periodo");
        printf("6. %47s\n", "Vendas por Periodo");
        printf("7. %47s\n", "Faturamento Consolidado");
        printf("8. %47s\n", "Consolidado em Dinheiro");
        printf("9. %47s\n", "Consolidado em Cartao");
        printf("0. %47s\n", "Voltar ao Menu Principal");

        opcao = lerInteiroIntervalo("Digite o codigo do relatorio desejado: ", 0, 9);

        switch (opcao) {
            case 1: relatorioClientesAlfabetico(s);     break;
            case 2: relatorioClientesPorPeriodo(s);     break;
            case 3: relatorioProdutosAlfabetico(s);     break;
            case 4: relatorioProdutosEstoqueCritico(s); break;
            case 5: relatorioProdutosMaisVendidos(s);   break;
            case 6: relatorioVendasPorPeriodo(s);       break;
            case 7: relatorioFaturamentoConsolidado(s); break;
            case 8: relatorioConsolidadoDinheiro(s);    break;
            case 9: relatorioConsolidadoCartao(s);      break;
            case 0: printf("\nVoltando...\n");          break;
        }
    } while (opcao != 0);
}

/* --------------     Clientes em ordem alfabetica     -------------- */
void relatorioClientesAlfabetico(const Sistema *s) {
    listarClientes(&s->clientes);
}

/* --------------     Clientes que compraram no periodo     -------------- */
void relatorioClientesPorPeriodo(const Sistema *s) {
    if (s->vendas.total == 0) {
        printf("\nAVISO: Nao existem vendas registradas no sistema!\n");
        return;
    }

    char dataIni[11], dataFim[11];
    int iniInt = 0, fimInt = 0;
    printf("\n============== CLIENTES POR PERIODO ==============\n");
    lerPeriodo(dataIni, dataFim, &iniInt, &fimInt);

    ResumoClientePeriodo *resumo = NULL;
    int totalUnicos = 0;

    for (int i = 0; i < s->vendas.total; i++) {
        Venda v = s->vendas.dados[i];
        int dataVenda = dataParaInteiro(v.data);

        if (dataVenda < iniInt || dataVenda > fimInt) {
            continue;
        }

        // Verifica se o cliente já foi adicionado ao resumo para evitar duplicidade
        int pos = -1;
        for (int j = 0; j < totalUnicos; j++) {
            if (resumo[j].codigoCliente == v.codigoCliente) {
                pos = j;
                break;
            }
        }

        if (pos != -1) {
            resumo[pos].qtdCompras++;
            resumo[pos].totalGasto += v.valorFinal;
        } else {
            ResumoClientePeriodo *temp = realloc(resumo, (size_t)(totalUnicos + 1) * sizeof(ResumoClientePeriodo));
            if (temp == NULL) {
                printf("\nERRO: Falha ao alocar memoria para o relatorio!\n");
                free(resumo);
                return;
            }
            resumo = temp;
            resumo[totalUnicos].codigoCliente = v.codigoCliente;
            strcpy(resumo[totalUnicos].nomeCliente, v.nomeCliente);
            resumo[totalUnicos].qtdCompras = 1;
            resumo[totalUnicos].totalGasto = v.valorFinal;
            totalUnicos++;
        }
    }

    if (totalUnicos == 0) {
        printf("\nAVISO: Nenhum cliente comprou no periodo!\nPeriodo: %s a %s\n", dataIni, dataFim);
        return;
    }

    // Ordena alfabeticamente pelo nome do cliente
    for (int i = 0; i < totalUnicos - 1; i++) {
        for (int j = 0; j < totalUnicos - 1 - i; j++) {
            if (strcmp(resumo[j].nomeCliente, resumo[j + 1].nomeCliente) > 0) {
                ResumoClientePeriodo aux = resumo[j];
                resumo[j] = resumo[j + 1];
                resumo[j + 1] = aux;
            }
        }
    }

    printf("\nPeriodo: %s a %s\n", dataIni, dataFim);
    printf("==================================================\n");
    printf("%-4s | %-20s | %-4s | %-13s\n", "COD", "NOME", "QTD", "TOTAL");
    printf("--------------------------------------------------\n");

    for (int i = 0; i < totalUnicos; i++) {
        printf("%-4d | %-20.20s | %4d | R$ %10.2f\n",
               resumo[i].codigoCliente,
               resumo[i].nomeCliente,
               resumo[i].qtdCompras,
               resumo[i].totalGasto);
    }
    printf("==================================================\n");
    printf("%-35s %14d\n", "Total de clientes no periodo:", totalUnicos);
    printf("==================================================\n");

    free(resumo);
}

/* --------------     Produtos em ordem alfabetica     -------------- */
void relatorioProdutosAlfabetico(const Sistema *s) {
    if (s->produtos.total == 0) {
        printf("\nAVISO: Nao existem produtos cadastrados!\n");
        return;
    }

    Produto *copia = criarCopiaProdutosOrdenada(&s->produtos);
    if (copia == NULL) {
        return;
    }

    ListaProdutos listaOrdenada = {copia, s->produtos.total};
    listarProdutos(&listaOrdenada);

    free(copia);
}

/* --------------     Produtos com estoque zero ou minimo     -------------- */
void relatorioProdutosEstoqueCritico(const Sistema *s) {
    if (s->produtos.total == 0) {
        printf("\nAVISO: Nao existem produtos cadastrados!\n");
        return;
    }

    Produto *copia = criarCopiaProdutosOrdenada(&s->produtos);
    if (copia == NULL) {
        return;
    }

    int encontrados = 0;
    printf("\n=============== ESTOQUE CRITICO ==================\n");
    printf("%-4s | %-16s | %-5s | %-5s | %-8s\n", "COD", "DESCRICAO", "ATUAL", "MIN", "STATUS");
    printf("--------------------------------------------------\n");

    for (int i = 0; i < s->produtos.total; i++) {
        if (copia[i].ativo == 1 && copia[i].estoque <= copia[i].estoqueMinimo) {
            const char *status = (copia[i].estoque == 0) ? "ZERADO" : "MINIMO";
            printf("%-4d | %-16.16s | %5d | %5d | %-8s\n",
                   copia[i].codigo,
                   copia[i].descricao,
                   copia[i].estoque,
                   copia[i].estoqueMinimo,
                   status);
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("Nenhum produto com estoque zerado ou no minimo!\n");
    }
    printf("==================================================\n");
    printf("%-35s %14d\n", "Total de produtos criticos:", encontrados);
    printf("==================================================\n");

    free(copia);
}

/* --------------     Produtos mais vendidos no periodo     -------------- */
void relatorioProdutosMaisVendidos(const Sistema *s) {
    if (s->vendas.total == 0) {
        printf("\nAVISO: Nao existem vendas registradas no sistema!\n");
        return;
    }

    char dataIni[11], dataFim[11];
    int iniInt = 0, fimInt = 0;
    printf("\n============ PRODUTOS MAIS VENDIDOS ==============\n");
    lerPeriodo(dataIni, dataFim, &iniInt, &fimInt);

    RankingProduto *ranking = NULL;
    int totalProdutos = 0;

    for (int i = 0; i < s->vendas.total; i++) {
        Venda v = s->vendas.dados[i];
        int dataVenda = dataParaInteiro(v.data);

        if (dataVenda < iniInt || dataVenda > fimInt) {
            continue;
        }

        for (int j = 0; j < v.totalItens; j++) {
            ItemVenda item = v.itens[j];

            int pos = -1;
            for (int k = 0; k < totalProdutos; k++) {
                if (ranking[k].codigoProduto == item.codigoProduto) {
                    pos = k;
                    break;
                }
            }

            if (pos != -1) {
                ranking[pos].qtdVendida += item.quantidade;
                ranking[pos].valorTotal += item.subtotal;
            } else {
                RankingProduto *temp = realloc(ranking, (size_t)(totalProdutos + 1) * sizeof(RankingProduto));
                if (temp == NULL) {
                    printf("\nERRO: Falha ao alocar memoria para o ranking!\n");
                    free(ranking);
                    return;
                }
                ranking = temp;
                ranking[totalProdutos].codigoProduto = item.codigoProduto;
                strcpy(ranking[totalProdutos].descricao, item.descricao);
                ranking[totalProdutos].qtdVendida = item.quantidade;
                ranking[totalProdutos].valorTotal = item.subtotal;
                totalProdutos++;
            }
        }
    }

    if (totalProdutos == 0) {
        printf("\nAVISO: Nenhum produto vendido no periodo!\nPeriodo: %s a %s\n", dataIni, dataFim);
        return;
    }

    // Ordena em ordem DECRESCENTE de quantidade vendida (Bubble Sort)
    for (int i = 0; i < totalProdutos - 1; i++) {
        for (int j = 0; j < totalProdutos - 1 - i; j++) {
            if (ranking[j].qtdVendida < ranking[j + 1].qtdVendida) {
                RankingProduto aux = ranking[j];
                ranking[j] = ranking[j + 1];
                ranking[j + 1] = aux;
            }
        }
    }

    printf("\nPeriodo: %s a %s\n", dataIni, dataFim);
    printf("==================================================\n");
    printf("%-4s | %-16s | %-6s | %-15s\n", "POS", "DESCRICAO", "QTD", "TOTAL BRUTO");
    printf("--------------------------------------------------\n");

    for (int i = 0; i < totalProdutos; i++) {
        printf("#%-3d | %-16.16s | %6d | R$ %12.2f\n",
               i + 1,
               ranking[i].descricao,
               ranking[i].qtdVendida,
               ranking[i].valorTotal);
    }
    printf("==================================================\n");
    printf("* Valores brutos (antes de descontos da venda).\n");

    free(ranking);
}

/* --------------     Vendas por periodo     -------------- */
void relatorioVendasPorPeriodo(const Sistema *s) {
    if (s->vendas.total == 0) {
        printf("\nAVISO: Nao existem vendas registradas no sistema!\n");
        return;
    }

    char dataIni[11], dataFim[11];
    int iniInt = 0, fimInt = 0;
    printf("\n=============== VENDAS POR PERIODO ===============\n");
    lerPeriodo(dataIni, dataFim, &iniInt, &fimInt);

    int encontradas = 0;
    float somaPeriodo = 0.0f;

    for (int i = 0; i < s->vendas.total; i++) {
        Venda v = s->vendas.dados[i];
        int dataVenda = dataParaInteiro(v.data);

        if (dataVenda < iniInt || dataVenda > fimInt) {
            continue;
        }

        if (encontradas == 0) {
            printf("\nPeriodo: %s a %s\n", dataIni, dataFim);
            printf("==================================================\n");
        }
        encontradas++;
        somaPeriodo += v.valorFinal;

        const char *statusTexto = (v.situacao == 'p') ? "PAGO" : "ABERTO";
        printf("Venda #%03d   | Data: %-10s | Status: %8s\n", v.numeroVenda, v.data, statusTexto);
        printf("Cliente: %-41.41s\n", v.nomeCliente);
        printf("--------------------------------------------------\n");
        printf("%-9.9s | %-9s | %-9s | %-14s\n", "DESCRICAO", "QTD", "PRECO", "TOTAL");
        printf("--------------------------------------------------\n");

        for (int j = 0; j < v.totalItens; j++) {
            printf("%-9.9s | %9d | R$ %6.2f | R$ %11.2f\n",
                   v.itens[j].descricao,
                   v.itens[j].quantidade,
                   v.itens[j].precoVenda,
                   v.itens[j].subtotal);
        }

        printf("--------------------------------------------------\n");
        printf("%-35s R$ %11.2f\n", "Subtotal:", v.valorTotal);
        printf("%-35s %13.2f%%\n", "Desconto:", v.desconto);
        printf("%-35s R$ %11.2f\n", "VALOR FINAL:", v.valorFinal);

        if (v.situacao == 'p') {
            for (int k = 0; k < s->pagamentos.total; k++) {
                if (s->pagamentos.dados[k].numeroVenda == v.numeroVenda) {
                    Pagamento pag = s->pagamentos.dados[k];
                    if (strcmp(pag.tipo, "d") == 0) {
                        printf("%-20s %29s\n", "Forma de Pagamento:", "Dinheiro");
                    } else if (strcmp(pag.tipo, "c") == 0) {
                        printf("%-20s %29s\n", "Forma de Pagamento:", "Cartao");
                    } else if (strcmp(pag.tipo, "m") == 0) {
                        printf("%-20s %29s\n", "Forma de Pagamento:", "Misto (Dinheiro + Cartao)");
                        printf("%-35s R$ %11.2f\n", "  Parcela Dinheiro:", pag.parcelaDinheiro);
                        printf("%-35s R$ %11.2f\n", "  Parcela Cartao:", pag.parcelaCartao);
                    }
                    break;
                }
            }
        }
        printf("==================================================\n");
    }

    if (encontradas == 0) {
        printf("\nAVISO: Nenhuma venda encontrada no periodo!\nPeriodo: %s a %s\n", dataIni, dataFim);
        return;
    }

    printf("%-35s %14d\n", "Total de vendas no periodo:", encontradas);
    printf("%-35s R$ %11.2f\n", "Soma das vendas no periodo:", somaPeriodo);
    printf("==================================================\n");
}

/* --------------     Faturamento consolidado     -------------- */
void relatorioFaturamentoConsolidado(const Sistema *s) {
    if (s->vendas.total == 0) {
        printf("\nAVISO: Nao existem vendas registradas no sistema!\n");
        return;
    }

    char dataIni[11], dataFim[11];
    int iniInt = 0, fimInt = 0;
    printf("\n============ FATURAMENTO CONSOLIDADO =============\n");
    lerPeriodo(dataIni, dataFim, &iniInt, &fimInt);

    ResumoFinanceiro r = calcularResumoPeriodo(s, iniInt, fimInt);
    const char nomesFormas[2][10] = {"Dinheiro", "Cartao"};

    printf("\nPeriodo: %s a %s\n", dataIni, dataFim);
    printf("==================================================\n");
    printf("%-35s %14d\n", "Vendas pagas no periodo:", r.qtdVendasPagas);
    printf("%-35s %14d\n", "Vendas em aberto no periodo:", r.qtdVendasAbertas);
    printf("--------------------------------------------------\n");
    printf("%-9s | %-10s | %-10s | %-12s\n", "FORMA", "PURO", "MISTO", "TOTAL");
    printf("--------------------------------------------------\n");

    for (int lin = 0; lin < 2; lin++) {
        float puro = r.matrizPagamentos[lin][COL_PURO];
        float misto = r.matrizPagamentos[lin][COL_MISTO];
        float totalLinha = puro + misto;

        printf("%-9s | R$ %7.2f | R$ %7.2f | R$ %9.2f\n",
               nomesFormas[lin], puro, misto, totalLinha);
    }

    printf("--------------------------------------------------\n");
    printf("%-35s R$ %11.2f\n", "FATURAMENTO TOTAL (PAGAS):", r.totalFaturado);
    printf("==================================================\n");
}

/* --------------     Consolidado em dinheiro     -------------- */
void relatorioConsolidadoDinheiro(const Sistema *s) {
    if (s->vendas.total == 0) {
        printf("\nAVISO: Nao existem vendas registradas no sistema!\n");
        return;
    }

    char dataIni[11], dataFim[11];
    int iniInt = 0, fimInt = 0;
    printf("\n============= CONSOLIDADO DINHEIRO ===============\n");
    lerPeriodo(dataIni, dataFim, &iniInt, &fimInt);

    ResumoFinanceiro r = calcularResumoPeriodo(s, iniInt, fimInt);
    float puro = r.matrizPagamentos[LIN_DINHEIRO][COL_PURO];
    float misto = r.matrizPagamentos[LIN_DINHEIRO][COL_MISTO];
    float totalDinheiro = puro + misto;

    printf("\nPeriodo: %s a %s\n", dataIni, dataFim);
    printf("==================================================\n");
    printf("%-35s R$ %11.2f\n", "Pagamentos puros em Dinheiro:", puro);
    printf("%-35s R$ %11.2f\n", "Parcela em Dinheiro (Misto):", misto);
    printf("--------------------------------------------------\n");
    printf("%-35s R$ %11.2f\n", "TOTAL RECEBIDO EM DINHEIRO:", totalDinheiro);
    printf("==================================================\n");
}

/* --------------     Consolidado em cartao     -------------- */
void relatorioConsolidadoCartao(const Sistema *s) {
    if (s->vendas.total == 0) {
        printf("\nAVISO: Nao existem vendas registradas no sistema!\n");
        return;
    }

    char dataIni[11], dataFim[11];
    int iniInt = 0, fimInt = 0;
    printf("\n============== CONSOLIDADO CARTAO ================\n");
    lerPeriodo(dataIni, dataFim, &iniInt, &fimInt);

    ResumoFinanceiro r = calcularResumoPeriodo(s, iniInt, fimInt);
    float puro = r.matrizPagamentos[LIN_CARTAO][COL_PURO];
    float misto = r.matrizPagamentos[LIN_CARTAO][COL_MISTO];
    float totalCartao = puro + misto;

    printf("\nPeriodo: %s a %s\n", dataIni, dataFim);
    printf("==================================================\n");
    printf("%-35s R$ %11.2f\n", "Pagamentos puros em Cartao:", puro);
    printf("%-35s R$ %11.2f\n", "Parcela em Cartao (Misto):", misto);
    printf("--------------------------------------------------\n");
    printf("%-35s R$ %11.2f\n", "TOTAL RECEBIDO EM CARTAO:", totalCartao);
    printf("==================================================\n");
}

/* --------------     Funcoes auxiliares privadas     -------------- */
static void lerPeriodo(char *dataIni, char *dataFim, int *iniInt, int *fimInt) {
    printf("1. %47s\n", "Hoje");
    printf("2. %47s\n", "Todo o historico");
    printf("3. %47s\n", "Informar intervalo de datas");

    int modo = lerInteiroIntervalo("Escolha o periodo: ", 1, 3);

    if (modo == 1) {
        obterDataAtual(dataIni);
        strcpy(dataFim, dataIni);
        *iniInt = dataParaInteiro(dataIni);
        *fimInt = *iniInt;
        return;
    }

    if (modo == 2) {
        strcpy(dataIni, "01/01/2000");
        strcpy(dataFim, "31/12/2100");
        *iniInt = dataParaInteiro(dataIni);
        *fimInt = dataParaInteiro(dataFim);
        return;
    }

    while (1) {
        lerData("Data inicial (DD/MM/AAAA): ", dataIni);
        lerData("Data final   (DD/MM/AAAA): ", dataFim);

        *iniInt = dataParaInteiro(dataIni);
        *fimInt = dataParaInteiro(dataFim);

        if (*fimInt >= *iniInt) {
            return;
        }
        printf("\nERRO: Data final anterior a data inicial!\n\n");
    }
}

static Produto *criarCopiaProdutosOrdenada(const ListaProdutos *lista) {
    Produto *copia = (Produto *) malloc((size_t)lista->total * sizeof(Produto));
    if (copia == NULL) {
        printf("\nERRO: Falha ao alocar memoria para ordenacao!\n");
        return NULL;
    }

    memcpy(copia, lista->dados, (size_t)lista->total * sizeof(Produto));

    // Ordenação alfabética pela descrição (Bubble Sort)
    for (int i = 0; i < lista->total - 1; i++) {
        for (int j = 0; j < lista->total - 1 - i; j++) {
            if (strcmp(copia[j].descricao, copia[j + 1].descricao) > 0) {
                Produto aux = copia[j];
                copia[j] = copia[j + 1];
                copia[j + 1] = aux;
            }
        }
    }

    return copia;
}

static ResumoFinanceiro calcularResumoPeriodo(const Sistema *s, int iniInt, int fimInt) {
    ResumoFinanceiro r = {0};

    for (int i = 0; i < s->vendas.total; i++) {
        Venda v = s->vendas.dados[i];
        int dataVenda = dataParaInteiro(v.data);

        if (dataVenda < iniInt || dataVenda > fimInt) {
            continue;
        }

        if (v.situacao != 'p') {
            r.qtdVendasAbertas++;
            continue;
        }

        r.qtdVendasPagas++;
        r.totalFaturado += v.valorFinal;

        for (int k = 0; k < s->pagamentos.total; k++) {
            if (s->pagamentos.dados[k].numeroVenda == v.numeroVenda) {
                Pagamento pag = s->pagamentos.dados[k];
                if (strcmp(pag.tipo, "d") == 0) {
                    r.matrizPagamentos[LIN_DINHEIRO][COL_PURO] += pag.valorPago;
                } else if (strcmp(pag.tipo, "c") == 0) {
                    r.matrizPagamentos[LIN_CARTAO][COL_PURO] += pag.valorPago;
                } else if (strcmp(pag.tipo, "m") == 0) {
                    r.matrizPagamentos[LIN_DINHEIRO][COL_MISTO] += pag.parcelaDinheiro;
                    r.matrizPagamentos[LIN_CARTAO][COL_MISTO] += pag.parcelaCartao;
                }
                break;
            }
        }
    }

    return r;
}
