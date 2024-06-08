#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
//#include <jansson.h>
#include <locale.h>

#define DESC 300

typedef struct Tarefa {
    int id;
    char descricao[DESC];
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




//Declaração das Funções
void criarTarefa(Tarefa *tarefa);
void destruirTarefa(Tarefa *tarefa);
void limparBuffer();

typedef struct Pilha {
  Tarefa *topo;
  int tamanho;
} Pilha;



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

    printf("Digite a prioridade da tarefa (1 - alto, 0 - baixo): ");
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

	
	
	
	
//Função para salvar as tarefas

void salvarTarefasEmFicheiro(Tarefa *tarefasRealizadas, int nRealizadas, char *ficheiroSaida) {
  FILE *fp = fopen("Tarefa.txt", "w");

  if (fp != NULL) {
    for (int i = 0; i < nRealizadas; i++) {
      Tarefa *tarefa = &tarefasRealizadas[i];

      fprintf(fp, "ID: %d\n", tarefa->id);
      fprintf(fp, "Descrição: %s\n", tarefa->descricao);
      fprintf(fp, "Prioridade: %d\n", tarefa->prioridade);
      // ... (Escrever outros campos da tarefa)
      fprintf(fp, "\n");
    }

    fclose(fp);
    printf("Tarefas salvas com sucesso!\n");
  } else {
    printf("Erro ao abrir o arquivo para salvar as tarefas!\n");
  }
}
	
	
	
	
	
	
	
	
int main() {
    setlocale(LC_ALL, "Portuguese");

    while (1) {
        printf("\nMenu Principal:\n");
        printf("1. Registrar Tarefa\n");
        printf("2. Processar Tarefa\n");
        printf("3. Buscar Tarefa por ID\n");
        printf("4. Listar Tarefas\n");
        printf("5. Gerar Relatório\n");
        printf("6. Salvar Tarefas em Ficheiro\n");
        printf("7. Carregar Tarefas do Ficheiro\n");
        printf("0. Sair\n");

        int escolha;
        printf("Digite sua escolha: ");
        scanf("%d", &escolha);
        limparBuffer();

        switch (escolha) {
            case 1: {
                Tarefa *novaTarefa = (Tarefa*)malloc(sizeof(Tarefa));
                criarTarefa(novaTarefa);
                // inserirFila(&fila, novaTarefa); // Inserir na fila ou pilha, de acordo com a prioridade
                break;
            }
            case 2:
                // Processar Tarefa
                break;
            case 3:
                // Buscar Tarefa por ID
                break;
            case 4:
                // Listar Tarefas
                break;
            case 5:
                // Gerar Relatório
                break;
            case 6:
                // Salvar Tarefas em Ficheiro
                break;
            case 7:
                // Carregar Tarefas do Ficheiro
                break;
            case 0:
                printf("Saindo do programa...\n");
                exit(0);
            default:
                printf("Opção inválida!\n");
        }
    }

    return 0;
}
