
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/syscall.h>
#define NHILOS 3
#define RAICES 100000000
#define gettid() ((pid_t)syscall(SYS_gettid))

void * mainHiloGeneroso(void *p){ 
    printf("[THG%d] Empieza...\n",gettid());
    double result = 0;
    for(int i = 0; i < RAICES;i++){
        result += i;
        sched_yield();
    }
    printf("[THG%d] Resultado: %lf\n",gettid(),result);
    printf("[THG%d] Acaba...\n",gettid());
    pthread_exit(0);
}

void * mainHiloEgoista(void *p){
    printf("[THE%d] Empieza...\n",gettid());
    double result = 0;
    for(int i = 0; i < RAICES;i++){
        result += i;
    }
    printf("[THE%d] Resultado: %lf\n",gettid(),result);
    printf("[THE%d] Acaba...\n",gettid());
    pthread_exit(0);
}
int main(){
    pthread_t th[NHILOS];
    for(int i = 0;i<NHILOS;i++){
        switch(i){
            case 0: 
                if(pthread_create(&th[i],NULL,mainHiloEgoista,(void*) NULL) != 0){
                    perror("Error al crear el hilo de fork");
                    exit(1);
                }
                break;
            default:
                if(pthread_create(&th[i],NULL,mainHiloGeneroso,(void*) NULL) != 0){
                    perror("Error al crear un hilo");
                    exit(1);
                }
        }
    }
    for(int i = 0;i<NHILOS;i++){
        pthread_join(th[i],NULL);
    }
    return EXIT_SUCCESS;
}
