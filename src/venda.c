#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sistema.h"
#include "venda.h"
#include "auxiliar.h"

static void marcarVendaComoPaga(Venda *v);
static void registrarPagamento(Venda *v, Caixa *caixa, ListaPagamentos *pagamentos, const char *tipo, float parcelaDinheiro, float parcelaCartao);
static void pagarComDinheiro(Venda *v, Caixa *caixa, ListaPagamentos *pagamentos);
static void pagarComCartao(Venda *v, Caixa *caixa, ListaPagamentos *pagamentos);
static void pagarMisto(Venda *v, Caixa *caixa, ListaPagamentos *pagamentos);

void novaVenda(ListaVendas *vendas, ListaPagamentos *pagamentos, const ListaClientes *clientes, ListaProdutos *produtos, Caixa *caixa) {
    if (caixa->caixaAberto == 0) {
        printf("\nERRO: Nao e possivel vender com o caixa fechado!\nAbra o caixa primeiro.\n");
        return;
    }

    if (clientes->total == 0) {
        printf("\nERRO: Nenhum cliente cadastrado no sistema!\n");
        return;
    }

    if (produtos->total == 0) {
        printf("\nERRO: Nenhum produto cadastrado no sistema!\n");
        return;
    }

    Venda v = {0};
    v.numeroVenda = vendas->total + 1;
    v.situacao = 'a';

    // Exibe apenas catálogo compacto (Código + Nome) para escolha rápida
    exibirCatalogoClientes(clientes);

    int codCliente = 0;
    int posCliente = -1;
    do {
        codCliente = lerInteiroIntervalo("Digite o codigo do cliente (0 para cancelar): ", 0, 99999);
        if (codCliente == 0) {
            printf("\nVenda cancelada.\n");
            return;
        }
        posCliente = buscarClientePorCodigo(clientes, codCliente);
        if (posCliente == -1) {
            printf("\nERRO: Cliente nao encontrado!\n");
        }
    } while (posCliente == -1);

    v.codigoCliente = codCliente;
    strcpy(v.nomeCliente, clientes->dados[posCliente].nome);

    // Sugere a data atual do sistema ou permite informar outra manualmente
    char dataHoje[11];
    obterDataAtual(dataHoje);
    printf("\nData atual do sistema: %s\n", dataHoje);
    int usarHoje = lerInteiroIntervalo("Usar data de hoje? (1 - Sim | 0 - Outra data): ", 0, 1);
    if (usarHoje == 1) {
        strcpy(v.data, dataHoje);
    } else {
        lerData("Data da venda (DD/MM/AAAA): ", v.data);
    }

    // Cabeçalho simétrico da venda (50 colunas)
    printf("\n==================================================\n");
    printf("Venda #%03d   | Data: %-10s | Status: %8s\n", v.numeroVenda, v.data, "ABERTO");
    printf("Cliente: %-41.41s\n", v.nomeCliente);
    printf("==================================================\n");

    char continuar = 's';
    do {
        exibirCatalogoProdutos(produtos);

        int codProduto = lerInteiroIntervalo("Digite o codigo do produto (0 para encerrar): ", 0, 99999);
        if (codProduto == 0) {
            break;
        }

        int posProduto = buscarProdutoPorCodigo(produtos, codProduto);
        if (posProduto == -1) {
            printf("\nERRO: Produto nao encontrado!\n");
            continue;
        }

        Produto *p = &produtos->dados[posProduto];

        if (p->ativo == 0) {
            printf("\nERRO: Produto DESCONTINUADO!\n");
            printf("Produto: %-41.41s\n", p->descricao);
            continue;
        }

        if (p->estoque <= 0) {
            printf("\nERRO: Estoque ZERADO! Venda bloqueada.\n");
            printf("Produto: %-41.41s\n", p->descricao);
            continue;
        }

        int qtd = lerInteiroIntervalo("Quantidade desejada: ", 1, 99999);

        if (qtd > p->estoque) {
            printf("\nAVISO: Quantidade indisponivel! Estoque atual: %d\n", p->estoque);
            int aceita = lerInteiroIntervalo("Levar unidades disponiveis? (1-Sim | 0-Nao): ", 0, 1);
            if (aceita == 1) {
                qtd = p->estoque;
            } else {
                printf("\nItem cancelado.\n");
                continue;
            }
        }

        // Calcula o preco de venda atualizado a partir do custo e da margem
        float precoUnitario = p->precoCompra * (1.0f + (p->margemLucro / 100.0f));
        p->precoVenda = precoUnitario;

        // Se o produto ja estiver no carrinho, apenas soma na quantidade existente
        int posItemExistente = -1;
        for (int i = 0; i < v.totalItens; i++) {
            if (v.itens[i].codigoProduto == p->codigo) {
                posItemExistente = i;
                break;
            }
        }

        float subtotalAdicionado = p->precoVenda * qtd;

        if (posItemExistente != -1) {
            v.itens[posItemExistente].quantidade += qtd;
            v.itens[posItemExistente].precoVenda = p->precoVenda;
            v.itens[posItemExistente].subtotal = v.itens[posItemExistente].quantidade * p->precoVenda;
        } else {
            int novoTotal = v.totalItens + 1;
            ItemVenda *temp = (ItemVenda *) realloc(v.itens, novoTotal * sizeof(ItemVenda));
            if (temp == NULL) {
                printf("\nERRO: Falha ao alocar memoria para o item!\n");
                break;
            }

            v.itens = temp;
            v.itens[v.totalItens].numeroVenda = v.numeroVenda;
            v.itens[v.totalItens].situacao = 'a';
            v.itens[v.totalItens].codigoProduto = p->codigo;
            strcpy(v.itens[v.totalItens].descricao, p->descricao);
            v.itens[v.totalItens].precoVenda = p->precoVenda;
            v.itens[v.totalItens].quantidade = qtd;
            v.itens[v.totalItens].subtotal = subtotalAdicionado;
            v.totalItens = novoTotal;
        }

        v.valorTotal += subtotalAdicionado;
        p->estoque -= qtd;

        if (p->estoque <= p->estoqueMinimo) {
            printf("\nAVISO: Produto atingiu o estoque minimo!\n");
            printf("Produto: %-27.27s | Restam: %3d\n", p->descricao, p->estoque);
        }

        printf("\n--------------------------------------------------\n");
        printf("%-35s R$ %11.2f\n", "Item adicionado! Subtotal atual:", v.valorTotal);
        printf("--------------------------------------------------\n");

        int opcContinuar = lerInteiroIntervalo("Adicionar outro produto? (1 - Sim | 0 - Nao): ", 0, 1);
        continuar = (opcContinuar == 1) ? 's' : 'n';

    } while (continuar == 's');

    if (v.totalItens == 0) {
        printf("\nAVISO: Nenhum item adicionado. Venda cancelada.\n");
        return;
    }

    // Exibe resumo dos itens do carrinho antes do desconto e pagamento
    printf("\n=============== RESUMO DO CARRINHO ===============\n");
    printf("%-9.9s | %-9s | %-9s | %-14s\n", "DESCRICAO", "QTD", "PRECO", "TOTAL");
    printf("--------------------------------------------------\n");
    for (int i = 0; i < v.totalItens; i++) {
        printf("%-9.9s | %9d | R$ %6.2f | R$ %11.2f\n",
               v.itens[i].descricao,
               v.itens[i].quantidade,
               v.itens[i].precoVenda,
               v.itens[i].subtotal);
    }
    printf("--------------------------------------------------\n");
    printf("%-35s R$ %11.2f\n", "Subtotal do carrinho:", v.valorTotal);
    printf("==================================================\n");

    while ((v.desconto = lerFloatPositivo("\nPercentual de desconto (0 para nenhum): ")) > 100.0f) {
        printf("\nERRO: Desconto nao pode ser superior a 100%%!\n");
    }

    v.valorFinal = v.valorTotal * (1.0f - (v.desconto / 100.0f));
    printf("\n--------------------------------------------------\n");
    printf("%-35s R$ %11.2f\n", "Subtotal da venda:", v.valorTotal);
    printf("%-35s %13.2f%%\n", "Desconto aplicado:", v.desconto);
    printf("%-35s R$ %11.2f\n", "TOTAL A PAGAR:", v.valorFinal);
    printf("--------------------------------------------------\n");

    processarPagamento(&v, caixa, pagamentos);
    adicionarVenda(vendas, v);
}

