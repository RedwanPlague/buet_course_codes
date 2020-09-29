#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define CYCLIST_COUNT 5
#define SERVICE_ROOM_COUNT 3
#define PAYMENT_ROOM_CAPACITY 3

// a mutex for each service room
pthread_mutex_t service_rooms[SERVICE_ROOM_COUNT];

// a semaphore for the payment room
sem_t payment_room;

// counter of cyclists taking service
int taking_service = 0;
// counter of cyclists waiting for departure
int waiting_for_departure = 0;
// a mutex to access counters since all threads will be accessing them
pthread_mutex_t counter_accessor;
// a mutex to wait at gate for everyone to depart
pthread_mutex_t entry_blocker;
// a mutex to wait for departure for everyone to finish taking service
pthread_mutex_t depart_blocker;

// random sleep function, makes thread sleep for random amount of time
int random_sleep () {
    struct timespec tim;
    tim.tv_sec = 0; // integer part of how many seconds you want to wait
    int ms = rand() % 1000;
    tim.tv_nsec = ms * 1000000; // fractional part of how many seconds you want to wait, given in nanoseconds
    nanosleep(&tim, NULL);
    return ms;
}

// target function for each cyclist thread
void *take_for_repair (void *arg) {
    int id = *((int *)arg);
    delete (int *)arg; // deleted here, created right before thread creation
    
    pthread_mutex_lock(&service_rooms[0]);
    
    pthread_mutex_lock(&counter_accessor);
    if (waiting_for_departure) {
        pthread_mutex_unlock(&counter_accessor);
        pthread_mutex_lock(&entry_blocker);
    }
    else {
        pthread_mutex_unlock(&counter_accessor);
    }

    printf("%d started taking service from serviceman %d\n", id, 1);
    random_sleep();
    printf("%d finished taking service from serviceman %d\n", id, 1);
    for (int i=1; i<SERVICE_ROOM_COUNT; i++) {
        pthread_mutex_lock(&service_rooms[i]);
        pthread_mutex_unlock(&service_rooms[i - 1]);
        printf("%d started taking service from serviceman %d\n", id, i+1);
        random_sleep();
        printf("%d finished taking service from serviceman %d\n", id, i+1);
    }
    pthread_mutex_unlock(&service_rooms[SERVICE_ROOM_COUNT-1]);

    sem_wait(&payment_room);    
    printf("%d started paying the service bill\n", id);
    random_sleep();
    printf("%d finished paying the service bill\n", id);
    sem_post(&payment_room);

    pthread_mutex_lock(&counter_accessor);
    waiting_for_departure++;
    pthread_mutex_unlock(&counter_accessor);
    sleep(2);
    printf("%d has departed\n", id);
    pthread_mutex_lock(&counter_accessor);
    waiting_for_departure--;
    if (waiting_for_departure == 0) {
        pthread_mutex_unlock(&entry_blocker);
    }
    pthread_mutex_unlock(&counter_accessor);

    return arg;
}

int main () {
    srand(time(NULL));

    int res;

    // initialize mutex for each service room
    for (int i=0; i<SERVICE_ROOM_COUNT; i++) {
        res = pthread_mutex_init(&service_rooms[i], NULL);
        if (res) {
            printf("Failed to initialize service room %d\n", i);
            return 0;
        }
    }
    
    // initialize semaphore for payment room
    res = sem_init(&payment_room, 0, PAYMENT_ROOM_CAPACITY);
    if (res) {
        printf("Failed to initialize payment room\n");
        return 0;
    }
    
    // initialize cyclist threads
    pthread_t cyclists[CYCLIST_COUNT];
    for (int i=0; i<CYCLIST_COUNT; i++) {
        int *id = new int; // created here, deleted in target function of thread
        *id = i+1;
        res = pthread_create(&cyclists[i], NULL, take_for_repair, (void *)id);
        if (res) {
            printf("Failed to initialize cyclist %d\n", i);
            return 0;
        }
    }
 
    // join cyclist threads
    for (int i=0; i<CYCLIST_COUNT; i++) {
        void *result;
        pthread_join(cyclists[i], &result);
    }

    // destroy semaphore for payment room
    res = sem_destroy(&payment_room);
    if (res) {
        printf("Failed to destroy payment room\n");
    }
    
    // destroy mutex for each service room
    for (int i=0; i<SERVICE_ROOM_COUNT; i++) {
        res = pthread_mutex_destroy(&service_rooms[i]);
        if (res) {
            printf("Failed to destroy service room %d\n", i);
        }
    }
 
    return 0;
}