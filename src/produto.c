#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "produto.h"
#include "auxiliar.h"

/* --------------     Funções de cadastro     -------------- */
void cadastrarCategoria(ListaCategorias *lista) {
    Categoria nova;

    int maiorCodigo = 0;
    for (int i = 0; i < lista->total; i++) {
        if (lista->dados[i].codigo > maiorCodigo) {
            maiorCodigo = lista->dados[i].codigo;
        }
    }
    nova.codigo = maiorCodigo + 1;

    printf("\n============== CADASTRO DE CATEGORIA =============\n");

    lerTexto("Nome da Categoria: ", nova.nome, sizeof(nova.nome));

    if (adicionarCategoria(lista, nova)) {
        printf("\nCategoria cadastrada com sucesso!\n");
        printf("Categoria: %-27.27s | Cod: #%03d\n", nova.nome, nova.codigo);
    }
}

int adicionarCategoria(ListaCategorias *lista, Categoria novo) {
    int novoTotal = lista->total + 1;

    Categoria *temp = realloc(lista->dados, novoTotal * sizeof(Categoria));

    if (temp == NULL) {
        printf("\nERRO: Falha ao alocar memoria para Categoria!\n");
        return 0;
    }

    lista->dados = temp;
    lista->dados[lista->total] = novo;
    lista->total = novoTotal;

    return 1;
}

void alterarCategoria(ListaCategorias *lista, ListaProdutos *listaProdutos) {
    if (lista->total == 0) {
        printf("\nAVISO: Nao existem categorias cadastradas!\n");
        return;
    }

    exibirCatalogoCategorias(lista);

    int codigo = lerInteiroIntervalo("Codigo da categoria a alterar (0 = voltar): ", 0, 99999);
    if (codigo == 0) {
        return;
    }

    int pos = buscarCategoriaPorCodigo(lista, codigo);
    if (pos == -1) {
        printf("\nERRO: Categoria nao encontrada!\n");
        return;
    }

    lerTexto("Novo nome da categoria: ", lista->dados[pos].nome, sizeof(lista->dados[pos].nome));

    // Atualiza o nome da categoria nos produtos vinculados
    for (int i = 0; i < listaProdutos->total; i++) {
        if (listaProdutos->dados[i].codigoCategoria == codigo) {
            strcpy(listaProdutos->dados[i].nomeCategoria, lista->dados[pos].nome);
        }
    }

    printf("\nCategoria atualizada com sucesso!\n");
    printf("Categoria: %-27.27s | Cod: #%03d\n", lista->dados[pos].nome, lista->dados[pos].codigo);
}

// Gera o proximo codigo disponivel na faixa da categoria (ex.: categoria 1, código 1000+, categoria 4, código 4000+)
static int gerarCodigoProduto(const ListaProdutos *lista, int codigoCategoria) {
    int base = codigoCategoria * 1000;
    int teto = base + 999;
    int maior = base - 1;

    for (int i = 0; i < lista->total; i++) {
        int cod = lista->dados[i].codigo;
        if (cod >= base && cod <= teto && cod > maior) {
            maior = cod;
        }
    }

    return maior + 1;
}

