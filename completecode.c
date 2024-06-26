#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <ctype.h>
#include "jsmn.h"

#define DESC 300

typedef struct Tarefa { // Estrutura de dados para armazenar informações de uma tarefa
    int id;
    char descricao[DESC];
    int tipo;
    int prioridade; // 1: alto, 0: baixo
    struct tm dataCriacao;
    struct tm dataConclusao; 
    int estado; // 0: em espera, 1: em execução, 2: concluída, 3: falha
    char *payloadJSON;
} Tarefa;

typedef struct Node {   // Estrutura de dados para armazenar um nó da fila de prioridade
    Tarefa* dados;
    struct Node* next;
} Node;

typedef struct PriorityQueue {  // Estrutura de dados para armazenar a fila de prioridade
    Node* front;
} PriorityQueue;

typedef struct StackNode {  // Estrutura de dados para armazenar um nó da pilha
    Tarefa* dados;
    struct StackNode* next;
} StackNode;

typedef struct Stack {  // Estrutura de dados para armazenar a pilha
    StackNode* top;
} Stack;

// Declaração de funções
static int jsoneq(const char *json, jsmntok_t *tok, const char *s);
void imprimirPayloadJSON(const char *payloadJSON, int tipo);
void gerarRelatorio(PriorityQueue *q, Tarefa tarefasRealizadas[], int nRealizadas);
void buscarTarefaPorID(PriorityQueue *q, Tarefa tarefasRealizadas[], int nRealizadas, int id);
void listarTarefas(PriorityQueue *q, Tarefa tarefasRealizadas[], int nRealizadas);
void imprimirDataHora(struct tm *data);
int menu();
void criarTarefa(PriorityQueue *q, Tarefa tarefasRealizadas[], int nRealizadas, Tarefa *tarefa);
void destruirTarefa(Tarefa *tarefa);
void limparBuffer();
PriorityQueue* createQueue();
void enqueue(PriorityQueue* q, Tarefa* tarefa);
Tarefa* dequeue(PriorityQueue* q);
void freeQueue(PriorityQueue* q);
void push(Stack* stack, Tarefa* tarefa);
Tarefa* pop(Stack* stack);
void freeStack(Stack* stack);
void pushLowPriorityTasks(Stack* stack, Tarefa* tarefa);
void salvarTarefasEmFicheiro(Tarefa *tarefasRealizadas, int nRealizadas);
void carregarTarefasDoFicheiro(PriorityQueue *q, Stack *lowPriorityStack);
int verificarIDusado(PriorityQueue *q, Tarefa tarefasRealizadas[], int nRealizadas, int id); 
void swap(Tarefa *a, Tarefa *b);
int partition(Tarefa tarefas[], int low, int high);
void quickSort(Tarefa tarefas[], int low, int high);
void executarTarefa(Tarefa tarefasRealizadas[], int nRealizadas, PriorityQueue *q);
int allDigits(const char *str);


int allDigits(const char *str) {
    while (*str) {
        if (!isdigit(*str))
            return 0;
        str++;
    }
    return 1;
}

void criarTarefa(PriorityQueue *q, Tarefa *tarefasRealizadas, int nRealizadas, Tarefa *tarefa) {
    int numeroCop;
    char nome[100], novonome[100], caminho[100], cor[15], NomImpress[100];
    char payload[1000] = "";
    char idInput[10];

    if (!tarefa) {  // verificar se a tarefa foi alocada corretamente
        printf("Erro ao alocar memória para a tarefa!\n");  // Exibir mensagem de erro
        return;  // Retornar ao chamador
    }

     int idValido = 0;   // Variável para verificar se o ID é válido
do {
    printf("Digite o ID da tarefa (5 dígitos): ");
    fgets(idInput, sizeof(idInput), stdin);
    idInput[strcspn(idInput, "\n")] = '\0'; 

    // verificar se o ID é válido
    if (!allDigits(idInput) || strlen(idInput) != 5) {
        printf("Erro: O ID deve ter 5 dígitos!\n");
    } else {
        tarefa->id = atoi(idInput); // converter a string para inteiro
        if (verificarIDusado(q, tarefasRealizadas, nRealizadas, tarefa->id)) {
            printf("Erro: O ID já foi utilizado! Por favor, insira um ID diferente.\n");
        } else {
            idValido = 1;
        }
    }
} while (!idValido);

    printf("Digite a descrição da tarefa: ");   // Solicitar a descrição da tarefa ao usuário
    fgets(tarefa->descricao, DESC, stdin);  // Ler a descrição da tarefa
    tarefa->descricao[strcspn(tarefa->descricao, "\n")] = 0;  // Remover o caractere de nova linha da descrição

    printf("Digite a prioridade da tarefa (1 - alto, 0 - baixo): ");  // Solicitar a prioridade da tarefa ao usuário
    scanf("%d", &tarefa->prioridade);   // Ler a prioridade da tarefa
    limparBuffer();  // Limpar o buffer de entrada

    // Solicitar o tipo de tarefa ao usuário
    printf("Digite o tipo de tarefa:\n");
    printf("1-Copiar//colar\n");
    printf("2-Impressão\n");
    // printf("3-Registro\n");
    scanf("%d", &tarefa->tipo);

    strcat(payload, "{\n\t");
    switch(tarefa->tipo) {
        case 1:
            printf("Nome do documento: ");
            scanf("%s", nome);
            strcat(payload, "\"Nome\": \"");
            strcat(payload, nome);
            strcat(payload, "\",\n\t");

            printf("Nome do novo documento: ");
            scanf("%s", novonome);
            strcat(payload, "\"Novo.nome\": \"");
            strcat(payload, novonome);
            strcat(payload, "\",\n\t");

            printf("Local: ");
            scanf("%s", caminho);
            strcat(payload, "\"Local\": \"");
            strcat(payload, caminho);
            strcat(payload, "\",\n\t");
            break;

        case 2:
            printf("Número de cópias: ");
            scanf("%d", &numeroCop);
            strcat(payload, "\"Numero.de.copias\": ");
            char numeroCopStr[10];
            sprintf(numeroCopStr, "%d", numeroCop);
            strcat(payload, numeroCopStr);
            strcat(payload, ",\n\t");

            printf("Cor: ");
            scanf("%s", cor);
            strcat(payload, "\"Cor\": \"");
            strcat(payload, cor);
            strcat(payload, "\",\n\t");

            printf("Nome impressora: ");
            scanf("%s", NomImpress);
            strcat(payload, "\"Nome.impressora\": \"");
            strcat(payload, NomImpress);
            strcat(payload, "\"\n\t");
            break;
            default:
            printf("Degite um número válido");
            break;
    }
    strcat(payload, "}");

   // printf("payload: %s\n", payload);
    tarefa->payloadJSON = (char*)malloc(strlen(payload) + 1);
    strcpy(tarefa->payloadJSON, payload);   
    limparBuffer();

    // Obter a data e hora atuais
    time_t tempoAtual = time(NULL);  // Obter o tempo atual
    tarefa->dataCriacao = *localtime(&tempoAtual);   // Obter a data e hora local
    tarefa->dataConclusao = (struct tm){0};  // Inicializar a data de conclusão como 0

    tarefa->estado = 0;  // Definir o estado da tarefa como "em espera"

    printf("Tarefa criada com sucesso!\n");   // Exibir mensagem de sucesso
}


