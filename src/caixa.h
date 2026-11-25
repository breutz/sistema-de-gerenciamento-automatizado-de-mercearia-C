#ifndef CAIXA_H
#define CAIXA_H

typedef struct {
    float saldoAbertura;
    float saldoCaixa;
    float totalSangrias;
    int caixaAberto;
    int pagamentoInicialSessao; // Indice do primeiro pagamento desta sessao de caixa
} Caixa;

struct Sistema;

void aberturaCaixa(struct Sistema *s);
void realizarSangria(struct Sistema *s);
void fechamentoCaixa(struct Sistema *s);

#endif
