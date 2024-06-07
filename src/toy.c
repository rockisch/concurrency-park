/**
 * Esse arquivo tem como objetivo a implementação de um brinquedo em um parque.
 * Você pode criar novas funções dentro desse arquivo caso julgue necessário.
 * ATENÇÃO: NÃO APAGUE OU EDITE O NOME OU O(S) ARGUMENTO(S) DAS FUNÇÕES FORNECIDAS
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#include "toy.h"
#include "shared.h"


// Thread que o brinquedo vai usar durante toda a simulacao do sistema
void *turn_on(void *args){
    toy_t *self = (toy_t*) args;

    debug("[ON] - O brinquedo  [%d] foi ligado.\n", self->id); // Altere para o id do brinquedo

    pthread_mutex_lock(&self->mutex);
    while (1) {
        if (self->waiting) {
            // Check if there was a client already waiting
            struct timespec wait_time;
            wait_time.tv_sec = time(NULL) + 1;
            while (1) {
                // Wait for either the max capacity, or for a fixed interval
                int r = pthread_cond_timedwait(&self->wait_sig, &self->mutex, &wait_time);
                if (self->waiting == self->capacity || r > 0) break;
            }
            int total = self->waiting < self->capacity ? self->waiting : self->capacity;
            self->waiting -= total;
            // Signal clients that the ride will start
            for (int i = 0; i < total; i++) {
                pthread_cond_signal(&self->start_sig);
            }
            debug("[TOY] Starting ride [%d] with [%d] clients\n", self->id, total);
            sleep(1);
            debug("[TOY] Ending ride [%d]\n", self->id);
            // Signal clients that the ride ended
            for (int i = 0; i < total; i++) {
                sem_post(&self->exit_sem);
            }
        } else {
            // If there were no clients waiting, wait for a signal
            pthread_cond_wait(&self->wait_sig, &self->mutex);
            // If we received a signal, but there are no people waiting, we're closing
            if (self->waiting == 0) {
                break;
            }
        }
    }
    pthread_mutex_unlock(&self->mutex);

    debug("[OFF] - O brinquedo [%d] foi desligado.\n", self->id); // Altere para o id do brinquedo

    pthread_exit(NULL);
}


// Essa função recebe como argumento informações e deve iniciar os brinquedos.
void open_toys(toy_args *args){
    global_toys_n = args->n;
    global_toys = args->toys;
    for (int i = 0; i < args->n; i++) {
        toy_t *toy = args->toys[i];
        pthread_create(&toy->thread, NULL, turn_on, toy);
    }
}

// Desligando os brinquedos
void close_toys(){
    for (int i = 0; i < global_toys_n; i++) {
        toy_t *toy = global_toys[i];
        pthread_mutex_lock(&toy->mutex);
        // Signal the toy to close
        pthread_cond_signal(&toy->wait_sig);
        pthread_mutex_unlock(&toy->mutex);
    }
    for (int i = 0; i < global_toys_n; i++) {
        toy_t *toy = global_toys[i];
        pthread_join(toy->thread, NULL);
    }
}
