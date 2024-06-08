#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
//#include <jansson.h>
#include <locale.h>

typedef struct Tarefa {
  int id;
  char *descricao;
  int prioridade; // 1: alto, 0: baixo, 2: médio
  struct tm dataCriacao;
  struct tm dataConclusao;
  int estado; // 0: em espera, 1: em execução, 2: concluída, 3: falha
  char *payloadJSON;
} Tarefa;

typedef struct PriorityQueue {
    Node* front;
   // Node* rear;
} PriorityQueue;


// Define o struct Node
typedef struct Node {
    Tarefa* dados;
   /* int prioridade;
    struct Node* prev;*/
    struct Node* next;
} Node;

typedef struct StackNode {
    Tarefa* dados;
    struct StackNode* next;
} StackNode;

typedef struct Stack {
    StackNode* top;
} Stack;



// Função para criar uma nova tarefa
void criartarefa(Tarefa *tarefa) {
    // Initialize tarefa with zeroed memory
    memset(tarefa, 0, sizeof(Tarefa));

    // Solicitar e armazenar os dados da tarefa do usuário
    printf("Digite o ID da tarefa: ");
    scanf("%d", &tarefa->id);

    printf("Digite a descrição da tarefa: ");
    scanf(" %[^\n]", tarefa->descricao);

    printf("Digite a prioridade da tarefa (1 - alto, 0 - baixo, 2 - médio): ");
    scanf("%d", &tarefa->prioridade);

    // Initialize dataCriacao with current time
    time_t now = time(NULL);
    tarefa->dataCriacao = *localtime(&now);
}

 //fila de prioridade 
PriorityQueue* createQueue() { // Criar a fila de prioridade
    PriorityQueue* q = malloc(sizeof(PriorityQueue));   // Alocar memória para a fila
    if (!q) {   // Se a alocação de memória falhar
        printf("Erro ao alocar memória para a fila de prioridade!\n");  // Exibir uma mensagem de erro
        exit(EXIT_FAILURE); // Encerrar o programa
    }
    q->front = NULL;    // O primeiro nó da fila é NULL
    return q;   // Retornar a fila
}

void enqueue(PriorityQueue* q, Tarefa* tarefa) {    // Adicionar um nó à fila
    Node* newNode = malloc(sizeof(Node));   // Alocar memória para o nó
    if (!newNode) { // Se a alocação de memória falhar
        printf("Erro ao alocar memória para o nó!\n");  // Exibir uma mensagem de erro
        exit(EXIT_FAILURE); // Encerrar o programa
    }
    newNode->dados = tarefa;    // Atribuir os dados da tarefa ao nó
    newNode->next = NULL;   // O próximo nó é NULL

    if (q->front == NULL || q->front->dados->prioridade < tarefa->prioridade) {  // Se a fila estiver vazia ou a prioridade do nó for maior que a do primeiro nó
        newNode->next = q->front;   // O próximo nó é o primeiro nó
        q->front = newNode; // O primeiro nó é o nó criado
    } else {    // Se a fila não estiver vazia
        Node* temp = q->front;  // Criar um nó temporário
        while (temp->next != NULL && temp->next->dados->prioridade >= tarefa->prioridade) { // Enquanto o próximo nó não for NULL
            temp = temp->next;  // O nó temporário é o próximo nó   
        }
        newNode->next = temp->next; // O próximo nó é o nó criado
        temp->next = newNode;   // O nó temporário é o nó criado
    }
}

// Adicionar tarefas de baixa prioridade ao stack
void pushLowPriorityTasks(Stack* stack, Tarefa* tarefa) { // Adicionar tarefas de baixa prioridade ao stack
    if (tarefa->prioridade == 0) {  // Se a prioridade da tarefa for baixa
        push(stack, tarefa);    // Adicionar a tarefa ao stack
    }
}


