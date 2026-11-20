#include "func_hash.h"

int hash_divisao(long chave, int tam){
    return chave % tam;
}

int hash_dobra (long chave, int tam){
    long valor = (chave < 0) ? -chave : chave;
    long soma = 0;
    const long base = 100;

    while (valor > 0) {
        soma += valor % base;
        valor /= base;
    }

    return (int)(soma % tam);
}