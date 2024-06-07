/**
 * Esse arquivo tem como objetivo a implementação da bilheteria em um parque.
 * Você pode criar novas funções dentro desse arquivo caso julgue necessário.
 * ATENÇÃO: NÃO APAGUE OU EDITE O NOME OU O(S) ARGUMENTO(S) DAS FUNÇÕES FORNECIDAS.
*/
#include <stdio.h>
#include <stdlib.h>
#include <tickets.h>
#include <unistd.h>

#include <queue.h>
#include "shared.h"


// Thread que implementa uma bilheteria
void *sell(void *args){

    debug("[INFO] - Bilheteria Abriu!\n");

    // ticket_t *self = (ticket_t*)args;
    int loop = 1;
    while (loop) {
        sem_wait(&global_gate_sem);
        // We'll get notified whenever a client is added to the queue, or when all clients left the
        // park
        pthread_mutex_lock(&global_gate_mutex);
        if (is_queue_empty(gate_queue)) {
            // If queue is empty all clients left
            loop = 0;
        } else {
            // Otherwise, we need to wake up the client thread with a signal
            int client_id = dequeue(gate_queue);
            for (int i = 0; i < global_clients_n; i++) {
                client_t *client = global_clients[i];
                if (client->id == client_id) {
                    debug("[TICKET] Allowing client [%d] in\n", client_id);
                    sem_post(&client->enter_sig);
                    break;
                }
            }
        }
        pthread_mutex_unlock(&global_gate_mutex);
    }
    debug("[INFO] - Bilheteria Fechou!\n");

    pthread_exit(NULL);
}

// Essa função recebe como argumento informações sobre a bilheteria e deve iniciar os atendentes.
void open_tickets(tickets_args *args){
    global_tickets_n = args->n;
    global_tickets = args->tickets;
    for (int i = 0; i < args->n; i++) {
        ticket_t *ticket = args->tickets[i];
        pthread_create(&ticket->thread, NULL, sell, ticket);
    }
}

// Essa função deve finalizar a bilheteria
void close_tickets(){
    for (int i = 0; i < global_tickets_n; i++) {
        sem_post(&global_gate_sem);
    }
    for (int i = 0; i < global_tickets_n; i++) {
        ticket_t *ticket = global_tickets[i];
        pthread_join(ticket->thread, NULL);
    }
}
