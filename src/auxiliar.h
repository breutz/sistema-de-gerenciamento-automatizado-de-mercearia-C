#ifndef AUXILIAR_H
#define AUXILIAR_H

/* Limpa qualquer caractere restante no buffer do teclado */
void limparBuffer(void);
/* Lê um número inteiro garantindo que esteja entre min e max */
int lerInteiroIntervalo(const char *mensagem, int min, int max);
/* Lê um número float garantindo que seja maior ou igual a zero */
float lerFloatPositivo(const char *mensagem);
/* Lê uma string com segurança garantindo o limite de caracteres */
void lerTexto(const char *mensagem, char *destino, int tamanhoMaximo);

/* Funções auxiliares de data (DD/MM/AAAA) */
void obterDataAtual(char *destino);
int validarData(const char *data);
void lerData(const char *mensagem, char *destino);
int dataParaInteiro(const char *data);

#endif