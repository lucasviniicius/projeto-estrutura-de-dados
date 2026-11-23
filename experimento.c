#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

#include "experimento.h"
#include "func_hash.h"
#include "lista.h"
#include "hash_aberto.h"
#include "hash_separado.h"

#define NUM_BUSCAS 10000

static double obter_tempo_segundos(void) {
#ifdef _WIN32
    static LARGE_INTEGER freq;
    static int inicializado = 0;
    if (!inicializado) {
        QueryPerformanceFrequency(&freq);
        inicializado = 1;
    }
    LARGE_INTEGER agora;
    QueryPerformanceCounter(&agora);
    return (double)agora.QuadPart / (double)freq.QuadPart;
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
#endif
}

/* Gerador pseudoaleatorio xorshift64 deterministico e portavel */
static uint64_t estado_rng = 42ULL;

static uint64_t proximo_aleatorio(void) {
    uint64_t x = estado_rng;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    estado_rng = x;
    return x;
}

static int sortear_indice(int limite) {
    return (int)(proximo_aleatorio() % (uint64_t)limite);
}

int carregar_base(const char* caminho, Aluno* vetor, int max_registros) {
    if (caminho == NULL || vetor == NULL || max_registros <= 0) {
        return -1;
    }

    FILE* f = fopen(caminho, "r");
    if (f == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo: %s\n", caminho);
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
            strncpy(vetor[count].nome, nome, sizeof(vetor[count].nome) - 1);
            vetor[count].nome[sizeof(vetor[count].nome) - 1] = '\0';
            strncpy(vetor[count].curso, curso, sizeof(vetor[count].curso) - 1);
            vetor[count].curso[sizeof(vetor[count].curso) - 1] = '\0';
            count++;
        }
    }

    fclose(f);
    return count;
}