void destruirTarefa(Tarefa *tarefa) {   // Função para liberar a memória alocada para uma tarefa
    free(tarefa->payloadJSON);  // Liberar a memória alocada para o payload JSON
    free(tarefa);   // Liberar a memória alocada para a tarefa
}

void limparBuffer() {   // Função para limpar o buffer de entrada
    int c;  // Variável para armazenar o caractere lido
    while ((c = getchar()) != '\n' && c != EOF);     // Ler os caracteres do buffer até encontrar uma nova linha ou o final do arquivo
}

PriorityQueue* createQueue() {  // Função para criar uma nova fila de prioridade
    PriorityQueue* q = malloc(sizeof(PriorityQueue));       // Alocar memória para a fila de prioridade
    if (!q) {   // Verificar se a fila foi alocada corretamente
        printf("Erro ao alocar memória para a fila de prioridade!\n");  // Exibir mensagem de erro
        exit(EXIT_FAILURE); // Encerrar o programa com falha7
    }
    q->front = NULL;    // Inicializar o início da fila como nulo
    return q;   // Retornar a fila de prioridade criada
}

void enqueue(PriorityQueue* q, Tarefa* tarefa) {    // Função para adicionar uma tarefa à fila de prioridade
    Node* newNode = malloc(sizeof(Node));   // Alocar memória para um novo nó
    if (!newNode) {    // Verificar se o nó foi alocado corretamente
        printf("Erro ao alocar memória para o nó!\n");      // Exibir mensagem de erro
        exit(EXIT_FAILURE); // Encerrar o programa com falha
    }
    newNode->dados = tarefa;    // Atribuir a tarefa ao novo nó
    newNode->next = NULL;   // Definir o próximo nó como nulo

    if (q->front == NULL || q->front->dados->prioridade < tarefa->prioridade) { // Verificar se a fila está vazia ou a prioridade da tarefa é maior que a prioridade da primeira tarefa na fila
        newNode->next = q->front;   // Definir o próximo nó como o início da fila
        q->front = newNode; // Definir o novo nó como o início da fila
    } else {    // Se a prioridade da tarefa for menor ou igual à prioridade da primeira tarefa na fila
        Node* temp = q->front;  // Criar um nó temporário para percorrer a fila
        while (temp->next != NULL && temp->next->dados->prioridade >= tarefa->prioridade) { // Percorrer a fila até encontrar a posição correta para a nova tarefa
            temp = temp->next;  // Avançar para o próximo nó
        }
        newNode->next = temp->next; // Definir o próximo nó do novo nó como o próximo nó do nó temporário
        temp->next = newNode;   // Definir o novo nó como o próximo nó do nó temporário
    }
}

Tarefa* dequeue(PriorityQueue* q) { // Função para remover uma tarefa da fila de prioridade
    if (q->front == NULL) { // Verificar se a fila está vazia
        printf("FILA VAZIA!!!\n");  // Exibir mensagem de erro
        return NULL;    // Retornar nulo
    }
    Node* temp = q->front;  // Criar um nó temporário para armazenar o início da fila
    Tarefa* tarefa = temp->dados;   // Obter a tarefa do nó temporário
    q->front = q->front->next;  // Definir o próximo nó como o início da fila
    free(temp); // Liberar a memória alocada para o nó temporário
    return tarefa;  // Retornar a tarefa removida
}

