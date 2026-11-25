# Sistema de Gerenciamento de Mercearia - Bere is Nice (Versão 4)

## Objetivo

Concluir a profissionalização do sistema de gerenciamento da Mercearia Dona Berê em linguagem C, oferecendo controle de acesso por perfil, cadastros dinâmicos completos, vendas com carrinho e pagamento misto, controle administrativo de caixa, relatórios gerenciais ordenados e filtrados por período, além de salvamento seguro em arquivos binários.

---

## Problemas Resolvidos

- Falta de controle de acesso por usuário e perfil administrativo em operações sensíveis de caixa;
- Limitação de código concentrado em um único arquivo com variáveis globais;
- Cadastros rígidos sem suporte a categorias dinâmicas, alteração de registros e descontinuação segura de produtos;
- Dificuldade para lidar com pagamentos combinados nos dois sentidos (dinheiro + cartão e cartão + dinheiro) e recusa de cartão;
- Ausência de relatórios gerenciais por período, ranking de vendas e visão consolidada por forma de pagamento;
- Risco de corrupção de arquivos durante a gravação dos dados.

---

## Organização Modular do Sistema

O projeto foi separado em módulos independentes (arquivos `.h` para estruturas e protótipos e `.c` para implementações), eliminando variáveis globais e dependências circulares:

- `bere4.c` — Função `main` enxuta que carrega os arquivos, faz o login (até 3 tentativas), inicia o menu principal, salva os dados ao sair e libera toda a memória alocada.
- `sistema.h` — Estrutura principal `Sistema` que concentra o caixa, todas as listas dinâmicas e os dados da sessão atual.
- `auxiliar.h` e `auxiliar.c` — Funções de apoio para limpeza de buffer, leitura segura de inteiros, reais, textos com remoção automática de espaços nas pontas e manipulação/validação de datas.
- `usuario.h` e `usuario.c` — Cadastro, listagem, alteração, exclusão (protegendo o usuário logado e o último administrador), autenticação, autorização administrativa e gravação em `usuarios.dat`.
- `cliente.h` e `cliente.c` — Cadastro, alteração, exclusão com deslocamento na lista, catálogo compacto, relatório ordenado em cópia temporária e gravação em `clientes.dat`.
- `produto.h` e `produto.c` — Cadastro e alteração de categorias, cadastro e alteração de produtos, descontinuação/reativação preservando histórico de vendas, catálogos e gravação em `categorias.dat` e `produtos.dat`.
- `caixa.h` e `caixa.c` — Abertura de caixa, sangria respeitando reserva mínima de R$ 50,00 para troco e fechamento com conferência dos pagamentos da sessão atual.
- `venda.h` e `venda.c` — Carrinho dinâmico com consolidação de produtos repetidos, cálculo de preço no ato da venda, desconto, processamento modular de pagamentos, recuperação de vendas em aberto e gravação em `vendas.dat` e `pagamentos.dat`.
- `relatorio.h` e `relatorio.c` — Submenu de relatórios com ordenação alfabética, estoque crítico, ranking de mais vendidos, filtros por período e consolidação financeira com matriz 2x2.
- `menu.h` e `menu.c` — Menu principal, submenus de manutenção de cadastros, submenu de vendas e aviso ao encerrar com o caixa aberto.

---

## Estruturas de Dados Utilizadas

### Usuario
- `login`: texto de 8 a 12 caracteres (sem espaços)
- `senha`: texto de 6 a 8 caracteres (sem espaços)
- `tipo`: 1 para Administrador ou 2 para Usuário Comum

### Cliente
- `codigo`: identificador numérico único
- `nome`: nome completo
- `nomeSocial`: nome social
- `cpf`: documento do cliente
- `endereco`: rua e número
- `bairro`: bairro
- `telefone`: celular ou WhatsApp

### Categoria e Produto
- `Categoria`: `codigo` e `nome`
- `Produto`: `codigo`, `descricao`, `codigoCategoria`, `nomeCategoria`, `precoCompra`, `margemLucro`, `precoVenda`, `estoque`, `estoqueMinimo` e `ativo` (1 para ativo, 0 para descontinuado)

