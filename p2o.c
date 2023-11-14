#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <math.h>
#include <sys/time.h>
#define NHILOS 16
#define STACK_SIZE 2048*2048
#define ITERACIONES 10e9
typedef struct {
    long double result;
    int child_id;
    int running;
} childState;
int childMain(void * data){
    int indice = ((childState*)data)->child_id; 
    printf("[TH%d] Creado...\n",indice);
    double ciclos = ITERACIONES/(NHILOS*10);
    for(unsigned long i = 0;i < ciclos;i++){
        for(unsigned long j = indice*10 + i*NHILOS*10;j < 10+indice*10 + i*NHILOS*10;j++){
            ((childState*)data)->result += 1./(pow(16.,j)) * ( 4./(8.*j+1.) -2./(8.*j+4.) -1./(8.*j+5.) -1./(8.*j+6.) );
        }

    }
    printf("[TH%d] Acabando...\n",indice);
    ((childState*)data)->running = 0;
    return 0;
}
float time_diff(struct timeval inicio, struct timeval final){
    return (final.tv_sec - inicio.tv_sec +(final.tv_usec - inicio.tv_usec) / 1.e6) * 1.e3;
}
int main(){
    void * stack;
    stack = malloc(STACK_SIZE*NHILOS);
    childState *state = malloc(sizeof(childState)*NHILOS);
    struct timeval inicio, fin;
    for(int i=0; i<NHILOS; i++){
        state[i].running = 1;
        state[i].child_id = i;
        clone(childMain, stack+STACK_SIZE*(i+1), CLONE_VM, (void*) &state[i]);
    }
    //Hilos
    gettimeofday(&inicio, NULL);
    while(1){
        int count = 0;
        for(int i=0; i<NHILOS; i++){
            if(state[i].running){
                count++;
            }
        }
        if(count == 0){
            break;
        }
    }
    gettimeofday(&fin, NULL);

    long double sumaHilos = 0;
    for(int i = 0;i< NHILOS;i++){
        sumaHilos +=state[i].result; 
    }
    float tiempoHilos = time_diff(inicio, fin);
    printf("Los hilos han tardado %.16f ms y el resultado es %.19Lf\n", tiempoHilos,sumaHilos);
    free(stack);
    free(state);

    //Secuencial
    gettimeofday(&inicio, NULL);
    long double sumaMain = 0;
    for(unsigned long i = 0;i < ITERACIONES;i++){
        sumaMain += 1./(pow(16.,i)) * ( 4./(8.*i+1.) -2./(8.*i+4.) -1./(8.*i+5.) -1./(8.*i+6.) );
    }
    gettimeofday(&fin, NULL);
    float tiempoMain = time_diff(inicio, fin);
    printf("El proceso principal ha tardado %.16f ms y el resultado es %.19Lf\n", tiempoMain,sumaMain);
    
    //Comparacion
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
