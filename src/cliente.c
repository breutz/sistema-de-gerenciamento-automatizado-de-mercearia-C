#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cliente.h"
#include "auxiliar.h"

/* --------------     Funções de cadastro     -------------- */
void cadastrarCliente(ListaClientes *lista) {
    Cliente novo;

    // Garante código único mesmo se a lista estiver fora de ordem
    int maiorCodigo = 0;
    for (int i = 0; i < lista->total; i++) {
        if (lista->dados[i].codigo > maiorCodigo) {
            maiorCodigo = lista->dados[i].codigo;
        }
    }
    novo.codigo = maiorCodigo + 1;

    printf("\n=============== CADASTRO DE CLIENTE ==============\n");

    lerTexto("Nome completo: ", novo.nome, sizeof(novo.nome));
    lerTexto("Nome social (ou '-' se nao tiver): ", novo.nomeSocial, sizeof(novo.nomeSocial));
    lerTexto("CPF: ", novo.cpf, sizeof(novo.cpf));
    lerTexto("Endereco (Rua e numero): ", novo.endereco, sizeof(novo.endereco));
    lerTexto("Bairro: ", novo.bairro, sizeof(novo.bairro));
    lerTexto("Telefone/WhatsApp: ", novo.telefone, sizeof(novo.telefone));

    if (adicionarCliente(lista, novo)) {
        printf("\nCliente cadastrado com sucesso!\n");
        printf("Cliente: %-29.29s | Cod: #%03d\n", novo.nome, novo.codigo);
    }
}

int adicionarCliente(ListaClientes *lista, Cliente novo) {
    int novoTotal = lista->total + 1;

    Cliente *temp = realloc(lista->dados, novoTotal * sizeof(Cliente));

    if (temp == NULL) {
        printf("\nERRO: Falha ao alocar memoria para Cliente!\n");
        return 0;
    }

    lista->dados = temp;
    lista->dados[lista->total] = novo;
    lista->total = novoTotal;

    return 1;
}

void alterarCliente(ListaClientes *lista) {
    if (lista->total == 0) {
        printf("\nAVISO: Nao existem clientes cadastrados!\n");
        return;
    }

    exibirCatalogoClientes(lista);

    int codigo = lerInteiroIntervalo("Codigo do cliente a alterar (0 = voltar): ", 0, 99999);
    if (codigo == 0) {
        return;
    }

    int pos = buscarClientePorCodigo(lista, codigo);
    if (pos == -1) {
        printf("\nERRO: Cliente nao encontrado!\n");
        return;
    }

    Cliente *c = &lista->dados[pos];
    int opcao = 0;

    do {
        printf("\n=============== ALTERAR CLIENTE ==================\n");
        printf("Cliente: %-41.41s\n", c->nome);
        printf("--------------------------------------------------\n");
        printf("1. %47s\n", "Alterar Nome Completo");
        printf("2. %47s\n", "Alterar Nome Social");
        printf("3. %47s\n", "Alterar CPF");
        printf("4. %47s\n", "Alterar Endereco (Rua e Numero)");
        printf("5. %47s\n", "Alterar Bairro");
        printf("6. %47s\n", "Alterar Telefone/WhatsApp");
        printf("0. %47s\n", "Concluir Alteracoes");

        opcao = lerInteiroIntervalo("Escolha o campo para alterar: ", 0, 6);

        switch (opcao) {
            case 1:
                lerTexto("Novo nome completo: ", c->nome, sizeof(c->nome));
                printf("\nNome atualizado com sucesso!\n");
                break;
            case 2:
                lerTexto("Novo nome social (ou '-'): ", c->nomeSocial, sizeof(c->nomeSocial));
                printf("\nNome social atualizado com sucesso!\n");
                break;
            case 3:
                lerTexto("Novo CPF: ", c->cpf, sizeof(c->cpf));
                printf("\nCPF atualizado com sucesso!\n");
                break;
            case 4:
                lerTexto("Novo endereco (Rua e numero): ", c->endereco, sizeof(c->endereco));
                printf("\nEndereco atualizado com sucesso!\n");
                break;
            case 5:
                lerTexto("Novo bairro: ", c->bairro, sizeof(c->bairro));
                printf("\nBairro atualizado com sucesso!\n");
                break;
            case 6:
                lerTexto("Novo telefone/WhatsApp: ", c->telefone, sizeof(c->telefone));
                printf("\nTelefone atualizado com sucesso!\n");
                break;
            case 0:
                printf("\nAlteracoes concluidas!\n");
                break;
        }
    } while (opcao != 0);
}

void excluirCliente(ListaClientes *lista) {
    if (lista->total == 0) {
        printf("\nAVISO: Nao existem clientes cadastrados!\n");
        return;
    }

    exibirCatalogoClientes(lista);

    int codigo = lerInteiroIntervalo("Codigo do cliente a excluir (0 = voltar): ", 0, 99999);
    if (codigo == 0) {
        return;
    }

    int pos = buscarClientePorCodigo(lista, codigo);
    if (pos == -1) {
        printf("\nERRO: Cliente nao encontrado!\n");
        return;
    }

    printf("\nConfirma excluir este cliente?\n");
    printf("Cliente: %-29.29s | Cod: #%03d\n", lista->dados[pos].nome, lista->dados[pos].codigo);
    int confirma = lerInteiroIntervalo("(1 - Sim | 0 - Cancelar): ", 0, 1);
    if (confirma == 0) {
        printf("\nExclusao cancelada.\n");
        return;
    }

    // Desloca os elementos a direita para preencher a posicao removida
    for (int i = pos; i < lista->total - 1; i++) {
        lista->dados[i] = lista->dados[i + 1];
    }
    lista->total--;

    if (lista->total == 0) {
        free(lista->dados);
        lista->dados = NULL;
    } else {
        Cliente *temp = realloc(lista->dados, (size_t)lista->total * sizeof(Cliente));
        if (temp != NULL) {
            lista->dados = temp;
        }
    }

    printf("\nCliente excluido com sucesso!\n");
}

