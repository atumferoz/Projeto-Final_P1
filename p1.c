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

// Define o struct Node
typedef struct Node {
    Tarefa* dados;
    int prioridade;
    struct Node* prev;
    struct Node* next;
} Node;

void criartarefa(Tarefa *tarefa) {
  // Alocar memória para a estrutura Tarefa
  tarefa = malloc(sizeof(Tarefa));

  if (!tarefa) {
    printf("Erro ao alocar memória para a tarefa!\n");
    return;
  }

  // Solicitar e armazenar os dados da tarefa do usuário
  printf("Digite o ID da tarefa: ");
  scanf("%d", &tarefa->id);

  printf("Digite a descrição da tarefa: ");
  scanf(" %[^\n]", tarefa->descricao);

  printf("Digite a prioridade da tarefa (1 - alto, 0 - baixo, 2 - médio): ");
  scanf("%d", &tarefa->prioridade);
}

priorityqueue(Node** head, Tarefa* tarefa) { // Fila de prioridade
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
void enqueue(priorityqueue* q, Tarefa* tarefa) {
    Node* newNode = malloc(sizeof(Node));
    newNode->dados = tarefa;
    newNode->prioridade = tarefa->prioridade;
    newNode->next = NULL;

    if (q->front == NULL || q->front->prioridade < newNode->prioridade) {
        newNode->next = q->front;
        q->front = newNode;
    } else {
        Node* temp = q->front;
        while (temp->next != NULL && temp->next->prioridade >= newNode->prioridade)
            temp = temp->next;
        newNode->next = temp->next;
        temp->next = newNode;
    }
}


