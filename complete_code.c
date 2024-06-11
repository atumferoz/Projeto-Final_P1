#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

#define DESC 300

typedef struct Tarefa {
    int id;
    char descricao[DESC];
    int prioridade; // 1: alto, 0: baixo, 2: médio
    struct tm dataCriacao;
    struct tm dataConclusao;
    int estado; // 0: em espera, 1: em execução, 2: concluída, 3: falha
    char *payloadJSON;
} Tarefa;

typedef struct Node {
    Tarefa* dados;
    struct Node* next;
} Node;

typedef struct PriorityQueue {
    Node* front;
} PriorityQueue;

typedef struct StackNode {
    Tarefa* dados;
    struct StackNode* next;
} StackNode;

typedef struct Stack {
    StackNode* top;
} Stack;

// Function prototypes
void criarTarefa(Tarefa *tarefa);
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
void salvarTarefasEmFicheiro(Tarefa *tarefasRealizadas, int nRealizadas, char *ficheiroSaida);
void carregarTarefasDoFicheiro(PriorityQueue *q, Stack *lowPriorityStack, const char *ficheiroEntrada);
void buscarTarefaPorID(PriorityQueue *q, int id);
void listarTarefas(PriorityQueue *q);

void criarTarefa(Tarefa *tarefa) {
    if (!tarefa) {
        printf("Erro ao alocar memória para a tarefa!\n");
        return;
    }

    printf("Digite o ID da tarefa: ");
    scanf("%d", &tarefa->id);
    limparBuffer();

    printf("Digite a descrição da tarefa: ");
    fgets(tarefa->descricao, DESC, stdin);
    tarefa->descricao[strcspn(tarefa->descricao, "\n")] = 0; // Remove newline character

    printf("Digite a prioridade da tarefa (1 - alto, 0 - baixo, 2 - médio): ");
    scanf("%d", &tarefa->prioridade);
    limparBuffer();

    // Inicializar os campos de data e hora com a data/hora atual
    time_t tempoAtual = time(NULL);
    struct tm *dataHoraAtual = localtime(&tempoAtual);
    tarefa->dataCriacao = *dataHoraAtual;

    tarefa->dataConclusao.tm_year = 0; // Tarefa ainda não concluída
    tarefa->dataConclusao.tm_mon = 0;
    tarefa->dataConclusao.tm_mday = 0;
    tarefa->dataConclusao.tm_hour = 0;
    tarefa->dataConclusao.tm_min = 0;
    tarefa->dataConclusao.tm_sec = 0;

    tarefa->estado = 0; // Tarefa em espera

    // Alocar memória e solicitar o payload JSON ao usuário
    printf("Digite o payload JSON da tarefa: ");
    char buffer[1024];
    fgets(buffer, sizeof(buffer), stdin);
    tarefa->payloadJSON = (char*)malloc(strlen(buffer) + 1);
    strcpy(tarefa->payloadJSON, buffer);

    printf("Tarefa criada com sucesso!\n");
}

void destruirTarefa(Tarefa *tarefa) {
    free(tarefa->payloadJSON);
    free(tarefa);
}

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

PriorityQueue* createQueue() {
    PriorityQueue* q = malloc(sizeof(PriorityQueue));
    if (!q) {
        printf("Erro ao alocar memória para a fila de prioridade!\n");
        exit(EXIT_FAILURE);
    }
    q->front = NULL;
    return q;
}

void enqueue(PriorityQueue* q, Tarefa* tarefa) {
    Node* newNode = malloc(sizeof(Node));
    if (!newNode) {
        printf("Erro ao alocar memória para o nó!\n");
        exit(EXIT_FAILURE);
    }
    newNode->dados = tarefa;
    newNode->next = NULL;

    if (q->front == NULL || q->front->dados->prioridade < tarefa->prioridade) {
        newNode->next = q->front;
        q->front = newNode;
    } else {
        Node* temp = q->front;
        while (temp->next != NULL && temp->next->dados->prioridade >= tarefa->prioridade) {
            temp = temp->next;
        }
        newNode->next = temp->next;
        temp->next = newNode;
    }
}

