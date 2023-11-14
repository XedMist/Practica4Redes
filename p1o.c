#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <math.h>
#include <sys/wait.h>
#define ITERACIONES 10e9
#define NPROCESOS 16
float time_diff(struct timeval inicio, struct timeval final){
    return (final.tv_sec - inicio.tv_sec +(final.tv_usec - inicio.tv_usec) / 1.e6) * 1.e3;
}
int main(){
    char ** nombres = malloc(sizeof(char*)*NPROCESOS);
    struct timeval inicio,final;
    for(int i = 0;i <NPROCESOS;i++){
        char * nombre = malloc(sizeof(char)*30);
        sprintf(nombre,"P%d-%d",getpid(),i);
        nombres[i] = nombre;
    }
    gettimeofday(&inicio, NULL);
    for(int i = 0; i < NPROCESOS; i++){
        if(fork() == 0){
            FILE *f = fopen(nombres[i],"w");
            double ciclos = ITERACIONES/(NPROCESOS*10);
            long double resultadoParcial = 0;
            for(unsigned long j = 0;j < ciclos;j++){
                for(unsigned long k = i*10 + j*NPROCESOS*10;k < 10+i*10 + j*NPROCESOS*10;k++){
                    resultadoParcial+= 1./(pow(16.,k)) * ( 4./(8.*k+1.) -2./(8.*k+4.) -1./(8.*k+5.) -1./(8.*k+6.) );
                }

            }
            fprintf(f,"%.19Lf\n",resultadoParcial);
            fclose(f);
            exit(EXIT_SUCCESS);
        }
    }
    for(int i = 0;i < NPROCESOS;i++){
        wait(NULL);
    }
    long double sumaProcesos = 0;
    FILE *f;
    for(int i = 0;i < NPROCESOS;i++){
        long double resultadoParcial;
        if((f = fopen(nombres[i],"r")) == NULL){
            printf("Error al abrir el archivo %s\n",nombres[i]);
            exit(EXIT_FAILURE);
        }
        if(fscanf(f,"%Lf",&resultadoParcial) == EOF){
            printf("Error al leer el archivo %s\n",nombres[i]);
            exit(EXIT_FAILURE);

        }
        sumaProcesos += resultadoParcial;
        fclose(f);
        if(remove(nombres[i]) != 0){
            perror("Error");
            exit(EXIT_FAILURE);
        }
        free(nombres[i]);
    }
    gettimeofday(&final, NULL);
    float tiempoProcesos = time_diff(inicio, final);
    printf("Los procesos han tardado %.16f ms y el resultado es %.19Lf\n", tiempoProcesos,sumaProcesos);
    free(nombres);
    gettimeofday(&inicio, NULL);
    long double sumaMain = 0;
    for(unsigned long i = 0;i < ITERACIONES;i++){
        sumaMain += 1./(pow(16.,i)) * ( 4./(8.*i+1.) -2./(8.*i+4.) -1./(8.*i+5.) -1./(8.*i+6.) );
    }
    gettimeofday(&final, NULL);
    float tiempoMain = time_diff(inicio, final);
    printf("El proceso principal ha tardado %.16f ms y el resultado es %.19Lf\n", tiempoMain,sumaMain);
    printf("La diferencia es %.19Lf\n",fabsl(sumaMain-sumaProcesos));

    if(tiempoMain > tiempoProcesos){
        printf("Los procesos se han ahorrado %.16f ms con respecto al proceso principal\n",tiempoMain-tiempoProcesos);
    }else if(tiempoMain < tiempoProcesos){
        printf("El proceso principal se ha ahorrado %.16f ms con respecto a los procesos\n",tiempoProcesos-tiempoMain);
    }else {
        printf("Los procesos y el proceso principal han tardado lo mismo\n");
    }
    return EXIT_SUCCESS;
}
