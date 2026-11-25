#ifndef CLIENTE_H
#define CLIENTE_H

typedef struct {
    int codigo;
    char nome[60];
    char nomeSocial[60];
    char cpf[15];
    char endereco[80]; // Rua e Número
    char bairro[40];
    char telefone[20];
} Cliente;

typedef struct {
    Cliente *dados;
    int total;
} ListaClientes;

/* --------------     Protótipos de cadastro e manutenção     -------------- */
void cadastrarCliente(ListaClientes *lista);
int adicionarCliente(ListaClientes *lista, Cliente novo);
void alterarCliente(ListaClientes *lista);
void excluirCliente(ListaClientes *lista);

/* --------------     Protótipos de consulta, listagem e ordenação     -------------- */
void exibirCatalogoClientes(const ListaClientes *lista);
void listarClientes(const ListaClientes *lista);
void ordenarClientesPorNome(ListaClientes *lista);
int buscarClientePorCodigo(const ListaClientes *lista, int codigo);

/* --------------     Protótipos de persistência de dados     -------------- */
int salvarClientes(const ListaClientes *lista);
int carregarClientes(ListaClientes *lista);

#endif