# Projeto 2º GQ — Comparação de Estruturas de Dados em C

Projeto desenvolvido para a disciplina **Estrutura de Dados Lineares**, com o objetivo de implementar, testar e comparar o desempenho de diferentes estruturas de dados (alocadas dinamicamente e organizadas em módulos/TADs em linguagem C) na inserção e busca de registros de um **Cadastro Nacional de Estudantes** com até **10 milhões de registros**.

---

## Sumário

1. [Arquitetura e Módulos](#arquitetura-e-módulos)
2. [Estruturas de Dados Implementadas](#estruturas-de-dados-implementadas)
3. [Funções Hash Implementadas](#funções-hash-implementadas)
4. [Justificativa do Tamanho das Tabelas Hash](#justificativa-do-tamanho-das-tabelas-hash)
5. [Metodologia dos Experimentos](#metodologia-dos-experimentos)
6. [Compilação e Execução](#compilação-e-execução)
7. [Resultados dos Experimentos](#resultados-dos-experimentos)
8. [Análise Comparativa e Conclusões](#análise-comparativa-e-conclusões)
9. [Apresentação (Slides)](#apresentação-slides)

---

## Arquitetura e Módulos

O código-fonte segue o conceito de **Tipo Abstrato de Dados (TAD)**, com estruturas internas encapsuladas nos arquivos `.c`, cabeçalhos protegidos por *include guards* e alocação dinâmica de memória com liberação completa ao final de cada experimento:

| Arquivo | Responsabilidade |
| :--- | :--- |
| `aluno.h` | Definição do registro `Aluno` (`matricula`, `nome`, `curso`). |
| `lista.h` / `lista.c` | TAD **Lista Duplamente Encadeada** (estrutura linear). |
| `func_hash.h` / `func_hash.c` | Tipo de ponteiro `FuncHash` e as duas funções hash (`hash_divisao` e `hash_fnv1a`). |
| `hash_aberto.h` / `hash_aberto.c` | TAD **Tabela Hash com Endereçamento Aberto** (sondagem linear). |
| `hash_separado.h` / `hash_separado.c` | TAD **Tabela Hash com Encadeamento Separado** (listas encadeadas por bucket). |
| `experimento.h` / `experimento.c` | Carregamento da base, sorteio determinístico das buscas, cronometragem de alta precisão e exportação para `resultados.csv`. |
| `main.c` | Ponto de entrada do programa. |
| `Makefile` | Automação de compilação em Windows e Linux/macOS. |
| `resultados.csv` | Dados brutos coletados na execução completa dos experimentos. |
| `slides/apresentacao.pdf` | Slides da apresentação com tabelas, gráficos e conclusões. |

---

## Estruturas de Dados Implementadas

1. **Lista Duplamente Encadeada (`Lista`)**
   - Mantém ponteiros para `inicio` e `final` além da contagem `qtd`.
   - **Inserção:** realizada diretamente no final da lista em tempo $O(1)$.
   - **Busca:** percurso sequencial do início ao fim em tempo $O(n)$.

2. **Tabela Hash com Endereçamento Aberto (`HashAberta`)**
   - Armazena ponteiros `Aluno*` em um vetor dinâmico de tamanho fixo `capacidade`.
   - **Tratamento de colisões:** **Sondagem Linear**, calculada por `pos_i = (pos + i) % capacidade`.
   - **Contagem de colisões:** utiliza um vetor auxiliar `indice_usado` de tamanho `capacidade` para contabilizar fielmente quantas vezes uma nova matrícula gera um **índice hash inicial** já produzido anteriormente por outra matrícula (evitando inflar a contagem com deslocamentos secundários da sondagem linear).

3. **Tabela Hash com Encadeamento Separado (`HashSeparado`)**
   - Vetor dinâmico de ponteiros para listas encadeadas simples (`NoHash**`).
   - **Tratamento de colisões:** inserção no início da lista do bucket em tempo $O(1)$ e busca percorrendo apenas a lista correspondente ao índice `pos`.
   - **Contagem de colisões:** incrementa sempre que `vetor[pos] != NULL` no momento da inserção, medindo exatamente as colisões no índice hash inicial.

---

## Funções Hash Implementadas

Todas as quatro configurações de tabela hash testam as duas funções abaixo sobre a chave `matricula`:

1. **Método da Divisão (`hash_divisao`)** — *método apresentado em sala*
   - Calcula o resto da divisão da matrícula pela capacidade $m$ da tabela:
     $$h_1(k) = k \bmod m$$
   - Protegido contra chaves negativas (`r < 0 ? r + tam : r`).

2. **FNV-1a de 64 bits (`hash_fnv1a`)** — *método pesquisado*
   - Implementa o algoritmo **Fowler–Noll–Vo (variante 1a, 64 bits)** sobre os 8 bytes da matrícula:
     - Inicializa `hash = 14695981039346656037` (*FNV offset basis*).
     - Para cada byte da chave, aplica `hash ^= byte` seguido de `hash *= 1099511628211` (*FNV prime*).
     - Reduz o resultado ao intervalo da tabela com `hash % tam`.
   - Proporciona dispersão pseudoaleatória uniforme independente de padrões aritméticos na chave.

---

## Justificativa do Tamanho das Tabelas Hash

As quatro configurações de tabelas hash compartilham o **mesmo tamanho fixo $m$** em cada volume, sem redimensionamento durante o experimento:

| Volume ($N$) | Capacidade ($m$) | Fator de Carga (30%) | Fator de Carga (60%) | Fator de Carga (100%) |
| ---: | ---: | ---: | ---: | ---: |
| 100.000 | **130.003** (primo) | 0,2308 | 0,4615 | 0,7692 |
| 1.000.000 | **1.300.021** (primo) | 0,2308 | 0,4615 | 0,7692 |
| 10.000.000 | **13.000.027** (primo) | 0,2308 | 0,4615 | 0,7692 |

**Por que esses valores foram escolhidos?**
1. **Limite de fator de carga ($\alpha \approx 0{,}77 < 0{,}80$):** no endereçamento aberto com sondagem linear, a capacidade precisa ser estritamente maior que $N$ ($\alpha < 1$), e acima de $\alpha \approx 0{,}80$ o agrupamento primário (*primary clustering*) faz o número médio de sondagens crescer drasticamente. Escolher $m \approx 1{,}3 \cdot N$ garante o mesmo $\alpha$ máximo (~`0,7692`) nos três volumes, permitindo comparar o efeito puro da escala de dados.
2. **Números primos:** escolher $m$ primo reduz colisões sistemáticas no método da divisão quando há regularidades nos dígitos da chave.

---

## Metodologia dos Experimentos

Para garantir comparação justa entre as cinco implementações:
- **Base de dados:** `alunos.txt` com `10.000.000` de registros únicos (`matricula;nome;curso`), preservando exatamente a ordem original do arquivo.
- **Volumes testados:** `100.000`, `1.000.000` e `10.000.000` registros.
- **Etapas de medição:** inserção medida separadamente nos intervalos **0–30%**, **30–60%** e **60–100%**, com o contador de colisões zerado antes de cada intervalo.
- **Buscas:** após cada etapa, são executadas **10.000 buscas** por matrículas existentes sorteadas aleatoriamente entre os registros já inseridos (gerador `xorshift64` com semente fixa `42`), utilizando **exatamente as mesmas 10.000 matrículas e a mesma ordem de busca** nas cinco implementações. Todas as 10.000 buscas têm seu retorno verificado (`10000/10000`).
- **Isolamento das medições:** o tempo de leitura de `alunos.txt` e o tempo de sorteio das matrículas ficam completamente fora da cronometragem.
- **Precisão temporal:** utilização de temporizador monotônico de alta resolução (`QueryPerformanceCounter` no Windows e `clock_gettime(CLOCK_MONOTONIC)` em sistemas POSIX), evitando o arredondamento grosseiro de `1 ms` da função `clock()` no Windows.

---

## Compilação e Execução

### Pré-requisitos
- Compilador C compatível com C11 (`gcc` — ex.: MSYS2/MinGW-w64 no Windows ou GCC no Linux/macOS).
- Arquivo de entrada `alunos.txt` (não versionado no repositório devido ao tamanho de ~365 MB).

### Compilando com `gcc`
```bash
gcc -O2 -std=c11 -Wall -Wextra -Wpedantic main.c experimento.c func_hash.c lista.c hash_aberto.c hash_separado.c -o projeto
```

Ou utilizando o `Makefile`:
```bash
make
```

### Executando
Por padrão, o executável lê `alunos.txt` no diretório atual e grava os resultados em `resultados.csv`:
```bash
./projeto
```

Também é possível informar caminhos personalizados via linha de comando:
```bash
./projeto ../alunos.txt resultados.csv
```

> **Observação:** o experimento completo de **10 milhões de registros** executa 10.000 buscas sequenciais na Lista Duplamente Encadeada em 3M, 6M e 10M elementos, levando cerca de ~45 minutos no total (enquanto todas as tabelas hash juntas executam em poucos segundos). Os resultados completos já foram executados previamente e salvos em [`resultados.csv`](resultados.csv).

---

## Resultados dos Experimentos

*(Compilado com `gcc 15.2.0 -O2 -std=c11`, 10.000 buscas por etapa, todas validadas com 100% de acerto.)*

### Volume 1 — 100.000 registros ($m = 130.003$)

| Estrutura | Etapa | Inserção (s) | Busca 10k (s) | Colisões (intervalo) | Fator de Carga ($\alpha$) |
| :--- | :---: | ---: | ---: | ---: | ---: |
| Lista Dupla | 0–30% | 0,002242 | 0,590137 | — | — |
| Lista Dupla | 30–60% | 0,001848 | 1,199315 | — | — |
| Lista Dupla | 60–100% | 0,003324 | 2,210032 | — | — |
| Hash Aberto (Divisão) | 0–30% | 0,002321 | 0,000147 | 3.209 | 0,2308 |
| Hash Aberto (Divisão) | 30–60% | 0,002087 | 0,000183 | 8.651 | 0,4615 |
| Hash Aberto (Divisão) | 60–100% | 0,003021 | 0,000285 | 18.306 | 0,7692 |
| Hash Aberto (FNV-1a) | 0–30% | 0,001814 | 0,000126 | 3.191 | 0,2308 |
| Hash Aberto (FNV-1a) | 30–60% | 0,002198 | 0,000394 | 8.738 | 0,4615 |
| Hash Aberto (FNV-1a) | 60–100% | 0,003275 | 0,000473 | 18.321 | 0,7692 |
| Hash Separado (Divisão) | 0–30% | 0,001723 | **0,000062** | 3.209 | 0,2308 |
| Hash Separado (Divisão) | 30–60% | 0,001691 | **0,000090** | 8.651 | 0,4615 |
| Hash Separado (Divisão) | 60–100% | 0,002377 | **0,000136** | 18.306 | 0,7692 |
| Hash Separado (FNV-1a) | 0–30% | 0,001773 | 0,000118 | 3.191 | 0,2308 |
| Hash Separado (FNV-1a) | 30–60% | 0,001735 | 0,000163 | 8.738 | 0,4615 |
| Hash Separado (FNV-1a) | 60–100% | 0,002675 | 0,000361 | 18.321 | 0,7692 |

### Volume 2 — 1.000.000 de registros ($m = 1.300.021$)

| Estrutura | Etapa | Inserção (s) | Busca 10k (s) | Colisões (intervalo) | Fator de Carga ($\alpha$) |
| :--- | :---: | ---: | ---: | ---: | ---: |
| Lista Dupla | 0–30% | 0,019053 | 25,207525 | — | — |
| Lista Dupla | 30–60% | 0,023770 | 77,693622 | — | — |
| Lista Dupla | 60–100% | 0,027051 | 139,005702 | — | — |
| Hash Aberto (Divisão) | 0–30% | 0,032478 | 0,000285 | 31.822 | 0,2308 |
| Hash Aberto (Divisão) | 30–60% | 0,029212 | 0,000418 | 86.575 | 0,4615 |
| Hash Aberto (Divisão) | 60–100% | 0,035804 | 0,000572 | 181.258 | 0,7692 |
| Hash Aberto (FNV-1a) | 0–30% | 0,031672 | 0,000687 | 31.808 | 0,2308 |
| Hash Aberto (FNV-1a) | 30–60% | 0,030137 | 0,000726 | 87.104 | 0,4615 |
| Hash Aberto (FNV-1a) | 60–100% | 0,037830 | 0,001025 | 182.512 | 0,7692 |
| Hash Separado (Divisão) | 0–30% | 0,028060 | **0,000263** | 31.822 | 0,2308 |
| Hash Separado (Divisão) | 30–60% | 0,024929 | **0,000366** | 86.575 | 0,4615 |
| Hash Separado (Divisão) | 60–100% | 0,039616 | **0,000469** | 181.258 | 0,7692 |
| Hash Separado (FNV-1a) | 0–30% | 0,028963 | 0,000841 | 31.808 | 0,2308 |
| Hash Separado (FNV-1a) | 30–60% | 0,026247 | 0,000637 | 87.104 | 0,4615 |
| Hash Separado (FNV-1a) | 60–100% | 0,037280 | 0,000773 | 182.512 | 0,7692 |

### Volume 3 — 10.000.000 de registros ($m = 13.000.027$)

| Estrutura | Etapa | Inserção (s) | Busca 10k (s) | Colisões (intervalo) | Fator de Carga ($\alpha$) |
| :--- | :---: | ---: | ---: | ---: | ---: |
| Lista Dupla | 0–30% | 0,177562 | 420,613466 | — | — |
| Lista Dupla | 30–60% | 0,208295 | 842,367101 | — | — |
| Lista Dupla | 60–100% | 0,286307 | 1464,248078 | — | — |
| Hash Aberto (Divisão) | 0–30% | 0,587060 | 0,000519 | 281.036 | 0,2308 |
| Hash Aberto (Divisão) | 30–60% | 0,654860 | 0,001144 | 783.706 | 0,4615 |
| Hash Aberto (Divisão) | 60–100% | 0,727728 | 0,000920 | 1.690.734 | 0,7692 |
| Hash Aberto (FNV-1a) | 0–30% | 0,466460 | 0,000948 | 306.503 | 0,2308 |
| Hash Aberto (FNV-1a) | 30–60% | 0,476160 | 0,001639 | 840.267 | 0,4615 |
| Hash Aberto (FNV-1a) | 60–100% | 0,664518 | 0,001379 | 1.777.928 | 0,7692 |
| Hash Separado (Divisão) | 0–30% | 0,435912 | 0,000742 | 281.036 | 0,2308 |
| Hash Separado (Divisão) | 30–60% | 0,436783 | **0,000562** | 783.706 | 0,4615 |
| Hash Separado (Divisão) | 60–100% | 0,580657 | **0,000621** | 1.690.734 | 0,7692 |
| Hash Separado (FNV-1a) | 0–30% | 0,414650 | 0,000800 | 306.503 | 0,2308 |
| Hash Separado (FNV-1a) | 30–60% | 0,408271 | 0,001036 | 840.267 | 0,4615 |
| Hash Separado (FNV-1a) | 60–100% | 0,593270 | 0,001119 | 1.777.928 | 0,7692 |

---

## Análise Comparativa e Conclusões

### 1. Estrutura Linear vs. Tabelas Hash
- **Busca ($O(n)$ vs. $O(1)$):** a diferença é avassaladora. Com 10 milhões de registros preenchidos (100%), a **Lista Duplamente Encadeada** levou **1.464,25 segundos (~24 min 24 s)** para 10.000 buscas, enquanto a **Tabela Hash com Encadeamento Separado (Divisão)** realizou as mesmas 10.000 buscas em **0,000621 s (0,62 ms)** — cerca de **2,36 milhões de vezes mais rápida**.
- **Inserção:** a Lista Dupla apresentou o menor tempo total de inserção em 10M (**0,672 s** vs. `1,416 s – 1,970 s` nas tabelas hash), pois insere diretamente pelo ponteiro `final` com boa localidade sequencial, sem cálculo de hash, acessos aleatórios à tabela de ~104 MB ou resolução de colisões. Porém, esse ganho de ~0,8 s na carga é insignificante diante dos quase 25 minutos perdidos em apenas 10.000 buscas.

### 2. Comportamento conforme aumenta o volume ($100\text{k} \to 1\text{M} \to 10\text{M}$)
- **Lista Dupla:** o tempo de busca cresceu proporcionalmente ao número de nós (em 10M, `420,6 s` em 3M $\to$ `842,4 s` em 6M $\to$ `1.464,2 s` em 10M), com um salto adicional entre 100k e 1M quando os nós deixam de caber no cache L3 da CPU e passam a sofrer *cache misses* em memória RAM.
- **Tabelas Hash:** o tempo de busca permaneceu praticamente constante na casa de **sub-milissegundo a ~1,6 ms** para 10.000 consultas mesmo multiplicando o volume por **100×** (de 100 mil para 10 milhões), refletindo o custo esperado $O(1)$ com fator de carga controlado ($\alpha \le 0{,}77$). O ligeiro aumento de `0,14 ms` (100k) para `0,62 ms` (10M) decorre da maior taxa de *cache misses* conforme o vetor principal cresce de ~1 MB para ~104 MB.

### 3. Encadeamento Separado vs. Endereçamento Aberto (Sondagem Linear)
- O **Encadeamento Separado** superou o **Endereçamento Aberto** tanto na inserção quanto na busca em todos os volumes:
  - **Inserção total (10M):** `1,416 s` (FNV-1a) e `1,453 s` (Divisão) no Separado, contra `1,607 s` (FNV-1a) e `1,970 s` (Divisão) no Aberto.
  - **Busca em 100% (10M):** `0,621 ms` (Divisão) e `1,119 ms` (FNV-1a) no Separado, contra `0,920 ms` (Divisão) e `1,379 ms` (FNV-1a) no Aberto.
- **Motivo:** no encadeamento separado, cada bucket tem em média no máximo $\alpha = 0{,}77$ elemento e a inserção na cabeça da lista é sempre $O(1)$. Já na sondagem linear, à medida que $\alpha$ se aproxima de `0,77`, ocorre **agrupamento primário (*primary clustering*)**, formando blocos contíguos ocupados que aumentam o número de sondagens tanto na inserção quanto na busca.

### 4. Método da Divisão vs. FNV-1a
- **Velocidade de busca:** o **Método da Divisão** foi consistentemente mais rápido nas buscas em todos os volumes (`0,621 ms` vs. `1,119 ms` no Hash Separado em 10M; `0,920 ms` vs. `1,379 ms` no Hash Aberto em 10M), pois executa apenas uma operação aritmética de módulo, enquanto a FNV-1a realiza 8 iterações de XOR e multiplicação por chave.
- **Colisões:** em 100 mil e 1 milhão, ambas empataram estatisticamente (`30.166` vs. `30.250` em 100k; `299.655` vs. `301.424` em 1M). Já em **10 milhões**, a Divisão produziu **5,8% menos colisões** (`2.755.476` vs. `2.924.698`). Isso ocorre porque as matrículas de 8 dígitos são **únicas** no universo de ~90 milhões de inteiros ($[10^7, 10^8)$), havendo apenas ~6,9 chaves possíveis por resto $\bmod 13.000.027$ (amostragem sem reposição). A FNV-1a projeta as chaves no espaço de $2^{64}$ antes do módulo, comportando-se exatamente como um hash aleatório uniforme com reposição (cujo valor teórico esperado é $N - m(1 - e^{-N/m}) \approx 2.924.760$, batendo quase perfeitamente com os `2.924.698` observados).

### 5. Melhor Implementação Geral
Para o cenário do Cadastro Nacional de Estudantes (alto volume de registros e buscas frequentes por matrícula), a **Tabela Hash com Encadeamento Separado utilizando o Método da Divisão** apresentou o **melhor desempenho geral**, combinando os menores tempos de busca em todos os volumes, menor quantidade de colisões em 10 milhões, inserção rápida e ausência de degradação por agrupamento primário.

---

## Apresentação (Slides)

Os slides completos com tabelas, gráficos comparativos e conclusões estão disponíveis em:
- [`slides/apresentacao.pdf`](slides/apresentacao.pdf)
