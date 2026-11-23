#include <stdio.h>
#include <stdlib.h>

#include "aluno.h"
#include "experimento.h"

#define MAX_REGISTROS 10000000

int main(int argc, char* argv[]) {
    const char* caminho_base = (argc > 1) ? argv[1] : "alunos.txt";
    const char* caminho_csv = (argc > 2) ? argv[2] : "resultados.csv";

    Aluno* base = malloc((size_t)MAX_REGISTROS * sizeof(Aluno));
    if (base == NULL) {
        fprintf(stderr, "Erro de alocacao de memoria para a base de dados.\n");
        return 1;
    }

    printf("Carregando base de dados (%s)...\n", caminho_base);
    fflush(stdout);
    int total = carregar_base(caminho_base, base, MAX_REGISTROS);
    if (total <= 0) {
        free(base);
        return 1;
    }

    printf("Registros lidos: %d. Iniciando experimentos (10.000 buscas por etapa)...\n", total);
    fflush(stdout);

    if (!rodar_experimentos(base, total, caminho_csv)) {
        free(base);
        return 1;
    }

    free(base);
    printf("\nConcluido! Resultados salvos em %s\n", caminho_csv);
    return 0;
}
