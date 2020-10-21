//eita amrta
#include<iostream>
#include<cstdio>
#include<pthread.h>
#include<unistd.h>
#include<semaphore.h>
#include<cstring>
#include <ctime>
#include<queue>
#include<cstdlib>

using namespace std;

#define num_of_cycles 50
#define S 3 // service_size
#define C 5 // paymentroom_capacity

pthread_mutex_t service_lock[S], depart_lock, count_lock;
sem_t paymentroom;
int dc = 0; // departure_count

void init_semaphore()
{
    int res;

    res = sem_init(&paymentroom,0,C);
    if(res != 0){
        printf("Failed\n");
    }

    res = pthread_mutex_init(&depart_lock,NULL);
    if(res != 0){
        printf("Failed\n");
    }

    res = pthread_mutex_init(&count_lock,NULL);
    if(res != 0){
        printf("Failed\n");
    }

    for(int i=0; i<S; i++)
    { 
        //cout << "ekhane" << endl;
        res = pthread_mutex_init(&service_lock[i], NULL);
        if(res != 0){
            printf("Failed\n");
        }
    }
}

void* service(void* arg)
{
    pthread_mutex_lock(&service_lock[0]);
    for(int i=1; i<S+1; i++)
    {
        //pthread_mutex_lock(&service_lock[i]);
        
        printf("1 %s started taking service from serviceman %d\n",(char*)arg, i);

        int stime = (rand() % 5000) + 1;
        usleep(1000* stime);

        printf("2 %s finished taking service from serviceman %d\n",(char*)arg, i);

        if(i < S)
        {
            pthread_mutex_lock(&service_lock[i]);
        }
        pthread_mutex_unlock(&service_lock[i-1]);
    }
    //pthread_mutex_unlock(&service_lock[S-1]);
    return NULL;
} 

void* payment(void* arg)
{
    sem_wait(&paymentroom);
    printf("3 %s started paying the service bill\n",(char*)arg); 

    int stime = (rand() % 5000) + 1;
    usleep(1000* stime);

    
    printf("4 %s finished paying the service bill\n",(char*)arg);  

    sem_post(&paymentroom);

    return NULL;
}

void* depart(void* arg)
{
    pthread_mutex_lock(&count_lock);
    dc = dc + 1;
    
    if(dc == 1)
    {
        pthread_mutex_lock(&service_lock[0]);

        for(int i=1; i<S; i++)
        {
            pthread_mutex_lock(&service_lock[i]);

            int stime = (rand() % 5000) + 1;
            usleep(1000* stime);

            pthread_mutex_unlock(&service_lock[i]);
        }

        /*for(int i=S-1; i>0; i--)
        {      
            pthread_mutex_unlock(&service_lock[i]);
        }*/
    }

    pthread_mutex_unlock(&count_lock);

    for(int i=S-1; i>0; i--)
    {
        pthread_mutex_lock(&service_lock[i]);

        int stime = (rand() % 5000) + 1;
        usleep(1000* stime);

        
        pthread_mutex_unlock(&service_lock[i]);
    }
    

    pthread_mutex_lock(&count_lock);
    dc = dc - 1;
    if(dc == 0)
    {
        pthread_mutex_unlock(&service_lock[0]);
    }
    pthread_mutex_unlock(&count_lock);

    return NULL;    
}

void* sync_ser_pay(void* arg)
{
    service(arg);
    payment(arg);
    depart(arg);

    pthread_exit((void*)strcat((char*)arg," has departed\n"));
    //return NULL;
}

void destroy_semaphore()
{
    int res;

    res = sem_destroy(&paymentroom);
    if(res != 0){
        printf("Failed\n");
    }

    res = pthread_mutex_destroy(&depart_lock);
    if(res != 0){
        printf("Failed\n");
    }

    res = pthread_mutex_destroy(&count_lock);
    if(res != 0){
        printf("Failed\n");
    }

    for(int i=0; i<S; i++)
    { 
        //cout << "ekhane" << endl;
        res = pthread_mutex_destroy(&service_lock[i]);
        if(res != 0){
            printf("Failed\n");
        }
    }
}

int main() 
{
    init_semaphore();

    //int num_of_cycles;
    //cin >> num_of_cycles;
    //num_of_cycles = 10;
  
    pthread_t cycles[num_of_cycles];

    for(int i=0; i<num_of_cycles; i++)
    {
        int res;

        char *id = new char[3];
        strcpy(id,to_string(i+1).c_str());

        res = pthread_create(&cycles[i],NULL,sync_ser_pay,(void *)id);

        if(res != 0){
            printf("Thread creation failed\n");
        }  
    }

    for(int i = 0; i < num_of_cycles; i++){
        void *result;
        pthread_join(cycles[i],&result);
        printf("5 %s",(char*)result);
    }

    destroy_semaphore();

    //cout << "here";
    return 0;
}