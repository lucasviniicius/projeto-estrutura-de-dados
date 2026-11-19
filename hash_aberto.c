#include <stdlib.h>
#include "hash_aberto.h"
#include "func_hash.h"

struct hash_aberta {
    int qtd;
    int capacidade;
    long colisoes;
    Aluno** itens;
};

int sondagem_linear(int pos, int i, int capacidade){
    return (pos + i) % capacidade;
}

HashAberta* criar_tabela_aberta(int capacidade){
    HashAberta* hash = malloc(sizeof(HashAberta));

    hash->capacidade = capacidade;
    hash->qtd = 0;
    hash->colisoes = 0;

    hash->itens = calloc((size_t)capacidade, sizeof(Aluno*));
    if(!hash->itens){
        free(hash);
        return NULL;
    }

    return hash;
}

int inserir_aberta(HashAberta* hash, Aluno al){
    if(hash == NULL || hash->qtd == hash->capacidade){
        return 0;
    }

    int pos = hash_divisao(al.matricula, hash->capacidade);
    for(int i = 0; i < hash->capacidade; i++){
        int newPos = sondagem_linear(pos, i, hash->capacidade);
        if(hash->itens[newPos] == NULL){
            Aluno* no = malloc(sizeof(Aluno));
            *no = al;
            hash->itens[newPos] = no;
            hash->qtd++;
            return 1;
        }

        hash->colisoes++;
    }

    return 0;
}

Aluno* busca_aberta(HashAberta* hash, long matricula){
    if(hash == NULL || hash->qtd == 0){
        return 0;
    }

    int pos = hash_divisao(matricula, hash->capacidade);

    for(int i = 0; i < hash->capacidade; i++){
        int newPos = sondagem_linear(pos, i, hash->capacidade);

        if(hash->itens[newPos] == NULL){
            return NULL;
        }

        if(hash->itens[newPos]->matricula == matricula){
            return hash->itens[newPos];
        }
    }

    return NULL;
}

void liberar_aberta(HashAberta* hash){
    if(hash == NULL){
        return;
    }

    if(hash->itens != NULL){
        for(int i = 0; i < hash->capacidade; i++){
            free(hash->itens[i]);
        }
        free(hash->itens);
    }

    free(hash);
}