void cadastrarProduto(ListaProdutos *lista, ListaCategorias *listaCategorias) {
    if (listaCategorias->total == 0) {
        printf("\nAVISO: Nenhuma categoria cadastrada no sistema!\n");
        int opcao = lerInteiroIntervalo("Cadastrar categoria agora? (1 - Sim | 0 - Nao): ", 0, 1);

        if (opcao == 0) {
            printf("\nOperacao cancelada!\n");
            return;
        }

        cadastrarCategoria(listaCategorias);
        if (listaCategorias->total == 0) {
            return;
        }
    }

    Produto novo;

    printf("\n=============== CADASTRO DE PRODUTO ==============\n");

    lerTexto("Descricao: ", novo.descricao, sizeof(novo.descricao));

    exibirCatalogoCategorias(listaCategorias);

    int codigoCategoria = 0;
    int posCategoria = -1;
    do {
        codigoCategoria = lerInteiroIntervalo("Informe o codigo da categoria: ", 1, 99999);
        posCategoria = buscarCategoriaPorCodigo(listaCategorias, codigoCategoria);
        if (posCategoria == -1) {
            printf("\nERRO: Categoria nao encontrada!\n");
        }
    } while (posCategoria == -1);

    novo.codigo = gerarCodigoProduto(lista, codigoCategoria);
    strcpy(novo.nomeCategoria, listaCategorias->dados[posCategoria].nome);
    novo.codigoCategoria = codigoCategoria;

    novo.precoCompra = lerFloatPositivo("Preco de compra: R$ ");
    novo.margemLucro = lerFloatPositivo("Margem de lucro (%): ");

    novo.precoVenda = novo.precoCompra * (1.0f + (novo.margemLucro / 100.0f));
    printf("%-35s R$ %11.2f\n", "Preco de venda calculado:", novo.precoVenda);

    novo.estoque = lerInteiroIntervalo("Estoque inicial: ", 0, 99999);
    novo.estoqueMinimo = lerInteiroIntervalo("Estoque minimo: ", 0, 99999);
    novo.ativo = 1;

    if (adicionarProduto(lista, novo)) {
        printf("\nProduto cadastrado com sucesso!\n");
        printf("Produto: %-28.28s | Cod: #%04d\n", novo.descricao, novo.codigo);
    }
}

int adicionarProduto(ListaProdutos *lista, Produto novo) {
    int novoTotal = lista->total + 1;

    Produto *temp = realloc(lista->dados, novoTotal * sizeof(Produto));

    if (temp == NULL) {
        printf("\nERRO: Falha ao alocar memoria para Produto!\n");
        return 0;
    }

    lista->dados = temp;
    lista->dados[lista->total] = novo;
    lista->total = novoTotal;

    return 1;
}

void alterarProduto(ListaProdutos *lista, ListaCategorias *listaCategorias) {
    if (lista->total == 0) {
        printf("\nAVISO: Nao existem produtos cadastrados!\n");
        return;
    }

    listarProdutos(lista);

    int codigo = lerInteiroIntervalo("Codigo do produto a alterar (0 = voltar): ", 0, 99999);
    if (codigo == 0) {
        return;
    }

    int pos = buscarProdutoPorCodigo(lista, codigo);
    if (pos == -1) {
        printf("\nERRO: Produto nao encontrado!\n");
        return;
    }

    Produto *p = &lista->dados[pos];
    int opcao = 0;

    do {
        printf("\n=============== ALTERAR PRODUTO ==================\n");
        printf("Produto: %-41.41s\n", p->descricao);
        printf("--------------------------------------------------\n");
        printf("1. %47s\n", "Alterar Descricao");
        printf("2. %47s\n", "Alterar Categoria");
        printf("3. %47s\n", "Alterar Preco de Compra");
        printf("4. %47s\n", "Alterar Margem de Lucro");
        printf("5. %47s\n", "Alterar Estoque Atual");
        printf("6. %47s\n", "Alterar Estoque Minimo");
        printf("0. %47s\n", "Concluir Alteracoes");

        opcao = lerInteiroIntervalo("Escolha o campo para alterar: ", 0, 6);

        switch (opcao) {
            case 1:
                lerTexto("Nova descricao: ", p->descricao, sizeof(p->descricao));
                printf("\nDescricao atualizada com sucesso!\n");
                break;

            case 2: {
                if (listaCategorias->total == 0) {
                    printf("\nAVISO: Nenhuma categoria disponivel.\n");
                    break;
                }
                exibirCatalogoCategorias(listaCategorias);
                int codCat = lerInteiroIntervalo("Novo codigo de categoria (0 = cancelar): ", 0, 99999);
                if (codCat == 0) break;
                int posCat = buscarCategoriaPorCodigo(listaCategorias, codCat);
                if (posCat == -1) {
                    printf("\nERRO: Categoria nao encontrada!\n");
                    break;
                }
                p->codigoCategoria = codCat;
                strcpy(p->nomeCategoria, listaCategorias->dados[posCat].nome);
                printf("\nCategoria atualizada com sucesso!\n");
                break;
            }

            case 3:
                p->precoCompra = lerFloatPositivo("Novo preco de compra: R$ ");
                p->precoVenda = p->precoCompra * (1.0f + (p->margemLucro / 100.0f));
                printf("%-35s R$ %11.2f\n", "Novo preco de venda:", p->precoVenda);
                break;

            case 4:
                p->margemLucro = lerFloatPositivo("Nova margem de lucro (%): ");
                p->precoVenda = p->precoCompra * (1.0f + (p->margemLucro / 100.0f));
                printf("%-35s R$ %11.2f\n", "Novo preco de venda:", p->precoVenda);
                break;

            case 5:
                p->estoque = lerInteiroIntervalo("Novo estoque atual: ", 0, 99999);
                printf("\nEstoque atualizado com sucesso!\n");
                break;

            case 6:
                p->estoqueMinimo = lerInteiroIntervalo("Novo estoque minimo: ", 0, 99999);
                printf("\nEstoque minimo atualizado com sucesso!\n");
                break;

            case 0:
                printf("\nAlteracoes concluidas!\n");
                break;
        }
    } while (opcao != 0);
}

