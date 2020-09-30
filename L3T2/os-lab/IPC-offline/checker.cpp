#include <cstdio>
#include <cstdlib>
#include <cassert>

using namespace std;

#ifndef PARAMED
#define CYCLIST_COUNT 10
#define SERVICE_ROOM_COUNT 3
#define PAYMENT_ROOM_CAPACITY 2
#endif
#define PAYID 1000005
#define WAITID 1000006
#define DONEID 1000007

int rooms[CYCLIST_COUNT+1]; // which room a certain cyclist is at
int cyclists[SERVICE_ROOM_COUNT+1]; // which cyclist is at a certain room
int payment_room; // count of people in payment room
int exit_line; // count of people in exit line
int line; // line of input file we are currently reading
char s[15];

void call_error () {
    printf("error at line %d\n", line);
    exit(EXIT_FAILURE);
}

void start_service () {
    int cyclist, room;
    scanf("%d", &cyclist);
    for (int i=0; i<5; i++) scanf("%s", s);
    scanf("%d", &room);
    if (cyclists[room]) {
        printf("room %d is already occupied by %d\n", room, cyclists[room]);
        call_error();
    }
    if (rooms[cyclist]) {
        printf("%d must leave room %d first\n", cyclist, rooms[cyclist]);
        call_error();
    }
    if (room == 1 && exit_line) {
        printf("%d cannot start service, people are waiting in departure line:\n", cyclist);
        for (int i=1; i<=CYCLIST_COUNT; i++) {
            if (rooms[i] == WAITID) {
                printf("%d is waiting\n", i);
            }
        }
        call_error();
    }
    cyclists[room] = cyclist;
    rooms[cyclist] = room;
}

void finish_service () {
    int cyclist, room;
    scanf("%d", &cyclist);
    for (int i=0; i<5; i++) scanf("%s", s);
    scanf("%d", &room);
    if (cyclists[room] != cyclist || rooms[cyclist] != room) {
        printf("%d is not at room %d\n", cyclist, room);
        printf("%d is at room %d\n", cyclist, rooms[cyclist]);
        printf("room %d has %d\n", room, cyclists[room]);
        call_error();
    }
    cyclists[room] = 0;
    rooms[cyclist] = 0;
}

void start_paying () {
    int cyclist;
    scanf("%d", &cyclist);
    for (int i=0; i<5; i++) scanf("%s", s);
    payment_room++;
    if (payment_room > PAYMENT_ROOM_CAPACITY) {
        printf("too many people at payment room:");
        for (int i=1; i<=CYCLIST_COUNT; i++) {
            if (rooms[i] == PAYID) {
                printf(" %d", i);
            }
        }
        printf("\n");
        call_error();
    }
    rooms[cyclist] = PAYID;
} 

void finish_paying () {
    int cyclist;
    scanf("%d", &cyclist);
    for (int i=0; i<5; i++) scanf("%s", s);
    if (rooms[cyclist] != PAYID) {
        printf("%d was not paying bill\n", cyclist);
        call_error();
    }
    rooms[cyclist] = WAITID;
    payment_room--;
    exit_line++;
    assert(payment_room >= 0);
} 

void depart () {
    int cyclist;
    scanf("%d", &cyclist);
    for (int i=0; i<2; i++) scanf("%s", s);
    if (rooms[cyclist] != WAITID) {
        printf("%d can't depart, not his time\n", cyclist);
        call_error();
    }
    bool ok = true;
    for (int i=1; ok && i<=SERVICE_ROOM_COUNT; i++) {
        if (cyclists[i]) {
            ok = false;
        }
    }
    if (!ok) {
        printf("%d can't depart, not all rooms empty:\n", cyclist);
        for (int i=1; i<=SERVICE_ROOM_COUNT; i++) {
            if (cyclists[i]) {
                printf("room %d has %d\n", i, cyclists[i]);
            }
        }
        call_error();
    }
    rooms[cyclist] = DONEID;
    exit_line--;
    assert(exit_line >= 0);
}

void all_empty () {
    for (int i=0; i<5; i++) scanf("%s", s);
    bool ok = true;
    for (int i=1; ok && i<=SERVICE_ROOM_COUNT; i++) {
        if (cyclists[i]) {
            ok = false;
        }
    }
    if (!ok) {
        printf("wrong, not all rooms empty:\n");
        for (int i=1; i<=SERVICE_ROOM_COUNT; i++) {
            if (cyclists[i]) {
                printf("room %d has %d\n", i, cyclists[i]);
            }
        }
        call_error();
    }
}

void failure () {
    puts("Something failed dude");
    call_error();
}

int main () {
    int type;
    while (scanf("%d", &type) != EOF) {
        ++line;
        switch (type) {
            case 1: start_service(); break;
            case 2: finish_service(); break;
            case 3: start_paying(); break;
            case 4: finish_paying(); break;
            case 5: depart(); break;
            case 6: all_empty(); break;
            case 7: failure(); break;
        }
    }
    printf("ok. %d lines\n", line);
}