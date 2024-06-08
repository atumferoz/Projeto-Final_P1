#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
//#include <jansson.h>
#include <locale.h>

#define DESC 300


typedef struct Tarefa {
  int id;
  char *descricao;
  int prioridade; // 1: alto, 0: baixo
  struct tm dataCriacao;
  struct tm dataConclusao;
  int estado; // 0: em espera, 1: em execução, 2: concluída, 3: falha
  char *payloadJSON;
} Tarefa;




/*typedef struct PriorityQueue { // Fila de prioridade
    int size;
    int capacity;
    int* array;
} PriorityQueue;*/


typedef struct Pilha {
  Tarefa *topo;
  int tamanho;
} Pilha;



void criarTarefa(Tarefa *tarefa) {
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

  printf("Digite a prioridade da tarefa (1 - alto, 0 - baixo): ");
  scanf("%d", &tarefa->prioridade);

}

void destruirTarefa(Tarefa *tarefa) {
  free(tarefa->descricao);
  free(tarefa->payloadJSON);
  free(tarefa);
}

/*PriorityQueue* createQueue(int capacity) {
    PriorityQueue* queue = (PriorityQueue*) malloc(sizeof(PriorityQueue));
    queue->capacity = capacity;
    queue->size = 0;
    queue->array = (int*) malloc(queue->capacity * sizeof(int));
    return queue;
}

void enqueue(PriorityQueue* queue, int item) {
    if (queue->size == queue->capacity) {
        printf("Queue Overflow\n");
        return;
    }
    // Insert the new item at the end of the queue
    int i = queue->size++;
    queue->array[i] = item;

    // Bubble up
    while (i != 0 && queue->array[i] > queue->array[(i - 1) / 2]) {
        swap(&queue->array[i], &queue->array[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

int dequeue(PriorityQueue* queue) {
    if (queue->size <= 0) {
        printf("Queue Underflow\n");
        return INT_MIN;
    }
    int max = queue->array[0];
    queue->array[0] = queue->array[--queue->size];

    // Bubble down
    int i = 0;
    while (2 * i + 1 < queue->size) {
        int j = 2 * i + 1;
        if (j + 1 < queue->size && queue->array[j] < queue->array[j + 1])
            j++;
        if (queue->array[i] >= queue->array[j])
            break;
        swap(&queue->array[i], &queue->array[j]);
        i = j;
    }*/

	
	main()
	{
		setlocale(LC_ALL, "Portuguese");

		  // Loop principal do programa
	  while (1) {
	    // Apresentar o menu de opções
	    printf("\nMenu Principal:\n");
	    printf("1. Registrar Tarefa\n");
	    printf("2. Processar Tarefa\n");
	    printf("3. Buscar Tarefa por ID\n");
	    printf("4. Listar Tarefas\n");
	    printf("5. Gerar Relatório\n");
	    printf("6. Salvar Tarefas em Ficheiro\n");
	    printf("7. Carregar Tarefas do Ficheiro\n");
	    printf("0. Sair\n");
	
	    // Obter a escolha do usuário
	    int escolha;
	    printf("Digite sua escolha: ");
	    scanf("%d", &escolha);
	
	    switch (escolha) {
	      case 1: // Registrar Tarefa
	        Tarefa *novaTarefa = malloc(sizeof(Tarefa));
	        criarTarefa(novaTarefa);
	        //inserirFila(&fila, novaTarefa); // Inserir na fila ou pilha, de acordo com a prioridade
	        break;
	
	      case 2: // Processar Tarefa
	        break;
	
	      case 3: // Buscar Tarefa por ID
	        break;
	
	      case 4: // Listar Tarefas
	        break;
	
	      case 5: // Gerar Relatório
	        break;
	
	      case 6: // Salvar Tarefas em Ficheiro
	        break;
	
	      case 7: // Carregar Tarefas do Ficheiro
	        break;
	
	      case 0: // Sair
	        // Liberar memória alocada para as estruturas de dados
	        // ...
	
	        printf("Saindo do programa...\n");
	        exit(0);
	
	      default:
	        printf("Opção inválida!\n");
	    }
	  }

  	return 0;
		
	}
