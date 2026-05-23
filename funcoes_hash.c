#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define TABLE_SIZE 11
#define ARQUIVO_DADOS "usuarios.csv"

//struct de cada user pra inserir na lista do array
typedef struct No {
    char email[100];
    char senha_hash[30];

    struct No *prox;
} No;

void inicializar_tabela(No **tabela) {
    //inicializa NULL em todas as posiçoes
    for (int i = 0; i < TABLE_SIZE; i++) {
        tabela[i] = NULL;
    }
}

// Função pra transformar string em valor numérico
unsigned int valor_str(char *str) {
    unsigned int valor = 7;
    for (int i = 0; i < strlen(str); i++) {
        valor = 31 * valor + (int) str[i];
    }
    return valor;
}

// Função hash de indexação do nó (a partir do user (método da multiplicação))
int cria_hash(unsigned int valor_str) {
    const double A = 0.6180339887; //inverso do num. áureo
    double parte_fracionaria = fmod((valor_str * A), 1.0);
    double valor_novo = TABLE_SIZE * (parte_fracionaria);
    int indice = (int) valor_novo;
    return indice;
}

// Função hash de criptografia da senha
void hash_criptografia(const char *senha, char *senha_hash) {
    unsigned long hash = 5381; //número primo grande, que é ideal pra evitar colisões de senhas parecidas
    for (int i = 0; i < strlen(senha); i++) {
        // hash << 5 é o mesmo que multiplicar por 2^5 (32) e o + hash faz com que seja 33 (outro nº primo)
        // é mais eficiente multiplicar com o operador bitwise, pois não sabemos quantos carac. uma senha terá, e isso evita lentidão
        hash = ((hash << 5) + hash) + (int) senha[i]; //hash * 33 + carac atual
    }
    sprintf(senha_hash, "%lx", hash); //transforma o hash (que é um long) em hex, dentro de senha_hash
}

// vai retornar 0 se tiver problema, 1 se ok
int valida_senha(const char *senha) {
    int tamanho = strlen(senha);

    if (tamanho < 4 || tamanho > 6) return 0;
    for (int i = 0; i < tamanho; i++)
        if (!isdigit((unsigned char)senha[i])) return 0; //unsigned char lida com casos de carac com acento

    return 1; //caso passe em tudo
}

// params: tabela de nós (ptr de ptr), email e senha já hasheada
void insere_hash(No *tabela[], char *email, char *senha_hash){
    int indice = cria_hash(valor_str(email));

    No *novo = malloc(sizeof(No));

    strcpy(novo->email, email);
    strcpy(novo->senha_hash, senha_hash);

    novo->prox = tabela[indice];
    tabela[indice] = novo;
}

No* busca_hash(No *tabela[], char *email) {
    int indice = cria_hash(valor_str(email));
    No *atual = tabela[indice];

    while (atual != NULL) {
        if (strcmp(atual->email, email) == 0) {
            return atual;
        }
        atual = atual->prox;
    }
    return NULL;
}

int remove_hash(No *tabela[], char *email) {
    int indice = cria_hash(valor_str(email));
    No *atual = tabela[indice];
    No *anterior = NULL;

    while (atual != NULL) {
        if (strcmp(atual->email, email) == 0) {
            if (anterior == NULL) {
                tabela[indice] = atual->prox;
            } else {
                anterior->prox = atual->prox;
            }
            free(atual);
            return 1;
        }
        anterior = atual;
        atual = atual->prox;
    }
    return 0; //erro
}

void libera_hash(No *tabela[]){
    for (int i = 0; i < TABLE_SIZE; i++){
        No *atual = tabela[i];

        while (atual != NULL){
            No *temp = atual;

            atual = atual->prox;

            free(temp);
        }
        tabela[i] = NULL;
    }
}

void imprime_tabela(No *tabela[]) {

    printf("\n========== TABELA HASH ==========\n");

    for (int i = 0; i < TABLE_SIZE; i++) {

        printf("\nIndice [%d]\n", i);

        No *atual = tabela[i];

        // posição vazia
        if (atual == NULL) {
            printf("Vazio\n");
            continue;
        }

        int colisao = 0;

        while (atual != NULL) {

            printf("Usuario: %s\n", atual->email);
            printf("Hash da senha: %s\n", atual->senha_hash);

            if (atual->prox != NULL) {
                printf("COLISAO\n");
                colisao++;
            }

            atual = atual->prox;
        }

        if (colisao > 0) {
            printf("\nTotal de colisoes: %d\n", colisao);
        }
    }

    printf("\n================================\n");
}

