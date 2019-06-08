/* Author: Semjon Kerner <semjon.kerner@fu-berlin.de>
 * Last Edit: 2018 */

/* INCLUDES */
#include <alsa/asoundlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <signal.h>

/* DEFINES */
#if DEBUG_FLAG==1
#define DEBUG(...) printf( __VA_ARGS__)
#else
#define DEBUG(...)
#endif

/* DATATYPES */
typedef struct {
    int SPI_LED;            // SPI GPIO PORT
    char LED_C[32][3];      // LED COLORS TO PRINT
    int I2C_GPIO;           // I2C GPIO PORT
    int I2C_ADC;            // I2C ADRESS
    int I2C_DISPLAY;        // I2C ADRESS
    int ADC_REG[4];         // last Values of ADC Registers
    char ADC_DATA;          // bitfield (0) 1 if (no) new data
} gpio_interface_t;

typedef struct step_link {
    int step;               // stepnumber of a step sequencer (quantification)
    struct timespec t;      // no quantification
    char midi[3];           // midi note
    //int bpm;              // when bpm changes, t must be recalculated
    struct step_link* prev;
    struct step_link* next;
} step_link_t;

typedef struct {
    int steps;
    step_link_t* first;
    step_link_t* actual;
    step_link_t* last;
} ampis_recorder_t;

typedef struct {
    int run;                // Indicates programm run mode
    int rec;                // Controlls play_thread Mode
    int clock;              // Controlls clock_thread Mode
    int bar;
} ampis_mode_t;

/* GLOBAL VARIABLES */
int sleeptime;
int bpm;
ampis_mode_t mode;
pthread_mutex_t mutex;

/* FUNCTIONS */
/* helper */
int read_midi(snd_rawmidi_t *in, char buf[3]);
void setbpm(int bpm);
int check_port(int *p);
int midi_ports_init(snd_rawmidi_t *midichan[2]);

/* recorder */
void record_link(char midi[3], ampis_recorder_t* r);
void init_recorder(ampis_recorder_t* r);
int play_link(ampis_recorder_t* r, char *midi);
void quantize(step_link_t *act, struct timespec *start, int steps);

/* interface */
int get_ampis_mode();

/* main */
void *interface_thread(void *arg);
void *snd_midi_clk_thread(void *arg);
void *play_thread(void *arg);
int main(int argc, char *argv[]);
