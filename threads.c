#include <pthread.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

struct Parametros
{
    int linhasA;
    int colunasA;
    int linhasB;
    int colunasB;
    int **matrizA;
    int **matrizB;
    int **matrizR;
    int filho;
    int linhaInicial;
    int colunaInicial;
    int linhaFinal;
    int colunaFinal;
};

int **alocacaoDinamica(int linhas, int colunas);
void zerarMatriz(int **matriz, int linhas, int colunas);
void liberarMatriz(int **matriz, int linhas);
void *threadFuncao(void *arg);
int salvarMatrizResultado(const char *nomeArquivo, char nomeMatriz, int linha, int coluna, int **matriz, double tempo);
int multiplicar(int colunasA, int colunasB, int linhasR, int colunasR, int **matrizA, int **matrizB);
void lerESalvarMatriz(const char *nomeArquivo, int linhas, int colunas, int **matriz);

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
    int linhasA, colunasA, linhasB, colunasB;

    fscanf(arquivoMatrizA, "%d %d", &linhasA, &colunasA);
    fscanf(arquivoMatrizB, "%d %d", &linhasB, &colunasB);

    if (colunasA != linhasB)
    {
        printf("Impossível multiplicar matrizes!\n");
        return 3;
    }

    float p = (float)linhasA * colunasB / 8;
    int qntElementos = (int)ceil(p);
    int qntFilhos = (int)linhasA * colunasB / p;

    int **matrizA = alocacaoDinamica(linhasA, colunasA);
    int **matrizB = alocacaoDinamica(linhasB, colunasB);
    int **matrizR = alocacaoDinamica(linhasA, colunasB);

    zerarMatriz(matrizA, linhasA, colunasA);
    zerarMatriz(matrizB, linhasB, colunasB);
    zerarMatriz(matrizR, linhasA, colunasB);

    lerESalvarMatriz("matrizA.txt", linhasA, colunasA, matrizA);
    lerESalvarMatriz("matrizB.txt", linhasB, colunasB, matrizB);

    int posicaoInicial[] = {0, 0};
    int posicaoFinal[] = {0, 0};

    pthread_t tid[qntFilhos];
    void *thread_return[qntFilhos];

    for (int i = 0; i < qntFilhos; i++)
    {
        int auxiliar = (int)(posicaoInicial[0] * colunasB + posicaoInicial[1]) + qntElementos;
        if (auxiliar > linhasA * colunasA)
        {
            auxiliar = (linhasA * colunasB);
        }
        posicaoFinal[0] = auxiliar / colunasB;
        posicaoFinal[1] = auxiliar % colunasB;

        printf("--%d %d %d--\n", posicaoInicial[0], posicaoInicial[1], i);
        struct Parametros parametros = {linhasA, colunasA, linhasB, colunasB, matrizA, matrizB, matrizR, i, posicaoInicial[0], posicaoInicial[1], posicaoFinal[0], posicaoFinal[1]};

        int status = pthread_create(&tid[i], NULL, threadFuncao, (void *)&parametros);
        if (status != 0)
        {
            printf("Erro na criação da thread. Código do erro: %d\n", status);
            return 1;
        }
        pthread_join(tid[i], &thread_return[i]);

        posicaoInicial[0] = posicaoFinal[0];
        posicaoInicial[1] = posicaoFinal[1];
    }

    for (int i = 0; i < qntFilhos; i++)
    {
        pthread_join(tid[i], &thread_return[i]);
    }

    printf("\nMatriz\n");
    for (int i = 0; i < linhasA; i++)
    {
        for (int j = 0; j < colunasB; j++)
        {
            printf("%d ", matrizR[i][j]);
        }
        printf("--\n");
    }

        liberarMatriz(matrizA, linhasA);
    liberarMatriz(matrizB, linhasB);
    liberarMatriz(matrizR, linhasA);
    return 0;
}

void *threadFuncao(void *arg)
{
    clock_t inicio = clock();
    struct Parametros *parametros = (struct Parametros *)arg;
    char nomeArquivo[255];
    sprintf(nomeArquivo, "./matrizResultadoThread %d .txt", parametros->filho);
    printf("/%d %d %d\n", parametros->linhaInicial, parametros->colunaInicial, parametros->filho);
    for (int i = parametros->linhaInicial; i <= parametros->linhaFinal && i < parametros->linhasA; i++)
    {
        if (i < parametros->linhaFinal)
        {
            for (int j = parametros->colunaInicial; j < parametros->colunasB; j++)
            {
                parametros->matrizR[i][j] = multiplicar(parametros->colunasA, parametros->colunasB, i, j, parametros->matrizA, parametros->matrizB);
            }
            parametros->colunaInicial = 0;
        }
        else if (i == parametros->linhaFinal)
        {
            for (int j = parametros->colunaInicial; j < parametros->colunaFinal; j++)
            {
                parametros->matrizR[i][j] = multiplicar(parametros->colunasA, parametros->colunasB, i, j, parametros->matrizA, parametros->matrizB);
            }
        }
    }
    clock_t fim = clock();
    double decorrido = (double)(fim - inicio) / CLOCKS_PER_SEC;
    salvarMatrizResultado(nomeArquivo, 'r', parametros->linhasA, parametros->linhasB, parametros->matrizR, decorrido);
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

void liberarMatriz(int **matriz, int linhas)
{
    for (int i = 0; i < linhas; i++)
    {
        free(matriz[i]);
    }
    free(matriz);
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

int multiplicar(int colunasA, int colunasB, int linhasR, int colunasR, int **matrizA, int **matrizB)
{
    int resultado = 0;
    for (int i = 0; i < colunasA; i++)
    {
        resultado += matrizA[linhasR][i] * matrizB[i][colunasR];
    }
    return resultado;
}

void lerESalvarMatriz(const char *nomeArquivo, int linhas, int colunas, int **matriz)
{
    FILE *arquivo = fopen(nomeArquivo, "r");
    char linhaArq[50];
    fgets(linhaArq, 50, arquivo);
    for (int i = 0; i < linhas && !feof(arquivo); i++)
    {
        for (int j = 0; j < colunas; j++)
        {
            int numeroLinha;
            fgets(linhaArq, 50, arquivo);
            sscanf(linhaArq, "%*s %d", &numeroLinha);
            matriz[i][j] = numeroLinha;
        }
    }
    fclose(arquivo);
}