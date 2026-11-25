#include <stdio.h>
#include "sistema.h"
#include "menu.h"
#include "auxiliar.h"
#include "caixa.h"
#include "venda.h"
#include "relatorio.h"

void menuPrincipal(Sistema *s) {
    int opcao = 0;

    do {
        printf("\n================== BERE IS NICE ==================\n");   
        printf("1. %47s\n", "Cadastros");
        printf("2. %47s\n", "Vendas");
        printf("3. %47s\n", "Abertura de Caixa");
        printf("4. %47s\n", "Fechamento de Caixa");
        printf("5. %47s\n", "Relatorios");
        printf("0. %47s\n", "Sair");

        opcao = lerInteiroIntervalo("Digite o codigo do submenu em que deseja entrar: ", 0, 5);

        switch (opcao) {
        case 1: menuCadastros(s); break;
        
        case 2: menuVendas(s); break;

        case 3: aberturaCaixa(s); break;

        case 4: fechamentoCaixa(s); break;

        case 5: menuRelatorios(s); break;

        case 0:
            if (s->caixa.caixaAberto == 1) {
                printf("\n==================================================\n");
                printf("AVISO: O sistema esta fechando com caixa ABERTO!\n");
                printf("Os dados de vendas serao preservados nos arquivos.\n");
                printf("Sera necessario reabrir o caixa no proximo inicio.\n");
                printf("==================================================\n");
            } else {
                printf("\nEncerrando o sistema...\n");
            }
            break;

        default: printf("\nERRO: Opcao invalida!\n"); break;
        }
    } while (opcao != 0);
}

static void submenuUsuarios(Sistema *s) {
    int opcao = 0;
    do {
        printf("\n============= MANUTENCAO DE USUARIOS =============\n");
        printf("1. %47s\n", "Cadastrar Novo Usuario");
        printf("2. %47s\n", "Listar Usuarios");
        printf("3. %47s\n", "Alterar Usuario");
        printf("4. %47s\n", "Excluir Usuario");
        printf("0. %47s\n", "Voltar ao Menu de Cadastros");

        opcao = lerInteiroIntervalo("Escolha uma opcao: ", 0, 4);

        switch (opcao) {
            case 1: cadastrarUsuario(&s->usuarios); break;
            case 2: listarUsuarios(&s->usuarios); break;
            case 3:
                if (s->usuarioLogado.tipo == 1 || autorizarAdmin(&s->usuarios)) {
                    alterarUsuario(&s->usuarios, s->usuarioLogado.login);
                }
                break;
            case 4:
                if (s->usuarioLogado.tipo == 1 || autorizarAdmin(&s->usuarios)) {
                    excluirUsuario(&s->usuarios, s->usuarioLogado.login);
                }
                break;
            case 0: break;
        }
    } while (opcao != 0);
}

static void submenuClientes(Sistema *s) {
    int opcao = 0;
    do {
        printf("\n============= MANUTENCAO DE CLIENTES =============\n");
        printf("1. %47s\n", "Cadastrar Novo Cliente");
        printf("2. %47s\n", "Alterar Cliente");
        printf("3. %47s\n", "Excluir Cliente");
        printf("0. %47s\n", "Voltar ao Menu de Cadastros");

        opcao = lerInteiroIntervalo("Escolha uma opcao: ", 0, 3);

        switch (opcao) {
            case 1: cadastrarCliente(&s->clientes); break;
            case 2: alterarCliente(&s->clientes); break;
            case 3: excluirCliente(&s->clientes); break;
            case 0: break;
        }
    } while (opcao != 0);
}

static void submenuProdutos(Sistema *s) {
    int opcao = 0;
    do {
        printf("\n============= MANUTENCAO DE PRODUTOS =============\n");
        printf("1. %47s\n", "Cadastrar Novo Produto");
        printf("2. %47s\n", "Alterar Produto");
        printf("3. %47s\n", "Descontinuar / Reativar Produto");
        printf("0. %47s\n", "Voltar ao Menu de Cadastros");

        opcao = lerInteiroIntervalo("Escolha uma opcao: ", 0, 3);

        switch (opcao) {
            case 1: cadastrarProduto(&s->produtos, &s->categorias); break;
            case 2: alterarProduto(&s->produtos, &s->categorias); break;
            case 3: alternarStatusProduto(&s->produtos); break;
            case 0: break;
        }
    } while (opcao != 0);
}

static void submenuCategorias(Sistema *s) {
    int opcao = 0;
    do {
        printf("\n============ MANUTENCAO DE CATEGORIAS ============\n");
        printf("1. %47s\n", "Cadastrar Nova Categoria");
        printf("2. %47s\n", "Listar Categorias");
        printf("3. %47s\n", "Alterar Categoria");
        printf("0. %47s\n", "Voltar ao Menu de Cadastros");

        opcao = lerInteiroIntervalo("Escolha uma opcao: ", 0, 3);

        switch (opcao) {
            case 1: cadastrarCategoria(&s->categorias); break;
            case 2: listarCategorias(&s->categorias); break;
            case 3: alterarCategoria(&s->categorias, &s->produtos); break;
            case 0: break;
        }
    } while (opcao != 0);
}

void menuCadastros(Sistema *s) {
    int opcao = 0;

    do {
        printf("\n================ MENU DE CADASTROS ===============\n");
        printf("1. %47s\n", "Usuarios");
        printf("2. %47s\n", "Clientes");
        printf("3. %47s\n", "Produtos");
        printf("4. %47s\n", "Categorias");
        printf("0. %47s\n", "Voltar ao Menu Principal");

        opcao = lerInteiroIntervalo("Digite o codigo do submenu em que deseja entrar: ", 0, 4);

        switch (opcao) {
            case 1: submenuUsuarios(s); break;

            case 2: submenuClientes(s); break;

            case 3: submenuProdutos(s); break;

            case 4: submenuCategorias(s); break;

            case 0: printf("\nVoltando...\n"); break;

            default: printf("\nERRO: Opcao invalida!\n");
        }
    } while (opcao != 0);
}

void menuVendas(Sistema *s) {
    int opcao = 0;
    do {
        printf("\n================== MENU DE VENDAS ================\n");
        printf("1. %47s\n", "Nova Venda");
        printf("2. %47s\n", "Sangria");
        printf("3. %47s\n", "Pagamento");
        printf("0. %47s\n", "Voltar ao Menu Principal");

        opcao = lerInteiroIntervalo("Digite o codigo do submenu em que deseja entrar: ", 0, 3);

        switch (opcao) {
            case 1: novaVenda(&s->vendas, &s->pagamentos, &s->clientes, &s->produtos, &s->caixa); break;

            case 2: realizarSangria(s); break;

            case 3: recuperarVendaAberta(&s->vendas, &s->caixa, &s->pagamentos); break;

            case 0: printf("\nVoltando...\n"); break;

            default: printf("\nERRO: Opcao invalida!\n");
        }
    } while (opcao != 0);
}