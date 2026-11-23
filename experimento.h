#ifndef EXPERIMENTO_H
#define EXPERIMENTO_H

#include "aluno.h"

int carregar_base(const char* caminho, Aluno* vetor, int max_registros);
int rodar_experimentos(Aluno* base, int total_lidos, const char* caminho_csv);

#endif
