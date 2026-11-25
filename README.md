# Sistema da Berê — BERÊ 4

Sistema de gerenciamento de mercearia e padaria desenvolvido em linguagem C para terminal, com interface formatada em 50 colunas (padrão de cupom de caixa).

## Principais Recursos

- Autenticação por login e senha com dois perfis de acesso (Administrador e Usuário Comum) e autorização administrativa sob demanda.
- Cadastro, alteração e exclusão/descontinuação sem limite fixo de registros para usuários, clientes, categorias e produtos.
- Vendas com carrinho dinâmico, consolidação automática de itens repetidos, cálculo do preço de venda no ato, controle de estoque e desconto percentual.
- Pagamentos em dinheiro (com troco), cartão (com nova tentativa em caso de recusa), pagamento misto nos dois sentidos e recuperação de vendas em aberto.
- Controle de caixa com abertura, sangria preservando reserva mínima de R$ 50,00 para troco e fechamento com conferência de divergência por sessão.
- 9 relatórios gerenciais com ordenação alfabética, ranking de mais vendidos, filtros por período (hoje, histórico completo ou datas manuais) e tabela consolidada com matriz 2x2.
- Salvamento automático e seguro em arquivos binários com gravação atômica.

---

## Estrutura do Projeto

- `src/bere4.c` — Ponto de entrada do programa, tela de login, carregamento/salvamento e liberação de memória.
- `src/sistema.h` — Estrutura central que reúne caixa, listas de dados e usuário logado.
- `src/auxiliar.h` e `src/auxiliar.c` — Leitura segura do teclado, remoção de espaços extras, validação de números e datas.
- `src/usuario.h` e `src/usuario.c` — Login, validação de credenciais, autorização de administrador, manutenção e persistência de usuários.
- `src/cliente.h` e `src/cliente.c` — Cadastro, alteração, exclusão, catálogo compacto, ordenação por nome e persistência de clientes.
- `src/produto.h` e `src/produto.c` — Categorias, produtos (inclusão, alteração e descontinuação), catálogos e persistência.
- `src/caixa.h` e `src/caixa.c` — Abertura, sangria e fechamento de caixa com conferência da sessão.
- `src/venda.h` e `src/venda.c` — Carrinho de compras, formas de pagamento, vendas abertas e persistência de vendas e pagamentos.
- `src/relatorio.h` e `src/relatorio.c` — Menu de relatórios, filtros por período, ordenação, ranking e faturamento consolidado.
- `src/menu.h` e `src/menu.c` — Menu principal e submenus de navegação.
- `docs/DOCUMENTACAO_SOLUCAO.md` — Visão técnica e estrutural do sistema.
- `docs/MANUAL_DO_USUARIO.md` — Guia prático de operação para o dia a dia na mercearia.
- `docs/ROTEIRO_APRESENTACAO.md` — Divisão da apresentação por integrante e perguntas de revisão.

---

## Como Compilar e Executar

### Pelo terminal (Windows / PowerShell)

```powershell
cd src
gcc -Wall -Wextra -g *.c -o bere4.exe
.\bere4.exe
```

### Pelo VS Code

Abra a pasta `projeto-04` no VS Code e pressione `Ctrl + Shift + B`. O executável será gerado em `src/bere4.exe`.

---

## Primeiro Acesso

Quando o sistema é aberto pela primeira vez sem arquivo de usuários, ele cria automaticamente uma conta inicial de administrador:

- Login: `admin123`
- Senha: `123456`
