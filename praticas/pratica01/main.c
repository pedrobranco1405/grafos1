#include "grafo_lista.h"
#include "grafo_matriz.h"

#include <stdio.h>
#include <stdlib.h>

static int testar_matriz(void)
{
    GrafoMatriz *grafo = criar_grafo_matriz(5);

    if (grafo == NULL) {
        return 0;
    }

    inserir_aresta_matriz(grafo, 0, 1);
    inserir_aresta_matriz(grafo, 0, 2);
    inserir_aresta_matriz(grafo, 1, 3);

    printf("Matriz: grau(0) = %d\n", grau_matriz(grafo, 0));
    printf("Matriz: 0 e 2 sao adjacentes? %s\n",
           sao_adjacentes_matriz(grafo, 0, 2) ? "sim" : "nao");

    remover_aresta_matriz(grafo, 0, 2);
    printf("Matriz: apos remover (0, 2), sao adjacentes? %s\n",
           sao_adjacentes_matriz(grafo, 0, 2) ? "sim" : "nao");

    liberar_grafo_matriz(grafo);
    return 1;
}

static int testar_lista(void)
{
    GrafoLista *grafo = criar_grafo_lista(5);

    if (grafo == NULL) {
        return 0;
    }

    inserir_aresta_lista(grafo, 0, 1);
    inserir_aresta_lista(grafo, 0, 2);
    inserir_aresta_lista(grafo, 1, 3);

    printf("Lista: grau(0) = %d\n", grau_lista(grafo, 0));
    printf("Lista: 0 e 2 sao adjacentes? %s\n",
           sao_adjacentes_lista(grafo, 0, 2) ? "sim" : "nao");

    remover_aresta_lista(grafo, 0, 2);
    printf("Lista: apos remover (0, 2), sao adjacentes? %s\n",
           sao_adjacentes_lista(grafo, 0, 2) ? "sim" : "nao");

    liberar_grafo_lista(grafo);
    return 1;
}

int main(void)
{
    if (!testar_matriz() || !testar_lista()) {
        fprintf(stderr, "Erro: nao foi possivel alocar memoria para os grafos.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
