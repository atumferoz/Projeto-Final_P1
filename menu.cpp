#include <stdio.h>
#include <windows.h>
int main() {

  system("title Gestor de tarefas");
  system("color 60");
  int linhas = 30; // Define o número de linhas
  int colunas = 70; // Define o número de colunas

  // Imprime a linha superior
  for (int i = 0; i < colunas; i++) {
    printf("#");
  }
  printf("\n");

  // Imprime as linhas do meio
  for (int i = 0; i < linhas / 2 - 1; i++) {
    printf("#");
    for (int j = 0; j < colunas - 2; j++) {
      printf(" ");
    }
    printf("#\n");
  }

  // Imprime a linha adicional
  for (int i = 0; i < colunas; i++) {
    printf("#");
  }
  printf("\n");

  // Imprime as linhas do meio
  for (int i = 0; i < linhas / 3 - 1; i++) {
    printf("#");
    for (int j = 0; j < colunas - 2; j++) {
      printf(" ");
    }
    printf("#\n");
  }

  // Imprime a linha inferior
  for (int i = 0; i < colunas; i++) {
    printf("#");
  }
  printf("\n");
  getchar();
  return 0;
}
