#ifndef HASH_ABERTO_H
#define HASH_ABERTO_H

#include "aluno.h"
#include "func_hash.h"


typedef struct hash_aberta HashAberta;

int sondagem_linear(int pos, int i, int capacidade);
HashAberta* criar_tabela_aberta(int capacidade);
int inserir_aberta(HashAberta* hash, Aluno al, FuncHash func_hash);
Aluno* busca_aberta(HashAberta* hash, long long matricula, FuncHash func_hash);
void liberar_aberta(HashAberta* hash);
int obter_qtd_aberta(HashAberta* hash);
long long obter_colisoes_aberta(HashAberta* hash);
void zerar_colisoes_aberta(HashAberta* hash);
#endif