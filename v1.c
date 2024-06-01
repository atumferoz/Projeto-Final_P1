#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <jansson.h>
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



typedef struct Fila {
  Tarefa *inicio, *fim;
  int tamanho;
} Fila;

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
	
	
	
	main()
	{
		setlocale(LC_ALL, "Portuguese");
		
	}
