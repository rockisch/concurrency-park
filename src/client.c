/**
 * Esse arquivo tem como objetivo a implementação do cliente em um parque.
 * Você pode criar novas funções dentro desse arquivo caso julgue necessário.
 * ATENÇÃO: NÃO APAGUE OU EDITE O NOME OU O(S) ARGUMENTO(S) DAS FUNÇÕES FORNECIDAS
*/

#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "client.h"
#include "queue.h"
#include "shared.h"

// Funcao onde o cliente compra as moedas para usar os brinquedos
void buy_coins(client_t *self){
    self->coins = rand() % MAX_COINS;
}

// Função onde o cliente espera a liberacao da bilheteria para adentrar ao parque.
void wait_ticket(client_t *self){
    // Wait for a signal from the ticket thread that is handling your client ID
    sem_wait(&self->enter_sig);
}

// Funcao onde o cliente entra na fila da bilheteria
void queue_enter(client_t *self){
    // Add ourselves to the queue
    pthread_mutex_lock(&global_gate_mutex);
    enqueue(gate_queue, self->id);
    pthread_mutex_unlock(&global_gate_mutex);

    // Let ticket thread know that 1 change was made to the queue
    sem_post(&global_gate_sem);

    debug("[WAITING] - Turista [%d] entrou na fila do portao principal\n", self->id);

    wait_ticket(self);

    buy_coins(self);

    debug("[CASH] - Turista [%d] comprou [%d] moedas.\n", self->id, self->coins);
}

// Thread que implementa o fluxo do cliente no parque.
void *enjoy(void *arg){
    client_t *self = (client_t*)arg;

    queue_enter(self);

    while (self->coins > 0) {
        toy_t *toy = self->toys[rand() % self->number_toys];
        // Since we don't know whether the toy is waiting on 'wait_sig', we use an outer mutex
        // and increase the number of people waiting ourselves. This way, when the toy is able
        // to get the mutex it'll either know there are already people waiting, or it'll get woken
        // by the signaling of 'wait_sig' we do below.
        pthread_mutex_lock(&toy->mutex);
        debug("[CLIENT] Client [%d] waiting for toy [%d]\n", self->id, toy->id);
        toy->waiting += 1;
        pthread_cond_signal(&toy->wait_sig);
        // Wait for toy to start the ride
        pthread_cond_wait(&toy->start_sig, &toy->mutex);
        self->coins--;
        pthread_mutex_unlock(&toy->mutex);
        // Wait for the ride to end
        sem_wait(&toy->exit_sem);
    }

    debug("[EXIT] - O turista saiu do parque.\n");
    pthread_exit(NULL);
}

// Essa função recebe como argumento informações sobre o cliente e deve iniciar os clientes.
void open_gate(client_args *args){
    global_clients = args->clients;
    global_clients_n = args->n;

    for (int i = 0; i < args->n; i++) {
        client_t *client = args->clients[i];
        pthread_create(&client->thread, NULL, enjoy, client);
    }
}

// Essa função deve finalizar os clientes
void close_gate(){
    for (int i = 0; i < global_clients_n; i++) {
        client_t *client = global_clients[i];
        pthread_join(client->thread, NULL);
    }
}
