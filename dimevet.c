
//Programa para gerar dois vetores de floats com base em uma dimensão N.


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
// Adicionado para usar o mesmo mecanismo de tempo do programa concorrente
#include "timer.h" 

#define MAX 100 // Valor máximo para os elementos do vetor
// Descomentar a linha abaixo para imprimir os vetores gerados no console
#define LOG 

int main(int argc, char* argv[]) {
    float *vetor1, *vetor2; 
    long int n; // Qtde de elementos dos vetores
    int fator = 1; // Fator para alternar o sinal
    double produto_interno = 0; // Variável para armazenar o resultado
    FILE * descritorArquivo; // Descritor do arquivo de saida
    size_t ret; // Retorno da funcao de escrita no arquivo de saida
    
    // Variáveis para tomada de tempo, igual ao programa concorrente
    double inicio, fim, elapsed_time;

    // As variáveis 'struct timespec' foram removidas
    // struct timespec start_time, end_time;

    // Recebe os argumentos de entrada
    if(argc < 3) {
        fprintf(stderr, "Use: %s <dimensao> <nome arquivo saida>\n", argv[0]);
        return 1;
    }
    n = atol(argv[1]);

    // Aloca memória para os dois vetores
    vetor1 = (float*) malloc(sizeof(float) * n);
    if(!vetor1) {
        fprintf(stderr, "Erro de alocacao da memoria do vetor 1\n");
        return 2;
    }
    vetor2 = (float*) malloc(sizeof(float) * n);
    if(!vetor2) {
        fprintf(stderr, "Erro de alocacao da memoria do vetor 2\n");
        free(vetor1);
        return 2;
    }

    // Preenche os vetores com valores float aleatorios com sinais alternados
    srand(time(NULL));
    for(long int i=0; i<n; i++) {
    
        vetor1[i] = (rand() % MAX) / 3.0 * fator;
        vetor2[i] = (rand() % MAX) / 3.0 * fator;
        
        // Alterna o fator para o próximo elemento
        fator *= -1; 
    }
    
    // Inicia a tomada de tempo usando GET_TIME
    GET_TIME(inicio);
    
    // Calcular o produto interno
    for(long int i=0; i<n; i++) {
        produto_interno += vetor1[i] * vetor2[i];
    }
    
    // Finaliza a tomada de tempo usando GET_TIME
    GET_TIME(fim);

    // Calcula o tempo decorrido com a subtração simples
    elapsed_time = fim - inicio;
    
    /* Imprimir na saida padrao os vetores gerados para verificação no formato solicitado
    #ifdef LOG
    // Imprime a dimensão
    fprintf(stdout, "%ld\n", n);
    
    // Imprime o primeiro vetor
    for(long int i=0; i<n; i++) {
        fprintf(stdout, "%.2f ", vetor1[i]);
    }
    fprintf(stdout, "\n");
    
    // Imprime o segundo vetor
    for(long int i=0; i<n; i++) {
        fprintf(stdout, "%.2f ", vetor2[i]);
    }
    fprintf(stdout, "\n");

    // Imprime o resultado do produto interno para verificação
    fprintf(stdout, "Produto Interno Calculado: %lf\n", produto_interno);
    // Imprime o tempo de execução para verificação
    fprintf(stdout, "Tempo de Execucao (Produto Interno): %lf segundos\n", elapsed_time);
    #endif
    */

    // Abre o arquivo para escrita binaria
    descritorArquivo = fopen(argv[2], "wb");
    if(!descritorArquivo) {
        fprintf(stderr, "Erro de abertura do arquivo\n");
        return 3;
    }

    // Escreve a dimensao N no arquivo binário
    ret = fwrite(&n, sizeof(long int), 1, descritorArquivo);
    
    // Escreve os elementos do primeiro vetor no arquivo binário
    ret = fwrite(vetor1, sizeof(float), n, descritorArquivo);
    if(ret < n) {
        fprintf(stderr, "Erro de escrita do vetor 1 no arquivo\n");
        return 4;
    }

    // Escreve os elementos do segundo vetor no arquivo binário
    ret = fwrite(vetor2, sizeof(float), n, descritorArquivo);
    if(ret < n) {
        fprintf(stderr, "Erro de escrita do vetor 2 no arquivo\n");
        return 4;
    }

    // Escreve o resultado do produto interno no arquivo binário
    ret = fwrite(&produto_interno, sizeof(double), 1, descritorArquivo);
    if (ret < 1) {
        fprintf(stderr, "Erro de escrita do produto interno no arquivo\n");
        return 5;
    }

    // Escreve o tempo de execução na última linha do arquivo binário
    ret = fwrite(&elapsed_time, sizeof(double), 1, descritorArquivo);
    if (ret < 1) {
        fprintf(stderr, "Erro de escrita do tempo de execucao no arquivo\n");
        return 5;
    }

    // Finaliza o uso das variaveis
    fclose(descritorArquivo);
    free(vetor1);
    free(vetor2);
    return 0;
}