void alternarStatusProduto(ListaProdutos *lista) {
    if (lista->total == 0) {
        printf("\nAVISO: Nao existem produtos cadastrados!\n");
        return;
    }

    listarProdutos(lista);

    int codigo = lerInteiroIntervalo("Codigo do produto (0 = voltar): ", 0, 99999);
    if (codigo == 0) {
        return;
    }

    int pos = buscarProdutoPorCodigo(lista, codigo);
    if (pos == -1) {
        printf("\nERRO: Produto nao encontrado!\n");
        return;
    }

    Produto *p = &lista->dados[pos];
    const char *acao = (p->ativo == 1) ? "DESCONTINUAR" : "REATIVAR";

    printf("\nProduto: %-41.41s\n", p->descricao);
    printf("Status atual: %-36s\n", (p->ativo == 1) ? "ATIVO" : "DESCONTINUADO");
    printf("Deseja %s este produto?\n", acao);
    int confirma = lerInteiroIntervalo("(1 - Sim | 0 - Cancelar): ", 0, 1);

    if (confirma == 1) {
        p->ativo = (p->ativo == 1) ? 0 : 1;
        printf("\nStatus do produto atualizado!\n");
        printf("Produto: %-25.25s | %-13s\n",
               p->descricao, (p->ativo == 1) ? "ATIVO" : "DESCONTINUADO");
    } else {
        printf("\nOperacao cancelada.\n");
    }
}

/* --------------     Funções de consulta, listagem e ordenação     -------------- */
// Exibe lista de categorias para selecao nos cadastros
void exibirCatalogoCategorias(const ListaCategorias *lista) {
    if (lista->total == 0) {
        printf("\nAVISO: Nao existem categorias cadastradas!\n");
        return;
    }

    printf("\n============= CATALOGO DE CATEGORIAS =============\n");
    printf("%-4s | %-43s\n", "COD", "NOME DA CATEGORIA");
    printf("--------------------------------------------------\n");

    for (int i = 0; i < lista->total; i++) {
        printf("%-4d | %-43.43s\n", lista->dados[i].codigo, lista->dados[i].nome);
    }
    printf("==================================================\n");
}

