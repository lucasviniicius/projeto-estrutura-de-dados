#include <stdlib.h>
#include "lista.h"

typedef struct elem Elem;

struct elem {
    Aluno dado;
    Elem* prox;
    Elem* ant;
};

struct lista {
    int qtd;
    Elem* inicio;
    Elem* final;
};

Lista* criar_lista_dupla(void) {
    Lista* li = malloc(sizeof(Lista));

    if (li != NULL) {
        li->inicio = NULL;
        li->final = NULL;
        li->qtd = 0;
    }

    return li;
}

void liberar_lista_dupla(Lista* li) {
    if (li == NULL) {
        return;
    }

    Elem* no = li->inicio;
    while (no != NULL) {
        Elem* atual = no;
        no = no->prox;
        free(atual);
    }

    free(li);
}

int inserir_lista_dupla(Lista* li, Aluno al) {
    if (li == NULL) {
        return 0;
    }

    Elem* no = malloc(sizeof(Elem));
    if (no == NULL) {
        return 0;
    }

    no->dado = al;

    if (li->qtd == 0) {
        no->ant = NULL;
        li->inicio = no;
    } else {
        no->ant = li->final;
        li->final->prox = no;
    }

    no->prox = NULL;
    li->final = no;
    li->qtd++;

    return 1;
}

Aluno* buscar_lista_dupla(Lista* li, long long matricula) {
    if (li == NULL || li->qtd == 0) {
        return NULL;
    }

    Elem* no = li->inicio;
    while (no != NULL) {
        if (no->dado.matricula == matricula) {
            return &(no->dado);
        }
        no = no->prox;
    }

    return NULL;
}

int obter_qtd_lista_dupla(Lista* li) {
    return (li != NULL) ? li->qtd : 0;
}
