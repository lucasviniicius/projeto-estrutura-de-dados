#include <stdlib.h>
#include "hash_aberto.h"

struct hash_aberta {
    int qtd;
    int capacidade;
    long long colisoes;
    Aluno** itens;
    unsigned char* indice_usado;
};

static int sondagem_linear(int pos, int i, int capacidade) {
    return (pos + i) % capacidade;
}

HashAberta* criar_tabela_aberta(int capacidade) {
    if (capacidade <= 0) {
        return NULL;
    }

    HashAberta* hash = malloc(sizeof(HashAberta));
    if (hash == NULL) {
        return NULL;
    }

    hash->capacidade = capacidade;
    hash->qtd = 0;
    hash->colisoes = 0;

    hash->itens = calloc((size_t)capacidade, sizeof(Aluno*));
    if (hash->itens == NULL) {
        free(hash);
        return NULL;
    }

    hash->indice_usado = calloc((size_t)capacidade, sizeof(unsigned char));
    if (hash->indice_usado == NULL) {
        free(hash->itens);
        free(hash);
        return NULL;
    }

    return hash;
}

int inserir_aberta(HashAberta* hash, Aluno al, FuncHash func_hash) {
    if (hash == NULL || func_hash == NULL || hash->qtd >= hash->capacidade) {
        return 0;
    }

    int pos = func_hash(al.matricula, hash->capacidade);
    if (pos < 0 || pos >= hash->capacidade) {
        return 0;
    }

    for (int i = 0; i < hash->capacidade; i++) {
        int newPos = sondagem_linear(pos, i, hash->capacidade);
        if (hash->itens[newPos] == NULL) {
            Aluno* no = malloc(sizeof(Aluno));
            if (no == NULL) {
                return 0;
            }
            *no = al;
            hash->itens[newPos] = no;
            hash->qtd++;

            if (hash->indice_usado[pos]) {
                hash->colisoes++;
            } else {
                hash->indice_usado[pos] = 1;
            }

            return 1;
        }
    }

    return 0;
}

Aluno* busca_aberta(HashAberta* hash, long long matricula, FuncHash func_hash) {
    if (hash == NULL || func_hash == NULL || hash->qtd == 0) {
        return NULL;
    }

    int pos = func_hash(matricula, hash->capacidade);
    if (pos < 0 || pos >= hash->capacidade) {
        return NULL;
    }

    for (int i = 0; i < hash->capacidade; i++) {
        int newPos = sondagem_linear(pos, i, hash->capacidade);

        if (hash->itens[newPos] == NULL) {
            return NULL;
        }

        if (hash->itens[newPos]->matricula == matricula) {
            return hash->itens[newPos];
        }
    }

    return NULL;
}

void liberar_aberta(HashAberta* hash) {
    if (hash == NULL) {
        return;
    }

    if (hash->itens != NULL) {
        for (int i = 0; i < hash->capacidade; i++) {
            free(hash->itens[i]);
        }
        free(hash->itens);
    }

    free(hash->indice_usado);
    free(hash);
}

int obter_qtd_aberta(HashAberta* hash) {
    return (hash != NULL) ? hash->qtd : 0;
}

long long obter_colisoes_aberta(HashAberta* hash) {
    return (hash != NULL) ? hash->colisoes : 0;
}

void zerar_colisoes_aberta(HashAberta* hash) {
    if (hash != NULL) {
        hash->colisoes = 0;
    }
}
