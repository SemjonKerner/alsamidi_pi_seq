/* midiroute.c by Matthias Nagorni */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <alsa/asoundlib.h>

#define MAX_MIDI_PORTS   4

/* Open ALSA sequencer wit num_in writeable ports and num_out readable ports. */
/* The sequencer handle and the port IDs are returned.                        */  
int open_seq(snd_seq_t **seq_handle, int in_ports[], int out_ports[], int num_in, int num_out);

/* Read events from writeable port and route them to readable port 0  */
/* if NOTEON / OFF event with note < split_point. NOTEON / OFF events */
/* with note >= split_point are routed to readable port 1. All other  */
/* events are routed to both readable ports.                          */
void midi_route(snd_seq_t *seq_handle, int out_ports[], int split_point);

int main(int argc, char *argv[]);