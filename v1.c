#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
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

typedef struct Node {
    Tarefa tarefa;
    struct Node *next;
} Node;

typedef struct {
    Node *front;
    Node *rear;
} Fila;

typedef struct {
    Node *top;
} Pilha;

Fila filaAltaPrioridade = {NULL, NULL};
Pilha pilhaBaixaPrioridade = {NULL};

Tarefa tarefasRealizadas[100]; // Assume a maximum of 100 completed tasks
int nRealizadas = 0;

// Declaração das Funções
void criarTarefa(Tarefa *tarefa);
void destruirTarefa(Tarefa *tarefa);
void limparBuffer();
void salvarTarefasEmFicheiro();
void carregarTarefasDeFicheiro();
void listarTarefas();
void listarTarefasRealizadas();
void listarTarefasEmEspera();
void marcarTarefaComoConcluida(Tarefa *tarefa);
void buscarTarefaPorID(int id);
void OrdenaTarefasPorID();
void processarTarefaPrioridade();
void processarTarefa();
void inserirFila(Fila *fila, Tarefa tarefa);
Tarefa removerFila(Fila *fila);
void empilhar(Pilha *pilha, Tarefa tarefa);
Tarefa desempilhar(Pilha *pilha);

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
    tarefa->descricao[strcspn(tarefa->descricao, "\n")] = 0; // Remove o ultimo carater

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

    // Inserir a tarefa na fila ou na pilha com base na prioridade
    if (tarefa->prioridade) {
        inserirFila(&filaAltaPrioridade, *tarefa);
    } else {
        empilhar(&pilhaBaixaPrioridade, *tarefa);
    }

    printf("Tarefa criada e salva com sucesso!\n");
}

void destruirTarefa(Tarefa *tarefa) {
    free(tarefa->payloadJSON);
    free(tarefa);
}

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void salvarTarefasEmFicheiro() {
    FILE *fp = fopen("Tarefa.txt", "w");

    if (fp != NULL) {
        for (int i = 0; i < nRealizadas; i++) {
            Tarefa *tarefa = &tarefasRealizadas[i];

            fprintf(fp, "%d;", tarefa->id);
            fprintf(fp, "%s;", tarefa->descricao);
            fprintf(fp, "%d;", tarefa->prioridade);
            // Escrever outros campos da tarefa
            fprintf(fp, "%d-%d-%d;%d:%d:%d;",
                tarefa->dataCriacao.tm_year + 1900, tarefa->dataCriacao.tm_mon + 1, tarefa->dataCriacao.tm_mday,
                tarefa->dataCriacao.tm_hour, tarefa->dataCriacao.tm_min, tarefa->dataCriacao.tm_sec);
            fprintf(fp, "%d;", tarefa->estado);
            fprintf(fp, "%s\n", tarefa->payloadJSON);
        }

        fclose(fp);
        printf("Tarefas salvas com sucesso!\n");
    } else {
        printf("Erro ao abrir o arquivo para salvar as tarefas!\n");
    }
}

void carregarTarefasDeFicheiro() {
    FILE *fp = fopen("Tarefa.txt", "r");
    if (fp == NULL) {
        fprintf(stderr, "Erro: Ocorreu um erro ao abrir o ficheiro!\n");
        return;
    }

    nRealizadas = 0;
    char linha[1500];
    while (fgets(linha, sizeof(linha), fp)) {
        tarefasRealizadas[nRealizadas].payloadJSON = (char*)malloc(1024);

        sscanf(linha, "%d;%299[^;];%d;%d-%d-%d;%d:%d:%d;%d;%1023[^\n]",
               &tarefasRealizadas[nRealizadas].id, tarefasRealizadas[nRealizadas].descricao, &tarefasRealizadas[nRealizadas].prioridade,
               &tarefasRealizadas[nRealizadas].dataCriacao.tm_year, &tarefasRealizadas[nRealizadas].dataCriacao.tm_mon, &tarefasRealizadas[nRealizadas].dataCriacao.tm_mday,
               &tarefasRealizadas[nRealizadas].dataCriacao.tm_hour, &tarefasRealizadas[nRealizadas].dataCriacao.tm_min, &tarefasRealizadas[nRealizadas].dataCriacao.tm_sec,
               &tarefasRealizadas[nRealizadas].estado, tarefasRealizadas[nRealizadas].payloadJSON);

        tarefasRealizadas[nRealizadas].dataCriacao.tm_year -= 1900;
        tarefasRealizadas[nRealizadas].dataCriacao.tm_mon -= 1;
        nRealizadas++;
    }

    fclose(fp);
    printf("Tarefas carregadas com sucesso!\n");
}