void freeQueue(PriorityQueue* q) {  // Função para liberar a memória alocada para a fila de prioridade
    while (q->front != NULL) {  // Enquanto houver elementos na fila
        Node* temp = q->front;  // Criar um nó temporário para armazenar o início da fila
        q->front = q->front->next;  // Definir o próximo nó como o início da fila
        destruirTarefa(temp->dados);        // Liberar a memória alocada para a tarefa
        free(temp); // Liberar a memória alocada para o nó
    }
    free(q);    // Liberar a memória alocada para a fila de prioridade
}

void push(Stack* stack, Tarefa* tarefa) {   // Função para adicionar uma tarefa à pilha
    StackNode* newNode = malloc(sizeof(StackNode)); // Alocar memória para um novo nó
    if (!newNode) { // Verificar se o nó foi alocado corretamente
        printf("Erro ao alocar memória para o nó!\n");  // Exibir mensagem de erro
        exit(EXIT_FAILURE); // Encerrar o programa com falha
    }
    newNode->dados = tarefa;    // Atribuir a tarefa ao novo nó
    newNode->next = stack->top; // Definir o próximo nó como o topo da pilha
    stack->top = newNode;   // Definir o novo nó como o topo da pilha
}

Tarefa* pop(Stack* stack) { // Função para remover uma tarefa da pilha
    if (stack->top == NULL) {   // Verificar se a pilha está vazia
        printf("PILHA VAZIA!!!\n"); // Exibir mensagem de erro
        return NULL;    // Retornar nulo
    }
    StackNode* temp = stack->top;   // Criar um nó temporário para armazenar o topo da pilha
    Tarefa* tarefa = temp->dados;   // Obter a tarefa do nó temporário
    stack->top = stack->top->next;  // Definir o próximo nó como o topo da pilha
    free(temp); // Liberar a memória alocada para o nó temporário
    return tarefa;  // Retornar a tarefa removida
}

void freeStack(Stack* stack) {  // Função para liberar a memória alocada para a pilha
    while (stack->top != NULL) {    // Enquanto houver elementos na pilha
        StackNode* temp = stack->top;   // Criar um nó temporário para armazenar o topo da pilha
        stack->top = stack->top->next;  // Definir o próximo nó como o topo da pilha
        destruirTarefa(temp->dados);    // Liberar a memória alocada para a tarefa
        free(temp); // Liberar a memória alocada para o nó
    }
    free(stack);    // Liberar a memória alocada para a pilha
}

void pushLowPriorityTasks(Stack* stack, Tarefa* tarefa) {   // Função para adicionar tarefas de baixa prioridade à pilha
    if (tarefa->prioridade == 0) {  // Verificar se a prioridade da tarefa é baixa
        push(stack, tarefa);    // Adicionar a tarefa à pilha
    }
}

void salvarTarefasEmFicheiro(Tarefa *tarefasRealizadas, int nRealizadas) {
    FILE *fp = fopen("tarefas.txt", "w");   // Abrir o ficheiro para escrita
    if (fp != NULL) {
        for (int i = 0; i < nRealizadas; i++) {
            Tarefa *tarefa = &tarefasRealizadas[i];
            fprintf(fp, "%d;%s;%d;%d;%d-%d-%d;%d:%d:%d;%d;%d-%d-%d;%d:%d:%d;%s;\n",
                tarefa->id, tarefa->descricao, tarefa->prioridade, tarefa->tipo,
                tarefa->dataCriacao.tm_year + 1900, tarefa->dataCriacao.tm_mon + 1, tarefa->dataCriacao.tm_mday,
                tarefa->dataCriacao.tm_hour, tarefa->dataCriacao.tm_min, tarefa->dataCriacao.tm_sec,
                tarefa->estado,
                tarefa->dataConclusao.tm_year + 1900, tarefa->dataConclusao.tm_mon + 1, tarefa->dataConclusao.tm_mday,
                tarefa->dataConclusao.tm_hour, tarefa->dataConclusao.tm_min, tarefa->dataConclusao.tm_sec,
                tarefa->payloadJSON ? tarefa->payloadJSON : ""); // Adiciona o payload JSON em uma linha
        }
        fclose(fp);
        printf("Tarefas salvas com sucesso!\n");
    } else {
        printf("Erro ao abrir o arquivo para salvar as tarefas!\n");
    }
}


void carregarTarefasDoFicheiro(PriorityQueue *q, Stack *lowPriorityStack) {
    FILE *fp = fopen("tarefas.txt", "r"); // Abrir o ficheiro para leitura
    if (fp != NULL) {
        char linha[2048];
        while (fgets(linha, sizeof(linha), fp)) { // Enquanto não for o final do ficheiro
            Tarefa *tarefa = (Tarefa*)malloc(sizeof(Tarefa));   // Alocar memória para uma nova tarefa
            tarefa->payloadJSON = (char*)malloc(2048); // Alocar memória para o payload JSON

            // Localizar o início do payload JSON
            char *payloadStart = strchr(linha, '{');
            if (payloadStart) {
                // Copiar a parte antes do payload
                sscanf(linha, "%d;%299[^;];%d;%d;%d-%d-%d;%d:%d:%d;%d;%d-%d-%d;%d:%d:%d;",
                    &tarefa->id, tarefa->descricao, &tarefa->prioridade, &tarefa->tipo,
                    &tarefa->dataCriacao.tm_year, &tarefa->dataCriacao.tm_mon, &tarefa->dataCriacao.tm_mday,
                    &tarefa->dataCriacao.tm_hour, &tarefa->dataCriacao.tm_min, &tarefa->dataCriacao.tm_sec,
                    &tarefa->estado,
                    &tarefa->dataConclusao.tm_year, &tarefa->dataConclusao.tm_mon, &tarefa->dataConclusao.tm_mday,
                    &tarefa->dataConclusao.tm_hour, &tarefa->dataConclusao.tm_min, &tarefa->dataConclusao.tm_sec);

                tarefa->dataCriacao.tm_year -= 1900;
                tarefa->dataCriacao.tm_mon -= 1;
                tarefa->dataConclusao.tm_year -= 1900;
                tarefa->dataConclusao.tm_mon -= 1;

                // Copiar o payload JSON incluindo as quebras de linha
                strcpy(tarefa->payloadJSON, payloadStart);
                while (strchr(tarefa->payloadJSON, '}') == NULL) {
                    if (fgets(linha, sizeof(linha), fp) == NULL) {
                        break;
                    }
                    strcat(tarefa->payloadJSON, linha);
                }

                // Remover o ponto e vírgula final
                char *semicolon = strrchr(tarefa->payloadJSON, ';');
                if (semicolon) {
                    *semicolon = '\0';
                }

                if (tarefa->prioridade == 0) {
                    pushLowPriorityTasks(lowPriorityStack, tarefa);  // Adicionar tarefas de baixa prioridade à pilha
                } else {
                    enqueue(q, tarefa);  // Adicionar tarefas de alta prioridade à fila de prioridade
                }
            }
        }
        fclose(fp);
        printf("Tarefas carregadas com sucesso!\n");
    } else {
        printf("Erro ao abrir o arquivo para carregar as tarefas!\n");
    }
}

