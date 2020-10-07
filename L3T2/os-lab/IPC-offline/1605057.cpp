#include<iostream>
#include<cstdio>
#include<pthread.h>
#include<unistd.h>
#include<semaphore.h>
#include<cstring>
#include <time.h>
#include<stdlib.h>

using namespace std;

#define number_of_cycles 10
#define number_of_servicemen 3
#define number_of_consumers 10
#define arr_size 2


int paid = 0, taking_service=0;
sem_t next_cycle, force, pay, depart;
sem_t arr_of_sem[number_of_servicemen];
pthread_mutex_t mutex, leave;

void random_sleep () {
    int ms = 1 + rand() % 300;
    usleep(1000*ms);
}

void* go_to_servicing(void* arg){
    int id = *((int *)arg);
    delete (int *)arg;

    //pthread_mutex_lock(&mutex);
    //pthread_mutex_unlock(&mutex);
    sem_wait(&force);// eta 3 ta servicemen er jonno 3 diye initialize korsi
    sem_wait(&next_cycle);
    sem_wait(&arr_of_sem[0]);

    sem_post(&next_cycle);

    sem_wait(&depart);
    taking_service++;
    if(taking_service==1)
    {
        pthread_mutex_lock(&leave);
    }
    sem_post(&depart);

    for(int i=0;i<number_of_servicemen;i++)
    {

        printf("%d started taking service from serviceman %d\n",id,i+1);
        random_sleep();
        if(i+1<number_of_servicemen)
        {
            sem_wait(&arr_of_sem[i+1]);
        }
        printf("%d finished taking service from serviceman %d\n",id,i+1);

        sem_post(&arr_of_sem[i]);

        if(i+1>=number_of_servicemen)
        {
            sem_wait(&depart);
            taking_service--;
            if(taking_service==0)
            {
                pthread_mutex_unlock(&leave);
            }
            sem_post(&depart);
        }
    }
    sem_post(&force);

    //printf("%d ta ase\n\n",taking_service);

    sem_wait(&pay);
    printf("%d started paying the service bill\n",id);

    sem_wait(&next_cycle);// depart korte chay jara
    ++paid;
    if(paid==1)
    {
        sem_wait(&arr_of_sem[0]);
    }
    random_sleep();
    printf("%d finished paying the service bill\n",id);

    sem_post(&next_cycle);
    sem_post(&pay);

    pthread_mutex_lock(&leave);
    printf("%d has departed\n",id);
    pthread_mutex_unlock(&leave);

    pthread_mutex_lock(&mutex);
    paid--;
    if(paid==0)
    {
        sem_post(&arr_of_sem[0]);
    }
    pthread_mutex_unlock(&mutex);

    return NULL;
}


int main(int argc, char* argv[])
{
    srand(time(0));
    int res;

    for(int i=0;i<number_of_servicemen;i++)
    {
        int x;
        x=sem_init(&arr_of_sem[i],0,1);
        if(x !=0)
        {
            printf("Failed\n");
        }
    }

    res = sem_init(&pay,0,2);
    if(res != 0){
        printf("Failed\n");
    }
    res = sem_init(&next_cycle,0,1);
    if(res != 0){
        printf("Failed\n");
    }

    res = sem_init(&depart,0,1);
    if(res != 0){
        printf("Failed\n");
    }

    res = sem_init(&force,0,3);
    if(res != 0){
        printf("Failed\n");
    }

    res = pthread_mutex_init(&mutex,NULL);
    if(res != 0){
        printf("Failed\n");
    }

    res = pthread_mutex_init(&leave,NULL);
    if(res != 0){
        printf("Failed\n");
    }

    pthread_t cycles[number_of_cycles];
    for(int i = 0; i < number_of_cycles; i++){
        // char *id = new char[4];
        // strcpy(id, to_string(i+1).c_str());
        int *id = new int;
        *id = i+1;

        res = pthread_create(&cycles[i],NULL,go_to_servicing,(void *)id);

        if(res != 0){
            printf("Thread creation failed\n");
        }
    }

    for(int i = 0; i < number_of_cycles; i++){
        void *result;
        pthread_join(cycles[i],&result);
    }

    for(int i=0;i<number_of_servicemen;i++)
    {
        int x;
        x=sem_destroy(&arr_of_sem[i]);
        if(x !=0)
        {
            printf("Failed\n");
        }
    }

    res = sem_destroy(&pay);
    if(res != 0){
        printf("Failed\n");
    }

    res = sem_destroy(&next_cycle);
    if(res != 0){
        printf("Failed\n");
    }

    res = sem_destroy(&depart);
    if(res != 0){
        printf("Failed\n");
    }

    res = sem_destroy(&force);
    if(res != 0){
        printf("Failed\n");
    }

    res = pthread_mutex_destroy(&mutex);
    if(res != 0){
        printf("Failed\n");
    }

    res = pthread_mutex_destroy(&leave);
    if(res != 0){
        printf("Failed\n");
    }

    return 0;
}
