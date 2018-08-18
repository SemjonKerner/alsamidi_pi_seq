// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>

#include <alsa/asoundlib.h>     /* Interface to the ALSA system */
//#include <unistd.h>             /* for sleep() function */
#include <time.h>

#define WAITTIME 25000 // 120BPM

int main(int argc, char *argv[]) {
   int mode = SND_RAWMIDI_SYNC;
   snd_rawmidi_t* midiout = NULL;
   const char* portname = "hw:1,0,0";  // see alsarawportlist.c example program
   if ((argc > 1) && (strncmp("hw:", argv[1], 3) == 0)) {
      portname = argv[1];
   }
   if (snd_rawmidi_open(NULL, &midiout, portname, mode) < 0) {
      printf("Problem opening MIDI output");
      exit(1);
   }

   char mclk[1] = {0xF8};
   while (1) {
           if (snd_rawmidi_write(midiout, mclk, 1) < 0) break;
           usleep(WAITTIME);
   }

   snd_rawmidi_close(midiout);
   midiout = NULL;    // snd_rawmidi_close() does not clear invalid pointer,
   return 0;          // so might be a good idea to erase it after closing.
}
