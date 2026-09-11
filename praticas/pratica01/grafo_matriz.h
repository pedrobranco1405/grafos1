#ifndef GRAFO_MATRIZ_H
#define GRAFO_MATRIZ_H

typedef struct {
    int n;
    int **adj;
} GrafoMatriz;

GrafoMatriz *criar_grafo_matriz(int n);
int inserir_aresta_matriz(GrafoMatriz *grafo, int origem, int destino);
int remover_aresta_matriz(GrafoMatriz *grafo, int origem, int destino);
int grau_matriz(const GrafoMatriz *grafo, int vertice);
int sao_adjacentes_matriz(const GrafoMatriz *grafo, int origem, int destino);
void liberar_grafo_matriz(GrafoMatriz *grafo);

#endif