void imprimirPayloadJSON(const char *payloadJSON, int tipo) {
    jsmn_parser parser;
    int num_tokens = 256; // Aumentei o número de tokens
    jsmntok_t tokens[num_tokens];
    int r;

    jsmn_init(&parser);
    r = jsmn_parse(&parser, payloadJSON, strlen(payloadJSON), tokens, num_tokens);

    if (r < 0) {
        printf("Erro ao parsear o payload JSON. Código de erro: %d\n", r); // Imprime o código de erro
        return;
    }

    if (r < 1 || tokens[0].type != JSMN_OBJECT) {
        printf("O payload JSON não é um objeto válido.\n");
        return;
    }

    // Identifica o tipo de tarefa e imprime as informações correspondentes
    if (tipo == 1) {
        for (int i = 1; i < r; i++) {
            if (tokens[i].type == JSMN_STRING) {
                if (jsoneq(payloadJSON, &tokens[i], "Nome") == 0) {
                    printf("Nome: %.*s\n", tokens[i+1].end - tokens[i+1].start, payloadJSON + tokens[i+1].start);
                    i++; // Pula para o próximo par chave-valor
                } else if (jsoneq(payloadJSON, &tokens[i], "Novo.nome") == 0) {
                    printf("Novo nome: %.*s\n", tokens[i+1].end - tokens[i+1].start, payloadJSON + tokens[i+1].start);
                    i++; // Pula para o próximo par chave-valor
                } else if (jsoneq(payloadJSON, &tokens[i], "Local") == 0) {
                    printf("Local: %.*s\n", tokens[i+1].end - tokens[i+1].start, payloadJSON + tokens[i+1].start);
                    i++; // Pula para o próximo par chave-valor
                }
            }
        }
    } else if (tipo == 2) {
        for (int i = 1; i < r; i++) {
            if (tokens[i].type == JSMN_STRING) {
                if (jsoneq(payloadJSON, &tokens[i], "Numero.de.copias") == 0) {
                    printf("Número de cópias: %.*s\n", tokens[i+1].end - tokens[i+1].start, payloadJSON + tokens[i+1].start);
                    i++; // Pula para o próximo par chave-valor
                } else if (jsoneq(payloadJSON, &tokens[i], "Cor") == 0) {
                    printf("Cor: %.*s\n", tokens[i+1].end - tokens[i+1].start, payloadJSON + tokens[i+1].start);
                    i++; // Pula para o próximo par chave-valor
                } else if (jsoneq(payloadJSON, &tokens[i], "Nome.impressora") == 0) {
                    printf("Nome impressora: %.*s\n", tokens[i+1].end - tokens[i+1].start, payloadJSON + tokens[i+1].start);
                    i++; // Pula para o próximo par chave-valor
                }
            }
        }
    } else {
        printf("Tipo de tarefa desconhecido ou não suportado.\n");
    }
}

// Função auxiliar para comparar strings JSON
static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
    if (tok->type == JSMN_STRING &&
        (int) strlen(s) == tok->end - tok->start &&
        strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
        return 0;
    }
    return -1;
}

