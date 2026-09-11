#ifndef BUSCA_PROFUNDIDADE_H
#define BUSCA_PROFUNDIDADE_H

#include "../pratica01/grafo_lista.h"

typedef struct {
    int *dados;
    int topo, capacidade;
} Pilha;

/* Mesmo contrato de grafo de busca_largura.h. Vetores com n elementos.
 * Inicialize visitado com 0, entrada/saida com 0, pred com -1 e tempo com 0.
 * Visita somente o componente de u, preservando os vertices ja visitados.
 * Os quatro vetores devem ser distintos. Tempos comecam em 1.
 */
void dfs_recursiva(GrafoLista *g, int u, int *visitado,
                   int *entrada, int *saida, int *pred, int *tempo);

/* Inicializa os vetores e percorre TODOS os componentes com DFS recursiva.
 * Retorna 1 no sucesso, 0 para argumentos invalidos/falta de memoria.
 */
int dfs(GrafoLista *g, int *entrada, int *saida, int *pred);

/* Inicializa visitado e visita o componente da origem usando pilha LIFO.
 * Retorna 1 no sucesso e 0 no erro.
 */
int dfs_iterativa(GrafoLista *g, int origem, int *visitado);

/* Retorna 1 se houver ciclo, 0 se nao houver, -1 no erro.
 * Considera arestas nao direcionadas: a volta ao pai nao forma ciclo.
 */
int tem_ciclo(GrafoLista *g);

#endif
