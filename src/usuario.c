#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "usuario.h"
#include "auxiliar.h"


/* --------------     Funções de cadastro     -------------- */
void cadastrarUsuario(ListaUsuarios *lista) {
    Usuario novo;
    char tempLogin[50], tempSenha[50];

    printf("\n=============== CADASTRO DE USUARIO ==============\n");

    while (1) {
        lerTexto("Nome de usuario (8 a 12 caracteres): ", tempLogin, sizeof(tempLogin));
        lerTexto("Senha (6 a 8 caracteres): ", tempSenha, sizeof(tempSenha));

        if (!validarCredenciais(tempLogin, tempSenha)) {
            continue;
        }

        if (buscarUsuarioPorLogin(lista, tempLogin) != -1) {
            printf("\nERRO: Ja existe um usuario com este login!\n\n");
            continue;
        }

        break;
    }

    novo.tipo = lerInteiroIntervalo("Hierarquia (1 - Admin | 2 - Comum): ", 1, 2);

    strcpy(novo.login, tempLogin);
    strcpy(novo.senha, tempSenha);

    if (adicionarUsuario(lista, novo)) {
        printf("\nUsuario cadastrado com sucesso!\n");
        printf("Login: %-22.22s | Perfil: %-10s\n",
               novo.login, (novo.tipo == 1) ? "Admin" : "Comum");
    }
}

int adicionarUsuario(ListaUsuarios *lista, Usuario novo) {
    int novoTotal = lista->total + 1;

    Usuario *temp = realloc(lista->dados, novoTotal * sizeof(Usuario));
    if (temp == NULL) {
        printf("\nERRO: Falha ao alocar memoria para Usuario!\n");
        return 0;
    }

    lista->dados = temp;
    lista->dados[lista->total] = novo;
    lista->total = novoTotal;

    return 1;
}

static int contarAdministradores(const ListaUsuarios *lista) {
    int qtd = 0;
    for (int i = 0; i < lista->total; i++) {
        if (lista->dados[i].tipo == 1) {
            qtd++;
        }
    }
    return qtd;
}

void listarUsuarios(const ListaUsuarios *lista) {
    if (lista->total == 0) {
        printf("\nAVISO: Nao existem usuarios cadastrados!\n");
        return;
    }

    printf("\n============== LISTA DE USUARIOS =================\n");
    printf("%-4s | %-15s | %-25s\n", "NUM", "LOGIN", "PERFIL");
    printf("--------------------------------------------------\n");

    for (int i = 0; i < lista->total; i++) {
        printf("#%03d | %-15.15s | %-25s\n",
               i + 1,
               lista->dados[i].login,
               (lista->dados[i].tipo == 1) ? "Administrador" : "Usuario Comum");
    }
    printf("==================================================\n");
}