void gerarRelatorio(PriorityQueue *q, Tarefa tarefasRealizadas[], int nRealizadas) {
    FILE *fp = fopen("relatorio.txt", "w");   // Abrir o ficheiro para escrita
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo para gerar o relatório!\n");
        return;
    }

    // Função auxiliar para imprimir o payload JSON no arquivo
    void imprimirPayloadJSONNoArquivo(FILE *fp, const char *payloadJSON, int tipo) {
        jsmn_parser parser;
        int num_tokens = 256; // Aumentei o número de tokens
        jsmntok_t tokens[num_tokens];
        int r;

        jsmn_init(&parser);
        r = jsmn_parse(&parser, payloadJSON, strlen(payloadJSON), tokens, num_tokens);

        if (r < 0) {
            fprintf(fp, "Erro ao parsear o payload JSON. Código de erro: %d\n", r); // Imprime o código de erro
            return;
        }

        if (r < 1 || tokens[0].type != JSMN_OBJECT) {
            fprintf(fp, "O payload JSON não é um objeto válido.\n");
            return;
        }

        if (tipo == 1) {
            for (int i = 1; i < r; i++) {
                if (tokens[i].type == JSMN_STRING) {
                    if (jsoneq(payloadJSON, &tokens[i], "Nome") == 0) {
                        fprintf(fp, "Nome: %.*s\n", tokens[i+1].end - tokens[i+1].start, payloadJSON + tokens[i+1].start);
                        i++;
                    } else if (jsoneq(payloadJSON, &tokens[i], "Novo.nome") == 0) {
                        fprintf(fp, "Novo nome: %.*s\n", tokens[i+1].end - tokens[i+1].start, payloadJSON + tokens[i+1].start);
                        i++;
                    } else if (jsoneq(payloadJSON, &tokens[i], "Local") == 0) {
                        fprintf(fp, "Local: %.*s\n", tokens[i+1].end - tokens[i+1].start, payloadJSON + tokens[i+1].start);
                        i++;
                    }
                }
            }
        } else if (tipo == 2) {
            for (int i = 1; i < r; i++) {
                if (tokens[i].type == JSMN_STRING) {
                    if (jsoneq(payloadJSON, &tokens[i], "Numero.de.copias") == 0) {
                        fprintf(fp, "Número de cópias: %.*s\n", tokens[i+1].end - tokens[i+1].start, payloadJSON + tokens[i+1].start);
                        i++;
                    } else if (jsoneq(payloadJSON, &tokens[i], "Cor") == 0) {
                        fprintf(fp, "Cor: %.*s\n", tokens[i+1].end - tokens[i+1].start, payloadJSON + tokens[i+1].start);
                        i++;
                    } else if (jsoneq(payloadJSON, &tokens[i], "Nome.impressora") == 0) {
                        fprintf(fp, "Nome impressora: %.*s\n", tokens[i+1].end - tokens[i+1].start, payloadJSON + tokens[i+1].start);
                        i++;
                    }
                }
            }
        } else {
            fprintf(fp, "Tipo de tarefa desconhecido ou não suportado.\n");
        }
    }

    // Escrever tarefas pendentes da fila de prioridade
    Node *temp = q->front;
    fprintf(fp, "Tarefas Pendentes:\n");
    while (temp != NULL) {
        Tarefa *tarefa = temp->dados;
        fprintf(fp, "ID: %d\n", tarefa->id);
        fprintf(fp, "Descrição: %s\n", tarefa->descricao);
        fprintf(fp, "Prioridade(1-Alto 0-Baixo): %d\n", tarefa->prioridade);
        fprintf(fp, "Tipo: %d\n", tarefa->tipo);
        fprintf(fp, "Data de Criação: %04d-%02d-%02d %02d:%02d:%02d\n",
                tarefa->dataCriacao.tm_year + 1900, tarefa->dataCriacao.tm_mon + 1, tarefa->dataCriacao.tm_mday,
                tarefa->dataCriacao.tm_hour, tarefa->dataCriacao.tm_min, tarefa->dataCriacao.tm_sec);
        fprintf(fp, "Estado: %d\n", tarefa->estado);
        imprimirPayloadJSONNoArquivo(fp, tarefa->payloadJSON, tarefa->tipo);
        fprintf(fp, "\n");
        temp = temp->next;
    }

    // Escrever tarefas realizadas
    fprintf(fp, "Tarefas Realizadas:\n");
    for (int i = 0; i < nRealizadas; i++) {
        Tarefa *tarefa = &tarefasRealizadas[i];
        fprintf(fp, "ID: %d\n", tarefa->id);
        fprintf(fp, "Descrição: %s\n", tarefa->descricao);
        fprintf(fp, "Prioridade(1-Alto 0-Baixo): %d\n", tarefa->prioridade);
        fprintf(fp, "Tipo: %d\n", tarefa->tipo);
        fprintf(fp, "Data de Criação: %04d-%02d-%02d %02d:%02d:%02d\n",
                tarefa->dataCriacao.tm_year + 1900, tarefa->dataCriacao.tm_mon + 1, tarefa->dataCriacao.tm_mday,
                tarefa->dataCriacao.tm_hour, tarefa->dataCriacao.tm_min, tarefa->dataCriacao.tm_sec);
        fprintf(fp, "Data de Conclusão: %04d-%02d-%02d %02d:%02d:%02d\n",
                tarefa->dataConclusao.tm_year + 1900, tarefa->dataConclusao.tm_mon + 1, tarefa->dataConclusao.tm_mday,
                tarefa->dataConclusao.tm_hour, tarefa->dataConclusao.tm_min, tarefa->dataConclusao.tm_sec);
        fprintf(fp, "Estado: %d\n", tarefa->estado);
        imprimirPayloadJSONNoArquivo(fp, tarefa->payloadJSON, tarefa->tipo);
        fprintf(fp, "\n");
    }

    fclose(fp);
    printf("Relatório gerado com sucesso em 'relatorio.txt'!\n");
}