void listarTarefas() {
    printf("A listar todas as tarefas realizadas:\n");
    listarTarefasRealizadas();
    printf("\nA listar todas as tarefas em espera:\n");
    listarTarefasEmEspera();
}

void listarTarefasRealizadas() {
    for (int i = 0; i < nRealizadas; i++) {
        Tarefa *tarefa = &tarefasRealizadas[i];
        printf("ID: %d\n", tarefa->id);
        printf("Descrição: %s\n", tarefa->descricao);
        printf("Prioridade: %s\n", tarefa->prioridade ? "Alto" : "Baixo");
        printf("Data de Criação: %d-%02d-%02d %02d:%02d:%02d\n", 
               tarefa->dataCriacao.tm_year + 1900, tarefa->dataCriacao.tm_mon + 1, tarefa->dataCriacao.tm_mday,
               tarefa->dataCriacao.tm_hour, tarefa->dataCriacao.tm_min, tarefa->dataCriacao.tm_sec);
        printf("Data de Conclusão: %d-%02d-%02d %02d:%02d:%02d\n", 
               tarefa->dataConclusao.tm_year + 1900, tarefa->dataConclusao.tm_mon + 1, tarefa->dataConclusao.tm_mday,
               tarefa->dataConclusao.tm_hour, tarefa->dataConclusao.tm_min, tarefa->dataConclusao.tm_sec);
        printf("Estado: ");
        switch (tarefa->estado) {
            case 0: printf("Em espera\n"); break;
            case 1: printf("Em execução\n"); break;
            case 2: printf("Concluída\n"); break;
            case 3: printf("Falha\n"); break;
        }
        printf("Payload JSON: %s\n", tarefa->payloadJSON);
        printf("---------------------------------------\n");
    }
}

void listarTarefasEmEspera() {
    Node *current = filaAltaPrioridade.front;
    printf("Fila de Alta Prioridade:\n");
    while (current) {
        Tarefa *tarefa = &current->tarefa;
        printf("ID: %d, Descrição: %s, Prioridade: %d\n", tarefa->id, tarefa->descricao, tarefa->prioridade);
        current = current->next;
    }

    current = pilhaBaixaPrioridade.top;
    printf("Pilha de Baixa Prioridade:\n");
    while (current) {
        Tarefa *tarefa = &current->tarefa;
        printf("ID: %d, Descrição: %s, Prioridade: %d\n", tarefa->id, tarefa->descricao, tarefa->prioridade);
        current = current->next;
    }
}

void marcarTarefaComoConcluida(Tarefa *tarefa) {
    tarefa->estado = 2;
    time_t tempoAtual = time(NULL);
    struct tm *dataHoraAtual = localtime(&tempoAtual);
    tarefa->dataConclusao = *dataHoraAtual;
}

