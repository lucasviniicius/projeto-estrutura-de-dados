#include "aluno.h"
#include "func_hash.h"

typedef struct hash_separado HashSeparado;
typedef struct no_hash NoHash;

HashSeparado* criar_tabela_separado(int capacidade);
int inserir_separado(HashSeparado* hash, Aluno al, FuncHash func_hash);
Aluno* buscar_separado(HashSeparado* hash, long matricula, FuncHash func_hash);
void liberar_separado(HashSeparado* hash);