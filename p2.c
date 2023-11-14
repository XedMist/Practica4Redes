#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#define NHILOS 3
#define gettid() ((pid_t)syscall(SYS_gettid))
void * mainHiloNormal(void *p){
    printf("[%d]:Soy un hilo normal\n",gettid());
    sleep(25);
    pthread_exit(0);
}

void * mainHiloFork(void *p){
    printf("[%d]:Soy el hilo fork\n",gettid());
    if(fork() == 0){
        printf("[%d]:Soy el hijo del hilo fork\n",getpid());
        sleep(20);
        printf("[%d]:Soy el hijo del hilo fork y acabo\n",getpid());
        exit(0);
    }
    printf("[%d]:Soy el hilo fork y acabo\n",getpid());
    sleep(15);
    pthread_exit(0);
}
int main(){
    pthread_t th[NHILOS];
    for(int i = 0;i<NHILOS;i++){
        switch(i){
            case 0: 
                if(pthread_create(&th[i],NULL,mainHiloFork,(void*) NULL) != 0){
                    perror("Error al crear el hilo de fork");
                    exit(1);
                }
                break;
            default:
                if(pthread_create(&th[i],NULL,mainHiloNormal,(void*) NULL) != 0){
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
