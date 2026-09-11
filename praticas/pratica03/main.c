#include "dag.h"

#include <stdio.h>
#include <stdlib.h>

#define MAX_VERTICES_TESTE 8

/* Aceita qualquer ordenacao correta: DAGs podem ter varias respostas. */
static int ordem_valida(GrafoLista *g, const int *ordem, int tamanho)
{
    int posicao[MAX_VERTICES_TESTE];
    int u;
    if (ordem == NULL || tamanho != g->n || g->n > MAX_VERTICES_TESTE) {
        return 0;
    }
    for (u = 0; u < g->n; u++) {
        posicao[u] = -1;
    }
    for (u = 0; u < tamanho; u++) {
        int v = ordem[u];
        if (v < 0 || v >= g->n || posicao[v] != -1) {
            return 0;
        }
        posicao[v] = u;
    }
    for (u = 0; u < g->n; u++) {
        const No *no;
        for (no = g->adj[u]; no != NULL; no = no->prox) {
            if (posicao[u] >= posicao[no->destino]) {
                return 0;
            }
        }
    }
    return 1;
}

static void imprimir_ordem(const char *nome, const int *ordem, int tamanho)
{
    int i;
    printf("%s:", nome);
    if (ordem == NULL) {
        puts(" ordenacao impossivel (ciclo)");
        return;
    }
    if (tamanho == 0) {
        printf(" (vazia)");
    }
    for (i = 0; i < tamanho; i++) {
        printf(" %d", ordem[i]);
    }
    putchar('\n');
}

static int verificar(GrafoLista *g, int esperado, int exibir)
{
    int tamanho_kahn = -1, tamanho_dfs = -1;
    int *kahn = ordenacao_topologica_kahn(g, &tamanho_kahn);
    int *dfs = ordenacao_topologica_dfs(g, &tamanho_dfs);
    int resultado = eh_dag(g) == esperado;

    if (esperado == 1) {
        resultado = resultado && ordem_valida(g, kahn, tamanho_kahn) &&
                    ordem_valida(g, dfs, tamanho_dfs);
    } else {
        resultado = resultado && kahn == NULL && dfs == NULL &&
                    tamanho_kahn == 0 && tamanho_dfs == 0;
    }
    if (exibir) {
        imprimir_ordem("Kahn", kahn, tamanho_kahn);
        imprimir_ordem("DFS ", dfs, tamanho_dfs);
    }
    free(kahn);
    free(dfs);
    return resultado;
}

static int testar_exemplo(const char *nome, int n, const int arcos[][2],
                          int quantidade, int esperado)
{
    GrafoLista *g = criar_digrafo(n);
    int i, resultado;
    if (g == NULL) {
        return 0;
    }
    for (i = 0; i < quantidade; i++) {
        if (!inserir_arco(g, arcos[i][0], arcos[i][1])) {
            liberar_digrafo(g);
            return 0;
        }
    }
    printf("\n=== %s ===\n", nome);
    resultado = verificar(g, esperado, 1);
    liberar_digrafo(g);
    return resultado;
}

/* Oraculo independente: experimenta todas as permutacoes de 3 vertices.
 * Um grafo e DAG se alguma permutacao respeitar todos os seus arcos.
 */
static int existe_ordem(unsigned int mascara)
{
    const int permutacoes[6][3] = {
        {0, 1, 2}, {0, 2, 1}, {1, 0, 2},
        {1, 2, 0}, {2, 0, 1}, {2, 1, 0}
    };
    int p;
    for (p = 0; p < 6; p++) {
        int posicao[3], i, u, v;
        int valida = 1;
        for (i = 0; i < 3; i++) {
            posicao[permutacoes[p][i]] = i;
        }
        for (u = 0; u < 3; u++) {
            for (v = 0; v < 3; v++) {
                if ((mascara & (1u << (3 * u + v))) != 0 &&
                    posicao[u] >= posicao[v]) {
                    valida = 0;
                }
            }
        }
        if (valida) {
            return 1;
        }
    }
    return 0;
}

