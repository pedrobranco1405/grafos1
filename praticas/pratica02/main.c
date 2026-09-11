#include "busca_largura.h"
#include "busca_profundidade.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_VERTICES 8
#define MAX_NOS (MAX_VERTICES * (MAX_VERTICES - 1))

/* Armazenamento local apenas para os exemplos: usa o mesmo GrafoLista
 * da pratica01, sem precisar vincular outra pratica na compilacao do CI.
 * Cada aresta nao direcionada ocupa dois nos, um em cada lista.
 */
typedef struct {
    GrafoLista grafo;
    No *adj[MAX_VERTICES];
    No nos[MAX_NOS];
} Exemplo;

static void montar_exemplo(Exemplo *exemplo, int n,
                           const int arestas[][2], int quantidade)
{
    int i;
    memset(exemplo, 0, sizeof(*exemplo));
    exemplo->grafo.n = n;
    exemplo->grafo.adj = exemplo->adj;
    for (i = 0; i < quantidade; i++) {
        int u = arestas[i][0];
        int v = arestas[i][1];
        No *ida = &exemplo->nos[2 * i];
        No *volta = &exemplo->nos[2 * i + 1];
        ida->destino = v;
        ida->prox = exemplo->adj[u];
        exemplo->adj[u] = ida;
        volta->destino = u;
        volta->prox = exemplo->adj[v];
        exemplo->adj[v] = volta;
    }
}

static int adjacentes(GrafoLista *g, int u, int v)
{
    const No *no;
    for (no = g->adj[u]; no != NULL; no = no->prox) {
        if (no->destino == v) {
            return 1;
        }
    }
    return 0;
}

static int verificar_buscas(GrafoLista *g, const int *dist_esperada)
{
    int dist[MAX_VERTICES], pred[MAX_VERTICES], visitado[MAX_VERTICES];
    int entrada[MAX_VERTICES], saida[MAX_VERTICES], pai[MAX_VERTICES];
    int tempos[2 * MAX_VERTICES + 1] = {0};
    int origem, u, v;

    if (!bfs(g, 0, dist, pred) || !dfs(g, entrada, saida, pai)) {
        return 0;
    }
    puts("Vertice | Distancia de 0 | Predecessor BFS | Entrada DFS | Saida DFS");
    for (u = 0; u < g->n; u++) {
        printf("%7d | %14d | %15d | %11d | %9d\n",
               u, dist[u], pred[u], entrada[u], saida[u]);
        if (dist[u] != dist_esperada[u] || entrada[u] < 1 ||
            entrada[u] >= saida[u] || saida[u] > 2 * g->n) {
            return 0;
        }
        if (tempos[entrada[u]] || tempos[saida[u]]) {
            return 0;
        }
        tempos[entrada[u]] = tempos[saida[u]] = 1;
        if (pai[u] != -1 &&
            (!adjacentes(g, pai[u], u) || entrada[pai[u]] >= entrada[u] ||
             saida[u] >= saida[pai[u]])) {
            return 0;
        }
        /* Intervalos DFS devem ser aninhados ou disjuntos. */
        for (v = 0; v < u; v++) {
            if ((entrada[u] < entrada[v] && entrada[v] < saida[u] &&
                 saida[u] < saida[v]) ||
                (entrada[v] < entrada[u] && entrada[u] < saida[v] &&
                 saida[v] < saida[u])) {
                return 0;
            }
        }
    }

    /* Todas as origens: alcance da DFS, caminhos e distancias minimas BFS. */
    for (origem = 0; origem < g->n; origem++) {
        if (!bfs(g, origem, dist, pred) ||
            !dfs_iterativa(g, origem, visitado) ||
            dist[origem] != 0 || pred[origem] != -1) {
            return 0;
        }
        for (u = 0; u < g->n; u++) {
            const No *no;
            if (visitado[u] != (dist[u] != -1)) {
                return 0;
            }
            if (dist[u] == -1) {
                if (pred[u] != -1) {
                    return 0;
                }
                continue;
            }
            if (u != origem &&
                (pred[u] < 0 || pred[u] >= g->n ||
                 !adjacentes(g, pred[u], u) || dist[pred[u]] + 1 != dist[u])) {
                return 0;
            }
            for (no = g->adj[u]; no != NULL; no = no->prox) {
                if (dist[no->destino] < 0 ||
                    abs(dist[u] - dist[no->destino]) > 1) {
                    return 0;
                }
            }
        }
    }
    return 1;
}

