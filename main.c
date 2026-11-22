#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "aluno.h"
#include "experimento.h"

#define MAX_REGISTROS 10000000

int main() {
    srand(42);

    Aluno* base = malloc((size_t)MAX_REGISTROS * sizeof(Aluno));
    if (base == NULL) {
        printf("Erro de alocacao de memoria.\n");
        return 1;
    }

    printf("Carregando base de dados...\n");
    int total = carregar_base("alunos.txt", base, MAX_REGISTROS);
    if (total <= 0) {
        free(base);
        return 1;
    }

    printf("Registros lidos: %d. Iniciando testes...\n", total);
    rodar_experimentos(base, total);

    free(base);
    printf("\nConcluido! Resultados salvos em resultados.csv\n");
    return 0;
}