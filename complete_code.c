#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

#define DESC 300

typedef struct Tarefa { // Estrutura de dados para armazenar informações de uma tarefa
    int id;
    char descricao[DESC];
    int prioridade; // 1: alto, 0: baixo, 2: médio
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

// Function prototypes
int menu();
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

void criarTarefa(Tarefa *tarefa) {  // Função para criar uma nova tarefa
    if (!tarefa) {  // Verificar se a tarefa foi alocada corretamente
        printf("Erro ao alocar memória para a tarefa!\n");  // Exibir mensagem de erro
        return; // Retornar da função
    }

    printf("Digite o ID da tarefa: ");  // Solicitar ao usuário o ID da tarefa
    scanf("%d", &tarefa->id);   // Ler o ID da tarefa
    limparBuffer(); // Limpar o buffer de entrada

    printf("Digite a descrição da tarefa: ");   // Solicitar ao usuário a descrição da tarefa
    fgets(tarefa->descricao, DESC, stdin);  // Ler a descrição da tarefa
    tarefa->descricao[strcspn(tarefa->descricao, "\n")] = 0; // Remove newline character

    printf("Digite a prioridade da tarefa (1 - alto, 0 - baixo, 2 - médio): "); // Solicitar ao usuário a prioridade da tarefa
    scanf("%d", &tarefa->prioridade);   // Ler a prioridade da tarefa
    limparBuffer(); // Limpar o buffer de entrada

    // Inicializar os campos de data e hora com a data/hora atual
    time_t tempoAtual = time(NULL); // Obter o tempo atual
    tarefa->dataCriacao = *localtime(&tempoAtual);;   // Atribuir a data/hora atual à data de criação da tarefa

    tarefa->dataConclusao = (struct tm){0}; // Tarefa ainda não concluída

    tarefa->estado = 0; // Tarefa em espera

    // Alocar memória e solicitar o payload JSON ao usuário
    printf("Digite o payload JSON da tarefa: ");    // Solicitar ao usuário o payload JSON da tarefa
    char buffer[1024];      // Buffer para armazenar a entrada do usuário
    fgets(buffer, sizeof(buffer), stdin);   // Ler a entrada do usuário
    tarefa->payloadJSON = (char*)malloc(strlen(buffer) + 1);    // Alocar memória para o payload JSON
    strcpy(tarefa->payloadJSON, buffer);    // Copiar o payload JSON para a estrutura de dados da tarefa

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

void salvarTarefasEmFicheiro(Tarefa *tarefasRealizadas, int nRealizadas, char *ficheiroSaida) { // Função para salvar as tarefas realizadas em um ficheiro
    FILE *fp = fopen(ficheiroSaida, "w");   // Abrir o ficheiro para escrita
    if (fp != NULL) {   // Verificar se o ficheiro foi aberto corretamente
    printf("sendo realizado");
        for (int i = 0; i < nRealizadas; i++) { // Percorrer as tarefas realizadas
            Tarefa *tarefa = &tarefasRealizadas[i]; // Obter a tarefa atual
            fprintf(fp, "ID: %d\n", tarefa->id);    // Escrever o ID da tarefa no ficheiro
            fprintf(fp, "Descrição: %s\n", tarefa->descricao);  // Escrever a descrição da tarefa no ficheiro
            fprintf(fp, "Prioridade: %d\n", tarefa->prioridade);    // Escrever a prioridade da tarefa no ficheiro
    //        fprintf(fp, "Data criação: %d\n", tarefa->prioridade);    // Escrever a prioridade da tarefa no ficheiro
            // Escrever outros campos da tarefa
            fprintf(fp, "\n");  // Escrever uma linha em branco
        }
        fclose(fp); // Fechar o ficheiro
        printf("Tarefas salvas com sucesso!\n");    // Exibir mensagem de sucesso
    } else {    // Se houver um erro ao abrir o ficheiro
        printf("Erro ao abrir o arquivo para salvar as tarefas!\n");    // Exibir mensagem de erro
    }
}

void carregarTarefasDoFicheiro(PriorityQueue *q, Stack *lowPriorityStack, const char *ficheiroEntrada) {    // Função para carregar as tarefas de um ficheiro
    FILE *fp = fopen(ficheiroEntrada, "r"); // Abrir o ficheiro para leitura
    if (fp != NULL) {   // Verificar se o ficheiro foi aberto corretamente
        while (!feof(fp)) { // Enquanto não for o final do ficheiro
            Tarefa *tarefa = (Tarefa*)malloc(sizeof(Tarefa));   // Alocar memória para uma nova tarefa
            if (fscanf(fp, "ID: %d\n", &tarefa->id) != 1) break;    // Ler o ID da tarefa
            fscanf(fp, "Descrição: %[^\n]\n", tarefa->descricao);   // Ler a descrição da tarefa
            fscanf(fp, "Prioridade: %d\n", &tarefa->prioridade);    // Ler a prioridade da tarefa
            // Ler outros campos da tarefa se necessário

            // Inicializar outros campos da tarefa
            time_t tempoAtual = time(NULL); // Obter o tempo atual
            tarefa->dataCriacao = *localtime(&tempoAtual);        // Atribuir a data/hora atual à data de criação da tarefa

            tarefa->dataConclusao = (struct tm){0};  // Tarefa ainda não concluída

            tarefa->estado = 0; // Tarefa em espera

            enqueue(q, tarefa); // Adicionar a tarefa à fila de prioridade
            pushLowPriorityTasks(lowPriorityStack, tarefa);   // Adicionar a tarefa à pilha de tarefas de baixa prioridade
        }
        fclose(fp); // Fechar o ficheiro
        printf("Tarefas carregadas com sucesso!\n");    // Exibir mensagem de sucesso
    } else {    // Se houver um erro ao abrir o ficheiro
        printf("Erro ao abrir o arquivo para carregar as tarefas!\n");  // Exibir mensagem de erro
    }
}

void buscarTarefaPorID(PriorityQueue *q, int id) {  // Função para buscar uma tarefa por ID
    Node *temp = q->front;  // Criar um nó temporário para percorrer a fila
    while (temp != NULL) {  // Enquanto houver elementos na fila
        if (temp->dados->id == id) {    // Verificar se o ID da tarefa corresponde ao ID fornecido
            printf("Tarefa encontrada:\n");   // Exibir mensagem de sucesso
            printf("ID: %d\n", temp->dados->id);    // Exibir o ID da tarefa
            printf("Descrição: %s\n", temp->dados->descricao);  // Exibir a descrição da tarefa
            printf("Prioridade: %d\n", temp->dados->prioridade);    // Exibir a prioridade da tarefa
            // Exibir outros campos da tarefa
            return;
        }
        temp = temp->next;  // Avançar para o próximo nó
    }
    printf("Tarefa com ID %d não encontrada.\n", id);   // Exibir mensagem de erro
}

void listarTarefas(PriorityQueue *q) {  // Função para listar as tarefas na fila de prioridade
    Node *temp = q->front;  // Criar um nó temporário para percorrer a fila
    if (!temp) {    // Verificar se a fila está vazia
        printf("Nenhuma tarefa na fila.\n");    // Exibir mensagem de erro
        return; // Retornar da função
    }
    while (temp != NULL) {  // Enquanto houver elementos na fila
        printf("ID: %d\n", temp->dados->id);    // Exibir o ID da tarefa
        printf("Descrição: %s\n", temp->dados->descricao);  // Exibir a descrição da tarefa
        printf("Prioridade: %d\n", temp->dados->prioridade);    // Exibir a prioridade da tarefa
        // Exibir outros campos da tarefa
        printf("\n");   // Exibir uma linha em branco
        temp = temp->next;  // Avançar para o próximo nó
    }
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
                criarTarefa(novaTarefa);    // Criar a nova tarefa
                enqueue(q, novaTarefa); // Adicionar a nova tarefa à fila de prioridade
                pushLowPriorityTasks(lowPriorityStack, novaTarefa);  // Adicionar a nova tarefa à pilha de tarefas de baixa prioridade
                break;  // Sair do switch
            }
            case 2: {
                // Executar tarefa
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
                    tarefasRealizadas[nRealizadas++] = *tarefa; // Adicionar a tarefa ao array de tarefas realizadas
                    printf("Tarefa concluída.\n");  // Exibir mensagem de conclusão
                }
                break;  // Sair do switch
            }
            case 3: {
                // Buscar tarefa por ID
                int id; // Variável para armazenar o ID da tarefa a buscar
                printf("Digite o ID da tarefa a buscar: "); // Solicitar ao usuário o ID da tarefa
                scanf("%d", &id);   // Ler o ID da tarefa
                limparBuffer(); // Limpar o buffer de entrada
                buscarTarefaPorID(q, id);   // Buscar a tarefa na fila de prioridade
                break;  // Sair do switch
            }
            case 4: {
                // Listar tarefas
                listarTarefas(q);   // Listar as tarefas na fila de prioridade
                break;  // Sair do switch
            }
            case 5:
                // Relatório
                printf("Tarefas realizadas:\n");    // Exibir o título do relatório
                for (int i = 0; i < nRealizadas; i++) { // Percorrer as tarefas realizadas
                    printf("ID: %d\n", tarefasRealizadas[i].id);    // Exibir o ID da tarefa
                    printf("Descrição: %s\n", tarefasRealizadas[i].descricao);  // Exibir a descrição da tarefa
                    printf("Prioridade: %d\n", tarefasRealizadas[i].prioridade);    // Exibir a prioridade da tarefa
                    // Exibir outros campos da tarefa se necessário
                    printf("\n");   // Exibir uma linha em branco
                }
                break;
            case 6: {
                // Salvar tarefas em ficheiro
                salvarTarefasEmFicheiro(tarefasRealizadas, nRealizadas, "tarefas.txt");   // Salvar as tarefas realizadas em um ficheiro
                break;  // Sair do switch
            }
            case 7:
                // Carregar tarefas do ficheiro
                carregarTarefasDoFicheiro(q, lowPriorityStack, "tarefas.txt");  // Carregar as tarefas do ficheiro
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
