#ifndef LISTA_H
#define LISTA_H

#include "aluno.h"

typedef struct lista Lista;

Lista* criar_lista_dupla(void);
void liberar_lista_dupla(Lista* l);
int inserir_lista_dupla(Lista* l, Aluno al);
Aluno* buscar_lista_dupla(Lista* l, long long matricula);
int obter_qtd_lista_dupla(Lista* l);

#endif