void processarPagamento(Venda *v, Caixa *caixa, ListaPagamentos *pagamentos) {
    while (v->situacao == 'a') {
        printf("\n============== PROCESSAR PAGAMENTO ===============\n");
        printf("%-35s R$ %11.2f\n", "Total a pagar:", v->valorFinal);
        printf("--------------------------------------------------\n");
        printf("1. %47s\n", "Dinheiro");
        printf("2. %47s\n", "Cartao");
        printf("3. %47s\n", "Misto (Dinheiro + Cartao)");
        printf("0. %47s\n", "Deixar venda em aberto");

        int opcao = lerInteiroIntervalo("Escolha a forma de pagamento: ", 0, 3);

        switch (opcao) {
            case 1: pagarComDinheiro(v, caixa, pagamentos); break;
            case 2: pagarComCartao(v, caixa, pagamentos);   break;
            case 3: pagarMisto(v, caixa, pagamentos);       break;
            case 0:
                printf("\nAVISO: Venda #%03d deixada em aberto.\n", v->numeroVenda);
                return;
            default:
                printf("\nERRO: Opcao Invalida!\n");
        }
    }
}

void recuperarVendaAberta(ListaVendas *vendas, Caixa *caixa, ListaPagamentos *pagamentos) {
    if (caixa->caixaAberto == 0) {
        printf("\nAVISO: O caixa esta fechado!\n");
        return;
    }

    int totalAbertas = 0;

    for (int i = 0; i < vendas->total; i++) {
        if (vendas->dados[i].situacao != 'a') {
            continue;
        }

        if (totalAbertas == 0) {
            printf("\n=========== VENDAS PENDENTES (ABERTAS) ===========\n");
        }
        totalAbertas++;

        printf("Venda #%03d   | Data: %-10s | Status: %8s\n",
               vendas->dados[i].numeroVenda, vendas->dados[i].data, "ABERTO");
        printf("Cliente: %-41.41s\n", vendas->dados[i].nomeCliente);
        printf("--------------------------------------------------\n");
        printf("%-9.9s | %-9s | %-9s | %-14s\n", "DESCRICAO", "QTD", "PRECO", "TOTAL");
        printf("--------------------------------------------------\n");

        for (int j = 0; j < vendas->dados[i].totalItens; j++) {
            printf("%-9.9s | %9d | R$ %6.2f | R$ %11.2f\n",
                   vendas->dados[i].itens[j].descricao,
                   vendas->dados[i].itens[j].quantidade,
                   vendas->dados[i].itens[j].precoVenda,
                   vendas->dados[i].itens[j].subtotal);
        }
        printf("--------------------------------------------------\n");
        printf("%-35s R$ %11.2f\n", "VALOR FINAL DA COMPRA:", vendas->dados[i].valorFinal);
        printf("==================================================\n");
    }

    if (totalAbertas == 0) {
        printf("\nAVISO: Nao existem vendas em aberto no momento!\n");
        return;
    }

    int opcao = lerInteiroIntervalo("Numero da venda a pagar (0 para voltar): ", 0, 99999);
    if (opcao == 0) {
        return;
    }

    int posicaoVenda = -1;
    for (int i = 0; i < vendas->total; i++) {
        if (vendas->dados[i].numeroVenda == opcao && vendas->dados[i].situacao == 'a') {
            posicaoVenda = i;
            break;
        }
    }

    if (posicaoVenda == -1) {
        printf("\nERRO: Venda nao encontrada ou ja esta paga!\n");
        return;
    }

    processarPagamento(&vendas->dados[posicaoVenda], caixa, pagamentos);
}

