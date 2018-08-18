/* Author: Semjon Kerner <semjon.kerner@fu-berlin.de>
 * Last Edit: 2018
 *
 * originaly based on examples
 * by Craig Stuart Sapp <craig@ccrma.stanford.edu>
 */

/* includes */
#include <alsa/asoundlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

/* defines */
#define DEBUG(...) printf( __VA_ARGS__)

/* datatypes */
typedef struct step_link {
    struct step_link* prev;
    struct step_link* next;
    struct timespec t;    
    char midi[3];
} step_link_t;

/* global variables */
int sleeptime;
int end;
pthread_mutex_t mutex;

/* functions */
void check_port(int *p);
void setbpm (int bpm);

void *snd_midi_clk_thread(void *arg);
int main(int argc, char *argv[]);
