#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void lerESalvarMatriz(const char *nomeArquivo, int linha, int coluna, int **matriz);
void zerarMatriz(int **matriz, int linhas, int colunas);
void multiplicarMatrizes(int lA, int cA, int cB, int **A, int **B, int **R);
int salvarMatrizResultado(const char *nomeArquivo, char nomeMatriz, int linha, int coluna, int **matriz, double tempo);
int **alocacaoDinamica(int linhas, int colunas);
void liberarMatriz(int **matriz, int linhas);

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Arquivos necessarios: <matrizA.txt> <matrizB.txt>\n");
        return 1;
    }

    FILE *arquivoMatrizA = fopen(argv[1], "r");
    FILE *arquivoMatrizB = fopen(argv[2], "r");

    if (arquivoMatrizA == NULL || arquivoMatrizB == NULL)
    {
        printf("Nao foi possivel abrir os arquivos.");
        return 2;
    }

    int lA, cA, lB, cB;

    fscanf(arquivoMatrizA, "%d %d", &lA, &cA);
    fscanf(arquivoMatrizB, "%d %d", &lB, &cB);

    if (cA != lB)
    {
        printf("Impossível multiplicar matrizes!\n");
        return 3;
    }

    int **A = alocacaoDinamica(lA, cA);
    int **B = alocacaoDinamica(lB, cB);
    int **R = alocacaoDinamica(lA, cB);

    zerarMatriz(R, lA, cB);
    zerarMatriz(A, lA, cA);
    zerarMatriz(B, lB, cB);

    lerESalvarMatriz("matrizA.txt", lA, cA, A);
    lerESalvarMatriz("matrizB.txt", lB, cB, B);

    clock_t inicio = clock();
    multiplicarMatrizes(lA, cA, cB, A, B, R);
    clock_t fim = clock();

    double decorrido = (double)(fim - inicio) / CLOCKS_PER_SEC;

    salvarMatrizResultado("matrizR.txt", 'r', lA, cB, R, decorrido);

    liberarMatriz(A, lA);
    liberarMatriz(B, lB);
    liberarMatriz(R, lA);

    return 0;
}

void lerESalvarMatriz(const char *nomeArquivo, int linha, int coluna, int **matriz)
{
    FILE *arquivo = fopen(nomeArquivo, "r");
    char linhaArq[50];
    fgets(linhaArq, 50, arquivo);
    for (int i = 0; i < linha && !feof(arquivo); i++)
    {
        for (int j = 0; j < coluna; j++)
        {
            int numeroLinha;
            fgets(linhaArq, 50, arquivo);
            sscanf(linhaArq, "%*s %d", &numeroLinha);
            matriz[i][j] = numeroLinha;
        }
    }

    fclose(arquivo);
}

void zerarMatriz(int **matriz, int linhas, int colunas)
{
    for (int i = 0; i < linhas; i++)
    {
        for (int j = 0; j < colunas; j++)
        {
            matriz[i][j] = 0;
        }
    }
}

void multiplicarMatrizes(int lA, int cA, int cB, int **A, int **B, int **R)
{
    for (int i = 0; i < lA; i++)
    {
        for (int j = 0; j < cB; j++)
        {
            for (int k = 0; k < cA; k++)
            {
                R[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int salvarMatrizResultado(const char *nomeArquivo, char nomeMatriz, int linha, int coluna, int **matriz, double tempo)
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
    fprintf(arquivo, "%f", tempo);

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