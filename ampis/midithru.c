// Based on midi example by Craig Stuart Sapp <craig@ccrma.stanford.edu>

#include <alsa/asoundlib.h>     /* Interface to the ALSA system */

int main(int argc, char *argv[]) {
   int mode = SND_RAWMIDI_SYNC;
   snd_rawmidi_t* midiin = NULL;
   snd_rawmidi_t* midiout = NULL;

   //TODO check wich port is wich device and sort out portin/out
   const char* portin = "hw:1,0,0";
   const char* portout = "hw:2,0,0";

   if (snd_rawmidi_open(&midiin, NULL, portin, mode) < 0) {
      puts("Problem opening MIDI input");
      exit(1);
   }

   if (snd_rawmidi_open(NULL, &midiout, portout, mode) < 0) {
      puts("Problem opening MIDI output");
      exit(1);
   }

   int i = 0;
   char buffer[3];
   while (1) {
      if (snd_rawmidi_read(midiin, buffer, 3) < 0) {
         puts("Problem reading MIDI input");
         break;
      }

      if (snd_rawmidi_write(midiout, buffer, 3) < 0) {
         puts("Problem writing to MIDI output");
         break;
      }

      printf("%d, 0x%x %d %d\n", i++, (unsigned char)buffer[0], (unsigned char)buffer[1], (unsigned char)buffer[2]);
   }

   snd_rawmidi_close(midiin);
   snd_rawmidi_close(midiout);
   midiin  = NULL;
   midiout = NULL;

   return 0;
}
