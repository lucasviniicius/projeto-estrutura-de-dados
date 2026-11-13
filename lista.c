#include <stdlib.h>
#include "lista.h"

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

Lista* criar_lista_dupla(){
    Lista* li = malloc(sizeof(Lista));

    if(li != NULL){
        li->inicio = NULL;
        li->final = NULL;
        li->qtd = 0;
    }

    return li;
}

void liberar_lista_dupla(Lista* li){
    if(li == NULL){
        return;
    }

    Elem* no = li->inicio;
    for(int i = 0; i < li->qtd; i++){
        Elem* atual = no;
        no = no->prox;
        free(atual);
    }

    free(li);
}

int inserir_lista_dupla(Lista* li, Aluno al){
    if(li == NULL){
        return 0;
    }

    Elem* no = malloc(sizeof(Elem));
    no->dado = al;

    if(li->qtd == 0){
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

int buscar_lista_dupla(Lista* li, long matricula){
    if(li == NULL || li->qtd == 0){
        return 0;
    }

    Elem* no = li->inicio;

    for(int i = 0; i < li->qtd; i++){
        if(no->dado.matricula == matricula){
            return 1;
        }
        no = no->prox;
    }

    return 0;
}