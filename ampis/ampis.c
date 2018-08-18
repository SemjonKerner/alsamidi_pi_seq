#include "ampis.h"

void *snd_midi_clk_thread(void *arg)
{
    char midiclk[1] = {0xF8};
    snd_rawmidi_t* midiout = (snd_rawmidi_t*) arg;

    while (end) {
        pthread_mutex_lock(&mutex);
        if (snd_rawmidi_write(midiout, midiclk, 1) < 0) {
            puts("Problem writing to MIDI output in thread");
            break;
        }
        pthread_mutex_unlock(&mutex);

        usleep(sleeptime);
    }    
    return NULL;
}

step_link_t* record_link(char midi[3], step_link_t* act,
                 step_link_t* last)
{
   struct timespec time;
   step_link_t* next;

   clock_gettime(CLOCK_REALTIME, &time);

   next = (step_link_t *)malloc(sizeof(step_link_t));
   next->prev = act;
   next->next = NULL;
   
   next->t = time;
   memcpy(next->midi, midi, 3);

   act->next = next;
   act = next;
   last = next;

   return act;
}

void free_link(step_link_t* start)
{
    step_link_t *act, *next;
    act = start;

    while(1) {
        printf("%lf: 0x%c, %c, %c\n",
               (double)act->t.tv_sec +
               (double)act->t.tv_nsec / 1000000000,
               act->midi[0], act->midi[1], act->midi[2]);

        next = act->next;
        free(act);
        act = next;
        if (act == NULL) break;
    }
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
   char buffer[3];
   pthread_t midiclkthread;
   step_link_t *first, *last, *actual;

   // Initialisation
   pthread_mutex_init(&mutex, NULL);
   setbpm(120); 
   check_port(ports);
   first = (step_link_t *)malloc(sizeof(step_link_t));
   first->next = NULL;
   first->prev = NULL;
   clock_gettime(CLOCK_REALTIME, &first->t);
   memcpy(first->midi, "ini", 3);
   actual = first;
   printf("%lf: 0x%c, %c, %c\n",
          (double)actual->t.tv_sec +
          (double)actual->t.tv_nsec / 1000000000,
          actual->midi[0], actual->midi[1], actual->midi[2]);

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

   if (pthread_create(&midiclkthread, NULL,
                      snd_midi_clk_thread, midiout) < 0) {
      puts("Problem starting thread");
      exit(1);
   }

   // main loop
   for(int i = 0;; i++) {
      if (snd_rawmidi_read(midiin, buffer, 3) < 0) {
         puts("Problem reading MIDI input");
         break;
      }

      if ((buffer[0] == 0xb0) && (buffer[1] == 2)) {
        setbpm(((int)buffer[2] * 2) + 46);
      }

      pthread_mutex_lock(&mutex);
      if (snd_rawmidi_write(midiout, buffer, 3) < 0) {
         puts("Problem writing to MIDI output");
         break;
      }
      pthread_mutex_unlock(&mutex);
   }

   // deinit
   pthread_mutex_unlock(&mutex);
   end = 0;
   pthread_join(midiclkthread, NULL);

   snd_rawmidi_close(midiin);
   snd_rawmidi_close(midiout);
   midiin  = NULL;
   midiout = NULL;

   return 0;
}