static int testar_exemplo(const char *nome, int n, const int arestas[][2],
                          int quantidade, const int *dist_esperada,
                          int componentes, int ciclo, int bipartido)
{
    Exemplo exemplo;
    int obt_componentes, obt_ciclo, obt_bipartido;
    montar_exemplo(&exemplo, n, arestas, quantidade);
    printf("\n=== %s ===\n", nome);
    obt_componentes = contar_componentes(&exemplo.grafo);
    obt_ciclo = tem_ciclo(&exemplo.grafo);
    obt_bipartido = eh_bipartido(&exemplo.grafo);
    printf("Componentes: %d | Ciclo: %d | Bipartido: %d\n",
           obt_componentes, obt_ciclo, obt_bipartido);
    return obt_componentes == componentes && obt_ciclo == ciclo &&
           obt_bipartido == bipartido &&
           verificar_buscas(&exemplo.grafo, dist_esperada);
}

static int testar_limites(void)
{
    GrafoLista vazio = {0, NULL};
    GrafoLista invalido = {1, NULL};
    Exemplo isolado;
    int dist[1], pred[1], visitado[1];
    montar_exemplo(&isolado, 1, NULL, 0);
    return contar_componentes(&vazio) == 0 && tem_ciclo(&vazio) == 0 &&
           eh_bipartido(&vazio) == 1 && dfs(&vazio, NULL, NULL, NULL) &&
           !bfs(&vazio, 0, dist, pred) &&
           contar_componentes(NULL) == -1 && tem_ciclo(NULL) == -1 &&
           eh_bipartido(NULL) == -1 && !bfs(NULL, 0, dist, pred) &&
           !dfs(NULL, dist, pred, visitado) &&
           !dfs_iterativa(NULL, 0, visitado) &&
           contar_componentes(&invalido) == -1 &&
           !bfs(&isolado.grafo, -1, dist, pred) &&
           !bfs(&isolado.grafo, 1, dist, pred) &&
           !bfs(&isolado.grafo, 0, NULL, pred) &&
           !bfs(&isolado.grafo, 0, dist, dist) &&
           !dfs_iterativa(&isolado.grafo, 1, visitado) &&
           !dfs(&isolado.grafo, NULL, pred, visitado);
}

int main(void)
{
    const int arvore[][2] = {{0, 1}, {0, 2}, {1, 3}, {1, 4}, {2, 5}};
    const int par[][2] = {{0, 1}, {1, 2}, {2, 3}, {3, 0}};
    const int impar[][2] = {{0, 1}, {1, 2}, {2, 0}};
    const int desconexo[][2] = {{0, 1}, {2, 3}, {3, 4}, {4, 2}, {5, 6}};
    const int dist_arvore[] = {0, 1, 1, 2, 2, 2};
    const int dist_par[] = {0, 1, 2, 1};
    const int dist_impar[] = {0, 1, 1};
    const int dist_desconexo[] = {0, 1, -1, -1, -1, -1, -1, -1};
    const int dist_isolados[] = {0, -1, -1};
    const int dist_unico[] = {0};

    if (!testar_exemplo("Arvore", 6, arvore, 5, dist_arvore, 1, 0, 1) ||
        !testar_exemplo("Ciclo par", 4, par, 4, dist_par, 1, 1, 1) ||
        !testar_exemplo("Ciclo impar", 3, impar, 3, dist_impar, 1, 1, 0) ||
        !testar_exemplo("Desconexo com ciclo fora da origem", 8, desconexo,
                        5, dist_desconexo, 4, 1, 0) ||
        !testar_exemplo("Vertices isolados", 3, NULL, 0, dist_isolados, 3, 0, 1) ||
        !testar_exemplo("Vertice unico", 1, NULL, 0, dist_unico, 1, 0, 1) ||
        !testar_limites()) {
        fputs("Falha na verificacao das buscas.\n", stderr);
        return EXIT_FAILURE;
    }
    puts("\nTodos os testes passaram.");
    return EXIT_SUCCESS;
}
