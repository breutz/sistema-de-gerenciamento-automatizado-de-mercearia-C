#include <stdio.h>
#include <string.h>
#include <math.h>
#include "caixa.h"
#include "sistema.h"
#include "usuario.h"
#include "auxiliar.h"

void aberturaCaixa(Sistema *s) {
    if (s->caixa.caixaAberto == 1) {
        printf("\nAVISO: O caixa ja esta aberto!\n");
        return;
    }

    if (s->usuarioLogado.tipo != 1) {
        if (autorizarAdmin(&s->usuarios) == 0) {
            return;
        }
    }

    printf("\n================ ABERTURA DE CAIXA ===============\n");

    float valor = lerFloatPositivo("Digite o valor de abertura: R$ ");

    s->caixa.saldoAbertura = valor;
    s->caixa.saldoCaixa = valor;
    s->caixa.totalSangrias = 0.0f;
    s->caixa.caixaAberto = 1;
    s->caixa.pagamentoInicialSessao = s->pagamentos.total;

    printf("--------------------------------------------------\n");
    printf("%-35s R$ %11.2f\n", "Caixa aberto! Valor de abertura:", valor);
    printf("==================================================\n");
}

void realizarSangria(Sistema *s) {
    if (s->caixa.caixaAberto == 0) {
        printf("\nAVISO: O caixa esta fechado!\n");
        return;
    }

    if (s->usuarioLogado.tipo != 1) {
        if (autorizarAdmin(&s->usuarios) == 0) {
            return;
        }
    }

    printf("\n=========== RETIRADA DE CAIXA (SANGRIA) ==========\n");
    printf("%-35s R$ %11.2f\n", "Saldo atual em gaveta:", s->caixa.saldoCaixa);
    printf("%-35s R$ %11.2f\n", "Reserva obrigatoria para troco:", 50.0f);
    printf("--------------------------------------------------\n");

    if (s->caixa.saldoCaixa <= 50.0f) {
        printf("\nERRO: Saldo insuficiente para sangria.\n"
               "O valor minimo de R$ 50.00 deve ser preservado!\n");
        return;
    }

    float valor = lerFloatPositivo("Digite o valor da retirada: R$ ");

    if (valor <= 0.0f) {
        printf("\nERRO: O valor da retirada deve ser maior que zero!\n");
        return;
    }

    if ((s->caixa.saldoCaixa - valor) < 50.0f) {
        printf("\nERRO: Retirada nao permitida!\n"
               "Necessario minimo de R$ 50.00 no caixa para troco.\n");
        printf("%-35s R$ %11.2f\n", "Maximo permitido para retirada:", s->caixa.saldoCaixa - 50.0f);
        return;
    }

    s->caixa.saldoCaixa -= valor;
    s->caixa.totalSangrias += valor;

    printf("--------------------------------------------------\n");
    printf("%-35s R$ %11.2f\n", "Sangria realizada com sucesso:", valor);
    printf("%-35s R$ %11.2f\n", "Novo saldo em gaveta:", s->caixa.saldoCaixa);
    printf("==================================================\n");
}

void fechamentoCaixa(Sistema *s) {
    if (s->caixa.caixaAberto == 0) {
        printf("\nAVISO: O caixa ja esta fechado!\n");
        return;
    }

    if (s->usuarioLogado.tipo != 1) {
        if (autorizarAdmin(&s->usuarios) == 0) {
            return;
        }
    }

    int qtdPagas = 0;
    float faturamento = 0.0f;
    float vlPagoD = 0.0f, vlPagoMd = 0.0f;
    float vlPagoC = 0.0f, vlPagoMc = 0.0f;

    // Considera apenas os pagamentos efetivados a partir da abertura deste caixa
    int inicio = s->caixa.pagamentoInicialSessao;
    if (inicio < 0 || inicio > s->pagamentos.total) {
        inicio = 0;
    }

    for (int i = inicio; i < s->pagamentos.total; i++) {
        Pagamento pag = s->pagamentos.dados[i];
        qtdPagas++;
        faturamento += pag.valorPago;

        if (strcmp(pag.tipo, "d") == 0) {
            vlPagoD += pag.valorPago;
        } else if (strcmp(pag.tipo, "c") == 0) {
            vlPagoC += pag.valorPago;
        } else if (strcmp(pag.tipo, "m") == 0) {
            vlPagoMd += pag.parcelaDinheiro;
            vlPagoMc += pag.parcelaCartao;
        }
    }

    float conciliacao = faturamento - s->caixa.saldoAbertura - vlPagoD - vlPagoMd - vlPagoC - vlPagoMc;

    printf("\n=============== FECHAMENTO DE CAIXA ==============\n");
    printf("%-35s %14d\n", "Quantidade de vendas pagas:", qtdPagas);
    printf("%-35s R$ %11.2f\n", "Faturamento total do dia:", faturamento);
    printf("%-35s R$ %11.2f\n", "Valor de abertura:", s->caixa.saldoAbertura);
    printf("%-35s R$ %11.2f\n", "Valor pago em Dinheiro:", vlPagoD);
    printf("%-35s R$ %11.2f\n", "Parcela Dinheiro (Misto):", vlPagoMd);
    printf("%-35s R$ %11.2f\n", "Valor pago em Cartao:", vlPagoC);
    printf("%-35s R$ %11.2f\n", "Parcela Cartao (Misto):", vlPagoMc);
    printf("%-35s R$ %11.2f\n", "Total de sangrias:", s->caixa.totalSangrias);
    printf("%-35s R$ %11.2f\n", "Saldo final em gaveta:", s->caixa.saldoCaixa);
    printf("--------------------------------------------------\n");
    printf("%-35s R$ %11.2f\n", "Resultado da conciliacao:", conciliacao);
    printf("==================================================\n");

    if (fabsf(conciliacao) > 0.009f) {
        printf("\nATENCAO: O caixa nao podera ser fechado porque ha\ndivergencia de valores.\n");
        printf("%-35s R$ %11.2f\n",
               (conciliacao > 0.0f) ? "Ajuste apurado (SOBRA):" : "Ajuste apurado (FALTA):",
               conciliacao);

        int confirma = lerInteiroIntervalo("\nRegistrar ajuste e fechar? (1 - Sim | 0 - Nao): ", 0, 1);
        if (confirma == 0) {
            printf("\nFechamento cancelado. Verifique o caixa.\n");
            return;
        }
        printf("\nAjuste de R$ %.2f registrado com sucesso!\n", conciliacao);
    } else {
        printf("\nConferencia concluida: Divergencia ZERO!\n");
    }

    s->caixa.caixaAberto = 0;
    s->caixa.saldoAbertura = 0.0f;
    s->caixa.saldoCaixa = 0.0f;
    s->caixa.totalSangrias = 0.0f;

    printf("Caixa fechado com sucesso!\n");
}
