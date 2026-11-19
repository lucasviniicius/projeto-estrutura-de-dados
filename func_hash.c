#include "func_hash.h"

int hash_divisao(long chave, int tam){
    return chave % tam;
}

int hash_multiplicacao (long chave, int tam){
    double A = 0.7834;
    double val = chave * A;
    val = val - (int) val;
    return (int) (tam * val);
}