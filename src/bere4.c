#include <stdio.h>
#include <stdlib.h>
#include "sistema.h"
#include "auxiliar.h"
#include "menu.h"

/* Libera toda a memoria dinamica alocada durante a execucao */
static void liberarSistema(Sistema *s) {
    for (int i = 0; i < s->vendas.total; i++) {
        free(s->vendas.dados[i].itens);
    }
    free(s->vendas.dados);
    free(s->pagamentos.dados);
    free(s->produtos.dados);
    free(s->categorias.dados);
    free(s->clientes.dados);
    free(s->usuarios.dados);
}

int main(void) {
    Sistema s = {0};

    // Carrega os registros salvos em disco
    carregarUsuarios(&s.usuarios);
    carregarClientes(&s.clientes);
    carregarCategorias(&s.categorias);
    carregarProdutos(&s.produtos);
    carregarVendas(&s.vendas);
    carregarPagamentos(&s.pagamentos);

    // Tela de Login e Autenticação (até 3 tentativas)
    char login[50], senha[50];
    int tentativas = 0;

    printf("\n============== BERE IS NICE - LOGIN ==============\n");
    while (tentativas < 3) {
        lerTexto("Login: ", login, sizeof(login));
        lerTexto("Senha: ", senha, sizeof(senha));

        int indice = autenticarUsuario(&s.usuarios, login, senha);
        if (indice != -1) {
            s.usuarioLogado = s.usuarios.dados[indice];
            s.logado = 1;
            printf("\n--------------------------------------------------\n");
            printf("%-15s %34.34s\n", "Usuario:", s.usuarioLogado.login);
            printf("%-15s %34s\n", "Perfil:",
                   (s.usuarioLogado.tipo == 1) ? "Administrador" : "Usuario Comum");
            printf("--------------------------------------------------\n");

            menuPrincipal(&s);
            break;
        }

        tentativas++;
        printf("\nERRO: Login ou senha invalidos! (%d/3)\n\n", tentativas);
    }

    if (s.logado == 1) {
        // Ao sair de uma sessão válida, garante que os arquivos sejam salvos
        salvarUsuarios(&s.usuarios);
        salvarClientes(&s.clientes);
        salvarCategorias(&s.categorias);
        salvarProdutos(&s.produtos);
        salvarVendas(&s.vendas);
        salvarPagamentos(&s.pagamentos);
    } else {
        printf("Acesso bloqueado. Numero maximo de tentativas.\n");
    }

    liberarSistema(&s);

    printf("\nSistema encerrado com seguranca. Bere is Nice!\n");
    return 0;
}