int adicionarVenda(ListaVendas *lista, Venda nova) {
    int novoTotal = lista->total + 1;
    Venda *temp = realloc(lista->dados, novoTotal * sizeof(Venda));

    if (temp == NULL) {
        printf("\nERRO: Falha ao alocar memoria para venda!\n");
        return 0;
    }

    lista->dados = temp;
    lista->dados[lista->total] = nova;
    lista->total = novoTotal;

    return 1;
}

int adicionarPagamento(ListaPagamentos *lista, Pagamento novo) {
    int novoTotal = lista->total + 1;
    Pagamento *temp = realloc(lista->dados, novoTotal * sizeof(Pagamento));

    if (temp == NULL) {
        printf("\nERRO: Falha ao alocar memoria para pagamento!\n");
        return 0;
    }

    lista->dados = temp;
    lista->dados[lista->total] = novo;
    lista->total = novoTotal;

    return 1;
}

/* --------------     Funções auxiliares privadas de pagamento     -------------- */
static void marcarVendaComoPaga(Venda *v) {
    v->situacao = 'p';
    for (int i = 0; i < v->totalItens; i++) {
        v->itens[i].situacao = 'p';
    }
}

static void registrarPagamento(Venda *v, Caixa *caixa, ListaPagamentos *pagamentos,
                               const char *tipo, float parcelaDinheiro, float parcelaCartao) {
    if (parcelaDinheiro > 0.0f) {
        caixa->saldoCaixa += parcelaDinheiro; // Apenas o dinheiro líquido entra na gaveta
    }

    Pagamento p = {0};
    p.numeroVenda = v->numeroVenda;
    p.valorPago = v->valorFinal;
    strcpy(p.tipo, tipo);
    p.parcelaDinheiro = parcelaDinheiro;
    p.parcelaCartao = parcelaCartao;

    if (adicionarPagamento(pagamentos, p)) {
        marcarVendaComoPaga(v);
        printf("\n--------------------------------------------------\n");
        printf("Pagamento concluido com sucesso! (Venda #%03d)\n", v->numeroVenda);
        printf("--------------------------------------------------\n");
    }
}