Tarefa* dequeue(PriorityQueue* q) { // Remover um nó da fila
    if (q->front == NULL) { // Se a fila estiver vazia
        printf("FILA VAZIA!!!\n");    // A fila está vazia
        return NULL;    // Retornar NULL
    }
    Node* temp = q->front;  // Criar um nó temporário
    Tarefa* tarefa = temp->dados;   // A tarefa é o dado do nó temporário
    q->front = q->front->next;  // O primeiro nó é o próximo nó
    free(temp); // Libetar a memória do nó temporário
    return tarefa;  // Retornar a tarefa
}

void freeQueue(PriorityQueue* q) {  // Libetar a memória da fila
    while (q->front != NULL) {  // Enquanto o primeiro nó não for NULL  
        Node* temp = q->front;  // Criar um nó temporário
        q->front = q->front->next;  // O primeiro nó é o próximo nó
        free(temp->dados);  // Libetar a memória dos dados do nó temporário
        free(temp); // Libetar a memória do nó temporário
    }
    free(q);    // Libetar a memória da fila
}

//Implementação dO STACK (PILHA)

void push(Stack* stack, Tarefa* tarefa) {   // Adicionar um nó à pilha
    StackNode* newNode = malloc(sizeof(StackNode));   // Alocar memória para o nó
    if (!newNode) { // Se a alocação de memória falhar
        printf("Erro ao alocar memória para o nó!\n");  // Exibir uma mensagem de erro
        exit(EXIT_FAILURE); // Encerrar o programa
    }
    newNode->dados = tarefa;    // Atribuir os dados da tarefa ao nó
    newNode->next = stack->top; // O próximo nó é o topo da pilha
    stack->top = newNode;   // O topo da pilha é o nó criado
}

Tarefa* pop(Stack* stack) { // Remover um nó da pilha
    if (stack->top == NULL) {   // Se a pilha estiver vazia
        printf("PILHA VAZIA!!!\n");   // A pilha está vazia
        return NULL;    // Retornar NULL
    }
    StackNode* temp = stack->top;   // Criar um nó temporário
    Tarefa* tarefa = temp->dados;   // A tarefa é o dado do nó temporário
    stack->top = stack->top->next;  // O topo da pilha é o próximo nó
    free(temp); // Libetar a memória do nó temporário
    return tarefa;  // Retornar a tarefa
}

void freeStack(Stack* stack) {  // Libetar a memória da pilha
    while (stack->top != NULL) {    // Enquanto o topo da pilha não for NULL
        StackNode* temp = stack->top;   // Criar um nó temporário
        stack->top = stack->top->next;  // O topo da pilha é o próximo nó
        free(temp->dados);  // Libetar a memória dos dados do nó temporário
        free(temp); // Libetar a memória do nó temporário
    }
}


int main() {
    PriorityQueue* q = createQueue();   // Criar a fila de prioridade
    Stack* lowPriorityStack = malloc(sizeof(Stack));        // Alocar memória para a pilha
    if (!lowPriorityStack) {    // Se a alocação de memória falhar
        printf("Erro ao alocar memória para a pilha!\n");   // Exibir uma mensagem de erro
        exit(EXIT_FAILURE); // Encerrar o programa
    }   
    lowPriorityStack->top = NULL;   // O topo da pilha é NULL

    Tarefa* tarefa1 = malloc(sizeof(Tarefa));   // Alocar memória para a tarefa
    criartarefa(tarefa1);   // Criar a tarefa
    enqueue(q, tarefa1);    // Adicionar a tarefa à fila
    pushLowPriorityTasks(lowPriorityStack, tarefa1);    // Adicionar a tarefa ao stack

    Tarefa* tarefa2 = malloc(sizeof(Tarefa));   // Alocar memória para a tarefa
    criartarefa(tarefa2);   // Criar a tarefa
    enqueue(q, tarefa2);    // Adicionar a tarefa à fila
    pushLowPriorityTasks(lowPriorityStack, tarefa2);    // Adicionar a tarefa ao stack

    Tarefa* t = dequeue(q); // Remover um nó da fila
    if (t != NULL) {    // Se a tarefa não for NULL
        printf("Tarefa ID %d com prioridade %d foi removida da fila\n", t->id, t->prioridade);  // Exibir a mensagem
        free(t);    // Libetar a memória da tarefa
    }

    Tarefa* lowPriorityTarefa = pop(lowPriorityStack);  // Remover um nó da pilha
    if (lowPriorityTarefa != NULL) {    // Se a tarefa não for NULL
        printf("Tarefa de baixa prioridade ID %d foi removida da pilha\n", lowPriorityTarefa->id);  // Exibir a mensagem
        free(lowPriorityTarefa);    // Libetar a memória da tarefa
    }

    freeQueue(q);   // Libetar a memória da fila
    freeStack(lowPriorityStack);    // Libetar a memória da pilha
    free(lowPriorityStack); // Libetar a memória da pilha
    return 0;
}



















