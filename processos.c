#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>

void lerESalvarMatriz(const char *nomeArquivo, int linha, int coluna, int matriz[][coluna]);
void zerarMatriz(int linha, int coluna, int matriz[][coluna]);
int salvarMatrizResultado(const char *nomeArquivo, char nomeMatriz, int linha, int coluna, int matriz[][coluna], double tempo);
void multiplicarMatrizes(int qntElementos, int posicao[], int index, int lA, int cA, int cB, int a[][cA], int b[][cB], int r[][cB]);

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

    int qntFilhos = (lA * cB) / qntElementos;
    if (qntFilhos == 0)
    {
        qntFilhos++;
    }

    int seg_id = shmget(IPC_PRIVATE, 2 * sizeof(int *), IPC_CREAT | 0666);
    int *posicao = shmat(seg_id, NULL, 0);
    posicao[0] = 0;
    posicao[1] = 0;
    for (int i = 0; i < qntFilhos; i++)
    {
        __pid_t pid;
        pid = fork();
        // multiplicarMatrizes(qntElementos, posicao, i, lA, cA, cB, A, B, R);
        if (pid < 0)
        {
            printf("Erro ao criar o filho");
        }
        else if (pid == 0)
        {
            multiplicarMatrizes(qntElementos, posicao, i, lA, cA, cB, A, B, R);
            return 0;
        }
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

void multiplicarMatrizes(int qntElementos, int posicao[], int index, int lA, int cA, int cB, int a[][cA], int b[][cB], int r[][cB])
{
    clock_t inicio = clock();
    int controleQntElementos = 0;
    char nomeArquivo[1000];
    sprintf(nomeArquivo, "./matrizesProcessos/matrizResultadoProcesso %d .txt", index);
    int i = posicao[0];
    while (i < lA && controleQntElementos < qntElementos)
    {
        if (posicao[1] == cB)
        {
            posicao[1] = 0;
            posicao[0]++;
            i++;
        }
        for (int j = posicao[1]; j < cB && controleQntElementos < qntElementos; j++)
        {
            int numero = 0;
            for (int k = 0; k < cA; k++)
            {
                r[i][j] += a[i][k] * b[k][j];
            }
            controleQntElementos++;
            posicao[1]++;
        }
    }
    clock_t fim = clock();
    double decorrido = (double)(fim - inicio) / CLOCKS_PER_SEC;
    salvarMatrizResultado(nomeArquivo, 'r', lA, cB, r, decorrido);
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