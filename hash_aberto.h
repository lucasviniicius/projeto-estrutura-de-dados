#include "aluno.h"

typedef struct hash_aberta HashAberta;

int sondagem_linear(int pos, int i, int capacidade);
HashAberta* criar_tabela_aberta(int capacidade);
int inserir_aberta(HashAberta* hash, Aluno al);
Aluno* busca_aberta(HashAberta* hash, long matricula);
void liberar_aberta(HashAberta* hash);