//Está parte foi codigo escrito por mim que ão fico implrementado no codigo final e tenho o guardado para futuras referencias e saber o que fiz de errado



/*priorityqueue(Node** head, Tarefa* tarefa) { // Fila de prioridade
    Node* node = (Node*) malloc(sizeof(Node));  // Alocar memória para o nó
    node->dados = tarefa;   // Atribuir os dados da tarefa ao nó
    node->prioridade = tarefa->prioridade;  // Use the priority from the Tarefa
    node->prev = NULL;  // O nó anterior é NULL
    node->next = NULL;  // O próximo nó é NULL

    if (*head == NULL) {    // Se a fila estiver vazia
        *head = node;   // O nó é o primeiro da fila
        return; 
    }

    Node* temp = *head; // Criar um nó temporário
    while (temp->next != NULL && temp->next->prioridade >= node->prioridade)  // Enquanto o próximo nó não for NULL
        temp = temp->next;  // O nó temporário é o próximo nó
    node->next = temp->next;  // O próximo nó é o nó criado
    temp->next = node;  // O nó anterior ao nó criado é o nó temporário
}

PriorityQueue* creatQueue() {  // Criar a fila de prioridade
    PriorityQueue* q = malloc(sizeof(PriorityQueue));   // Alocar memória para a fila
    q->front = NULL;    // O primeiro nó da fila é NULL
    return q;   // Retornar a fila
}


void enqueue(PriorityQueue* q, Tarefa* tarefa) {    // Adicionar um nó à fila
    Node* newNode = malloc(sizeof(Node));   // Alocar memória para o nó
    newNode->dados = tarefa;    // Atribuir os dados da tarefa ao nó
    newNode->prioridade = tarefa->prioridade;   // Atribuir a prioridade da tarefa ao nó
    newNode->next = NULL;   // O próximo nó é NULL

    if (q->front == NULL || q->front->prioridade < newNode->prioridade) {   // Se a fila estiver vazia ou a prioridade do nó for maior que a do primeiro nó
        newNode->next = q->front;   // O próximo nó é o primeiro nó
        q->front = newNode; // O primeiro nó é o nó criado
    } else {    // Se a fila não estiver vazia
        Node* temp = q->front;  // Criar um nó temporário
        while (temp->next != NULL && temp->next->prioridade >= newNode->prioridade) // Enquanto o próximo nó não for NULL
            temp = temp->next;  // O nó temporário é o próximo nó
        newNode->next = temp->next; // O próximo nó é o nó criado
        temp->next = newNode;   // O nó temporário é o nó criado
    }
}

int dequeue(PriorityQueue* q) { // Remover um nó da fila
    if (q->front == NULL) { // Se a fila estiver vazia
        printf("Queue underflow\n");    // A fila está vazia
        exit(EXIT_FAILURE); // Encerrar o programa
    }
    Node* temp = q->front;  // Criar um nó temporário
    int item = temp->dados->id;  // O item é o ID da tarefa
    q->front = q->front->next;  // O primeiro nó é o próximo nó
    free(temp); // Libetar a memória do nó temporário
    return item;    // Retornar o item
}
*/

