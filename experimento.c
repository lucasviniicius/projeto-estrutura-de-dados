#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "experimento.h"
#include "func_hash.h"
#include "lista.h"
#include "hash_aberto.h"
#include "hash_separado.h"

#define NUM_BUSCAS 1000

// Calcula o tempo decorrido em segundos usando <time.h>
double calcular_tempo(clock_t inicio, clock_t fim) {
    return (double)(fim - inicio) / CLOCKS_PER_SEC;
}

// Sorteia um indice aleatorio entre 0 e limite-1
int sortear_indice(int limite) {
    unsigned int r = ((unsigned int)rand() << 15) | (unsigned int)rand();
    return (int)(r % (unsigned int)limite);
}

int carregar_base(const char* caminho, Aluno* vetor, int max_registros) {
    FILE* f = fopen(caminho, "r");
    if (f == NULL) {
        printf("Erro ao abrir o arquivo: %s\n", caminho);
        return -1;
    }

    char linha[256];
    int count = 0;

    while (count < max_registros && fgets(linha, sizeof(linha), f)) {
        linha[strcspn(linha, "\r\n")] = '\0';

        char* mat = strtok(linha, ";");
        char* nome = strtok(NULL, ";");
        char* curso = strtok(NULL, ";");

        if (mat != NULL && nome != NULL && curso != NULL) {
            vetor[count].matricula = atoll(mat);
            strcpy(vetor[count].nome, nome);
            strcpy(vetor[count].curso, curso);
            count++;
        }
    }

    fclose(f);
    return count;
}

void testar_volume(Aluno* base, int volume, int cap_hash, FILE* csv) {
    int marcos[3] = {
        (int)(volume * 0.30),
        (int)(volume * 0.60),
        volume
    };
    int inicios[3] = {0, marcos[0], marcos[1]};
    const char* nomes_etapas[3] = {"30%", "60%", "100%"};

    // 1. Sorteia as 10.000 matriculas de busca antes de testar as estruturas
    long long* buscas[3];
    for (int e = 0; e < 3; e++) {
        buscas[e] = malloc(NUM_BUSCAS * sizeof(long long));
        for (int i = 0; i < NUM_BUSCAS; i++) {
            int idx = sortear_indice(marcos[e]);
            buscas[e][i] = base[idx].matricula;
        }
    }

    printf("\n=== VOLUME: %d REGISTROS (Tamanho Hash m = %d) ===\n", volume, cap_hash);

    // 2. Testa Lista Duplamente Encadeada
    Lista* lista = criar_lista_dupla();
    for (int e = 0; e < 3; e++) {
        clock_t t0 = clock();
        for (int i = inicios[e]; i < marcos[e]; i++) {
            inserir_lista_dupla(lista, base[i]);
        }
        double t_ins = calcular_tempo(t0, clock());

        t0 = clock();
        for (int b = 0; b < NUM_BUSCAS; b++) {
            buscar_lista_dupla(lista, buscas[e][b]);
        }
        double t_busca = calcular_tempo(t0, clock());

        printf("Lista Dupla             | %-4s | Ins: %.4fs | Busca: %.4fs\n",
               nomes_etapas[e], t_ins, t_busca);
        fprintf(csv, "%d;Lista Dupla;%s;%.6f;%.6f;0;0.0000\n",
                volume, nomes_etapas[e], t_ins, t_busca);
    }
    liberar_lista_dupla(lista);

    // 3. Testa Hash Aberto (Divisao e Dobra)
    FuncHash funcoes[2] = {hash_divisao, hash_dobra};
    const char* nomes_aberta[2] = {"Hash Aberto (Divisao)", "Hash Aberto (Dobra)"};

    for (int f = 0; f < 2; f++) {
        HashAberta* ha = criar_tabela_aberta(cap_hash);
        for (int e = 0; e < 3; e++) {
            zerar_colisoes_aberta(ha);

            clock_t t0 = clock();
            for (int i = inicios[e]; i < marcos[e]; i++) {
                inserir_aberta(ha, base[i], funcoes[f]);
            }
            double t_ins = calcular_tempo(t0, clock());

            t0 = clock();
            for (int b = 0; b < NUM_BUSCAS; b++) {
                busca_aberta(ha, buscas[e][b], funcoes[f]);
            }
            double t_busca = calcular_tempo(t0, clock());

            long long col = obter_colisoes_aberta(ha);
            double alfa = (double)obter_qtd_aberta(ha) / cap_hash;

            printf("%-23s | %-4s | Ins: %.4fs | Busca: %.4fs | Col: %lld | Alfa: %.4f\n",
                   nomes_aberta[f], nomes_etapas[e], t_ins, t_busca, col, alfa);
            fprintf(csv, "%d;%s;%s;%.6f;%.6f;%lld;%.4f\n",
                    volume, nomes_aberta[f], nomes_etapas[e], t_ins, t_busca, col, alfa);
        }
        liberar_aberta(ha);
    }

    // 4. Testa Hash Separado (Divisao e Dobra)
    const char* nomes_sep[2] = {"Hash Separado (Divisao)", "Hash Separado (Dobra)"};

    for (int f = 0; f < 2; f++) {
        HashSeparado* hs = criar_tabela_separado(cap_hash);
        for (int e = 0; e < 3; e++) {
            zerar_colisoes_separado(hs);

            clock_t t0 = clock();
            for (int i = inicios[e]; i < marcos[e]; i++) {
                inserir_separado(hs, base[i], funcoes[f]);
            }
            double t_ins = calcular_tempo(t0, clock());

            t0 = clock();
            for (int b = 0; b < NUM_BUSCAS; b++) {
                buscar_separado(hs, buscas[e][b], funcoes[f]);
            }
            double t_busca = calcular_tempo(t0, clock());

            long long col = obter_colisoes_separado(hs);
            double alfa = (double)obter_qtd_separado(hs) / cap_hash;

            printf("%-23s | %-4s | Ins: %.4fs | Busca: %.4fs | Col: %lld | Alfa: %.4f\n",
                   nomes_sep[f], nomes_etapas[e], t_ins, t_busca, col, alfa);
            fprintf(csv, "%d;%s;%s;%.6f;%.6f;%lld;%.4f\n",
                    volume, nomes_sep[f], nomes_etapas[e], t_ins, t_busca, col, alfa);
        }
        liberar_separado(hs);
    }

    for (int e = 0; e < 3; e++) {
        free(buscas[e]);
    }
}

void rodar_experimentos(Aluno* base, int total_lidos) {
    FILE* csv = fopen("resultados.csv", "w");
    if (csv == NULL) {
        printf("Erro ao criar resultados.csv\n");
        return;
    }
    fprintf(csv, "Volume;Estrutura;Etapa;Tempo_Insercao_s;Tempo_Busca_s;Colisoes;Fator_Carga\n");

    int volumes[3] = {100000, 1000000, 10000000};
    int capacidades[3] = {130003, 1300021, 13000003};

    for (int v = 0; v < 3; v++) {
        if (total_lidos >= volumes[v]) {
            testar_volume(base, volumes[v], capacidades[v], csv);
        }
    }

    fclose(csv);
}