// Exibe tabela de produtos ativos para selecao durante a venda
void exibirCatalogoProdutos(const ListaProdutos *lista) {
    if (lista->total == 0) {
        printf("\nAVISO: Nao existem produtos cadastrados!\n");
        return;
    }

    printf("\n============== CATALOGO DE PRODUTOS ==============\n");
    printf("%-4s | %-9s | %-9s | %-9s | %-7s\n", "COD", "DESCRICAO", "CATEGORIA", "PRECO", "ESTOQUE");
    printf("--------------------------------------------------\n");

    int ativos = 0;
    for (int i = 0; i < lista->total; i++) {
        if (lista->dados[i].ativo == 0) {
            continue;
        }
        printf("%-4d | %-9.9s | %-9.9s | R$ %6.2f | %7d\n",
               lista->dados[i].codigo,
               lista->dados[i].descricao,
               lista->dados[i].nomeCategoria,
               lista->dados[i].precoVenda,
               lista->dados[i].estoque);
        ativos++;
    }

    if (ativos == 0) {
        printf("Nenhum produto ativo disponivel para venda!\n");
    }
    printf("==================================================\n");
}

void listarCategorias(const ListaCategorias *lista) {
    if (lista->total == 0) {
        printf("\nAVISO: Nao existem categorias cadastradas!\n");
        return;
    }

    printf("\n============ RELATORIO DE CATEGORIAS =============\n");

    for (int i = 0; i < lista->total; i++) {
        printf("%-46s#%03d\n", "Categoria:", i + 1);
        printf("%-35s%15d\n", "Codigo:", lista->dados[i].codigo);
        printf("%-15s %34.34s\n", "Nome:", lista->dados[i].nome);
        printf("--------------------------------------------------\n");
    }
}

void listarProdutos(const ListaProdutos *lista) {
    if (lista->total == 0) {
        printf("\nAVISO: Nao existem produtos cadastrados!\n");
        return;
    }

    printf("\n============= RELATORIO DE PRODUTOS ==============\n");

    for (int i = 0; i < lista->total; i++) {
        printf("%-46s#%03d\n", "Produto:", i + 1);
        printf("%-35s%15d\n", "Codigo:", lista->dados[i].codigo);
        printf("%-15s %34.34s\n", "Descricao:", lista->dados[i].descricao);
        printf("%-15s %34.34s\n", "Categoria:", lista->dados[i].nomeCategoria);
        printf("%-35s R$ %11.2f\n", "Preco de compra:", lista->dados[i].precoCompra);
        printf("%-35s%14.2f%%\n", "Margem de lucro:", lista->dados[i].margemLucro);
        printf("%-35s R$ %11.2f\n", "Preco de venda:", lista->dados[i].precoVenda);
        printf("%-35s%15d\n", "Estoque atual:", lista->dados[i].estoque);
        printf("%-35s%15d\n", "Estoque minimo:", lista->dados[i].estoqueMinimo);
        printf("%-15s %34s\n", "Status:", (lista->dados[i].ativo == 1) ? "Ativo" : "Descontinuado");
        printf("--------------------------------------------------\n");
    }
}

// Busca uma categoria pelo codigo e retorna seu indice no vetor (-1 se nao encontrar)
int buscarCategoriaPorCodigo(const ListaCategorias *lista, int codigo) {
    for (int i = 0; i < lista->total; i++) {
        if (lista->dados[i].codigo == codigo) {
            return i;
        }
    }
    return -1;
}

// Busca um produto pelo codigo e retorna seu indice no vetor (-1 se nao encontrar)
int buscarProdutoPorCodigo(const ListaProdutos *lista, int codigo) {
    for (int i = 0; i < lista->total; i++) {
        if (lista->dados[i].codigo == codigo) {
            return i;
        }
    }
    return -1;
}

