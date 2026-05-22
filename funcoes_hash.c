#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define TABLE_SIZE 11

//struct de cada user pra inserir na lista do array
typedef struct No {
    char email[100];
    char senha_hash[100];

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
    printf("DEBUG (valor_str): %u\n", valor);
    return valor;
}

// Função hash de indexação do nó (a partir do user (método da multiplicação))
int cria_hash(unsigned int valor_str) {
    const double A = 0.6180339887; //inverso do num. áureo
    double parte_fracionaria = fmod((valor_str * A), 1.0);
    printf("DEBUG (parte frac): %f\n", parte_fracionaria);
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

int main() {
    No *tabela[TABLE_SIZE];
    inicializar_tabela(tabela);

    // DEBUG====================================================================
    char senha_hasheada[30];

    hash_criptografia("Minhasupersenha1", senha_hasheada);
    printf("senha hasheada: %s\n", senha_hasheada);
    int indice_hash = cria_hash(valor_str("guigaraposo2006@gmail.com"));
    printf("%d\n", indice_hash);
    insere_hash(tabela, "guigaraposo2006@gmail.com", senha_hasheada);

    printf("\n");

    hash_criptografia("Minhasupersenha2", senha_hasheada);
    printf("senha hasheada: %s\n", senha_hasheada);
    indice_hash = cria_hash(valor_str("guigaraposo2005@gmail.com"));
    printf("%d\n", indice_hash);
    insere_hash(tabela, "guigaraposo2005@gmail.com", senha_hasheada);

    printf("\n");

    hash_criptografia("Minhasupersenha3", senha_hasheada);
    printf("senha hasheada: %s\n", senha_hasheada);
    indice_hash = cria_hash(valor_str("guigaraposo2026@gmail.com"));
    printf("%d\n", indice_hash);
    insere_hash(tabela, "guigaraposo2026@gmail.com", senha_hasheada);

    printf("\n");

    hash_criptografia("Minhasupersenha4", senha_hasheada);
    printf("senha hasheada: %s\n", senha_hasheada);
    indice_hash = cria_hash(valor_str("guigaraposo2010@gmail.com"));
    printf("%d\n", indice_hash);
    insere_hash(tabela, "guigaraposo2010@gmail.com", senha_hasheada);

    printf("\n");
    //==========================================================================

    imprime_tabela(tabela);

    libera_hash(tabela);

    return 0;
}