Tarefa* dequeue(PriorityQueue* q) {
    if (q->front == NULL) {
        printf("FILA VAZIA!!!\n");
        return NULL;
    }
    Node* temp = q->front;
    Tarefa* tarefa = temp->dados;
    q->front = q->front->next;
    free(temp);
    return tarefa;
}

void freeQueue(PriorityQueue* q) {
    while (q->front != NULL) {
        Node* temp = q->front;
        q->front = q->front->next;
        destruirTarefa(temp->dados);
        free(temp);
    }
    free(q);
}

void push(Stack* stack, Tarefa* tarefa) {
    StackNode* newNode = malloc(sizeof(StackNode));
    if (!newNode) {
        printf("Erro ao alocar memória para o nó!\n");
        exit(EXIT_FAILURE);
    }
    newNode->dados = tarefa;
    newNode->next = stack->top;
    stack->top = newNode;
}

Tarefa* pop(Stack* stack) {
    if (stack->top == NULL) {
        printf("PILHA VAZIA!!!\n");
        return NULL;
    }
    StackNode* temp = stack->top;
    Tarefa* tarefa = temp->dados;
    stack->top = stack->top->next;
    free(temp);
    return tarefa;
}

void freeStack(Stack* stack) {
    while (stack->top != NULL) {
        StackNode* temp = stack->top;
        stack->top = stack->top->next;
        destruirTarefa(temp->dados);
        free(temp);
    }
    free(stack);
}

void pushLowPriorityTasks(Stack* stack, Tarefa* tarefa) {
    if (tarefa->prioridade == 0) {
        push(stack, tarefa);
    }
}

void salvarTarefasEmFicheiro(Tarefa *tarefasRealizadas, int nRealizadas, char *ficheiroSaida) {
    FILE *fp = fopen(ficheiroSaida, "w");
    if (fp != NULL) {
        for (int i = 0; i < nRealizadas; i++) {
            Tarefa *tarefa = &tarefasRealizadas[i];
            fprintf(fp, "ID: %d\n", tarefa->id);
            fprintf(fp, "Descrição: %s\n", tarefa->descricao);
            fprintf(fp, "Prioridade: %d\n", tarefa->prioridade);
            // Escrever outros campos da tarefa
            fprintf(fp, "\n");
        }
        fclose(fp);
        printf("Tarefas salvas com sucesso!\n");
    } else {
        printf("Erro ao abrir o arquivo para salvar as tarefas!\n");
    }
}

void carregarTarefasDoFicheiro(PriorityQueue *q, Stack *lowPriorityStack, const char *ficheiroEntrada) {
    FILE *fp = fopen(ficheiroEntrada, "r");
    if (fp != NULL) {
        while (!feof(fp)) {
            Tarefa *tarefa = (Tarefa*)malloc(sizeof(Tarefa));
            if (fscanf(fp, "ID: %d\n", &tarefa->id) != 1) break;
            fscanf(fp, "Descrição: %[^\n]\n", tarefa->descricao);
            fscanf(fp, "Prioridade: %d\n", &tarefa->prioridade);
            // Ler outros campos da tarefa se necessário

            // Inicializar outros campos da tarefa
            time_t tempoAtual = time(NULL);
            struct tm *dataHoraAtual = localtime(&tempoAtual);
            tarefa->dataCriacao = *dataHoraAtual;

            tarefa->dataConclusao.tm_year = 0;
            tarefa->dataConclusao.tm_mon = 0;
            tarefa->dataConclusao.tm_mday = 0;
            tarefa->dataConclusao.tm_hour = 0;
            tarefa->dataConclusao.tm_min = 0;
            tarefa->dataConclusao.tm_sec = 0;

            tarefa->estado = 0;

            enqueue(q, tarefa);
            pushLowPriorityTasks(lowPriorityStack, tarefa);
        }
        fclose(fp);
        printf("Tarefas carregadas com sucesso!\n");
    } else {
        printf("Erro ao abrir o arquivo para carregar as tarefas!\n");
    }
}

