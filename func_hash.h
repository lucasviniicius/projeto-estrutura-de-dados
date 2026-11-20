#ifndef FUNC_HASH_H
#define FUNC_HASH_H

typedef int (*FuncHash)(long chave, int tam);

int hash_divisao(long chave, int tam);
int hash_dobra(long chave, int tam);

#endif