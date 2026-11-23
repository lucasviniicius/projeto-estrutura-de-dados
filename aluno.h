#ifndef ALUNO_H
#define ALUNO_H

struct aluno {
    long long matricula;
    char nome[50];
    char curso[50];
};

typedef struct aluno Aluno;

#endif
