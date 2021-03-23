#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define Numero_Filosofos 5
#define esquerdo (ph_num + Numero_Filosofos - 1) % Numero_Filosofos
#define direito (ph_num + 1) % Numero_Filosofos

//Entrar os valores dos tempos de comer, pensar e sentar na mesa no argumento do "sleep".

pthread_mutex_t mutex;
pthread_cond_t condition[Numero_Filosofos];

static enum { Pensando, Faminto, Comendo } state[Numero_Filosofos];
int phil_num[Numero_Filosofos];

void *filosofando (void *arg);
void pegar_talheres(int ph_num);
void devolver_talheres(int ph_num);
void test(int ph_num);

int main(int argc, char *argv[])
{   

  int i;
  pthread_t ph_thread[Numero_Filosofos];
  pthread_mutex_init(&mutex, NULL);

  for (i = 0; i < Numero_Filosofos; i++) {
    pthread_cond_init(&condition[i], NULL);
    phil_num[i] = i;
  }

  
  for (i = 0; i < Numero_Filosofos; i++) {
    pthread_create(&ph_thread[i], NULL, filosofando, &phil_num[i]);
    printf("Filosofo #%d senta na mesa.\n", i + 1);
    sleep(0.0001);
  }
  for (i = 0; i < Numero_Filosofos; i++)
    pthread_join(ph_thread[i], NULL);

  
  pthread_mutex_destroy(&mutex);
  for (i = 0; i < Numero_Filosofos; i++)
    pthread_cond_destroy(&condition[i]);

  return(0);
}

void *filosofando(void *arg)
{

  while(1) {
    int *ph_num = arg;
    printf("Filosofo #%d comeca a pensar.\n", *ph_num + 1);
    sleep(0.0002);
    pegar_talheres(*ph_num);
    devolver_talheres(*ph_num);
  }
}

void pegar_talheres(int ph_num) {
  pthread_mutex_lock(&mutex);

  printf("Filosofo #%d esta com fome. Ele tenta pegar o talher.\n", ph_num + 1);
  state[ph_num] = Faminto;
  test(ph_num);
  while (state[ph_num] != Comendo) 
    pthread_cond_wait(&condition[ph_num], &mutex);

  pthread_mutex_unlock(&mutex);
}

void devolver_talheres(int ph_num) {
  pthread_mutex_lock(&mutex);

  printf("Filosofo #%d pega o talher. Ele entao pergunta se seu vizinho esta com fome.\n", ph_num + 1);
  state[ph_num] = Pensando;
  test(esquerdo);
  test(direito);

  pthread_mutex_unlock(&mutex);
}

void test(int ph_num) {
	
  if (state[ph_num] == Faminto && 
      state[esquerdo] != Comendo && 
      state[direito] != Comendo) {
    printf("Filosofo #%d comeca a comer.\n", ph_num + 1);
    state[ph_num] = Comendo;
    sleep(0.0003);
    pthread_cond_signal(&condition[ph_num]);
  }
}