void buscarTarefaPorID(PriorityQueue *q, int id) {
    Node *temp = q->front;
    while (temp != NULL) {
        if (temp->dados->id == id) {
            printf("Tarefa encontrada:\n");
            printf("ID: %d\n", temp->dados->id);
            printf("Descrição: %s\n", temp->dados->descricao);
            printf("Prioridade: %d\n", temp->dados->prioridade);
            // Exibir outros campos da tarefa
            return;
        }
        temp = temp->next;
    }
    printf("Tarefa com ID %d não encontrada.\n", id);
}

void listarTarefas(PriorityQueue *q) {
    Node *temp = q->front;
    if (!temp) {
        printf("Nenhuma tarefa na fila.\n");
        return;
    }
    while (temp != NULL) {
        printf("ID: %d\n", temp->dados->id);
        printf("Descrição: %s\n", temp->dados->descricao);
        printf("Prioridade: %d\n", temp->dados->prioridade);
        // Exibir outros campos da tarefa
        printf("\n");
        temp = temp->next;
    }
}

int main() {
    setlocale(LC_ALL, "Portuguese");
    PriorityQueue* q = createQueue();
    Stack* lowPriorityStack = malloc(sizeof(Stack));
    if (!lowPriorityStack) {
        printf("Erro ao alocar memória para a pilha!\n");
        exit(EXIT_FAILURE);
    }
    lowPriorityStack->top = NULL;

    Tarefa tarefasRealizadas[300];
    int nRealizadas = 0;

    while (1) {
        int choice;
        printf("Menu:\n");
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
        limparBuffer();

        switch (choice) {
            case 1: {
                // Criar tarefa
                Tarefa* novaTarefa = (Tarefa*)malloc(sizeof(Tarefa));
                criarTarefa(novaTarefa);
                enqueue(q, novaTarefa);
                pushLowPriorityTasks(lowPriorityStack, novaTarefa);
                break;
            }
            case 2: {
                // Executar tarefa
                Tarefa* tarefa = dequeue(q);
                if (tarefa != NULL) {
                    printf("Executando tarefa:\n");
                    printf("ID: %d\n", tarefa->id);
                    printf("Descrição: %s\n", tarefa->descricao);
                    printf("Prioridade: %d\n", tarefa->prioridade);
                    // Atualizar o estado da tarefa
                    tarefa->estado = 1;
                    // Executar a tarefa (simulação)
                    // Conclui a tarefa
                    tarefa->estado = 2;
                    time_t tempoAtual = time(NULL);
                    tarefa->dataConclusao = *localtime(&tempoAtual);
                    tarefasRealizadas[nRealizadas++] = *tarefa;
                    printf("Tarefa concluída.\n");
                }
                break;
            }
            case 3: {
                // Buscar tarefa por ID
                int id;
                printf("Digite o ID da tarefa a buscar: ");
                scanf("%d", &id);
                limparBuffer();
                buscarTarefaPorID(q, id);
                break;
            }
            case 4: {
                // Listar tarefas
                listarTarefas(q);
                break;
            }
            case 5:
                // Relatório
                printf("Tarefas realizadas:\n");
                for (int i = 0; i < nRealizadas; i++) {
                    printf("ID: %d\n", tarefasRealizadas[i].id);
                    printf("Descrição: %s\n", tarefasRealizadas[i].descricao);
                    printf("Prioridade: %d\n", tarefasRealizadas[i].prioridade);
                    // Exibir outros campos da tarefa se necessário
                    printf("\n");
                }
                break;
            case 6: {
                // Salvar tarefas em ficheiro
                salvarTarefasEmFicheiro(tarefasRealizadas, nRealizadas, "tarefas.txt");
                break;
            }
            case 7:
                // Carregar tarefas do ficheiro
                carregarTarefasDoFicheiro(q, lowPriorityStack, "tarefas.txt");
                break;
            case 0:
                printf("Saindo do programa...\n");
                freeQueue(q);
                freeStack(lowPriorityStack);
                exit(0);
            default:
                printf("Opção inválida!\n");
        }
    }
    return 0;
}