void buscarTarefaPorID(PriorityQueue *q, Tarefa tarefasRealizadas[], int nRealizadas, int id) {
    // ordenar as tarefas realizadas por ID
    quickSort(tarefasRealizadas, 0, nRealizadas - 1);

    // procurar na fila de prioridade
    Node *temp = q->front;  // Ponteiro para nó para percorrer a fila de prioridade
    while (temp != NULL) {  // Enquanto houver elementos na fila de prioridade
        if (temp->dados->id == id) {    // Verificar se o ID da tarefa corresponde ao ID fornecido
            printf("Tarefa encontrada na fila de prioridade:\n");   // Exibir mensagem de sucesso
            printf("ID: %d\n", temp->dados->id);    // Exibir o ID da tarefa
            printf("Descrição: %s\n", temp->dados->descricao);  // Exibir a descrição da tarefa
            printf("Prioridade(1-Alto 0-Baixo): %d\n", temp->dados->prioridade);    // Exibir a prioridade da tarefa
            printf("Tipo: %d\n", temp->dados->tipo); // Exibir o tipo da tarefa
            printf("Data Criação: ");   // Exibir a data de criação
            imprimirDataHora(&temp->dados->dataCriacao); // Exibir a data de criação
            printf("\n");   // Exibir uma nova linha
            printf("Data Conclusão: "); // Exibir a data de conclusão
            imprimirDataHora(&temp->dados->dataConclusao); // Exibir a data de conclusão
            printf("\n");   // Exibir uma nova linha
            return; // Retornar ao chamador
        }
        temp = temp->next;  // Avançar para o próximo nó na fila de prioridade
    }

    // procurar nas tarefas realizadas
    for (int i = 0; i < nRealizadas; i++) { // Percorrer o array de tarefas realizadas
        if (tarefasRealizadas[i].id == id) {    // Verificar se o ID da tarefa corresponde ao ID fornecido
            printf("Tarefa encontrada nas tarefas realizadas:\n");  // Exibir mensagem de sucesso
            printf("ID: %d\n", tarefasRealizadas[i].id);    // Exibir o ID da tarefa
            printf("Descrição: %s\n", tarefasRealizadas[i].descricao);  // Exibir a descrição da tarefa
            printf("Prioridade(1-Alto 0-Baixo): %d\n", tarefasRealizadas[i].prioridade);    // Exibir a prioridade da tarefa
            printf("Tipo: %d\n", tarefasRealizadas[i].tipo); // Exibir o tipo da tarefa
            printf("Data Criação: ");   // Exibir a data de criação
            imprimirDataHora(&tarefasRealizadas[i].dataCriacao); // Exibir a data de criação
            printf("\n");   // Exibir uma nova linha    
            printf("Data Conclusão: "); // Exibir a data de conclusão
            imprimirDataHora(&tarefasRealizadas[i].dataConclusao); // Exibir a data de conclusão
            printf("\n");   // Exibir uma nova linha
            return; // Retornar ao chamador
        }
    }

    printf("Tarefa com ID %d não encontrada.\n", id);   // Exibir mensagem de erro
}



void listarTarefas(PriorityQueue *q, Tarefa tarefasRealizadas[], int nRealizadas) {
    Node *temp = q->front;  // Ponteiro para nó para percorrer a fila de prioridade
    int temTarefasPendentes = 0;  // Flag para verificar se há tarefas pendentes

    // Percorre a fila de prioridade
    while (temp != NULL) {
        Tarefa *tarefa = temp->dados;
        printf("ID: %d\n", tarefa->id);    // Imprime o ID da tarefa
        printf("Descrição: %s\n", tarefa->descricao);  // Imprime a descrição da tarefa
        printf("Prioridade(1-Alto 0-Baixo): %d\n", tarefa->prioridade);    // Imprime a prioridade da tarefa
        printf("Tipo: %d\n", tarefa->tipo); // Imprime o tipo da tarefa
        printf("Data de Criação: %04d-%02d-%02d %02d:%02d:%02d\n",
               tarefa->dataCriacao.tm_year + 1900, tarefa->dataCriacao.tm_mon + 1, tarefa->dataCriacao.tm_mday,
               tarefa->dataCriacao.tm_hour, tarefa->dataCriacao.tm_min, tarefa->dataCriacao.tm_sec); // Imprime a data de criação
        if (tarefa->estado == 2) {  // Se a tarefa estiver concluída
            printf("Data de Conclusão: %04d-%02d-%02d %02d:%02d:%02d\n",
                   tarefa->dataConclusao.tm_year + 1900, tarefa->dataConclusao.tm_mon + 1, tarefa->dataConclusao.tm_mday,
                   tarefa->dataConclusao.tm_hour, tarefa->dataConclusao.tm_min, tarefa->dataConclusao.tm_sec); // Imprime a data de conclusão
        }
        printf("Estado(em espera=0): %d\n", tarefa->estado); // Imprime o estado da tarefa
        printf("Payload JSON:\n");
        imprimirPayloadJSON(tarefa->payloadJSON, tarefa->tipo);
        printf("\n");   // Imprime uma linha em branco
        temp = temp->next;  // Avança para o próximo nó na fila de prioridade
        temTarefasPendentes = 1;  // Define a flag para indicar que há tarefas pendentes
    }

   // Imprime uma linha separadora se houver tarefas pendentes e concluídas
    if (temTarefasPendentes && nRealizadas > 0) {
        printf("Tarefas Realizadas:\n");
    }

    // Imprime as tarefas concluídas do array
    for (int i = 0; i < nRealizadas; i++) {
        printf("ID: %d\n", tarefasRealizadas[i].id);    // Imprime o ID da tarefa
        printf("Descrição: %s\n", tarefasRealizadas[i].descricao);  // Imprime a descrição da tarefa
        printf("Prioridade(1-Alto 0-Baixo): %d\n", tarefasRealizadas[i].prioridade);    // Imprime a prioridade da tarefa
        printf("Tipo: %d\n", tarefasRealizadas[i].tipo); // Imprime o tipo da tarefa
        printf("Data de Criação: %04d-%02d-%02d %02d:%02d:%02d\n",
               tarefasRealizadas[i].dataCriacao.tm_year + 1900, tarefasRealizadas[i].dataCriacao.tm_mon + 1, tarefasRealizadas[i].dataCriacao.tm_mday,
               tarefasRealizadas[i].dataCriacao.tm_hour, tarefasRealizadas[i].dataCriacao.tm_min, tarefasRealizadas[i].dataCriacao.tm_sec); // Imprime a data de criação
        printf("Data de Conclusão: %04d-%02d-%02d %02d:%02d:%02d\n",
               tarefasRealizadas[i].dataConclusao.tm_year + 1900, tarefasRealizadas[i].dataConclusao.tm_mon + 1, tarefasRealizadas[i].dataConclusao.tm_mday,
               tarefasRealizadas[i].dataConclusao.tm_hour, tarefasRealizadas[i].dataConclusao.tm_min, tarefasRealizadas[i].dataConclusao.tm_sec); // Imprime a data de conclusão
        printf("Estado(sucesso=2): %d\n", tarefasRealizadas[i].estado); // Imprime o estado da tarefa
        printf("Payload JSON:\n");
        imprimirPayloadJSON(tarefasRealizadas[i].payloadJSON, tarefasRealizadas[i].tipo);
        printf("\n");   // Imprime uma linha em branco
    }

    // Se não houver tarefas (pendentes ou concluídas), imprime uma mensagem
    if (!temTarefasPendentes && nRealizadas == 0) {
        printf("Nenhuma tarefa pendente encontrada.\n");
    }
}

