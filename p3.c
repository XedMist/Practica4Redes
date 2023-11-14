#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define NHILOS 3

void * mainHilos(){
    printf("Hilo Genérico duerme 20s\n");
    sleep(20);
    pthread_exit(0);
}
void * mainHiloExit(){
    printf("Hilo Exit duerme 5s\n");
    sleep(5);
    printf("Hilo Exit hace exit\n");
    exit(0);
}
int main(){

    pthread_t th[NHILOS];

    for(int i = 0 ; i < NHILOS;i++){
        switch(i){
            case 0:
                if(pthread_create(&th[i],NULL,mainHiloExit,(void *) NULL) != 0){
                    perror("Error al crear un hilo");
                    exit(1);
                }
                break;
            default:
                if(pthread_create(&th[i],NULL,mainHilos,(void *)NULL) != 0){
                    perror("Error al crear un hilo");
                    exit(1);
                }
        }
    }
    for(int i = 0; i< NHILOS; i++){
        pthread_join(th[i],NULL);
    }
}
