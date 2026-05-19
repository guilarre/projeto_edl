#include <stdio.h>
#include <string.h>
#include <math.h>

#define TABLE_SIZE 11

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

int main() {
    // char senha_hasheada[30];
    // hash_criptografia("Minhasupersenha1", senha_hasheada);
    // printf("%s\n", senha_hasheada);
    // hash_criptografia("Minhasupersenha2", senha_hasheada);
    // printf("%s\n", senha_hasheada);
    int indice_hash = cria_hash(valor_str("guigaraposo2006@gmail.com"));
    printf("%d\n", indice_hash);

    indice_hash = cria_hash(valor_str("guigaraposo2005@gmail.com"));
    printf("%d\n", indice_hash);

    return 0;
}