void imprimirDataHora(struct tm *data) {
    printf("%04d-%02d-%02d %02d:%02d:%02d",
           data->tm_year + 1900, data->tm_mon + 1, data->tm_mday,
           data->tm_hour, data->tm_min, data->tm_sec);
}

int verificarIDusado(PriorityQueue *q, Tarefa tarefasRealizadas[], int nRealizadas, int id) {
    Node *temp = q->front;  // criar um nó temporário para percorrer a fila de prioridade
    while (temp != NULL) {  // enquanto houver elementos na fila de prioridade
        if (temp->dados->id == id) {    // verificar se o ID da tarefa corresponde ao ID fornecido
            printf("ID já utilizado na fila de prioridade!\n");   // exibir mensagem de erro
            return 1;  // ID ja utilizado
        }
        temp = temp->next;  // sair para o próximo nó na fila de prioridade
    }

    // verificar se o ID foi usado nas tarefas realizadas
    for (int i = 0; i < nRealizadas; i++) { // percorrer o array de tarefas realizadas
        if (tarefasRealizadas[i].id == id) {    //verificar se o ID da tarefa corresponde ao ID fornecido
            printf("ID já utilizado nas tarefas realizadas!\n");  // exibir mensagem de erro
            return 1;  // ID já utilizado
        }
    }

    return 0;  // ID nao utilizado
}


