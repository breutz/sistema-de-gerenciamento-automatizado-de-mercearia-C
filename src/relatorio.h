#ifndef RELATORIO_H
#define RELATORIO_H

#include "sistema.h"

/* Submenu principal de relatórios */
void menuRelatorios(const Sistema *s);

/* Relatórios gerenciais */
void relatorioClientesAlfabetico(const Sistema *s);
void relatorioClientesPorPeriodo(const Sistema *s);
void relatorioProdutosAlfabetico(const Sistema *s);
void relatorioProdutosEstoqueCritico(const Sistema *s);
void relatorioProdutosMaisVendidos(const Sistema *s);
void relatorioVendasPorPeriodo(const Sistema *s);
void relatorioFaturamentoConsolidado(const Sistema *s);
void relatorioConsolidadoDinheiro(const Sistema *s);
void relatorioConsolidadoCartao(const Sistema *s);

#endif
