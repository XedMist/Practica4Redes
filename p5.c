#include <bits/types/struct_timeval.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/syscall.h>
#include <sys/time.h>
#define NHILOS 16
#define ITERACIONES 10e9
#define gettid() ((pid_t)syscall(SYS_gettid))
//int iteraciones[NHILOS];
typedef struct{
    int indice;
    long double resultado;
} datosHilos;
void *mainHilos(void * datos){
    int indice = ((datosHilos*)datos)->indice;
    printf("[TH%d-%d] Creado...\n",gettid(),indice);
    double ciclos = ITERACIONES/(NHILOS*10);
    for(unsigned long i = 0;i < ciclos;i++){
        for(unsigned long j = indice*10 + i*NHILOS*10;j < 10+indice*10 + i*NHILOS*10;j++){
            //iteraciones[(int)p] += 1;
            ((datosHilos*)datos)->resultado += 1./(pow(16.,j)) * ( 4./(8.*j+1.) -2./(8.*j+4.) -1./(8.*j+5.) -1./(8.*j+6.) );
        }

    }
    printf("[TH%d-%d] Acabando...\n",gettid(),indice);
    pthread_exit(0);
}
float time_diff(struct timeval inicio, struct timeval final){
    return (final.tv_sec - inicio.tv_sec +(final.tv_usec - inicio.tv_usec) / 1.e6) * 1.e3;
}

int main(){
    
    pthread_t th[NHILOS];
    datosHilos* datos = (datosHilos*)calloc(NHILOS,sizeof(datosHilos));  
    struct timeval inicio, fin;

    for(int i = 0; i < NHILOS;i++){
        datos[i].indice = i;
        if(pthread_create(&th[i],NULL,mainHilos,(void *) &datos[i]) != 0){
            perror("No pude crear el hilo");
            exit(EXIT_FAILURE);
        }
    }
    gettimeofday(&inicio, NULL);
    for(int i = 0;i< NHILOS;i++){
        pthread_join(th[i],NULL);

    }
    gettimeofday(&fin, NULL);
    long double sumaHilos = 0;
    for(int i = 0;i< NHILOS;i++){
        sumaHilos +=datos[i].resultado; 
    }
    float tiempoHilos = time_diff(inicio, fin);
    printf("Los hilos han tardado %.16f ms y el resultado es %.19Lf\n", tiempoHilos,sumaHilos);
    free(datos);
    gettimeofday(&inicio, NULL);
    long double sumaMain = 0;
    for(unsigned long i = 0;i < ITERACIONES;i++){
        sumaMain += 1./(pow(16.,i)) * ( 4./(8.*i+1.) -2./(8.*i+4.) -1./(8.*i+5.) -1./(8.*i+6.) );
    }
    gettimeofday(&fin, NULL);
    float tiempoMain = time_diff(inicio, fin);
    printf("El proceso principal ha tardado %.16f ms y el resultado es %.19Lf\n", tiempoMain,sumaMain);
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