int menu()
{
        int choice; // Variável para armazenar a escolha do usuário
        printf("Menu:\n");  // Exibir o menu
        printf("1. Criar tarefa\n");
        printf("2. Executar tarefa\n");
        printf("3. Buscar tarefa por ID\n");
        printf("4. Listar tarefas\n");
        printf("5. Relatório\n");
        printf("6. Salvar tarefas em ficheiro\n");
        printf("7. Carregar tarefas do ficheiro\n");
        printf("0. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &choice);
        limparBuffer(); // Limpar o buffer de entrada
        return choice;
}


void swap(Tarefa *a, Tarefa *b) {   // Função para trocar duas tarefas
    Tarefa temp = *a;   // Variável temporária para armazenar a tarefa a
    *a = *b;    // Atribuir a tarefa b à tarefa a
    *b = temp;  // Atribuir a tarefa temporária à tarefa b
}

int partition(Tarefa tarefas[], int low, int high) {    // Função para particionar o array de tarefas
    int pivot = tarefas[high].id;  // elemento pivô
    int i = (low - 1);  // Índice do menor elemento

    for (int j = low; j <= high - 1; j++) {  // Percorrer o array
        // Se o elemento atual é menor ou igual ao pivô
        if (tarefas[j].id <= pivot) {   // Verificar se o ID da tarefa é menor ou igual ao pivô
            i++;  // Incrementar o índice do menor elemento
            swap(&tarefas[i], &tarefas[j]); // Trocar os elementos
        }
    }
    swap(&tarefas[i + 1], &tarefas[high]);  // Trocar os elementos
    return (i + 1); // Retornar o índice do pivô
}

void quickSort(Tarefa tarefas[], int low, int high) {   // Função para ordenar o array de tarefas
    if (low < high) {   // Verificar se o índice baixo é menor que o índice alto
        // Obter o índice da partição
        int pi = partition(tarefas, low, high);  // Obter o índice da partição

        // Ordenar os elementos separadamente antes e depois da partição
        quickSort(tarefas, low, pi - 1);    // Ordenar os elementos antes da partição
        quickSort(tarefas, pi + 1, high);   // Ordenar os elementos depois da partição
    }
}


void executarTarefa(Tarefa tarefasRealizadas[], int nRealizadas, PriorityQueue *q)
{
    Tarefa* tarefa = dequeue(q);    // Remover a tarefa da fila de prioridade
     if (tarefa != NULL) {   // Verificar se a tarefa foi removida com sucesso
                    printf("Executando tarefa:\n");   // Exibir mensagem de execução
                    printf("ID: %d\n", tarefa->id);   // Exibir o ID da tarefa
                    printf("Descrição: %s\n", tarefa->descricao);   // Exibir a descrição da tarefa
                    printf("Prioridade: %d\n", tarefa->prioridade); // Exibir a prioridade da tarefa
                    // Atualizar o estado da tarefa
                    tarefa->estado = 1; // Tarefa em execução
                    // Executar a tarefa (simulação)
                    // Conclui a tarefa
                    tarefa->estado = 2; // Tarefa concluída
                    time_t tempoAtual = time(NULL); // Obter o tempo atual
                    tarefa->dataConclusao = *localtime(&tempoAtual);    // Atribuir a data/hora atual à data de conclusão da tarefa
                    printf("Payload JSON:\n");
                    imprimirPayloadJSON(tarefa->payloadJSON, tarefa->tipo);
                    tarefasRealizadas[nRealizadas++] = *tarefa; // Adicionar a tarefa ao array de tarefas realizadas
                    printf("Tarefa concluída.\n");  // Exibir mensagem de conclusão
                }
                else {  // Se não houver tarefas na fila de prioridade
                    printf("Nenhuma tarefa na fila de prioridade para executar.\n"); // Exibir mensagem de erro
                }
}

/*void voltarParaMenu() {
    printf("Para voltar ao menu escreve EXIT\n"); // Exibir mensagem para pressionar Enter
    getchar();  // Aguardar a entrada do usuário
    limparBuffer(); // Limpar o buffer de entrada
    char exit[5];   // Variável para armazenar a entrada do usuário
    fgets(exit, 5, stdin);  // Ler a entrada do usuário
    if (strcmp(exit, "EXIT") == 0) {    // Verificar se a entrada do usuário é "EXIT"
        return; // Retornar ao chamador
    }
    else {  // Se a entrada do usuário não for "EXIT"
        printf("Comando inválido!\n");  // Exibir mensagem de erro
        voltarParaMenu();   // Chamar a função novamente
    }

}*/



int main() {    // Função principal
    int choice;
    setlocale(LC_ALL, "Portuguese");
    PriorityQueue* q = createQueue();   // Criar uma nova fila de prioridade
    Stack* lowPriorityStack = malloc(sizeof(Stack));    // Alocar memória para a pilha de tarefas de baixa prioridade
    if (!lowPriorityStack) {    // Verificar se a pilha foi alocada corretamente
        printf("Erro ao alocar memória para a pilha!\n");   // Exibir mensagem de erro
        exit(EXIT_FAILURE); // Encerrar o programa com falha
    }
    lowPriorityStack->top = NULL;   // Inicializar o topo da pilha como nulo

    Tarefa tarefasRealizadas[300];  // Array para armazenar as tarefas realizadas
    int nRealizadas = 0;    // Contador para o número de tarefas realizadas

    while (1) { // Loop infinito para exibir o menu

        choice = menu();
        switch (choice) {   // Verificar a escolha do usuário
            case 1: {
                // Criar tarefa
                Tarefa* novaTarefa = (Tarefa*)malloc(sizeof(Tarefa));   // Alocar memória para uma nova tarefa
                criarTarefa(q, tarefasRealizadas, nRealizadas, novaTarefa);    // Criar a nova tarefa
                enqueue(q, novaTarefa); // Adicionar a nova tarefa à fila de prioridade
                pushLowPriorityTasks(lowPriorityStack, novaTarefa);  // Adicionar a nova tarefa à pilha de tarefas de baixa prioridade
                break;  // Sair do switch
            }
            case 2: {
                // Executar tarefa
                executarTarefa(tarefasRealizadas, nRealizadas, q);   // Executar a tarefa
                break;  // Sair do switch
            }
            case 3: {
                // Buscar tarefa por ID
                int id; // Variável para armazenar o ID da tarefa a buscar
                printf("Digite o ID da tarefa a buscar: "); // Solicitar ao usuário o ID da tarefa
                scanf("%d", &id);   // Ler o ID da tarefa
                limparBuffer(); // Limpar o buffer de entrada
                buscarTarefaPorID(q, tarefasRealizadas, nRealizadas, id);   // Buscar a tarefa na fila de prioridade e no array de tarefas realizadas
                break;  // Sair do switch
            }
            case 4: {
                // Listar tarefas
                listarTarefas(q, tarefasRealizadas, nRealizadas);   // Listar as tarefas na fila de prioridade
                break;  // Sair do switch
            }
            case 5:
               gerarRelatorio(q, tarefasRealizadas, nRealizadas);
                break;
            case 6: {
                // Salvar tarefas em ficheiro
                salvarTarefasEmFicheiro(tarefasRealizadas, nRealizadas);   // Salvar as tarefas realizadas em um ficheiro
                break;  // Sair do switch
            }
            case 7:
                // Carregar tarefas do ficheiro
                carregarTarefasDoFicheiro(q, lowPriorityStack);  // Carregar as tarefas do ficheiro
                break;  // Sair do switch
            case 0:
                printf("Saindo do programa...\n");  // Exibir mensagem de saída
                freeQueue(q);   // Liberar a memória alocada para a fila de prioridade
                freeStack(lowPriorityStack);    // Liberar a memória alocada para a pilha de tarefas de baixa prioridade
                exit(0);    // Encerrar o programa
            default:
                printf("Opção inválida!\n");    // Exibir mensagem de erro
        }
    }
    return 0;
}