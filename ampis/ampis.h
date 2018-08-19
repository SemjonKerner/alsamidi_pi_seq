/* Author: Semjon Kerner <semjon.kerner@fu-berlin.de>
 * Last Edit: 2018
 *
 * originaly based on examples
 * by Craig Stuart Sapp <craig@ccrma.stanford.edu>
 */

/* INCLUDES */
#include <alsa/asoundlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

/* DEFINES */
#define DEBUG(...) printf( __VA_ARGS__)

/* DATATYPES */
typedef struct step_link {
    struct step_link* prev;
    struct step_link* next;
    struct timespec t;    
    char midi[3];
} step_link_t;

typedef struct {
    step_link_t* first;
    step_link_t* actual;
    step_link_t* last;
    int clock_prio;
    int rec;
} ampis_recorder_t;

/* GLOBAL VARIABLES */
int sleeptime;
int end;
pthread_mutex_t mutex;

/* FUNCTIONS */
/* helper */
void check_port(int *p);
void setbpm (int bpm);

/* recorder */
step_link_t* record_link(char midi[3],
                step_link_t* act, step_link_t* last);
void free_link(step_link_t* start);
void init_recorder(ampis_recorder_t* r);

/* main */
void *snd_midi_clk_thread(void *arg);
int main(int argc, char *argv[]);