void alterarUsuario(ListaUsuarios *lista, const char *loginLogado) {
    if (lista->total == 0) {
        printf("\nAVISO: Nao existem usuarios cadastrados!\n");
        return;
    }

    listarUsuarios(lista);

    char loginBusca[50];
    lerTexto("Login do usuario a alterar (ou '0' p/ voltar): ", loginBusca, sizeof(loginBusca));
    if (strcmp(loginBusca, "0") == 0) {
        return;
    }

    int pos = buscarUsuarioPorLogin(lista, loginBusca);
    if (pos == -1) {
        printf("\nERRO: Usuario nao encontrado!\n");
        return;
    }

    Usuario *u = &lista->dados[pos];
    int opcao = 0;

    do {
        printf("\n=============== ALTERAR USUARIO ==================\n");
        printf("Usuario: %-41.41s\n", u->login);
        printf("--------------------------------------------------\n");
        printf("1. %47s\n", "Alterar Senha");
        printf("2. %47s\n", "Alterar Perfil (Admin / Comum)");
        printf("0. %47s\n", "Concluir Alteracoes");

        opcao = lerInteiroIntervalo("Escolha o campo para alterar: ", 0, 2);

        switch (opcao) {
            case 1: {
                char novaSenha[50];
                do {
                    lerTexto("Nova senha (6 a 8 caracteres): ", novaSenha, sizeof(novaSenha));
                } while (!validarCredenciais(u->login, novaSenha));

                strcpy(u->senha, novaSenha);
                printf("\nSenha atualizada com sucesso!\n");
                break;
            }
            case 2: {
                int novoTipo = lerInteiroIntervalo("Novo perfil (1 - Admin | 2 - Comum): ", 1, 2);
                if (u->tipo == 1 && novoTipo == 2 && contarAdministradores(lista) <= 1) {
                    printf("\nERRO: Nao e possivel rebaixar o unico Admin!\n");
                    break;
                }
                if (strcmp(u->login, loginLogado) == 0 && novoTipo == 2) {
                    printf("\nAVISO: Perfil atualizado no proximo login.\n");
                }
                u->tipo = novoTipo;
                printf("\nPerfil atualizado com sucesso!\n");
                break;
            }
            case 0:
                printf("\nAlteracoes concluidas!\n");
                break;
        }
    } while (opcao != 0);
}

void excluirUsuario(ListaUsuarios *lista, const char *loginLogado) {
    if (lista->total == 0) {
        printf("\nAVISO: Nao existem usuarios cadastrados!\n");
        return;
    }

    listarUsuarios(lista);

    char loginBusca[50];
    lerTexto("Login do usuario a excluir (ou '0' p/ voltar): ", loginBusca, sizeof(loginBusca));
    if (strcmp(loginBusca, "0") == 0) {
        return;
    }

    int pos = buscarUsuarioPorLogin(lista, loginBusca);
    if (pos == -1) {
        printf("\nERRO: Usuario nao encontrado!\n");
        return;
    }

    if (strcmp(lista->dados[pos].login, loginLogado) == 0) {
        printf("\nERRO: Voce nao pode excluir o usuario logado!\n");
        return;
    }

    if (lista->dados[pos].tipo == 1 && contarAdministradores(lista) <= 1) {
        printf("\nERRO: Impossivel excluir o unico Administrador!\n");
        return;
    }

    printf("\nConfirma excluir este usuario?\n");
    printf("Login: %-43.43s\n", lista->dados[pos].login);
    int confirma = lerInteiroIntervalo("(1 - Sim | 0 - Cancelar): ", 0, 1);
    if (confirma == 0) {
        printf("\nExclusao cancelada.\n");
        return;
    }

    // Desloca os registros seguintes para preencher a posicao removida
    for (int i = pos; i < lista->total - 1; i++) {
        lista->dados[i] = lista->dados[i + 1];
    }
    lista->total--;

    Usuario *temp = realloc(lista->dados, (size_t)lista->total * sizeof(Usuario));
    if (temp != NULL) {
        lista->dados = temp;
    }

    printf("\nUsuario excluido com sucesso!\n");
}

/* --------------     Funções de autenticação     -------------- */
// Busca um usuario pelo login e retorna seu indice no vetor (-1 se nao encontrar)
int buscarUsuarioPorLogin(const ListaUsuarios *lista, const char login[]) {
    for (int i = 0; i < lista->total; i++) {
        if (strcmp(login, lista->dados[i].login) == 0) {
            return i;
        }
    }
    return -1;
}

int validarCredenciais(const char login[], const char senha[]) {
    int tamanhoLogin = (int)strlen(login);
    int tamanhoSenha = (int)strlen(senha);

    if (strchr(login, ' ') != NULL || strchr(senha, ' ') != NULL) {
        printf("\nERRO: Login e senha nao podem conter espacos!\n\n");
        return 0;
    }
    if (tamanhoLogin < 8 || tamanhoLogin > 12) {
        printf("\nERRO: Login deve ter entre 8 e 12 caracteres!\n\n");
        return 0;
    }
    if (tamanhoSenha < 6 || tamanhoSenha > 8) {
        printf("\nERRO: Senha deve ter entre 6 e 8 caracteres!\n\n");
        return 0;
    }
    return 1;
}

