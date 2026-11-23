#include <stdint.h>
#include "func_hash.h"

int hash_divisao(long long chave, int tam) {
    if (tam <= 0) {
        return -1;
    }

    long long r = chave % tam;
    if (r < 0) {
        r += tam;
    }

    return (int)r;
}

/*
 * Funcao hash FNV-1a (Fowler-Noll-Vo, 64 bits).
 * Aplica XOR byte a byte seguido de multiplicacao pelo primo FNV de 64 bits,
 * proporcionando boa dispersao para chaves inteiras de forma independente
 * do metodo da divisao.
 */
int hash_fnv1a(long long chave, int tam) {
    if (tam <= 0) {
        return -1;
    }

    uint64_t x = (uint64_t)chave;
    uint64_t hash = 14695981039346656037ULL;
    const uint64_t primo = 1099511628211ULL;

    for (int i = 0; i < 8; i++) {
        hash ^= (x & 0xFFULL);
        hash *= primo;
        x >>= 8;
    }

    return (int)(hash % (uint64_t)tam);
}
