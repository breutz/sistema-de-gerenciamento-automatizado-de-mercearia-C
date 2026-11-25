#ifndef PRODUTO_H
#define PRODUTO_H

typedef struct {
    int codigo;
    char nome[30];
} Categoria;

typedef struct {
    Categoria *dados;
    int total;
} ListaCategorias;

typedef struct {
    int codigo;
    char descricao[50];
    int codigoCategoria;
    char nomeCategoria[30];
    float precoCompra;
    float margemLucro;
    float precoVenda;
    int estoque;
    int estoqueMinimo;
    int ativo; // 1 = Ativo | 0 = Descontinuado
} Produto;

typedef struct {
    Produto *dados;
    int total;
} ListaProdutos;

/* --------------     Protótipos de cadastro e manutenção     -------------- */
void cadastrarCategoria(ListaCategorias *lista);
int adicionarCategoria(ListaCategorias *lista, Categoria novo);
void alterarCategoria(ListaCategorias *lista, ListaProdutos *listaProdutos);
void cadastrarProduto(ListaProdutos *lista, ListaCategorias *listaCategorias);
int adicionarProduto(ListaProdutos *lista, Produto novo);
void alterarProduto(ListaProdutos *lista, ListaCategorias *listaCategorias);
void alternarStatusProduto(ListaProdutos *lista);

/* --------------     Protótipos de consulta, listagem e ordenação     -------------- */
void exibirCatalogoCategorias(const ListaCategorias *lista);
void exibirCatalogoProdutos(const ListaProdutos *lista);
void listarCategorias(const ListaCategorias *lista);
void listarProdutos(const ListaProdutos *lista);
int buscarCategoriaPorCodigo(const ListaCategorias *lista, int codigo);
int buscarProdutoPorCodigo(const ListaProdutos *lista, int codigo);

/* --------------     Protótipos de persistência de dados     -------------- */
int salvarCategorias(const ListaCategorias *lista);
int carregarCategorias(ListaCategorias *lista);
int salvarProdutos(const ListaProdutos *lista);
int carregarProdutos(ListaProdutos *lista);

#endif