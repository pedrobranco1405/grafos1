#ifndef DAG_H
#define DAG_H

#include "../pratica01/grafo_lista.h"

/* Usa a estrutura GrafoLista, mas cada no representa apenas u -> destino.
 * Os auxiliares abaixo criam arestas DIRECIONADAS (sem inserir a volta).
 * Vertices numerados de 0 a n - 1; n = 0 representa o grafo vazio.
 */
GrafoLista *criar_digrafo(int n);
/* Retorna 1 no sucesso, 0 no erro ou arco repetido. Permite lacos. */
int inserir_arco(GrafoLista *g, int origem, int destino);
void liberar_digrafo(GrafoLista *g);

/* Retornam um vetor alocado com a ordem e definem *tamanho = g->n.
 * O chamador deve liberar o vetor com free().
 * Retornam NULL e zeram *tamanho se houver ciclo, argumento invalido ou
 * falha de memoria. tamanho deve ser um ponteiro valido.
 * No grafo vazio, retornam vetor liberavel nao NULL, com tamanho zero.
 * Nao modificam o grafo. Custo: O(V + E) em tempo e O(V) em memoria.
 */
int *ordenacao_topologica_kahn(GrafoLista *g, int *tamanho);
int *ordenacao_topologica_dfs(GrafoLista *g, int *tamanho);

/* 1: DAG (inclusive vazio); 0: contem ciclo; -1: argumento invalido/memoria. */
int eh_dag(GrafoLista *g);

#endif