### Venda e ItemVenda
- `Venda`: `numeroVenda`, `codigoCliente`, `nomeCliente`, `data`, ponteiro `itens` (vetor dinâmico de itens), `totalItens`, `valorTotal`, `desconto`, `valorFinal` e `situacao` (`'a'` aberto ou `'p'` pago)
- `ItemVenda`: `codigoProduto`, `numeroVenda`, `situacao`, `descricao`, `precoVenda`, `quantidade` e `subtotal`

### Pagamento e Caixa
- `Pagamento`: `numeroVenda`, `valorPago`, `tipo` (`"d"`, `"c"` ou `"m"`), `parcelaDinheiro` e `parcelaCartao`
- `Caixa`: `saldoAbertura`, `saldoCaixa`, `totalSangrias`, `caixaAberto` e `pagamentoInicialSessao`

### Estruturas Dinâmicas e Matriz
- Listas dinâmicas sequenciais gerenciadas com `malloc`, `calloc`, `realloc` e `free`, permitindo crescimento livre sem limite fixo.
- Matriz bidimensional `matrizPagamentos[2][2]` no módulo de relatórios para cruzar forma de pagamento (Dinheiro e Cartão) por modalidade (Puro e Misto).

---

## Funcionalidades Principais

### Acesso e Segurança
- Criação automática do administrador padrão (`admin123` / `123456`) na primeira execução.
- Validação rigorosa de tamanho e ausência de espaços para login e senha, sem truncar entradas maiores que o limite.
- Operações sensíveis (abertura, sangria, fechamento de caixa e alteração/exclusão de usuários) solicitam autorização de administrador quando acionadas por usuário comum.

### Vendas e Estoque
- Catálogo compacto de clientes e de produtos ativos (formatados em 50 colunas).
- Sugestão automática da data do computador com opção de digitar outra data validada (incluindo verificação de ano bissexto).
- Bloqueio automático de produtos com estoque zerado ou descontinuados, aviso de estoque mínimo e oferta de levar a quantidade disponível quando o pedido exceder o estoque.
- Consolidação automática de itens repetidos no carrinho e resumo dos itens antes do desconto.

### Pagamentos e Caixa
- Pagamento em dinheiro com cálculo de troco (somando na gaveta apenas o valor líquido da venda).
- Pagamento em cartão com simulação de aprovação (1) ou recusa (0), retornando ao menu de pagamento em caso de recusa sem perder a venda.
- Pagamento misto nos dois sentidos (dinheiro insuficiente completado no cartão, cartão parcial completado em dinheiro ou opção mista direta).
- Possibilidade de deixar a venda em aberto e quitá-la posteriormente pelo menu de pagamento.
- Sangria bloqueada caso deixe menos de R$ 50,00 no caixa.
- Fechamento conferindo apenas as movimentações da sessão aberta, com tolerância para precisão de ponto flutuante e registro de ajuste em caso de divergência.

### Relatórios Gerenciais
- Clientes em ordem alfabética (ordenados em cópia temporária para não alterar a lista original).
- Clientes que compraram em um período informado.
- Produtos em ordem alfabética e listagem de produtos ativos com estoque zerado ou no mínimo.
- Ranking de produtos mais vendidos no período em ordem decrescente.
- Vendas realizadas no período com itens, desconto e forma de pagamento.
- Faturamento consolidado em tabela 2x2 e consolidados individuais de dinheiro e cartão.
- Atalho rápido de período permitindo escolher Hoje, Todo o histórico ou digitar datas manualmente.

---

## Persistência em Arquivos

O sistema grava e recupera os dados em arquivos binários dentro da pasta `src`:

- `usuarios.dat`
- `clientes.dat`
- `categorias.dat`
- `produtos.dat`
- `vendas.dat`
- `pagamentos.dat`

Todas as gravações usam troca atômica (`.tmp` renomeado para `.dat` apenas após confirmar o sucesso de todas as escritas). No arquivo `vendas.dat`, cada venda é salva junto com seu bloco de itens em sequência e reconstruída dinamicamente na leitura. Ao encerrar o programa, toda a memória alocada é liberada.
