# Manual do Usuário — Sistema da Berê

Guia prático de uso diário para a equipe da Mercearia Dona Berê.

---

## 1. Como Abrir o Sistema e Fazer Login

1. Abra a pasta `src` e clique duas vezes em `bere4.exe` (ou execute `.\bere4.exe` pelo terminal).
2. Digite seu **Login** e sua **Senha** e aperte **Enter**.

### Primeiro acesso no computador
Se for a primeira vez que o sistema está sendo aberto, use a conta inicial da gerência:

- Login: `admin123`
- Senha: `123456`

Logo que entrar pela primeira vez, recomendamos trocar essa senha em **Cadastros → Usuários → Alterar Usuário** e criar uma conta para cada funcionário.

### Diferença entre Administrador e Usuário Comum

- **Administrador (Dona Berê / Gerente):** tem acesso liberado a tudo no sistema, incluindo abrir e fechar o caixa, fazer retiradas de dinheiro e alterar ou excluir contas de funcionários.
- **Usuário Comum (Operador de Caixa):** pode cadastrar clientes e produtos, passar compras, receber pagamentos e tirar relatórios. Quando precisar abrir o caixa, fazer sangria ou fechar o caixa, o sistema não bloqueia o atendimento: ele apenas pede o login e a senha do Administrador na hora para autorizar aquela operação.

Importante: você tem até 3 tentativas para digitar a senha correta. Se errar 3 vezes seguidas, o programa fecha por segurança.

---

## 2. Como Navegar pelas Telas

- Em todas as telas, digite o número da opção desejada e aperte **Enter**.
- A opção **0** sempre volta para a tela anterior ou fecha o programa.
- Sempre use ponto (`.`) no lugar de vírgula para centavos. Exemplo: digite `12.50` em vez de `12,50`.
- Ao terminar o expediente, **sempre saia pela opção 0 (Sair)** no Menu Principal. É nesse momento que tudo o que foi feito fica salvo no computador. Não feche a janela clicando no `X`.

---

## 3. Passo a Passo das Tarefas do Dia a Dia

### 3.1 Cadastros (Menu Principal → 1)

Dentro de **Cadastros**, você encontra quatro áreas:

#### Usuários (opção 1)
- **Cadastrar:** crie o login (de 8 a 12 letras ou números, sem espaços), a senha (de 6 a 8 caracteres, sem espaços) e escolha se é Administrador (1) ou Comum (2).
- **Listar:** mostra quem tem acesso ao sistema (as senhas nunca aparecem na tela).
- **Alterar e Excluir:** permitem trocar senha, mudar perfil ou remover um funcionário que não trabalha mais na mercearia (exige perfil ou senha de Administrador). O sistema não deixa excluir o usuário que está usando o caixa no momento nem o último Administrador.

#### Clientes (opção 2)
- **Cadastrar:** preencha nome completo, nome social (se o cliente não usar nome social, basta digitar `-`), CPF, endereço com número, bairro e telefone/WhatsApp. O código do cliente é criado sozinho.
- **Alterar:** mostra a lista de clientes, pede o código e deixa editar só o dado que mudou (como telefone ou endereço) sem precisar redigitar tudo.
- **Excluir:** remove um cadastro após pedir confirmação.

#### Produtos (opção 3)
- **Cadastrar:** informe o nome do produto, escolha a categoria pelo código, digite o preço de compra, a margem de lucro em porcentagem, a quantidade em estoque e o estoque mínimo para alerta. O preço de venda é calculado na hora.
- **Alterar:** serve para atualizar preço de compra, margem de lucro (o preço de venda recalcula sozinho), repor estoque quando chega mercadoria nova ou ajustar o estoque mínimo.
- **Descontinuar / Reativar:** quando a mercearia parar de vender um produto, use esta opção em vez de apagar. Ele some da tela de vendas e dos avisos de estoque, mas o histórico das vendas antigas dele continua guardado. Se voltar a vender no futuro, basta reativar.

