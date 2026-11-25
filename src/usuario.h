#ifndef USUARIO_H
#define USUARIO_H

typedef struct {
    char login[13]; // até 12 caracteres + o '\0'
    char senha[9];  // até 8 caracteres + o '\0'
    int tipo;       // 1 = ADM, 2 = Usuario comum
} Usuario;

typedef struct {
    Usuario *dados;
    int total;
} ListaUsuarios;

/* --------------     Protótipos de cadastro e manutenção     -------------- */
void cadastrarUsuario(ListaUsuarios *lista);
int adicionarUsuario(ListaUsuarios *lista, Usuario novo);
void listarUsuarios(const ListaUsuarios *lista);
void alterarUsuario(ListaUsuarios *lista, const char *loginLogado);
void excluirUsuario(ListaUsuarios *lista, const char *loginLogado);

/* --------------     Protótipos de autenticação     -------------- */
int buscarUsuarioPorLogin(const ListaUsuarios *lista, const char login[]);
int validarCredenciais(const char login[], const char senha[]);
int autenticarUsuario(const ListaUsuarios *lista, const char login[], const char senha[]);
int autorizarAdmin(const ListaUsuarios *lista);

/* --------------     Protótipos de persistência de dados     -------------- */
int salvarUsuarios(const ListaUsuarios *lista);
int carregarUsuarios(ListaUsuarios *lista);

#endif