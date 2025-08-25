/*
 * Programa 2: produto_interno_concorrente.c
 * Lê um arquivo de entrada contendo N, dois vetores E o resultado de referência.
 * Calcula a versão concorrente e a compara com a referência lida do arquivo.
 */
#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h> 
#include <math.h> 
#include "timer.h" 

float *vetor1; 
float *vetor2; 

// Argumentos para as threads 
typedef struct {
    long int n;
    int nthreads;
    int id;
} t_args;

// Funcao executada pelas threads 
void *ProdutoInternoConcorrente(void *args) {
    t_args *arg = (t_args*) args;
    long int ini, fim, fatia;
    double *produto_local;

    produto_local = (double*) malloc(sizeof(double));
    if (produto_local == NULL) {
        fprintf(stderr, "ERRO: malloc na thread %d\n", arg->id);
        exit(1);
    }
    *produto_local = 0;

    fatia = arg->n / arg->nthreads;
    ini = arg->id * fatia;
    fim = (arg->id == arg->nthreads - 1) ? arg->n : ini + fatia;

    for (long int i = ini; i < fim; i++) {
        *produto_local += vetor1[i] * vetor2[i];
    }
    pthread_exit((void*) produto_local);
}


int main(int argc, char *argv[]) {
    long int n;
    int nthreads;
    FILE *arq;
    double produto_interno_referencia; // Valor lido do arquivo
    // Variável para armazenar o tempo sequencial lido do arquivo
    double tempo_sequencial_referencia;
    double produto_interno_concorrente = 0; // Valor calculado pelas threads
    double *retorno_thread;
    double inicio, fim, tempo_concorrente;
    // Variável para a diferença de tempo
    double diferenca_tempo;
    pthread_t *tid_sistema;
    t_args *args;

    if (argc < 3) {
        printf("Use: %s <arquivo de entrada> <numero de threads>\n", argv[0]);
        return 1;
    }
    nthreads = atoi(argv[2]);

    arq = fopen(argv[1], "rb");
    if (!arq) {
        fprintf(stderr, "ERRO: Nao foi possivel abrir o arquivo '%s'\n", argv[1]);
        return 2;
    }

    // Lê N e os dois vetores
    fread(&n, sizeof(long int), 1, arq);
    vetor1 = (float*) malloc(sizeof(float) * n);
    vetor2 = (float*) malloc(sizeof(float) * n);
    if (!vetor1 || !vetor2) {
        fprintf(stderr, "ERRO: Falha ao alocar memoria para os vetores.\n");
        return 4;
    }
    fread(vetor1, sizeof(float), n, arq);
    fread(vetor2, sizeof(float), n, arq);
    
    // Lê o produto interno sequencial (penúltimo valor do arquivo)
    fread(&produto_interno_referencia, sizeof(double), 1, arq);
    // Lê o tempo de execução sequencial (último valor do arquivo)
    fread(&tempo_sequencial_referencia, sizeof(double), 1, arq);
    
    fclose(arq);

    if (nthreads > n) nthreads = n;

    // Parte concorrente 
    GET_TIME(inicio);

    tid_sistema = (pthread_t*) malloc(sizeof(pthread_t) * nthreads);
    args = (t_args*) malloc(sizeof(t_args) * nthreads);
    if (!tid_sistema || !args) {
        fprintf(stderr, "ERRO: Falha ao alocar memoria para estruturas de threads.\n");
        return 7;
    }

    for (int i = 0; i < nthreads; i++) {
        args[i].id = i;
        args[i].n = n;
        args[i].nthreads = nthreads;
        pthread_create(&tid_sistema[i], NULL, ProdutoInternoConcorrente, (void*) &args[i]);
    }

    for (int i = 0; i < nthreads; i++) {
        pthread_join(tid_sistema[i], (void**) &retorno_thread);
        produto_interno_concorrente += *retorno_thread;
        free(retorno_thread);
    }

    GET_TIME(fim);
    tempo_concorrente = fim - inicio;


    // Resultados 
    printf("\n =========Comparacao de resultados=========\n");
    printf("Resultado sequencial (lido do arquivo):   %lf\n", produto_interno_referencia);
    printf("Resultado concorrente (calculado):        %lf\n", produto_interno_concorrente);
    double variacao_relativa = fabs((produto_interno_referencia - produto_interno_concorrente) / (produto_interno_referencia));
    printf("Variacao relativa entre os resultados:    %e\n", variacao_relativa);

    // Cálculo e exibição dos tempos e da diferença
    diferenca_tempo = tempo_sequencial_referencia - tempo_concorrente;
    printf("\n=========Comparacao de tempos=========\n");
    printf("Tempo cequencial (lido do arquivo):       %lf segundos\n", tempo_sequencial_referencia);
    printf("Tempo concorrente (calculado):            %lf segundos\n", tempo_concorrente);
    printf("Diferenca (Sequencial - Concorrente):     %lf segundos\n\n", diferenca_tempo);

    
    
    // Liberação de memória
    free(vetor1);
    free(vetor2);
    free(tid_sistema);
    free(args);

    return 0;
}