static void pagarComDinheiro(Venda *v, Caixa *caixa, ListaPagamentos *pagamentos) {
    float valorEntregue = lerFloatPositivo("Valor recebido em dinheiro: R$ ");
    if (valorEntregue <= 0.0f) {
        printf("\nERRO: Informe um valor maior que zero!\n");
        return;
    }

    // Caso 1: Dinheiro cobre todo o valor da venda
    if (valorEntregue >= v->valorFinal) {
        float troco = valorEntregue - v->valorFinal;
        registrarPagamento(v, caixa, pagamentos, "d", v->valorFinal, 0.0f);
        printf("%-35s R$ %11.2f\n", "Valor recebido:", valorEntregue);
        printf("%-35s R$ %11.2f\n", "Troco a devolver:", troco);
        printf("==================================================\n");
        return;
    }

    // Caso 2: Dinheiro insuficiente -> oferece complementar no cartao
    float falta = v->valorFinal - valorEntregue;
    printf("\nAVISO: Valor insuficiente! Faltam R$ %.2f.\n", falta);

    int complementar = lerInteiroIntervalo("Pagar restante no cartao? (1 - Sim | 0 - Nao): ", 0, 1);
    if (complementar == 0) {
        printf("\nAVISO: Escolha outra forma ou deixe em aberto.\n");
        return;
    }

    printf("\nValor a cobrar na maquininha: R$ %.2f\n", falta);
    int aprovado = lerInteiroIntervalo("Cartao aprovado (1) ou reprovado (0)? ", 0, 1);
    if (aprovado == 0) {
        printf("\nAVISO: Cartao recusado! Escolha outra forma.\n");
        return;
    }

    registrarPagamento(v, caixa, pagamentos, "m", valorEntregue, falta);
    printf("%-35s R$ %11.2f\n", "Parcela em Dinheiro:", valorEntregue);
    printf("%-35s R$ %11.2f\n", "Parcela no Cartao:", falta);
    printf("==================================================\n");
}

