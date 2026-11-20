#include <stdlib.h>
#include "hash_separado.h"

struct no_hash{
    Aluno aluno;
    NoHash* prox;
};

struct hash_separado{
    int qtd;
    int capacidade;
    long colisoes;
    NoHash** vetor;
};

HashSeparado* criar_tabela_separado(int capacidade){
    HashSeparado* hash = malloc(sizeof(HashSeparado));
    if(hash == NULL){
        return NULL;
    }

    hash->capacidade = capacidade;
    hash->qtd = 0;
    hash->colisoes = 0;

    hash->vetor = calloc((size_t)capacidade, sizeof(NoHash*));
    if (hash->vetor == NULL) {
        free(hash);
        return NULL;
    }

    return hash;
}

int inserir_separado(HashSeparado* hash, Aluno al, FuncHash func_hash){
    if (hash == NULL || func_hash == NULL || hash->capacidade <= 0) {
        return 0;
    }

    int pos = func_hash(al.matricula, hash->capacidade);
    if (pos < 0 || pos >= hash->capacidade) {
        return 0;
    }

    if(hash->vetor[pos] != NULL){
        hash->colisoes++;
    }

    NoHash* no = malloc(sizeof(NoHash));
    if(no == NULL){
        return 0;
    }

    no->aluno = al;
    no->prox = hash->vetor[pos];
    hash->vetor[pos] = no;
    hash->qtd++;

    return 1;
}

Aluno* buscar_separado(HashSeparado* hash, long matricula, FuncHash func_hash){
    if (hash == NULL || func_hash == NULL || hash->qtd == 0 || hash->capacidade <= 0) {
        return NULL;
    }

    int pos = func_hash(matricula, hash->capacidade);
    if (pos < 0 || pos >= hash->capacidade) {
        return NULL;
    }

    NoHash* atual = hash->vetor[pos];

    while(atual != NULL){
        if(atual->aluno.matricula == matricula){
            return &(atual->aluno);
        }
        atual = atual->prox;
    }

    return NULL;
}

void liberar_separado(HashSeparado* hash){
    if(hash == NULL){
        return;
    }

    for(int i = 0; i < hash->capacidade; i++){
        NoHash* atual = hash->vetor[i];
        while(atual != NULL){
            NoHash* prox = atual->prox;
            free(atual);
            atual = prox;
        }
    }
    free(hash->vetor);
    free(hash);
}