int autenticarUsuario(const ListaUsuarios *lista, const char login[], const char senha[]) {
    for (int i = 0; i < lista->total; i++) {
        if (strcmp(login, lista->dados[i].login) == 0 && strcmp(senha, lista->dados[i].senha) == 0) {
            return i; // Retorna o indice do usuario autenticado
        }
    }
    return -1;
}

int autorizarAdmin(const ListaUsuarios *lista) {
    char login[50], senha[50];

    printf("\n========== AUTORIZACAO ADMINISTRATIVA ============\n");
    lerTexto("Login Admin: ", login, sizeof(login));
    lerTexto("Senha Admin: ", senha, sizeof(senha));

    int indice = autenticarUsuario(lista, login, senha);
    if (indice != -1 && lista->dados[indice].tipo == 1) {
        printf("\nOperacao autorizada pelo admin: %.12s.\n", lista->dados[indice].login);
        return 1;
    }

    printf("\nERRO: Autorizacao recusada! Credenciais invalidas\nou usuario sem permissoes administrativas.\n");
    return 0;
}

/* --------------     Funções de persistência de dados     -------------- */
int salvarUsuarios(const ListaUsuarios *lista) {
    // Grava primeiro em arquivo temporário (Gravação Atômica)
    FILE *arq = fopen("usuarios.tmp", "wb");
    if (arq == NULL) {
        printf("\nERRO: Falha ao criar arquivo temporario!\n");
        return 0;
    }

    // Grava a quantidade de usuários e verifica se deu certo
    if (fwrite(&lista->total, sizeof(int), 1, arq) != 1) {
        printf("\nERRO: Falha ao gravar a quantidade de usuarios!\n");
        fclose(arq);
        remove("usuarios.tmp");
        return 0;
    }

    // Se houver usuários cadastrados, grava o bloco inteiro
    if (lista->total > 0 && lista->dados != NULL) {
        if (fwrite(lista->dados, sizeof(Usuario), lista->total, arq) != (size_t)lista->total) {
            printf("\nERRO: Falha ao gravar os dados dos usuarios!\n");
            fclose(arq);
            remove("usuarios.tmp");
            return 0;
        }
    }

    fclose(arq);

    // Faz a troca atômica garantindo integridade do arquivo
    remove("usuarios.dat"); // Se o arquivo não existir na primeira vez, o retorno é ignorado
    if (rename("usuarios.tmp", "usuarios.dat") != 0) {
        printf("\nERRO: Falha ao realizar a troca dos arquivos!\n");
        return 0;
    }

    return 1;
}

int carregarUsuarios(ListaUsuarios *lista) {
    FILE *arq = fopen("usuarios.dat", "rb");

    // Caso o arquivo não exista (primeira execução), cria um admin padrão
    if (arq == NULL) {
        Usuario adminPadrao = {"admin123", "123456", 1};
        if (!adicionarUsuario(lista, adminPadrao)) {
            return 0;
        }
        return salvarUsuarios(lista);
    }

    int qtd = 0;
    if (fread(&qtd, sizeof(int), 1, arq) != 1) {
        fclose(arq);
        return 0;
    }

    if (qtd > 0) {
        Usuario *temp = (Usuario *) realloc(lista->dados, qtd * sizeof(Usuario));
        if (temp == NULL) {
            printf("\nERRO: Falha ao alocar memoria para usuarios!\n");
            fclose(arq);
            return 0;
        }

        if (fread(temp, sizeof(Usuario), qtd, arq) != (size_t)qtd) {
            printf("\nERRO: Falha na leitura do registro de usuarios!\n");
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
