#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void lerESalvarMatriz(const char *nomeArquivo, int linha, int coluna, int matriz[][coluna]);
void zerarMatriz(int linha, int coluna, int matriz[][coluna]);
void multiplicarMatrizes(int lA, int cA, int cB, int A[][cA], int B[][cB], int (*R)[cB]);
int salvarMatrizResultado(const char *nomeArquivo, char nomeMatriz, int linha, int coluna, int matriz[][coluna], double tempo);

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

    int A[lA][cA];
    int B[lB][cB];
    int R[lA][cB];

    zerarMatriz(lA, cB, R);
    lerESalvarMatriz("matrizA.txt", lA, cA, A);
    lerESalvarMatriz("matrizB.txt", lB, cB, B);

    clock_t inicio = clock();
    multiplicarMatrizes(lA, cA, cB, A, B, R);
    clock_t fim = clock();

    double decorrido = (double)(fim - inicio) / CLOCKS_PER_SEC;

    salvarMatrizResultado("matrizR.txt", 'r', lA, cB, R, decorrido);

    return 0;
}

void lerESalvarMatriz(const char *nomeArquivo, int linha, int coluna, int matriz[][coluna])
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
    /*for (int i = 0; i < linha; i++)
    {
        for (int j = 0; j < coluna; j++)
        {
            printf("%d ", matriz[i][j]);
        }
        printf("\n");
    }*/

    fclose(arquivo);
}

void zerarMatriz(int linha, int coluna, int matriz[][coluna])
{
    for (int i = 0; i < linha; i++)
    {
        for (int j = 0; j < coluna; j++)
        {
            matriz[i][j] = 0;
        }
    }
}

void multiplicarMatrizes(int lA, int cA, int cB, int A[][cA], int B[][cB], int (*R)[cB])
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

int salvarMatrizResultado(const char *nomeArquivo, char nomeMatriz, int linha, int coluna, int matriz[][coluna], double tempo)
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