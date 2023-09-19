#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <math.h>
#include <stdlib.h>

typedef struct {
    int linha;
    int coluna;
    int **dados;
} Matrizes;

void lerESalvarMatriz(const char *nomeArquivo, int linha, int coluna, int matriz[][coluna]);
void zerarMatriz(int linha, int coluna, int matriz[][coluna]);
int salvarMatrizResultado(const char *nomeArquivo, char nomeMatriz, int linha, int coluna, int** matriz, double tempo);
int multiplicar(int cA, int cB, int linhaR, int colunaR, int matA[][cA], int matB[][cB]);
Matrizes *criarMatrizesEmStruct(int linhas, int colunas, int qntFilhos);
void zerarMatrizEmStruct(Matrizes *matriz);
void liberarMatrizEmStruct(Matrizes *matriz);

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

    int qntElementos = 0;
    printf("Digite a quantidade de elementos por processo: ");
    scanf("%d", &qntElementos);

    int A[lA][cA];
    int B[lB][cB];
    int R[lA][cB];

    zerarMatriz(lA, cB, R);
    lerESalvarMatriz("matrizA.txt", lA, cA, A);
    lerESalvarMatriz("matrizB.txt", lB, cB, B);

    fclose(arquivoMatrizA);
    fclose(arquivoMatrizB);

    double auxF = (double)(lA * cB) / qntElementos;
    int qntFilhos = (int)ceil(auxF);
    if (qntFilhos == 0)
    {
        qntFilhos++;
    }

    Matrizes *matrizes = criarMatrizesEmStruct(lA, cB, qntFilhos);

    int posicaoInicial[] = {0, 0};
    int posicaoFinal[] = {0, 0};
    for (int qntFilhosLaco = 0; qntFilhosLaco < qntFilhos; qntFilhosLaco++)
    {
        int aux = (posicaoInicial[0] * cB + posicaoInicial[1]) + qntElementos;
        if (aux > lA * cA)
        {
            aux = (lA * cB);
        }
        posicaoFinal[0] = aux / cB;
        posicaoFinal[1] = aux % cB;

        pid_t pid;
        pid = fork();
        if (pid < 0)
        {
            printf("Erro ao criar o filho");
        }
        else if (pid == 0)
        {
            clock_t inicio = clock();
            char nomeArquivo[255];
            sprintf(nomeArquivo, "./matrizResultadoProcesso %d .txt", qntFilhosLaco);
            for (int i = posicaoInicial[0]; i <= posicaoFinal[0] && i < lA; i++)
            {
                if (i < posicaoFinal[0])
                {
                    for (int j = posicaoInicial[1]; j < cB; j++)
                    {
                        matrizes[qntFilhosLaco].dados[i][j] = multiplicar(cA, cB, i, j, A, B);
                    }
                    posicaoInicial[1] = 0;
                }
                else if (i == posicaoFinal[0])
                {
                    for (int j = posicaoInicial[1]; j < posicaoFinal[1]; j++)
                    {
                        matrizes[qntFilhosLaco].dados[i][j] = multiplicar(cA, cB, i, j, A, B);
                    }
                }
            }
            clock_t fim = clock();
            double decorrido = (double)(fim - inicio) / CLOCKS_PER_SEC;
            salvarMatrizResultado(nomeArquivo, 'r', lA, cB, matrizes[qntFilhosLaco].dados, decorrido);
            _exit(0);
        }
        posicaoInicial[0] = posicaoFinal[0];
        posicaoInicial[1] = posicaoFinal[1];
    }
    for (int i = 0; i < qntFilhos; i++)
    {
        wait(NULL);
    }
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
    fclose(arquivo);
}

int salvarMatrizResultado(const char *nomeArquivo, char nomeMatriz, int linha, int coluna, int** matriz, double tempo)
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

int multiplicar(int cA, int cB, int linhaR, int colunaR, int matA[][cA], int matB[][cB])
{
    int resultado = 0;
    for (int i = 0; i < cA; i++)
    {
        resultado += matA[linhaR][i] * matB[i][colunaR];
    }
    return resultado;
}

Matrizes *criarMatrizesEmStruct(int linhas, int colunas, int qntFilhos)
{
    Matrizes *matrizes = (Matrizes *)malloc(sizeof(Matrizes) * qntFilhos);
    for (int i = 0; i < qntFilhos; i++)
    {
        matrizes[i].linha = linhas;
        matrizes[i].coluna = colunas;

        matrizes[i].dados = (int **)malloc(sizeof(int *) * linhas);
        for (int j = 0; j < linhas; j++)
        {
            matrizes[i].dados[j] = (int *)malloc(sizeof(int) * colunas);
        }

        zerarMatrizEmStruct(&matrizes[i]);
    }
    return matrizes;
}

void zerarMatrizEmStruct(Matrizes *matriz)
{
    for (int i = 0; i < matriz->linha; i++)
    {
        for (int j = 0; j < matriz->coluna; j++)
        {
            matriz->dados[i][j] = 0;
        }
    }
}

void liberarMatrizEmStruct(Matrizes *matriz)
{
    for (int i = 0; i < matriz->linha; i++)
    {
        free(matriz->dados[i]);
    }
    free(matriz->dados);
    free(matriz);
}