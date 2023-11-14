#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <math.h>
#include <sys/time.h>


#define NHILOS 16

//Tamaño de la pila de cada hilo
#define STACK_SIZE 2048*2048
#define ITERACIONES 10e9

//Estructura para guardar el estado de cada hilo
typedef struct {
    long double result;
    int child_id;
    int running;
} childState;
//Funcion que ejecutaran los hilos
int childMain(void * data){

    int indice = ((childState*)data)->child_id; 
    printf("[TH%d] Creado...\n",indice);
    
    //Calculo de la suma
    double ciclos = ITERACIONES/(NHILOS*10);
    for(unsigned long i = 0;i < ciclos;i++){
        for(unsigned long j = indice*10 + i*NHILOS*10;j < 10+indice*10 + i*NHILOS*10;j++){
            ((childState*)data)->result += 1./(pow(16.,j)) * ( 4./(8.*j+1.) -2./(8.*j+4.) -1./(8.*j+5.) -1./(8.*j+6.) );
        }

    }
    printf("[TH%d] Acabando...\n",indice);
    //Actualizacion del estado del hilo
    ((childState*)data)->running = 0;
    return 0;
}
//Funcion auxiliar para calcular el tiempo
float time_diff(struct timeval inicio, struct timeval final){
    return (final.tv_sec - inicio.tv_sec +(final.tv_usec - inicio.tv_usec) / 1.e6) * 1.e3;
}
int main(){
    //Reserva de memoria para la pila y el estado de los hilos
    void * stack;
    stack = malloc(STACK_SIZE*NHILOS);
    childState *state = malloc(sizeof(childState)*NHILOS);
    //Estructuras para medir el tiempo
    struct timeval inicio, fin;
    //Creacion de los hilos 
    for(int i=0; i<NHILOS; i++){
        state[i].running = 1;
        state[i].child_id = i;
        //ClONE_VM para que compartan la memoria
        clone(childMain, stack+STACK_SIZE*(i+1), CLONE_VM, (void*) &state[i]);
    }
    //Inicio de la medicion del tiempo
    gettimeofday(&inicio, NULL);
    //Espera a que todos los hilos acaben
    while(1){
        //Comprobacion de si todos los hilos han acabado
        int count = 0;
        for(int i=0; i<NHILOS; i++){
            if(state[i].running){
                count++;
            }
        }

        if(count == 0){
            //Si todos los hilos han acabado se sale del bucle
            break;
        }
    }
    //Fin de la medicion del tiempo
    gettimeofday(&fin, NULL);

    //Junta de los resultados de los hilos
    long double sumaHilos = 0;
    for(int i = 0;i< NHILOS;i++){
        sumaHilos +=state[i].result; 
    }
    
    float tiempoHilos = time_diff(inicio, fin);
    printf("Los hilos han tardado %.16f ms y el resultado es %.19Lf\n", tiempoHilos,sumaHilos);
    
    //Liberacion de memoria
    free(stack);
    free(state);

    //Inicio de la medicion del tiempo
    gettimeofday(&inicio, NULL);


    //Calculo secuencial
    long double sumaMain = 0;
    for(unsigned long i = 0;i < ITERACIONES;i++){
        sumaMain += 1./(pow(16.,i)) * ( 4./(8.*i+1.) -2./(8.*i+4.) -1./(8.*i+5.) -1./(8.*i+6.) );
    }
    //Fin de la medicion del tiempo
    gettimeofday(&fin, NULL);

    //Impresion de los resultados
    float tiempoMain = time_diff(inicio, fin);
    printf("El proceso principal ha tardado %.16f ms y el resultado es %.19Lf\n", tiempoMain,sumaMain);

    //Comparacion de los resultados
    printf("La diferencia es %.19Lf\n",fabsl(sumaMain-sumaHilos));
    if(tiempoMain > tiempoHilos){
        printf("Los hilos se han ahorrado %.16f ms con respecto al proceso principal\n",tiempoMain-tiempoHilos);
    }else if(tiempoMain < tiempoHilos){
        printf("El proceso principal se ha ahorrado %.16f ms con respecto a los hilos\n",tiempoHilos-tiempoMain);
    }else {
        printf("Los hilos y el proceso principal han tardado lo mismo\n");
    }
    return EXIT_SUCCESS;
}