#### Categorias (opção 4)
- Permite cadastrar novas categorias (como Alimento, Padaria, Limpeza), listar as existentes ou corrigir o nome de alguma categoria já criada.
- **Dica:** se você tentar cadastrar o primeiro produto sem ter nenhuma categoria criada, o sistema pergunta se quer criar uma categoria na hora.

---

### 3.2 Abrir o Caixa no Início do Dia (Menu Principal → 3)

Antes de passar a primeira compra do dia, é obrigatório abrir o caixa:

1. No Menu Principal, escolha **3 (Abertura de Caixa)**.
2. Se você estiver como Usuário Comum, peça para o Administrador digitar login e senha.
3. Digite quanto dinheiro está entrando na gaveta como fundo de troco (pode ser `0` ou o valor em notas e moedas, como `100.00`).
4. Pronto! O caixa já está aberto para vendas.

---

### 3.3 Registrar uma Nova Venda (Menu Principal → 2 → 1)

1. **Escolha o cliente:** aparece uma lista rápida com o código e o nome de cada cliente. Digite o código do cliente (ou `0` para desistir da venda).
2. **Confirme a data:** o sistema já puxa a data de hoje do computador. Aperte **1** para confirmar. Se precisar lançar uma venda de outro dia, aperte **0** e digite no formato `DD/MM/AAAA` (exemplo: `15/11/2026`).
3. **Passe os produtos:**
   - Aparece a tabela de produtos disponíveis com código, nome, preço e estoque.
   - Digite o código do produto e a quantidade.
   - Se passar o mesmo produto duas vezes na mesma compra, o sistema junta tudo na mesma linha automaticamente.
   - Se o produto estiver **zerado** ou **descontinuado**, o sistema avisa e não deixa vender.
   - Se o cliente pedir mais unidades do que existem na loja, o sistema mostra quantas ainda têm e pergunta se ele quer levar o que sobrou.
   - Quando um produto chega no estoque mínimo, aparece um aviso na tela lembrando que está na hora de repor.
   - Depois de cada item, aperte **1** para passar mais produtos ou **0** para fechar o carrinho.
4. **Confira o carrinho e aplique desconto:** o sistema mostra a lista dos itens e o subtotal. Digite o desconto em porcentagem (`0` se não houver desconto).

---

### 3.4 Receber o Pagamento

Logo após o desconto, aparecem as 4 opções de pagamento:

- **1 — Dinheiro:** digite quanto o cliente entregou.
  - Se o valor cobrir a compra, o sistema mostra o **troco a devolver** e conclui a venda.
  - Se o dinheiro não for suficiente, o sistema mostra quanto faltou e pergunta se o cliente quer pagar o restante no cartão.
- **2 — Cartão:** escolha se vai passar o valor total (**1**) ou só uma parte no cartão e o resto em dinheiro (**0**).
  - Depois de passar na maquininha, digite **1** se aprovou ou **0** se recusou.
  - Se o cartão recusar, não se preocupe: a compra não se perde e o menu de pagamento volta para tentar outra forma.
  - Se o pagamento no cartão for parcial e aprovar, o sistema pede o valor em dinheiro para o restante e calcula o troco.
- **3 — Misto (Dinheiro + Cartão):** atalho rápido para quando o cliente já avisa quanto vai dar em dinheiro e quanto quer passar no cartão.
- **0 — Deixar venda em aberto:** use se o cliente precisar voltar depois para pagar. A venda fica guardada como pendente.

---

### 3.5 Receber uma Venda em Aberto (Menu Principal → 2 → 3)

1. Vá em **Vendas → 3 (Pagamento)**.
2. O sistema lista todas as compras que ficaram abertas, com os itens e o valor final.
3. Digite o número da venda que o cliente veio pagar (ou `0` para voltar) e finalize o pagamento normalmente.

---

### 3.6 Fazer Sangria / Retirada do Caixa (Menu Principal → 2 → 2)

Use quando houver muito dinheiro acumulado na gaveta e o gerente precisar recolher parte dele:

1. Vá em **Vendas → 2 (Sangria)** (exige Administrador).
2. A tela mostra quanto há na gaveta e lembra que sempre precisa sobrar pelo menos **R$ 50,00** para troco.
3. Digite o valor que vai retirar. Se a retirada deixasse menos de R$ 50,00 na gaveta, o sistema bloqueia e mostra qual é o valor máximo permitido.

---

### 3.7 Consultar Relatórios (Menu Principal → 5)

Os relatórios ajudam a acompanhar clientes, estoque e faturamento:

- **1 — Clientes em Ordem Alfabética:** lista completa de todos os clientes de A a Z.
- **2 — Clientes que Compraram no Período:** mostra quem comprou, quantas vezes e quanto gastou.
- **3 — Produtos em Ordem Alfabética:** lista completa dos produtos cadastrados de A a Z.
- **4 — Produtos com Estoque Zero ou Mínimo:** lista rápida do que precisa comprar para repor a prateleira.
- **5 — Produtos Mais Vendidos no Período:** ranking do que mais saiu na mercearia (do mais vendido para o menos vendido).
- **6 — Vendas por Período:** mostra cada cupom de venda com itens, desconto e forma de pagamento.
- **7 — Faturamento Consolidado:** resumo geral das vendas separando dinheiro, cartão e pagamentos mistos.
- **8 — Consolidado em Dinheiro:** tudo que entrou em dinheiro (inclusive a parte em dinheiro dos pagamentos mistos).
- **9 — Consolidado em Cartão:** tudo que entrou no cartão (inclusive a parte no cartão dos pagamentos mistos).

Nos relatórios que pedem período, você pode escolher rapidamente:
- **1 — Hoje** (puxa a data de hoje sozinho);
- **2 — Todo o histórico** (mostra desde o início);
- **3 — Informar intervalo de datas** (você escolhe data inicial e data final no formato `DD/MM/AAAA`).

---

### 3.8 Fechar o Caixa no Fim do Dia (Menu Principal → 4)

1. No Menu Principal, escolha **4 (Fechamento de Caixa)** (exige Administrador).
2. O sistema mostra o resumo do expediente: quantidade de vendas pagas, faturamento, valor de abertura, entradas em dinheiro e cartão, sangrias, saldo em gaveta e o resultado da conferência.
3. Se houver diferença na conferência, o sistema mostra o aviso de divergência informando se é **SOBRA** ou **FALTA** e pergunta se deseja registrar o ajuste e concluir o fechamento (**1 — Sim** ou **0 — Não**).
4. Após fechar o caixa, volte ao Menu Principal e escolha **0 (Sair)** para salvar e encerrar.

---

## 4. Problemas Comuns e Como Resolver

- **Apareceu "Entrada invalida! Digite apenas numeros":** foi digitada uma letra ou vírgula onde o sistema esperava número. Digite novamente usando apenas números e ponto (`.`) para centavos.
- **Apareceu "Data invalida":** confira se digitou dois dígitos para o dia, dois para o mês e quatro para o ano, separados por barra (ex.: `05/11/2026`), e se o dia existe naquele mês.
- **Apareceu "Nao e possivel vender com o caixa fechado":** volte ao Menu Principal e entre primeiro na opção **3 (Abertura de Caixa)**.
- **Apareceu "Autorizacao recusada":** login ou senha incorretos, ou a conta informada não é de Administrador.
- **Esqueci de fechar o caixa antes de sair:** o sistema mostra um aviso, mas salva suas vendas normalmente. No dia seguinte, basta abrir o caixa de novo.

---

## 5. Cuidados Diários e Cópia de Segurança (Backup)

- Sempre encerre o sistema pela opção **0 (Sair)** para garantir que tudo seja gravado.
- Uma vez por semana, copie todos os arquivos que terminam com `.dat` dentro da pasta `src` para um pendrive ou pasta na nuvem (`usuarios.dat`, `clientes.dat`, `categorias.dat`, `produtos.dat`, `vendas.dat` e `pagamentos.dat`). Se acontecer qualquer problema no computador, basta copiar esses arquivos de volta para recuperar todos os dados da mercearia.