void buscarTarefaPorID(int id) {
    for (int i = 0; i < nRealizadas; i++) {
        if (tarefasRealizadas[i].id == id) {
            Tarefa *tarefa = &tarefasRealizadas[i];
            printf("Tarefa encontrada:\n");
            printf("ID: %d\n", tarefa->id);
            printf("Descrição: %s\n", tarefa->descricao);
            printf("Prioridade: %s\n", tarefa->prioridade ? "Alto" : "Baixo");
            printf("Data de Criação: %d-%02d-%02d %02d:%02d:%02d\n", 
                   tarefa->dataCriacao.tm_year + 1900, tarefa->dataCriacao.tm_mon + 1, tarefa->dataCriacao.tm_mday,
                   tarefa->dataCriacao.tm_hour, tarefa->dataCriacao.tm_min, tarefa->dataCriacao.tm_sec);
            printf("Data de Conclusão: %d-%02d-%02d %02d:%02d:%02d\n", 
                   tarefa->dataConclusao.tm_year + 1900, tarefa->dataConclusao.tm_mon + 1, tarefa->dataConclusao.tm_mday,
                   tarefa->dataConclusao.tm_hour, tarefa->dataConclusao.tm_min, tarefa->dataConclusao.tm_sec);
            printf("Estado: ");
            switch (tarefa->estado) {
                case 0: printf("Em espera\n"); break;
                case 1: printf("Em execução\n"); break;
                case 2: printf("Concluída\n"); break;
                case 3: printf("Falha\n"); break;
            }
            printf("Payload JSON: %s\n", tarefa->payloadJSON);
            return;
        }
    }
    printf("Tarefa com ID %d não encontrada.\n", id);
}

void OrdenaTarefasPorID() {
    FILE *fp = fopen("Tarefa.txt", "r");
    if (fp == NULL) {
        fprintf(stderr, "Erro: Ocorreu um erro ao abrir o ficheiro!\n");
        exit(-1);
    }

    Tarefa tarefas[300];
    int n = 0;

    while (fscanf(fp, "%d;%299[^;];%d;%d-%d-%d;%d:%d:%d;%d;%1023[^\n]",
                  &tarefas[n].id, tarefas[n].descricao, &tarefas[n].prioridade,
                  &tarefas[n].dataCriacao.tm_year, &tarefas[n].dataCriacao.tm_mon, &tarefas[n].dataCriacao.tm_mday,
                  &tarefas[n].dataCriacao.tm_hour, &tarefas[n].dataCriacao.tm_min, &tarefas[n].dataCriacao.tm_sec,
                  &tarefas[n].estado, tarefas[n].payloadJSON) != EOF) {
        tarefas[n].dataCriacao.tm_year -= 1900;
        tarefas[n].dataCriacao.tm_mon -= 1;
        n++;
    }

    fclose(fp);

    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (tarefas[j].id > tarefas[j + 1].id) {
                Tarefa temp = tarefas[j];
                tarefas[j] = tarefas[j + 1];
                tarefas[j + 1] = temp;
            }
        }
    }

    fp = fopen("Tarefa.txt", "w");
    if (fp == NULL) {
        fprintf(stderr, "Erro: Ocorreu um erro ao abrir o ficheiro!\n");
        exit(-1);
    }

    for (int i = 0; i < n; i++) {
        fprintf(fp, "%d;%s;%d;%d-%d-%d;%d:%d:%d;%d;%s\n",
                tarefas[i].id, tarefas[i].descricao, tarefas[i].prioridade,
                tarefas[i].dataCriacao.tm_year + 1900, tarefas[i].dataCriacao.tm_mon + 1, tarefas[i].dataCriacao.tm_mday,
                tarefas[i].dataCriacao.tm_hour, tarefas[i].dataCriacao.tm_min, tarefas[i].dataCriacao.tm_sec,
                tarefas[i].estado, tarefas[i].payloadJSON);
    }

    fclose(fp);

    printf("Tarefas ordenadas e salvas no ficheiro 'Tarefa.txt'.\n");
}

