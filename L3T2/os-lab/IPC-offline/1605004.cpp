#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define CYCLIST_COUNT 50
#define SERVICE_ROOM_COUNT 3
#define PAYMENT_ROOM_CAPACITY 5

// a mutex for each service room
pthread_mutex_t service_rooms[SERVICE_ROOM_COUNT];

// a semaphore for the payment room
sem_t payment_room;

// counter of cyclists taking service
int taking_service = 0;
// counter of cyclists waiting for departure
int waiting_for_departure = 0;
// a mutex to access all shared counters
pthread_mutex_t counter_accessor;
// a mutex to wait at gate for everyone to depart
pthread_mutex_t entry_blocker;
// a mutex to wait for departure for everyone to finish taking service
pthread_mutex_t exit_blocker;

// random sleep function, makes thread sleep for random amount of time
int random_sleep () {
    struct timespec tim;
    tim.tv_sec = 0; // integer part of how many seconds you want to wait
    int ms = 1 + rand() % 999;
    tim.tv_nsec = ms * 1000000; // fractional part of how many seconds you want to wait, given in nanoseconds
    nanosleep(&tim, NULL);
    return ms;
}

// initializes all semaphores and mutexes
void initializer () {
    int res;

    // initialize mutex for each service room
    for (int i=0; i<SERVICE_ROOM_COUNT; i++) {
        res = pthread_mutex_init(&service_rooms[i], NULL);
        if (res) {
            printf("Failed to initialize service room %d\n", i);
            exit(EXIT_FAILURE);
        }
    }
    
    // initialize semaphore for payment room
    res = sem_init(&payment_room, 0, PAYMENT_ROOM_CAPACITY);
    if (res) {
        printf("Failed to initialize payment room\n");
        exit(EXIT_FAILURE);
    }
    
    // initialize mutexes for shared counters, entry and exit gates
    res = pthread_mutex_init(&counter_accessor, NULL);
    if (res) {
        printf("Failed to initialize shared counter mutex\n");
        exit(EXIT_FAILURE);
    }

    res = pthread_mutex_init(&entry_blocker, NULL);
    if (res) {
        printf("Failed to initialize entry gate mutex\n");
        exit(EXIT_FAILURE);
    }
    pthread_mutex_lock(&entry_blocker); // starts in locked state

    res = pthread_mutex_init(&exit_blocker, NULL);
    if (res) {
        printf("Failed to initialize exit gate mutex\n");
        exit(EXIT_FAILURE);
    }
}

// destroys all semaphores and mutexes
void destroyer () {
    int res;

    // destroy mutex for each service room
    for (int i=0; i<SERVICE_ROOM_COUNT; i++) {
        res = pthread_mutex_destroy(&service_rooms[i]);
        if (res) {
            printf("Failed to destroy service room %d\n", i);
        }
    }

    // destroy semaphore for payment room
    res = sem_destroy(&payment_room);
    if (res) {
        printf("Failed to destroy payment room\n");
    }
    
    // destroy mutexes for shared counters, entry and exit gates
    res = pthread_mutex_destroy(&counter_accessor);
    if (res) {
        printf("Failed to destroy shared counter mutex\n");
    }
    res = pthread_mutex_destroy(&entry_blocker);
    if (res) {
        printf("Failed to destroy entry gate mutex\n");
    }
    res = pthread_mutex_destroy(&exit_blocker);
    if (res) {
        printf("Failed to destroy exit gate mutex\n");
    }
}

void take_service (int id) {
    pthread_mutex_lock(&service_rooms[0]);
 
    while (true) {
        pthread_mutex_lock(&counter_accessor);
        if (waiting_for_departure) {
            pthread_mutex_unlock(&counter_accessor);
            pthread_mutex_lock(&entry_blocker);
        }
        else {
            printf("1 %d started taking service from serviceman %d\n", id, 1);
            fflush(stdout);
            taking_service++;
            if (taking_service == 1) {
                pthread_mutex_lock(&exit_blocker);
            }
            pthread_mutex_unlock(&counter_accessor);
            break;
        }
    }

    // most of service rooms
    for (int i=0; i<SERVICE_ROOM_COUNT; i++) {
        random_sleep();
        printf("2 %d finished taking service from serviceman %d\n", id, i+1);
        fflush(stdout);
        if (i+1 < SERVICE_ROOM_COUNT) {
            pthread_mutex_lock(&service_rooms[i+1]);
            printf("1 %d started taking service from serviceman %d\n", id, i+2);
            fflush(stdout);
        }
        pthread_mutex_unlock(&service_rooms[i]);
    }
    
    pthread_mutex_lock(&counter_accessor);
    taking_service--;
    if (taking_service == 0) {
        // printf("6 service zone is empty now\n");
        // fflush(stdout);
        pthread_mutex_unlock(&exit_blocker);
    }
    pthread_mutex_unlock(&counter_accessor);
}

void pay_bill (int id) {
    sem_wait(&payment_room);    

    printf("3 %d started paying the service bill\n", id);
    fflush(stdout);
    random_sleep();

    pthread_mutex_lock(&counter_accessor);
    printf("4 %d finished paying the service bill\n", id);
    fflush(stdout);
    waiting_for_departure++;
    pthread_mutex_unlock(&counter_accessor);

    sem_post(&payment_room);
}

void depart (int id) {
    pthread_mutex_lock(&counter_accessor);
    if (taking_service) {
        pthread_mutex_unlock(&counter_accessor);
        pthread_mutex_lock(&exit_blocker);
        pthread_mutex_unlock(&exit_blocker);
    }
    else {
        pthread_mutex_unlock(&counter_accessor);
    }

    random_sleep();
    printf("5 %d has departed\n", id);
    fflush(stdout);

    pthread_mutex_lock(&counter_accessor);
    waiting_for_departure--;
    if (waiting_for_departure == 0) {
        pthread_mutex_unlock(&entry_blocker);
    }
    pthread_mutex_unlock(&counter_accessor);
}

// target function for each cyclist thread
void *take_for_repair (void *arg) {
    int id = *((int *)arg);
    delete (int *)arg; // deleted here, created right before thread creation
    
    take_service(id);
    pay_bill(id);
    depart(id);

    return NULL;
}

int main () {
    srand(time(NULL));

    initializer();
    
    // create cyclist threads
    pthread_t cyclists[CYCLIST_COUNT];
    for (int i=0; i<CYCLIST_COUNT; i++) {
        int *id = new int; // created here, deleted in target function of thread
        *id = i+1;
        int res = pthread_create(&cyclists[i], NULL, take_for_repair, (void *)id);
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

    destroyer();
 
    return 0;
}