#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
int global = 0;
void *func_hilo(void * p){
    printf("[Hilo x]: Global es %d, incrementando... \n",global);
    global+=1;
    printf("[Hilo x]: i es %d\n",(int)p);
    for(;;);
}

int main(){

    pthread_t th[5];

    for(int i = 0; i< 5;i++){
        if(pthread_create(&th[i],NULL,func_hilo,(void *)i) != 0){
            perror("No se pudo crear el hilo");
            exit(1);
        }
    }

    printf("[HiloMain]: Xdd\n");
    for(int i = 0; i<5;i++){
        pthread_join(th[i],NULL);
    }
}


