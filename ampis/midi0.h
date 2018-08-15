/* seqdemo.c by Matthias Nagorni */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <alsa/asoundlib.h>
#include <poll.h>

snd_seq_t *open_seq();
void midi_action(snd_seq_t *seq_handle);