void salvar_dados(No *tabela[], const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo para salvar dados!\n");
        return;
    }
    for (int i = 0; i < TABLE_SIZE; i++) {
        No *atual = tabela[i];
        while (atual != NULL) {
            fprintf(arquivo, "%s;%s\n", atual->email, atual->senha_hash);
            atual = atual->prox;
        }
    }
    fclose(arquivo);
    printf("Dados salvos com sucesso em '%s'\n", nome_arquivo);
}

void carregar_dados(No *tabela[], const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) { //se arquivo não existir
        printf("Arquivo de dados não encontrado. Iniciando tabela de usuários vazia.\n");
        return;
    }

    char email[100];
    char senha_hash[30];

    // na format string abaixo: ' %99[^;]' = 1ª var, ';' = consome do buffer sem atribuir, '%29s' = 2ª var
    while (fscanf(arquivo, " %99[^;];%29s", email, senha_hash) == 2) {
        insere_hash(tabela, email, senha_hash);
    }

    fclose(arquivo);
    printf("Dados lidos com sucesso de %s.\n", nome_arquivo);
}

void menu(No *tabela[]) {
    int opcao = -1;
    char email[100];
    char senha[50];
    char senha_hash[30];
    No *usuario_encontrado = NULL;

    do {
        printf("\n--- SISTEMA DE AUTENTICACAO ---\n");
        printf("1. Cadastrar Usuario\n");
        printf("2. Login\n");
        printf("3. Remover Usuario\n");
        printf("4. Imprimir Tabela\n");
        printf("0. Sair\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao) != 1) {
            printf("Opcao invalida!\n");
            while (getchar() != '\n');
            continue;
        }
        getchar();

        switch (opcao) {
            case 1:
                printf("\n--- CADASTRO ---\n");
                printf("Digite o username/email: ");
                fgets(email, sizeof(email), stdin);
                email[strcspn(email, "\n")] = '\0'; 

                if (busca_hash(tabela, email) != NULL) {
                    printf("Erro: Este usuario ja esta cadastrado!\n");
                    break;
                }

                printf("Digite a senha (4 a 6 digitos numericos): ");
                fgets(senha, sizeof(senha), stdin);
                senha[strcspn(senha, "\n")] = '\0';

                if (!valida_senha(senha)) {
                    printf("Erro: Senha invalida! Use de 4 a 6 digitos numericos.\n");
                } else {
                    hash_criptografia(senha, senha_hash);
                    insere_hash(tabela, email, senha_hash);
                    printf("Usuario cadastrado com sucesso!\n");
                }
                break;

            case 2:
                printf("\n--- LOGIN ---\n");
                printf("Digite o username/email: ");
                fgets(email, sizeof(email), stdin);
                email[strcspn(email, "\n")] = '\0';

                printf("Digite a senha: ");
                fgets(senha, sizeof(senha), stdin);
                senha[strcspn(senha, "\n")] = '\0';

                usuario_encontrado = busca_hash(tabela, email);

                if (usuario_encontrado != NULL) {
                    hash_criptografia(senha, senha_hash);
                    if (strcmp(usuario_encontrado->senha_hash, senha_hash) == 0) {
                        printf("Login efetuado com sucesso! Bem-vindo, %s.\n", usuario_encontrado->email);
                    } else {
                        printf("Erro: Senha incorreta!\n");
                    }
                } else {
                    printf("Erro: Usuario nao encontrado!\n");
                }
                break;

            case 3:
                printf("\n--- REMOVER USUARIO ---\n");
                printf("Digite o username/email que deseja remover: ");
                fgets(email, sizeof(email), stdin);
                email[strcspn(email, "\n")] = '\0';

                if (remove_hash(tabela, email)) {
                    printf("Usuario removido com sucesso!\n");
                } else {
                    printf("Erro: Usuario nao encontrado na tabela.\n");
                }
                break;

            case 4:
                imprime_tabela(tabela);
                break;

            case 0:
                printf("Saindo do sistema e liberando memoria...\n");
                salvar_dados(tabela, ARQUIVO_DADOS);
                libera_hash(tabela);
                printf("Programa encerrado.\n");
                break;

            default:
                printf("Opcao invalida! Tente novamente.\n");
        }
    } while (opcao != 0);
}

int main() {
    No *tabela[TABLE_SIZE];

    inicializar_tabela(tabela);// todos os índices com NULL

    carregar_dados(tabela, ARQUIVO_DADOS);

    menu(tabela);

    return 0;
}
