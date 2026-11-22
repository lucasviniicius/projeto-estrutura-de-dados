#include "func_hash.h"

int hash_divisao(long long chave, int tam){
    return chave % tam;
}

int hash_dobra(long long chave, int tam) {
    if (chave < 0) chave = -chave;

    long long soma = 0;
    long long peso = 1;

    while (chave > 0) {
        long long bloco = chave % 1000;
        soma = (soma + bloco * peso) % tam;
        peso = (peso * 1000) % tam;
        chave /= 1000;
    }

    return (int)soma;
}