/* --------------     Funções de consulta, listagem e ordenação     -------------- */
// Exibe lista compacta (50 colunas) para seleção rápida durante a venda
void exibirCatalogoClientes(const ListaClientes *lista) {
    if (lista->total == 0) {
        printf("\nAVISO: Nao existem clientes cadastrados!\n");
        return;
    }

    printf("\n============== CATALOGO DE CLIENTES ==============\n");
    printf("%-4s | %-43s\n", "COD", "NOME DO CLIENTE");
    printf("--------------------------------------------------\n");

    for (int i = 0; i < lista->total; i++) {
        printf("%-4d | %-43.43s\n", lista->dados[i].codigo, lista->dados[i].nome);
    }
    printf("==================================================\n");
}

void listarClientes(const ListaClientes *lista) {
    if (lista->total == 0) {
        printf("\nAVISO: Nao existem clientes cadastrados!\n");
        return;
    }

    // Cria copia temporaria para ordenar sem alterar a lista original
    ListaClientes copia = {0};
    copia.dados = (Cliente *) malloc(lista->total * sizeof(Cliente));
    if (copia.dados == NULL) {
        printf("\nERRO: Falha ao alocar memoria para o relatorio!\n");
        return;
    }
    memcpy(copia.dados, lista->dados, lista->total * sizeof(Cliente));
    copia.total = lista->total;

    ordenarClientesPorNome(&copia);

    printf("\n============= RELATORIO DE CLIENTES ==============\n");

    for (int i = 0; i < copia.total; i++) {
        printf("%-46s#%03d\n", "Cliente:", i + 1);
        printf("%-35s%15d\n", "Codigo:", copia.dados[i].codigo);
        printf("%-15s %34.34s\n", "Nome:", copia.dados[i].nome);
        printf("%-15s %34.34s\n", "Nome Social:", copia.dados[i].nomeSocial);
        printf("%-15s %34.34s\n", "CPF:", copia.dados[i].cpf);
        printf("%-15s %34.34s\n", "Endereco:", copia.dados[i].endereco);
        printf("%-15s %34.34s\n", "Bairro:", copia.dados[i].bairro);
        printf("%-15s %34.34s\n", "Telefone:", copia.dados[i].telefone);
        printf("--------------------------------------------------\n");
    }

    free(copia.dados);
}

void ordenarClientesPorNome(ListaClientes *lista) {
    if (lista->total > 1) {
        for (int i = 0; i < lista->total - 1; i++) {
            for (int j = 0; j < lista->total - 1 - i; j++) {
                if (strcmp(lista->dados[j].nome, lista->dados[j + 1].nome) > 0) {
                    Cliente temp = lista->dados[j];
                    lista->dados[j] = lista->dados[j + 1];
                    lista->dados[j + 1] = temp;
                }
            }
        }
    }
}

// Busca um cliente pelo codigo e retorna seu indice no vetor (-1 se nao encontrar)
int buscarClientePorCodigo(const ListaClientes *lista, int codigo) {
    for (int i = 0; i < lista->total; i++) {
        if (lista->dados[i].codigo == codigo) {
            return i;
        }
    }
    return -1;
}

/* --------------     Funções de persistência de dados     -------------- */
int salvarClientes(const ListaClientes *lista) {
    // Grava primeiro em arquivo temporário (Gravação Atômica)
    FILE *arq = fopen("clientes.tmp", "wb");
    if (arq == NULL) {
        printf("\nERRO: Falha ao criar arquivo temporario!\n");
        return 0;
    }

    // Grava a quantidade de clientes e verifica se deu certo
    if (fwrite(&lista->total, sizeof(int), 1, arq) != 1) {
        printf("\nERRO: Falha ao gravar a quantidade de clientes!\n");
        fclose(arq);
        remove("clientes.tmp");
        return 0;
    }

    // Se houver clientes cadastrados, grava o bloco inteiro
    if (lista->total > 0 && lista->dados != NULL) {
        if (fwrite(lista->dados, sizeof(Cliente), lista->total, arq) != (size_t)lista->total) {
            printf("\nERRO: Falha ao gravar os dados dos clientes!\n");
            fclose(arq);
            remove("clientes.tmp");
            return 0;
        }
    }

    fclose(arq);

    // Faz a troca atômica garantindo integridade do arquivo
    remove("clientes.dat"); // Se o arquivo não existir na primeira vez, o retorno é ignorado
    if (rename("clientes.tmp", "clientes.dat") != 0) {
        printf("\nERRO: Falha ao realizar a troca dos arquivos!\n");
        return 0;
    }

    return 1;
}

int carregarClientes(ListaClientes *lista) {
    FILE *arq = fopen("clientes.dat", "rb");

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
        Cliente *temp = (Cliente *) realloc(lista->dados, qtd * sizeof(Cliente));
        if (temp == NULL) {
            printf("\nERRO: Falha ao alocar memoria para clientes!\n");
            fclose(arq);
            return 0;
        }

        if (fread(temp, sizeof(Cliente), qtd, arq) != (size_t)qtd) {
            printf("\nERRO: Falha na leitura do registro de clientes!\n");
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
