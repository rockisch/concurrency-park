#include "shared.h"

#include <queue.h>
#include <semaphore.h>


// Você pode declarar novas funções (ou variaveis compartilhadas) aqui
sem_t global_gate_sem; // Way to notify how many people there are in the queue
pthread_mutex_t global_gate_mutex = PTHREAD_MUTEX_INITIALIZER; // Sync 'gate_queue' access

// Because we're not allowed to edit 'main', and because the code frees memory before doing sync,
// we need to store global references to all items

// Number of clients
int global_clients_n;
// Clients list, will also be accessed by 'tickets' to locate the ID from the queue
client_t **global_clients;
// Number of tickets
int global_tickets_n;
// Tickets list
ticket_t **global_tickets;
// Number of toys
int global_toys_n;
// Toys list
toy_t **global_toys;

/**********************************
 *          ATENÇÃO               *
 *   NÃO EDITAR ESSAS VARIAVEIS   *
 *          ATENÇÃO               *
 *********************************/
Queue *gate_queue = NULL;
