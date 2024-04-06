/*<<<<<<< HEAD
bool deteccion_deadlock(){

}


bool encontrar_circulo(deadlock_kernel *a_evaluar, t_list *lista, t_list *lista_aux)
{
    list_add(lista_aux, (void *)a_evaluar);
    int *index = malloc(sizeof(int));
    //printf("entro a encontrar circulo\n");
    if (comparar_lista(a_evaluar, lista_aux, index)) // busca en la auxiliar si se cierra con atenriores
    {                                                //printf(" encontrar circulo: busca en lista auxiliar a capincho %d\n", a_evaluar->pid);
        free(index);
        return true;
    }
    if (comparar_lista(a_evaluar, lista, index))
    {
        //printf(" encontrar circulo: busca en lista de posibles deadlocka capincho %d\n", a_evaluar->pid);
        a_evaluar = list_remove(lista, *index);
        free(index);
        encontrar_circulo(a_evaluar, lista, lista_aux);
    }
    else
    {
        free(index);
        return false;
    }
}
=======

/*bool hayDeadlock(){

}*/
