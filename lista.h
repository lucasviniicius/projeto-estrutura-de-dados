#include "aluno.h"

typedef struct elem Elem;
typedef struct lista Lista;

Lista* criar_lista_dupla();
void liberar_lista_dupla(Lista* l);
int inserir_lista_dupla(Lista* l, Aluno al);
int buscar_lista_dupla(Lista* l, long matricula);