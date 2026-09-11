#include "busca_largura.h"

#include <stdlib.h>

static int grafo_valido(const GrafoLista *g)
{
    return g != NULL && g->n >= 0 && (g->n == 0 || g->adj != NULL);
}

static int iniciar_fila(Fila *fila, int capacidade)
{
    fila->dados = malloc((size_t)capacidade * sizeof(*fila->dados));
    fila->capacidade = capacidade;
    fila->inicio = fila->fim = fila->tamanho = 0;
    return fila->dados != NULL;
}

/* Cada vertice entra uma unica vez: a capacidade n nunca e excedida. */
static void enfileirar(Fila *fila, int valor)
{
    fila->dados[fila->fim] = valor;
    fila->fim = (fila->fim + 1) % fila->capacidade;
    fila->tamanho++;
}

static int desenfileirar(Fila *fila)
{
    int valor = fila->dados[fila->inicio];
    fila->inicio = (fila->inicio + 1) % fila->capacidade;
    fila->tamanho--;
    return valor;
}

int bfs(GrafoLista *g, int origem, int *dist, int *pred)
{
    Fila fila;
    int i;

    if (!grafo_valido(g) || dist == NULL || pred == NULL || dist == pred) {
        return 0;
    }
    for (i = 0; i < g->n; i++) {
        dist[i] = pred[i] = -1;
    }
    if (origem < 0 || origem >= g->n || !iniciar_fila(&fila, g->n)) {
        return 0;
    }

    dist[origem] = 0;
    enfileirar(&fila, origem);
    while (fila.tamanho > 0) {
        int u = desenfileirar(&fila);
        const No *vizinho;
        for (vizinho = g->adj[u]; vizinho != NULL; vizinho = vizinho->prox) {
            int v = vizinho->destino;
            if (dist[v] == -1) {
                dist[v] = dist[u] + 1;
                pred[v] = u;
                enfileirar(&fila, v);
            }
        }
    }
    free(fila.dados);
    return 1;
}

/* Uma unica floresta BFS permite contar componentes e testar as cores.
 * A busca reinicia em cada vertice ainda sem cor, inclusive isolados.
 */
static int analisar_componentes(GrafoLista *g, int testar_biparticao)
{
    Fila fila;
    int *cor;
    int inicio;
    int componentes = 0;

    if (!grafo_valido(g)) {
        return -1;
    }
    if (g->n == 0) {
        return testar_biparticao ? 1 : 0;
    }
    cor = malloc((size_t)g->n * sizeof(*cor));
    if (cor == NULL) {
        return -1;
    }
    if (!iniciar_fila(&fila, g->n)) {
        free(cor);
        return -1;
    }
    for (inicio = 0; inicio < g->n; inicio++) {
        cor[inicio] = -1;
    }
    for (inicio = 0; inicio < g->n; inicio++) {
        if (cor[inicio] != -1) {
            continue;
        }
        componentes++;
        cor[inicio] = 0;
        enfileirar(&fila, inicio);
        while (fila.tamanho > 0) {
            int u = desenfileirar(&fila);
            const No *vizinho;
            for (vizinho = g->adj[u]; vizinho != NULL; vizinho = vizinho->prox) {
                int v = vizinho->destino;
                if (cor[v] == -1) {
                    cor[v] = 1 - cor[u];
                    enfileirar(&fila, v);
                } else if (testar_biparticao && cor[v] == cor[u]) {
                    free(cor);
                    free(fila.dados);
                    return 0;
                }
            }
        }
    }
    free(cor);
    free(fila.dados);
    return testar_biparticao ? 1 : componentes;
}

int eh_bipartido(GrafoLista *g)
{
    return analisar_componentes(g, 1);
}

int contar_componentes(GrafoLista *g)
{
    return analisar_componentes(g, 0);
}