void processarTarefaPrioridade() {
    if (filaAltaPrioridade.front) {
        Tarefa tarefa = removerFila(&filaAltaPrioridade);
        printf("A processar tarefa de alta prioridade:\n");
        printf("ID: %d\n", tarefa.id);
        printf("Descrição: %s\n", tarefa.descricao);
        // Executar a tarefa (simulado aqui)
        printf("A executar tarefa...\n");
        marcarTarefaComoConcluida(&tarefa);
        tarefasRealizadas[nRealizadas++] = tarefa;
        printf("Tarefa concluída e registrada!\n");
    } else if (pilhaBaixaPrioridade.top) {
        Tarefa tarefa = desempilhar(&pilhaBaixaPrioridade);
        printf("A processar tarefa de baixa prioridade:\n");
        printf("ID: %d\n", tarefa.id);
        printf("Descrição: %s\n", tarefa.descricao);
        // Executar a tarefa (simulado aqui)
        printf("A executar tarefa...\n");
        marcarTarefaComoConcluida(&tarefa);
        tarefasRealizadas[nRealizadas++] = tarefa;
        printf("Tarefa concluída e registrada!\n");
    } else {
        printf("Não há tarefas a processar!\n");
    }
}

void processarTarefa() {
    processarTarefaPrioridade();
}

void inserirFila(Fila *fila, Tarefa tarefa) {
    Node *novoNo = (Node*)malloc(sizeof(Node));
    if (!novoNo) {
        printf("Erro ao alocar memória para a fila!\n");
        return;
    }
    novoNo->tarefa = tarefa;
    novoNo->next = NULL;
    if (fila->rear) {
        fila->rear->next = novoNo;
    } else {
        fila->front = novoNo;
    }
    fila->rear = novoNo;
}

Tarefa removerFila(Fila *fila) {
    Tarefa tarefa;
    if (fila->front) {
        Node *temp = fila->front;
        tarefa = temp->tarefa;
        fila->front = fila->front->next;
        if (!fila->front) {
            fila->rear = NULL;
        }
        free(temp);
    } else {
        printf("Fila está vazia!\n");
        tarefa.id = -1; // Indica que a fila está vazia
    }
    return tarefa;
}

void empilhar(Pilha *pilha, Tarefa tarefa) {
    Node *novoNo = (Node*)malloc(sizeof(Node));
    if (!novoNo) {
        printf("Erro ao alocar memória para a pilha!\n");
        return;
    }
    novoNo->tarefa = tarefa;
    novoNo->next = pilha->top;
    pilha->top = novoNo;
}

Tarefa desempilhar(Pilha *pilha) {
    Tarefa tarefa;
    if (pilha->top) {
        Node *temp = pilha->top;
        tarefa = temp->tarefa;
        pilha->top = pilha->top->next;
        free(temp);
    } else {
        printf("Pilha está vazia!\n");
        tarefa.id = -1; // Indica que a pilha está vazia
    }
    return tarefa;
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
        printf("8. Ordenar Tarefas por ID\n");
        printf("0. Sair\n");

        int escolha;
        printf("Digite sua escolha: ");
        scanf("%d", &escolha);
        limparBuffer();

        switch (escolha) {
            case 1: {
                Tarefa *novaTarefa = (Tarefa*)malloc(sizeof(Tarefa));
                criarTarefa(novaTarefa);
                destruirTarefa(novaTarefa);
                break;
            }
            case 2:
                processarTarefa();
                break;
            case 3:
                // Buscar Tarefa por ID
                int num;
                printf("Digite o número da tarefa que deseja buscar: ");
                scanf("%d", &num);
                limparBuffer();
                buscarTarefaPorID(num);
                break;
            case 4:
                listarTarefas();
                break;
            case 5:
                // Gerar Relatório
                break;
            case 6:
                salvarTarefasEmFicheiro();
                break;
            case 7:
                carregarTarefasDeFicheiro();
                break;
            case 8:
                OrdenaTarefasPorID();
                break;
            case 0:
                exit(0);
            default:
                printf("Opção inválida!\n");
        }
    }

    return 0;
}
