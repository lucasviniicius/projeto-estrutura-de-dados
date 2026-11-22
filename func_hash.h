#ifndef FUNC_HASH_H
#define FUNC_HASH_H

typedef int (*FuncHash)(long long chave, int tam);

int hash_divisao(long long chave, int tam);
int hash_dobra(long long chave, int tam);

#endif