static void pagarComCartao(Venda *v, Caixa *caixa, ListaPagamentos *pagamentos) {
    int pagarTotal = lerInteiroIntervalo("Total no cartao (1) ou parcial + dinheiro (0)? ", 0, 1);
    float valorCartao = v->valorFinal;

    if (pagarTotal == 0) {
        do {
            valorCartao = lerFloatPositivo("Valor a passar no cartao: R$ ");
            if (valorCartao <= 0.0f || valorCartao >= v->valorFinal) {
                printf("\nERRO: Informe valor entre R$ 0.01 e R$ %.2f!\n", v->valorFinal - 0.01f);
            }
        } while (valorCartao <= 0.0f || valorCartao >= v->valorFinal);
    }

    printf("\nValor a cobrar na maquininha: R$ %.2f\n", valorCartao);
    int aprovado = lerInteiroIntervalo("Cartao aprovado (1) ou reprovado (0)? ", 0, 1);
    if (aprovado == 0) {
        printf("\nAVISO: Cartao recusado! Escolha outra forma.\n");
        return;
    }

    // Pagamento 100% no cartao
    if (pagarTotal == 1) {
        registrarPagamento(v, caixa, pagamentos, "c", 0.0f, v->valorFinal);
        printf("%-35s R$ %11.2f\n", "Pago no Cartao:", v->valorFinal);
        printf("==================================================\n");
        return;
    }

    // Cartao parcial -> cobra o restante em dinheiro
    float falta = v->valorFinal - valorCartao;
    printf("\n%-35s R$ %11.2f\n", "Restante a pagar em dinheiro:", falta);

    float valorEntregue = 0.0f;
    do {
        valorEntregue = lerFloatPositivo("Valor recebido em dinheiro: R$ ");
        if (valorEntregue < falta) {
            printf("\nERRO: Valor insuficiente! Minimo: R$ %.2f\n", falta);
        }
    } while (valorEntregue < falta);

    float troco = valorEntregue - falta;
    registrarPagamento(v, caixa, pagamentos, "m", falta, valorCartao);
    printf("%-35s R$ %11.2f\n", "Parcela no Cartao:", valorCartao);
    printf("%-35s R$ %11.2f\n", "Parcela em Dinheiro:", falta);
    printf("%-35s R$ %11.2f\n", "Troco a devolver:", troco);
    printf("==================================================\n");
}

static void pagarMisto(Venda *v, Caixa *caixa, ListaPagamentos *pagamentos) {
    float valorDinheiro = 0.0f;
    do {
        valorDinheiro = lerFloatPositivo("Valor a pagar em dinheiro: R$ ");
        if (valorDinheiro <= 0.0f || valorDinheiro >= v->valorFinal) {
            printf("\nERRO: Informe valor entre R$ 0.01 e R$ %.2f!\n", v->valorFinal - 0.01f);
        }
    } while (valorDinheiro <= 0.0f || valorDinheiro >= v->valorFinal);

    float falta = v->valorFinal - valorDinheiro;
    printf("\nValor restante na maquininha: R$ %.2f\n", falta);

    int aprovado = lerInteiroIntervalo("Cartao aprovado (1) ou reprovado (0)? ", 0, 1);
    if (aprovado == 0) {
        printf("\nAVISO: Cartao recusado! Escolha outra forma.\n");
        return;
    }

    registrarPagamento(v, caixa, pagamentos, "m", valorDinheiro, falta);
    printf("%-35s R$ %11.2f\n", "Parcela em Dinheiro:", valorDinheiro);
    printf("%-35s R$ %11.2f\n", "Parcela no Cartao:", falta);
    printf("==================================================\n");
}

/* --------------     Funções de persistência de dados     -------------- */
int salvarVendas(const ListaVendas *lista) {
    FILE *arq = fopen("vendas.tmp", "wb");
    if (arq == NULL) {
        printf("\nERRO: Falha ao criar arquivo temporario!\n");
        return 0;
    }

    if (fwrite(&lista->total, sizeof(int), 1, arq) != 1) {
        printf("\nERRO: Falha ao gravar a quantidade de vendas!\n");
        fclose(arq);
        remove("vendas.tmp");
        return 0;
    }

    // Serialização em 2 níveis: primeiro a struct Venda, depois seu vetor dinâmico de itens
    for (int i = 0; i < lista->total; i++) {
        Venda vCopia = lista->dados[i];
        vCopia.itens = NULL; // Não grava endereço de memória RAM no disco

        if (fwrite(&vCopia, sizeof(Venda), 1, arq) != 1) {
            printf("\nERRO: Falha ao gravar os dados da venda #%d!\n", lista->dados[i].numeroVenda);
            fclose(arq);
            remove("vendas.tmp");
            return 0;
        }

        if (lista->dados[i].totalItens > 0 && lista->dados[i].itens != NULL) {
            if (fwrite(lista->dados[i].itens, sizeof(ItemVenda), lista->dados[i].totalItens, arq) != (size_t)lista->dados[i].totalItens) {
                printf("\nERRO: Falha ao gravar os itens da venda #%d!\n", lista->dados[i].numeroVenda);
                fclose(arq);
                remove("vendas.tmp");
                return 0;
            }
        }
    }

    fclose(arq);

    remove("vendas.dat");
    if (rename("vendas.tmp", "vendas.dat") != 0) {
        printf("\nERRO: Falha ao atualizar o arquivo de vendas!\n");
        return 0;
    }

    return 1;
}

