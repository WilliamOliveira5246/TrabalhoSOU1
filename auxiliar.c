#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void gerarMatriz(int linha, int coluna, int **matriz);
int salvarMatriz(const char *nomeArquivo, char nomeMatriz, int linha, int coluna, int **matriz);
int **alocacaoDinamica(int linhas, int colunas);
void liberarMatriz(int **matriz, int linhas);

int main()
{
    int lA, cA, lB, cB;
    scanf("%d %d %d %d", &lA, &cA, &lB, &cB);

    int **A = alocacaoDinamica(lA, cA);
    srand((unsigned int)time(NULL));
    gerarMatriz(lA, cA, A);
    salvarMatriz("matrizA.txt", 'a', lA, cA, A);

    int **B = alocacaoDinamica(lB, cB);
    sleep(1);
    srand((unsigned int)time(NULL));
    gerarMatriz(lB, cB, B);
    salvarMatriz("matrizB.txt", 'b', lB, cB, B);

    liberarMatriz(A, lA);
    liberarMatriz(B, lB);

    return 0;
}

void gerarMatriz(int linha, int coluna, int **matriz)
{
    for (int i = 0; i < linha; i++)
    {
        for (int j = 0; j < coluna; j++)
        {
            matriz[i][j] = rand() % 101;
        }
    }
}

int salvarMatriz(const char *nomeArquivo, char nomeMatriz, int linha, int coluna, int **matriz)
{
    FILE *arquivo = fopen(nomeArquivo, "w");
    if (arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo");
        return 0;
    }
    fprintf(arquivo, "%d %d\n", linha, coluna);
    for (int i = 0; i < linha; i++)
    {
        for (int j = 0; j < coluna; j++)
        {
            fprintf(arquivo, "%c%d%d %d\n", nomeMatriz, i + 1, j + 1, matriz[i][j]);
        }
    }

    fclose(arquivo);
    return 0;
}

int **alocacaoDinamica(int linhas, int colunas)
{
    int **matriz = (int **)malloc(linhas * sizeof(int *));
    for (int i = 0; i < linhas; i++)
    {
        matriz[i] = (int *)malloc(colunas * sizeof(int));
    }
    return matriz;
}

void liberarMatriz(int **matriz, int linhas)
{
    for (int i = 0; i < linhas; i++)
    {
        free(matriz[i]);
    }
    free(matriz);
}