/* --------------     Funções de persistência de dados     -------------- */
int salvarCategorias(const ListaCategorias *lista) {
    // Grava primeiro em arquivo temporário (Gravação Atômica)
    FILE *arq = fopen("categorias.tmp", "wb");
    if (arq == NULL) {
        printf("\nERRO: Falha ao criar arquivo temporario!\n");
        return 0;
    }

    if (fwrite(&lista->total, sizeof(int), 1, arq) != 1) {
        printf("\nERRO: Falha ao gravar a quantidade de categorias!\n");
        fclose(arq);
        remove("categorias.tmp");
        return 0;
    }

    if (lista->total > 0 && lista->dados != NULL) {
        if (fwrite(lista->dados, sizeof(Categoria), lista->total, arq) != (size_t)lista->total) {
            printf("\nERRO: Falha ao gravar os dados das categorias!\n");
            fclose(arq);
            remove("categorias.tmp");
            return 0;
        }
    }

    fclose(arq);

    remove("categorias.dat"); // Se o arquivo não existir na primeira vez, o retorno é ignorado
    if (rename("categorias.tmp", "categorias.dat") != 0) {
        printf("\nERRO: Falha ao realizar a troca dos arquivos!\n");
        return 0;
    }

    return 1;
}

int carregarCategorias(ListaCategorias *lista) {
    FILE *arq = fopen("categorias.dat", "rb");

    if (arq == NULL) {
        lista->dados = NULL;
        lista->total = 0;
        return 1;
    }

    int qtd = 0;
    if (fread(&qtd, sizeof(int), 1, arq) != 1) {
        fclose(arq);
        return 0;
    }

    if (qtd > 0) {
        Categoria *temp = (Categoria *) realloc(lista->dados, qtd * sizeof(Categoria));
        if (temp == NULL) {
            printf("\nERRO: Falha ao alocar memoria para categorias!\n");
            fclose(arq);
            return 0;
        }

        // Lê as categorias e carrega do arquivo
        if (fread(temp, sizeof(Categoria), qtd, arq) != (size_t)qtd) {
            printf("\nERRO: Falha na leitura do registro de categorias!\n");
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

int salvarProdutos(const ListaProdutos *lista) {
    // Grava primeiro em arquivo temporário (Gravação Atômica)
    FILE *arq = fopen("produtos.tmp", "wb");
    if (arq == NULL) {
        printf("\nERRO: Falha ao criar arquivo temporario!\n");
        return 0;
    }

    if (fwrite(&lista->total, sizeof(int), 1, arq) != 1) {
        printf("\nERRO: Falha ao gravar a quantidade de produtos!\n");
        fclose(arq);
        remove("produtos.tmp");
        return 0;
    }

    if (lista->total > 0 && lista->dados != NULL) {
        if (fwrite(lista->dados, sizeof(Produto), lista->total, arq) != (size_t)lista->total) {
            printf("\nERRO: Falha ao gravar os dados dos produtos!\n");
            fclose(arq);
            remove("produtos.tmp");
            return 0;
        }
    }

    fclose(arq);

    remove("produtos.dat"); // Se o arquivo não existir na primeira vez, o retorno é ignorado
    if (rename("produtos.tmp", "produtos.dat") != 0) {
        printf("\nERRO: Falha ao realizar a troca dos arquivos!\n");
        return 0;
    }

    return 1;
}

int carregarProdutos(ListaProdutos *lista) {
    FILE *arq = fopen("produtos.dat", "rb");

    if (arq == NULL) {
        lista->dados = NULL;
        lista->total = 0;
        return 1;
    }

    int qtd = 0;
    if (fread(&qtd, sizeof(int), 1, arq) != 1) {
        fclose(arq);
        return 0;
    }

    if (qtd > 0) {
        Produto *temp = (Produto *) realloc(lista->dados, qtd * sizeof(Produto));
        if (temp == NULL) {
            printf("\nERRO: Falha ao alocar memoria para produtos!\n");
            fclose(arq);
            return 0;
        }

        // Lê os produtos e carrega do arquivo
        if (fread(temp, sizeof(Produto), qtd, arq) != (size_t)qtd) {
            printf("\nERRO: Falha na leitura do registro de produtos!\n");
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