static int testar_todos_digrafos_pequenos(void)
{
    unsigned int mascara;
    for (mascara = 0; mascara < 512; mascara++) {
        GrafoLista *g = criar_digrafo(3);
        int u, v, resultado;
        if (g == NULL) {
            return 0;
        }
        for (u = 0; u < 3; u++) {
            for (v = 0; v < 3; v++) {
                if ((mascara & (1u << (3 * u + v))) != 0 &&
                    !inserir_arco(g, u, v)) {
                    liberar_digrafo(g);
                    return 0;
                }
            }
        }
        resultado = verificar(g, existe_ordem(mascara), 0);
        liberar_digrafo(g);
        if (!resultado) {
            fprintf(stderr, "Falha no digrafo de mascara %u.\n", mascara);
            return 0;
        }
    }
    puts("\n512 digrafos de 3 vertices verificados (incluindo lacos).");
    return 1;
}

static int testar_argumentos(void)
{
    GrafoLista sem_lista = {1, NULL};
    GrafoLista negativo = {-1, NULL};
    No no_invalido = {1, NULL};
    No *adj[] = {&no_invalido};
    GrafoLista destino_invalido = {1, adj};
    GrafoLista *g = criar_digrafo(2);
    GrafoLista *criacao_invalida = criar_digrafo(-1);
    int *kahn, *dfs;
    int resultado;
    if (g == NULL) {
        liberar_digrafo(criacao_invalida);
        return 0;
    }
    kahn = ordenacao_topologica_kahn(g, NULL);
    dfs = ordenacao_topologica_dfs(g, NULL);
    resultado = criacao_invalida == NULL && kahn == NULL && dfs == NULL &&
                verificar(NULL, -1, 0) && verificar(&sem_lista, -1, 0) &&
                verificar(&negativo, -1, 0) &&
                verificar(&destino_invalido, -1, 0) &&
                !inserir_arco(NULL, 0, 1) && !inserir_arco(g, -1, 0) &&
                !inserir_arco(g, 0, 2) && inserir_arco(g, 0, 1) &&
                !inserir_arco(g, 0, 1) && g->adj[1] == NULL &&
                verificar(g, 1, 0);
    free(kahn);
    free(dfs);
    liberar_digrafo(criacao_invalida);
    liberar_digrafo(g);
    liberar_digrafo(NULL);
    return resultado;
}

int main(void)
{
    const int dependencias[][2] = {
        {0, 1}, {0, 2}, {1, 3}, {2, 3}, {3, 4}, {1, 4}
    };
    const int cadeia[][2] = {{3, 2}, {2, 1}, {1, 0}};
    const int desconexo[][2] = {{0, 1}, {2, 3}};
    const int ciclo[][2] = {{0, 1}, {1, 2}, {2, 0}};
    const int ciclo_separado[][2] = {{0, 1}, {2, 3}, {3, 2}};
    const int laco[][2] = {{0, 0}};

    if (!testar_exemplo("DAG de dependencias", 5, dependencias, 6, 1) ||
        !testar_exemplo("Cadeia em ordem inversa", 4, cadeia, 3, 1) ||
        !testar_exemplo("DAG desconexo e vertice isolado", 5, desconexo, 2, 1) ||
        !testar_exemplo("Vertices sem arcos", 3, NULL, 0, 1) ||
        !testar_exemplo("Vertice unico", 1, NULL, 0, 1) ||
        !testar_exemplo("Grafo vazio", 0, NULL, 0, 1) ||
        !testar_exemplo("Ciclo direcionado", 3, ciclo, 3, 0) ||
        !testar_exemplo("Ciclo em outro componente", 5, ciclo_separado, 3, 0) ||
        !testar_exemplo("Laco", 1, laco, 1, 0) ||
        !testar_todos_digrafos_pequenos() || !testar_argumentos()) {
        fputs("Falha nos testes de ordenacao topologica.\n", stderr);
        return EXIT_FAILURE;
    }
    puts("Todos os testes passaram.");
    return EXIT_SUCCESS;
}
