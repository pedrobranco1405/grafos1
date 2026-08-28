#include "grafo_lista.h"

#include <stdlib.h>

static int vertice_valido(const GrafoLista *grafo, int vertice)
{
    return grafo != NULL && vertice >= 0 && vertice < grafo->n;
}

static No *criar_no(int destino)
{
    No *novo = malloc(sizeof(*novo));

    if (novo != NULL) {
        novo->destino = destino;
        novo->prox = NULL;
    }
    return novo;
}

GrafoLista *criar_grafo_lista(int n)
{
    GrafoLista *grafo;

    if (n <= 0) {
        return NULL;
    }

    grafo = malloc(sizeof(*grafo));
    if (grafo == NULL) {
        return NULL;
    }

    grafo->adj = calloc((size_t)n, sizeof(*grafo->adj));
    if (grafo->adj == NULL) {
        free(grafo);
        return NULL;
    }

    grafo->n = n;
    return grafo;
}

int sao_adjacentes_lista(const GrafoLista *grafo, int origem, int destino)
{
    const No *atual;

    if (!vertice_valido(grafo, origem) || !vertice_valido(grafo, destino)) {
        return 0;
    }

    atual = grafo->adj[origem];
    while (atual != NULL) {
        if (atual->destino == destino) {
            return 1;
        }
        atual = atual->prox;
    }
    return 0;
}

int inserir_aresta_lista(GrafoLista *grafo, int origem, int destino)
{
    No *ida;
    No *volta;

    if (!vertice_valido(grafo, origem) ||
        !vertice_valido(grafo, destino) || origem == destino ||
        sao_adjacentes_lista(grafo, origem, destino)) {
        return 0;
    }

    ida = criar_no(destino);
    volta = criar_no(origem);
    if (ida == NULL || volta == NULL) {
        free(ida);
        free(volta);
        return 0;
    }

    ida->prox = grafo->adj[origem];
    grafo->adj[origem] = ida;
    volta->prox = grafo->adj[destino];
    grafo->adj[destino] = volta;
    return 1;
}

static int remover_da_lista(No **inicio, int destino)
{
    No **atual = inicio;

    while (*atual != NULL) {
        if ((*atual)->destino == destino) {
            No *removido = *atual;
            *atual = removido->prox;
            free(removido);
            return 1;
        }
        atual = &(*atual)->prox;
    }
    return 0;
}

int remover_aresta_lista(GrafoLista *grafo, int origem, int destino)
{
    if (!vertice_valido(grafo, origem) || !vertice_valido(grafo, destino)) {
        return 0;
    }

    if (!remover_da_lista(&grafo->adj[origem], destino)) {
        return 0;
    }
    remover_da_lista(&grafo->adj[destino], origem);
    return 1;
}

int grau_lista(const GrafoLista *grafo, int vertice)
{
    const No *atual;
    int grau = 0;

    if (!vertice_valido(grafo, vertice)) {
        return -1;
    }

    atual = grafo->adj[vertice];
    while (atual != NULL) {
        grau++;
        atual = atual->prox;
    }
    return grau;
}

void liberar_grafo_lista(GrafoLista *grafo)
{
    int i;

    if (grafo == NULL) {
        return;
    }

    for (i = 0; i < grafo->n; i++) {
        No *atual = grafo->adj[i];
        while (atual != NULL) {
            No *proximo = atual->prox;
            free(atual);
            atual = proximo;
        }
    }
    free(grafo->adj);
    free(grafo);
}
