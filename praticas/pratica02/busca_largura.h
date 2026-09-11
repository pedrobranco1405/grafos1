#ifndef BUSCA_LARGURA_H
#define BUSCA_LARGURA_H

#include "../pratica01/grafo_lista.h"

typedef struct {
    int *dados;
    int capacidade, inicio, fim, tamanho;
} Fila;

/* Grafos simples, nao direcionados, com vertices de 0 a n - 1.
 * Os vetores recebidos devem ter espaco para n elementos.
 * BFS retorna 1 no sucesso ou 0 para argumentos invalidos/falta de memoria.
 * Vertices inalcancaveis recebem dist = pred = -1; pred[origem] = -1.
 * dist e pred devem ser vetores distintos.
 */
int bfs(GrafoLista *g, int origem, int *dist, int *pred);

/* Retorna 1 se bipartido, 0 caso contrario e -1 em caso de erro. */
int eh_bipartido(GrafoLista *g);

/* Retorna a quantidade de componentes (0 no grafo vazio), ou -1 no erro. */
int contar_componentes(GrafoLista *g);

#endif
