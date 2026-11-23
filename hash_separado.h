#ifndef HASH_SEPARADO_H
#define HASH_SEPARADO_H

#include "aluno.h"
#include "func_hash.h"

typedef struct hash_separado HashSeparado;

HashSeparado* criar_tabela_separado(int capacidade);
int inserir_separado(HashSeparado* hash, Aluno al, FuncHash func_hash);
Aluno* buscar_separado(HashSeparado* hash, long long matricula, FuncHash func_hash);
void liberar_separado(HashSeparado* hash);
int obter_qtd_separado(HashSeparado* hash);
long long obter_colisoes_separado(HashSeparado* hash);
void zerar_colisoes_separado(HashSeparado* hash);

#endif
