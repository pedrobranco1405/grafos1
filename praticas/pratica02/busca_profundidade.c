#include "busca_profundidade.h"

#include <limits.h>
#include <stdlib.h>

static int grafo_valido(const GrafoLista *g)
{
    return g != NULL && g->n >= 0 && (g->n == 0 || g->adj != NULL);
}

void dfs_recursiva(GrafoLista *g, int u, int *visitado,
                   int *entrada, int *saida, int *pred, int *tempo)
{
    const No *vizinho;

    if (!grafo_valido(g) || u < 0 || u >= g->n || visitado == NULL ||
        entrada == NULL || saida == NULL || pred == NULL || tempo == NULL) {
        return;
    }
    if (visitado[u]) {
        return;
    }
    visitado[u] = 1;
    entrada[u] = ++(*tempo);
    for (vizinho = g->adj[u]; vizinho != NULL; vizinho = vizinho->prox) {
        int v = vizinho->destino;
        if (!visitado[v]) {
            pred[v] = u;
            dfs_recursiva(g, v, visitado, entrada, saida, pred, tempo);
        }
    }
    saida[u] = ++(*tempo);
}

int dfs(GrafoLista *g, int *entrada, int *saida, int *pred)
{
    int *visitado;
    int u;
    int tempo = 0;

    if (!grafo_valido(g) || g->n > INT_MAX / 2) {
        return 0;
    }
    if (g->n == 0) {
        return 1;
    }
    if (entrada == NULL || saida == NULL || pred == NULL ||
        entrada == saida || entrada == pred || saida == pred) {
        return 0;
    }
    visitado = calloc((size_t)g->n, sizeof(*visitado));
    if (visitado == NULL) {
        return 0;
    }
    for (u = 0; u < g->n; u++) {
        entrada[u] = saida[u] = 0;
        pred[u] = -1;
    }
    for (u = 0; u < g->n; u++) {
        if (!visitado[u]) {
            dfs_recursiva(g, u, visitado, entrada, saida, pred, &tempo);
        }
    }
    free(visitado);
    return 1;
}

int dfs_iterativa(GrafoLista *g, int origem, int *visitado)
{
    Pilha pilha;
    No **proximo;
    int u;

    if (!grafo_valido(g) || visitado == NULL) {
        return 0;
    }
    for (u = 0; u < g->n; u++) {
        visitado[u] = 0;
    }
    if (origem < 0 || origem >= g->n) {
        return 0;
    }
    pilha.capacidade = g->n;
    pilha.topo = -1;
    pilha.dados = malloc((size_t)pilha.capacidade * sizeof(*pilha.dados));
    proximo = malloc((size_t)g->n * sizeof(*proximo));
    if (pilha.dados == NULL || proximo == NULL) {
        free(pilha.dados);
        free(proximo);
        return 0;
    }
    for (u = 0; u < g->n; u++) {
        proximo[u] = g->adj[u];
    }
    visitado[origem] = 1;
    pilha.dados[++pilha.topo] = origem;
    while (pilha.topo >= 0) {
        No *vizinho;
        u = pilha.dados[pilha.topo];
        vizinho = proximo[u];
        if (vizinho == NULL) {
            pilha.topo--;
        } else {
            int v = vizinho->destino;
            /* Guarda onde retomar ao voltar do filho, como na recursao. */
            proximo[u] = vizinho->prox;
            if (!visitado[v]) {
                visitado[v] = 1;
                pilha.dados[++pilha.topo] = v;
            }
        }
    }
    free(proximo);
    free(pilha.dados);
    return 1;
}

static int ciclo_dfs(GrafoLista *g, int u, int pai, int *visitado)
{
    const No *vizinho;
    visitado[u] = 1;
    for (vizinho = g->adj[u]; vizinho != NULL; vizinho = vizinho->prox) {
        int v = vizinho->destino;
        if (!visitado[v]) {
            if (ciclo_dfs(g, v, u, visitado)) {
                return 1;
            }
        } else if (v != pai) {
            return 1;
        }
    }
    return 0;
}

int tem_ciclo(GrafoLista *g)
{
    int *visitado;
    int u;
    int resultado = 0;

    if (!grafo_valido(g)) {
        return -1;
    }
    if (g->n == 0) {
        return 0;
    }
    visitado = calloc((size_t)g->n, sizeof(*visitado));
    if (visitado == NULL) {
        return -1;
    }
    for (u = 0; u < g->n; u++) {
        if (!visitado[u] && ciclo_dfs(g, u, -1, visitado)) {
            resultado = 1;
            break;
        }
    }
    free(visitado);
    return resultado;
}
