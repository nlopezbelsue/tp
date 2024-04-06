#ifndef _DEADLOCK_
#define _DEADLOCK_

void deadlock();
void deteccion_deadlock();
void lista_deadlock_destroyer(void *arg);
deadlock_kernel *buscar_en_otras_listas(int pid, int index, char *semaforo_retenido);
t_list *verificar_espera_circular(t_list *lista);
bool encontrar_circulo(deadlock_kernel *a_evaluar, t_list *lista, t_list *lista_aux);
bool comparar_lista(deadlock_kernel *alf, t_list *list, int *index);
void finalizar_involucrados(t_list *lista_deadlock);
void sacar_de_cola_bloqueados(sem_kernel *semaforo, int id);

#endif
