#ifndef __SHARED_H__
#define __SHARED_H__

#include <defs.h>

// Você pode declarar novas funções (ou variaveis compartilhadas) aqui
extern pthread_mutex_t global_gate_mutex;
extern sem_t global_gate_sem;

extern int global_clients_n;
extern client_t **global_clients;
extern int global_tickets_n;
extern ticket_t **global_tickets;
extern int global_toys_n;
extern toy_t **global_toys;


/**********************************
 *          ATENÇÃO               *
 *   NÃO EDITAR ESSAS VARIAVEIS   *
 *          ATENÇÃO               *
 *********************************/
extern Queue *gate_queue;

#endif