int carregarVendas(ListaVendas *lista) {
    FILE *arq = fopen("vendas.dat", "rb");
    if (arq == NULL) {
        lista->dados = NULL;
        lista->total = 0;
        return 1;
    }

    int qtd = 0;
    if (fread(&qtd, sizeof(int), 1, arq) != 1) {
        fclose(arq);
        return 0;
    }

    if (qtd > 0) {
        // calloc inicializa tudo zerado (inclusive os ponteiros itens = NULL)
        Venda *temp = (Venda *) calloc((size_t)qtd, sizeof(Venda));
        if (temp == NULL) {
            printf("\nERRO: Falha ao alocar memoria para vendas!\n");
            fclose(arq);
            return 0;
        }

        for (int i = 0; i < qtd; i++) {
            if (fread(&temp[i], sizeof(Venda), 1, arq) != 1) {
                printf("\nERRO: Falha na leitura do registro de vendas!\n");
                for (int k = 0; k < i; k++) free(temp[k].itens);
                free(temp);
                fclose(arq);
                return 0;
            }

            temp[i].itens = NULL; // Descarta qualquer ponteiro antigo antes de reconstruir

            if (temp[i].totalItens > 0) {
                temp[i].itens = (ItemVenda *) malloc((size_t)temp[i].totalItens * sizeof(ItemVenda));
                if (temp[i].itens == NULL ||
                    fread(temp[i].itens, sizeof(ItemVenda), temp[i].totalItens, arq) != (size_t)temp[i].totalItens) {
                    printf("\nERRO: Falha na leitura dos itens da venda!\n");
                    for (int k = 0; k <= i; k++) free(temp[k].itens);
                    free(temp);
                    fclose(arq);
                    return 0;
                }
            }
        }

        lista->dados = temp;
        lista->total = qtd;
    }

    fclose(arq);
    return 1;
}

int salvarPagamentos(const ListaPagamentos *lista) {
    FILE *arq = fopen("pagamentos.tmp", "wb");
    if (arq == NULL) {
        printf("\nERRO: Falha ao criar arquivo temporario!\n");
        return 0;
    }

    if (fwrite(&lista->total, sizeof(int), 1, arq) != 1) {
        printf("\nERRO: Falha ao gravar a quantidade de pagamentos!\n");
        fclose(arq);
        remove("pagamentos.tmp");
        return 0;
    }

    if (lista->total > 0 && lista->dados != NULL) {
        if (fwrite(lista->dados, sizeof(Pagamento), lista->total, arq) != (size_t)lista->total) {
            printf("\nERRO: Falha ao gravar os dados dos pagamentos!\n");
            fclose(arq);
            remove("pagamentos.tmp");
            return 0;
        }
    }

    fclose(arq);

    remove("pagamentos.dat");
    if (rename("pagamentos.tmp", "pagamentos.dat") != 0) {
        printf("\nERRO: Falha ao atualizar arquivo de pagamentos!\n");
        return 0;
    }

    return 1;
}

int carregarPagamentos(ListaPagamentos *lista) {
    FILE *arq = fopen("pagamentos.dat", "rb");
    if (arq == NULL) {
        lista->dados = NULL;
        lista->total = 0;
        return 1;
    }

    int qtd = 0;
    if (fread(&qtd, sizeof(int), 1, arq) != 1) {
        fclose(arq);
        return 0;
    }

    if (qtd > 0) {
        Pagamento *temp = (Pagamento *) realloc(lista->dados, qtd * sizeof(Pagamento));
        if (temp == NULL) {
            printf("\nERRO: Falha ao alocar memoria para pagamentos!\n");
            fclose(arq);
            return 0;
        }

        if (fread(temp, sizeof(Pagamento), qtd, arq) != (size_t)qtd) {
            printf("\nERRO: Falha na leitura do registro de pagamentos!\n");
            free(temp);
            fclose(arq);
            return 0;
        }

        lista->dados = temp;
        lista->total = qtd;
    }

    fclose(arq);
    return 1;
}