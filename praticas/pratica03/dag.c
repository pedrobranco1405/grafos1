#include "dag.h"

#include <stdlib.h>

static int grafo_valido(const GrafoLista *g)
{
    int u;
    if (g == NULL || g->n < 0 || (g->n > 0 && g->adj == NULL)) {
        return 0;
    }
    for (u = 0; u < g->n; u++) {
        const No *no;
        for (no = g->adj[u]; no != NULL; no = no->prox) {
            if (no->destino < 0 || no->destino >= g->n) {
                return 0;
            }
        }
    }
    return 1;
}

GrafoLista *criar_digrafo(int n)
{
    GrafoLista *g;
    if (n < 0) {
        return NULL;
    }
    g = malloc(sizeof(*g));
    if (g == NULL) {
        return NULL;
    }
    g->n = n;
    g->adj = NULL;
    if (n > 0) {
        g->adj = calloc((size_t)n, sizeof(*g->adj));
        if (g->adj == NULL) {
            free(g);
            return NULL;
        }
    }
    return g;
}

int inserir_arco(GrafoLista *g, int origem, int destino)
{
    No *novo;
    const No *atual;
    if (g == NULL || g->adj == NULL || origem < 0 || origem >= g->n ||
        destino < 0 || destino >= g->n) {
        return 0;
    }
    for (atual = g->adj[origem]; atual != NULL; atual = atual->prox) {
        if (atual->destino == destino) {
            return 0;
        }
    }
    novo = malloc(sizeof(*novo));
    if (novo == NULL) {
        return 0;
    }
    novo->destino = destino;
    novo->prox = g->adj[origem];
    g->adj[origem] = novo;
    return 1;
}

void liberar_digrafo(GrafoLista *g)
{
    int u;
    if (g == NULL) {
        return;
    }
    for (u = 0; u < g->n; u++) {
        No *atual = g->adj[u];
        while (atual != NULL) {
            No *proximo = atual->prox;
            free(atual);
            atual = proximo;
        }
    }
    free(g->adj);
    free(g);
}

/* estado distingue um ciclo (0) de uma falha de execucao (-1). */
static int *executar_kahn(GrafoLista *g, int *tamanho, int *estado)
{
    size_t *grau;
    int *ordem;
    int inicio = 0, fim = 0, u;

    *estado = -1;
    if (tamanho == NULL) {
        return NULL;
    }
    *tamanho = 0;
    if (!grafo_valido(g)) {
        return NULL;
    }
    /* Um elemento garante retorno nao NULL tambem para o DAG vazio. */
    ordem = malloc((size_t)(g->n > 0 ? g->n : 1) * sizeof(*ordem));
    grau = calloc((size_t)(g->n > 0 ? g->n : 1), sizeof(*grau));
    if (ordem == NULL || grau == NULL) {
        free(ordem);
        free(grau);
        return NULL;
    }
    for (u = 0; u < g->n; u++) {
        const No *no;
        for (no = g->adj[u]; no != NULL; no = no->prox) {
            grau[no->destino]++;
        }
    }
    /* O proprio vetor de resultado funciona como fila FIFO: os vertices
     * entram no fim e saem pelo inicio. Cada vertice entra uma unica vez.
     */
    for (u = 0; u < g->n; u++) {
        if (grau[u] == 0) {
            ordem[fim++] = u;
        }
    }
    while (inicio < fim) {
        const No *no;
        u = ordem[inicio++];
        for (no = g->adj[u]; no != NULL; no = no->prox) {
            int v = no->destino;
            grau[v]--;
            if (grau[v] == 0) {
                ordem[fim++] = v;
            }
        }
    }
    free(grau);
    if (fim != g->n) {
        *estado = 0;
        free(ordem);
        return NULL;
    }
    *estado = 1;
    *tamanho = fim;
    return ordem;
}

int *ordenacao_topologica_kahn(GrafoLista *g, int *tamanho)
{
    int estado;
    return executar_kahn(g, tamanho, &estado);
}

/* 0 = branco (nao visitado), 1 = cinza (ativo), 2 = preto (finalizado).
 * Uma aresta para cinza volta a um ancestral e identifica um ciclo.
 * Arestas para vertices pretos sao permitidas em um DAG.
 */
static int visitar_dfs(GrafoLista *g, int u, int *cor, int *pilha, int *topo)
{
    const No *no;
    cor[u] = 1;
    for (no = g->adj[u]; no != NULL; no = no->prox) {
        int v = no->destino;
        if (cor[v] == 1 ||
            (cor[v] == 0 && !visitar_dfs(g, v, cor, pilha, topo))) {
            return 0;
        }
    }
    cor[u] = 2;
    pilha[(*topo)++] = u;
    return 1;
}

int *ordenacao_topologica_dfs(GrafoLista *g, int *tamanho)
{
    int *ordem, *cor;
    int topo = 0, u;
    if (tamanho == NULL) {
        return NULL;
    }
    *tamanho = 0;
    if (!grafo_valido(g)) {
        return NULL;
    }
    ordem = malloc((size_t)(g->n > 0 ? g->n : 1) * sizeof(*ordem));
    cor = calloc((size_t)(g->n > 0 ? g->n : 1), sizeof(*cor));
    if (ordem == NULL || cor == NULL) {
        free(ordem);
        free(cor);
        return NULL;
    }
    for (u = 0; u < g->n; u++) {
        if (cor[u] == 0 && !visitar_dfs(g, u, cor, ordem, &topo)) {
            free(ordem);
            free(cor);
            return NULL;
        }
    }
    free(cor);
    /* Desempilhamento: a ordem topologica e o inverso da ordem de saida. */
    for (u = 0; u < topo / 2; u++) {
        int aux = ordem[u];
        ordem[u] = ordem[topo - 1 - u];
        ordem[topo - 1 - u] = aux;
    }
    *tamanho = topo;
    return ordem;
}

int eh_dag(GrafoLista *g)
{
    int tamanho, estado;
    int *ordem = executar_kahn(g, &tamanho, &estado);
    free(ordem);
    return estado;
}
