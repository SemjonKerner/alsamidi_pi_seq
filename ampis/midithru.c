// Based on midi example by Craig Stuart Sapp <craig@ccrma.stanford.edu>

#include <alsa/asoundlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>             /* for threading        */
#include <unistd.h>              /* for sleep() function */
#include <time.h>

int sleeptime;
int end;

void check_port(int *p)
{
    int card = -1;
    int device = -1;
    snd_ctl_t *ctl;
    char name[32];
    const char *sub_name;
    snd_rawmidi_info_t *info;

    snd_card_next(&card);

    while (card >= 0) {
        sprintf(name, "hw:%d", card);
        snd_ctl_open(&ctl, name, 0);
        snd_ctl_rawmidi_next_device(ctl, &device);
        while (device >= 0) {
            snd_rawmidi_info_alloca(&info);
            snd_rawmidi_info_set_device(info, device);
            snd_ctl_rawmidi_info(ctl, info);
            sub_name = snd_rawmidi_info_get_subdevice_name(info);

            printf("hw:%d,%d %s\n", card, device, sub_name);
            if (strcmp("MIDISTART", sub_name)) p[0] = card;
            if (strstr(sub_name, "Rocket") != NULL) p[1] = card;

            snd_ctl_rawmidi_next_device(ctl, &device);
        }
        snd_ctl_close(ctl);
        if (snd_card_next(&card) < 0) break;
    } 
    return;
}

void *snd_midi_clk(void *arg)
{
    char midiclk[1] = {0xF8};
    snd_rawmidi_t* midiout = (snd_rawmidi_t*) arg;

    while (end) {
        if (snd_rawmidi_write(midiout, midiclk, 1) < 0) {
            puts("Problem writing to MIDI output in thread");
            break;
        }

        usleep(sleeptime);
    }    
    return NULL;
}

void setbpm (int bpm) 
{
    sleeptime = (int)(2500000 / bpm);
    printf("BPM = %f, sleeping = %d\n", (double)(2500000 / sleeptime), sleeptime);
}

int main(int argc, char *argv[])
{
   int mode = SND_RAWMIDI_SYNC;
   snd_rawmidi_t* midiin = NULL;
   snd_rawmidi_t* midiout = NULL;
   int ports[2];
   end = 1;
   char portin[9];
   char portout[9];
   pthread_t midiinthread;

   setbpm(120); 

   check_port(ports);
   printf("ports[0] = %d, ports[1] = %d\n", ports[0], ports[1]);

   sprintf(portin, "hw:%d,0,0", ports[0]);
   sprintf(portout, "hw:%d,0,0", ports[1]);
   printf("in: %s, out %s\n", portin, portout);

   if (snd_rawmidi_open(&midiin, NULL, portin, mode) < 0) {
      puts("Problem opening MIDI input");
      exit(1);
   }

   if (snd_rawmidi_open(NULL, &midiout, portout, mode) < 0) {
      puts("Problem opening MIDI output");
      exit(1);
   }

   if (pthread_create(&midiinthread, NULL, snd_midi_clk, midiout) < 0) {
      puts("Problem starting thread");
      exit(1);
   }

   int i = 0;
   char buffer[3];
   while (1) {
      if (snd_rawmidi_read(midiin, buffer, 3) < 0) {
         puts("Problem reading MIDI input");
         break;
      }

      if ((buffer[0] == 0xb0) && (buffer[1] == 2)) {
        setbpm(((int)buffer[2] * 2) + 46);
      }

      if (snd_rawmidi_write(midiout, buffer, 3) < 0) {
         puts("Problem writing to MIDI output");
         break;
      }

      printf("%d, 0x%x %d %d\n",
             i++, (unsigned char)buffer[0],
             (unsigned char)buffer[1], (unsigned char)buffer[2]);
   }

   end = 0;

   snd_rawmidi_close(midiin);
   snd_rawmidi_close(midiout);
   midiin  = NULL;
   midiout = NULL;

   return 0;
}
