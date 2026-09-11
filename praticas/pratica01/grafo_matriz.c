#include "grafo_matriz.h"

#include <stdlib.h>

static int vertice_valido(const GrafoMatriz *grafo, int vertice)
{
    return grafo != NULL && vertice >= 0 && vertice < grafo->n;
}

GrafoMatriz *criar_grafo_matriz(int n)
{
    GrafoMatriz *grafo;
    int i;

    if (n <= 0) {
        return NULL;
    }

    grafo = malloc(sizeof(*grafo));
    if (grafo == NULL) {
        return NULL;
    }

    grafo->n = n;
    grafo->adj = calloc((size_t)n, sizeof(*grafo->adj));
    if (grafo->adj == NULL) {
        free(grafo);
        return NULL;
    }

    for (i = 0; i < n; i++) {
        grafo->adj[i] = calloc((size_t)n, sizeof(*grafo->adj[i]));
        if (grafo->adj[i] == NULL) {
            while (i > 0) {
                free(grafo->adj[--i]);
            }
            free(grafo->adj);
            free(grafo);
            return NULL;
        }
    }

    return grafo;
}

int inserir_aresta_matriz(GrafoMatriz *grafo, int origem, int destino)
{
    if (!vertice_valido(grafo, origem) ||
        !vertice_valido(grafo, destino) || origem == destino ||
        grafo->adj[origem][destino] != 0) {
        return 0;
    }

    grafo->adj[origem][destino] = 1;
    grafo->adj[destino][origem] = 1;
    return 1;
}

int remover_aresta_matriz(GrafoMatriz *grafo, int origem, int destino)
{
    if (!vertice_valido(grafo, origem) ||
        !vertice_valido(grafo, destino) ||
        grafo->adj[origem][destino] == 0) {
        return 0;
    }

    grafo->adj[origem][destino] = 0;
    grafo->adj[destino][origem] = 0;
    return 1;
}

int grau_matriz(const GrafoMatriz *grafo, int vertice)
{
    int grau = 0;
    int i;

    if (!vertice_valido(grafo, vertice)) {
        return -1;
    }

    for (i = 0; i < grafo->n; i++) {
        grau += grafo->adj[vertice][i];
    }

    return grau;
}

int sao_adjacentes_matriz(const GrafoMatriz *grafo, int origem, int destino)
{
    if (!vertice_valido(grafo, origem) || !vertice_valido(grafo, destino)) {
        return 0;
    }

    return grafo->adj[origem][destino] != 0;
}

void liberar_grafo_matriz(GrafoMatriz *grafo)
{
    int i;

    if (grafo == NULL) {
        return;
    }

    for (i = 0; i < grafo->n; i++) {
        free(grafo->adj[i]);
    }
    free(grafo->adj);
    free(grafo);
}