static int testar_volume(Aluno* base, int volume, int cap_hash, FILE* csv) {
    int marcos[3] = {
        (int)(volume * 0.30),
        (int)(volume * 0.60),
        volume
    };
    int inicios[3] = {0, marcos[0], marcos[1]};
    const char* nomes_etapas[3] = {"0-30%", "30-60%", "60-100%"};

    /* 1. Sorteia as 10.000 matriculas de busca antes de testar as estruturas */
    long long* buscas[3] = {NULL, NULL, NULL};
    for (int e = 0; e < 3; e++) {
        buscas[e] = malloc((size_t)NUM_BUSCAS * sizeof(long long));
        if (buscas[e] == NULL) {
            fprintf(stderr, "Erro de alocacao ao sortear matriculas.\n");
            for (int k = 0; k < e; k++) {
                free(buscas[k]);
            }
            return 0;
        }
        for (int i = 0; i < NUM_BUSCAS; i++) {
            int idx = sortear_indice(marcos[e]);
            buscas[e][i] = base[idx].matricula;
        }
    }

    printf("\n=== VOLUME: %d REGISTROS (Tamanho Hash m = %d) ===\n", volume, cap_hash);
    fflush(stdout);

    /* 2. Testa Lista Duplamente Encadeada */
    Lista* lista = criar_lista_dupla();
    if (lista == NULL) {
        fprintf(stderr, "Erro ao criar lista duplamente encadeada.\n");
        for (int e = 0; e < 3; e++) free(buscas[e]);
        return 0;
    }

    for (int e = 0; e < 3; e++) {
        double t0 = obter_tempo_segundos();
        for (int i = inicios[e]; i < marcos[e]; i++) {
            if (!inserir_lista_dupla(lista, base[i])) {
                fprintf(stderr, "Erro de alocacao na insercao da Lista Dupla.\n");
                liberar_lista_dupla(lista);
                for (int k = 0; k < 3; k++) free(buscas[k]);
                return 0;
            }
        }
        double t_ins = obter_tempo_segundos() - t0;

        int encontrados = 0;
        t0 = obter_tempo_segundos();
        for (int b = 0; b < NUM_BUSCAS; b++) {
            if (buscar_lista_dupla(lista, buscas[e][b]) != NULL) {
                encontrados++;
            }
        }
        double t_busca = obter_tempo_segundos() - t0;

        if (encontrados != NUM_BUSCAS) {
            fprintf(stderr, "ERRO: Lista Dupla encontrou %d/%d matriculas.\n",
                    encontrados, NUM_BUSCAS);
        }

        printf("Lista Dupla             | %-7s | Ins: %.6fs | Busca: %.6fs | OK: %d/%d\n",
               nomes_etapas[e], t_ins, t_busca, encontrados, NUM_BUSCAS);
        fflush(stdout);
        fprintf(csv, "%d;Lista Dupla;%s;%.6f;%.6f;0;0.0000\n",
                volume, nomes_etapas[e], t_ins, t_busca);
        fflush(csv);
    }
    liberar_lista_dupla(lista);

    /* 3. Testa Hash Aberto (Divisao e FNV-1a) */
    FuncHash funcoes[2] = {hash_divisao, hash_fnv1a};
    const char* nomes_aberta[2] = {"Hash Aberto (Divisao)", "Hash Aberto (FNV-1a)"};

    for (int f = 0; f < 2; f++) {
        HashAberta* ha = criar_tabela_aberta(cap_hash);
        if (ha == NULL) {
            fprintf(stderr, "Erro ao criar %s.\n", nomes_aberta[f]);
            for (int e = 0; e < 3; e++) free(buscas[e]);
            return 0;
        }

        for (int e = 0; e < 3; e++) {
            zerar_colisoes_aberta(ha);

            double t0 = obter_tempo_segundos();
            for (int i = inicios[e]; i < marcos[e]; i++) {
                if (!inserir_aberta(ha, base[i], funcoes[f])) {
                    fprintf(stderr, "Erro na insercao em %s.\n", nomes_aberta[f]);
                    liberar_aberta(ha);
                    for (int k = 0; k < 3; k++) free(buscas[k]);
                    return 0;
                }
            }
            double t_ins = obter_tempo_segundos() - t0;

            int encontrados = 0;
            t0 = obter_tempo_segundos();
            for (int b = 0; b < NUM_BUSCAS; b++) {
                if (busca_aberta(ha, buscas[e][b], funcoes[f]) != NULL) {
                    encontrados++;
                }
            }
            double t_busca = obter_tempo_segundos() - t0;

            if (encontrados != NUM_BUSCAS) {
                fprintf(stderr, "ERRO: %s encontrou %d/%d matriculas.\n",
                        nomes_aberta[f], encontrados, NUM_BUSCAS);
            }

            long long col = obter_colisoes_aberta(ha);
            double alfa = (double)obter_qtd_aberta(ha) / (double)cap_hash;

            printf("%-23s | %-7s | Ins: %.6fs | Busca: %.6fs | Col: %lld | Alfa: %.4f\n",
                   nomes_aberta[f], nomes_etapas[e], t_ins, t_busca, col, alfa);
            fflush(stdout);
            fprintf(csv, "%d;%s;%s;%.6f;%.6f;%lld;%.4f\n",
                    volume, nomes_aberta[f], nomes_etapas[e], t_ins, t_busca, col, alfa);
            fflush(csv);
        }
        liberar_aberta(ha);
    }

    /* 4. Testa Hash Separado (Divisao e FNV-1a) */
    const char* nomes_sep[2] = {"Hash Separado (Divisao)", "Hash Separado (FNV-1a)"};

    for (int f = 0; f < 2; f++) {
        HashSeparado* hs = criar_tabela_separado(cap_hash);
        if (hs == NULL) {
            fprintf(stderr, "Erro ao criar %s.\n", nomes_sep[f]);
            for (int e = 0; e < 3; e++) free(buscas[e]);
            return 0;
        }

        for (int e = 0; e < 3; e++) {
            zerar_colisoes_separado(hs);

            double t0 = obter_tempo_segundos();
            for (int i = inicios[e]; i < marcos[e]; i++) {
                if (!inserir_separado(hs, base[i], funcoes[f])) {
                    fprintf(stderr, "Erro na insercao em %s.\n", nomes_sep[f]);
                    liberar_separado(hs);
                    for (int k = 0; k < 3; k++) free(buscas[k]);
                    return 0;
                }
            }
            double t_ins = obter_tempo_segundos() - t0;

            int encontrados = 0;
            t0 = obter_tempo_segundos();
            for (int b = 0; b < NUM_BUSCAS; b++) {
                if (buscar_separado(hs, buscas[e][b], funcoes[f]) != NULL) {
                    encontrados++;
                }
            }
            double t_busca = obter_tempo_segundos() - t0;

            if (encontrados != NUM_BUSCAS) {
                fprintf(stderr, "ERRO: %s encontrou %d/%d matriculas.\n",
                        nomes_sep[f], encontrados, NUM_BUSCAS);
            }

            long long col = obter_colisoes_separado(hs);
            double alfa = (double)obter_qtd_separado(hs) / (double)cap_hash;

            printf("%-23s | %-7s | Ins: %.6fs | Busca: %.6fs | Col: %lld | Alfa: %.4f\n",
                   nomes_sep[f], nomes_etapas[e], t_ins, t_busca, col, alfa);
            fflush(stdout);
            fprintf(csv, "%d;%s;%s;%.6f;%.6f;%lld;%.4f\n",
                    volume, nomes_sep[f], nomes_etapas[e], t_ins, t_busca, col, alfa);
            fflush(csv);
        }
        liberar_separado(hs);
    }

    for (int e = 0; e < 3; e++) {
        free(buscas[e]);
    }

    return 1;
}

int rodar_experimentos(Aluno* base, int total_lidos, const char* caminho_csv) {
    FILE* csv = fopen(caminho_csv, "w");
    if (csv == NULL) {
        fprintf(stderr, "Erro ao criar %s\n", caminho_csv);
        return 0;
    }
    fprintf(csv, "Volume;Estrutura;Etapa;Tempo_Insercao_s;Tempo_Busca_s;Colisoes;Fator_Carga\n");

    /*
     * Capacidades escolhidas como numeros primos proximos a 1,3 * volume,
     * garantindo fator de carga maximo ~0,7692 mesmo em 100% de preenchimento
     * (essencial para o enderecamento aberto) e melhor dispersao no metodo da divisao.
     */
    int volumes[3] = {100000, 1000000, 10000000};
    int capacidades[3] = {130003, 1300021, 13000027};

    for (int v = 0; v < 3; v++) {
        if (total_lidos >= volumes[v]) {
            if (!testar_volume(base, volumes[v], capacidades[v], csv)) {
                fclose(csv);
                return 0;
            }
        }
    }

    fclose(csv);
    return 1;
}
