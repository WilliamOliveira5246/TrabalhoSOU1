#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void gerarMatriz(int linha, int coluna, int matriz[][coluna]);
int salvarMatriz(const char *nomeArquivo, char nomeMatriz, int linha, int coluna, int matriz[][coluna]);

int main()
{
    int lA, cA, lB, cB;
    scanf("%d %d %d %d", &lA, &cA, &lB, &cB);
    
    int A[lA][cA];
    srand((unsigned int)time(NULL));
    gerarMatriz(lA, cA, A);
    salvarMatriz("matrizA.txt", 'a', lA, cA, A);

    int B[lB][cB];
    sleep(1);
    srand((unsigned int)time(NULL));
    gerarMatriz(lB, cB, B);
    salvarMatriz("matrizB.txt", 'b', lB, cB, B);

    return 0;
}

void gerarMatriz(int linha, int coluna, int matriz[][coluna])
{
    for (int i = 0; i < linha; i++)
    {
        for (int j = 0; j < coluna; j++)
        {
            matriz[i][j] = rand() % 101; //[0:100]
        }
    }
}

int salvarMatriz(const char *nomeArquivo, char nomeMatriz, int linha, int coluna, int matriz[][coluna]) 
{
    FILE *arquivo = fopen(nomeArquivo, "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo");
        return 0;
    }
    fprintf(arquivo, "%d %d\n", linha, coluna);
    for (int i = 0; i < linha; i++) {
        for (int j = 0; j < coluna; j++) {
            fprintf(arquivo, "%c%d%d %d\n", nomeMatriz, i+1, j+1, matriz[i][j]);
        }
    }

    fclose(arquivo);
    return 0;
}