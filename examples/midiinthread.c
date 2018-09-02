//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Sat May  9 22:39:12 PDT 2009
// Last Modified: Sat May  9 23:15:31 PDT 2009
// Filename:      midiinthread.c
// Syntax:        C; ALSA 1.0
// $Smake:        gcc -o %b %f -lasound
//
// Description:	  Read ALSA Raw MIDI input using threads.
//

#include <alsa/asoundlib.h>      /* for alsa interface   */
#include <pthread.h>             /* for threading        */
#include <unistd.h>              /* for sleep() function */

// function declarations:
void     errormessage            (const char *format, ...);
void*    midiinfunction          (void * arg);

///////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
   int status;
   int mode = 0;
   pthread_t midiinthread;
   snd_rawmidi_t* midiin = NULL;
   const char* portname = "hw:1,0,0";  // see alsarawportlist.c example program
   if ((argc > 1) && (strncmp("hw:", argv[1], 3) == 0)) {
      portname = argv[1];
   }
   if ((status = snd_rawmidi_open(&midiin, NULL, portname, mode)) < 0) {
      errormessage("Problem opening MIDI input: %s", snd_strerror(status));
      exit(1);
   }

   // type "man pthread_create" for more information about this function:
   status = pthread_create(&midiinthread, NULL, midiinfunction, midiin);
   if (status == -1) {
      errormessage("Unable to create MIDI input thread.");
      exit(1);
   }

   sleep(60);  // do nothing for a while; thread does all the work.

   snd_rawmidi_close(midiin);
   midiin  = NULL;    // snd_rawmidi_close() does not clear invalid pointer,
   printf("\n");      // so might be a good idea to erase it after closing.
   return 0;
}

///////////////////////////////////////////////////////////////////////////

//////////////////////////////
//
// midiinfunction -- Thread function which waits around until a MIDI 
//      input byte arrives and then it prints the byte to the terminal.
//      This thread function does not end gracefully when the program
//      stops.

void *midiinfunction(void *arg) {
   // this is the parameter passed via last argument of pthread_create():
   snd_rawmidi_t* midiin = (snd_rawmidi_t*)arg;
   char buffer[1];
   int  count = 0;
   int status;

   while (1) {
      if (midiin == NULL) {
         break;
      }
      if ((status = snd_rawmidi_read(midiin, buffer, 1)) < 0) {
         errormessage("Problem reading MIDI input: %s", snd_strerror(status));
      }
      count++;
      if ((unsigned char)buffer[0] >= 0x80) {  // print command in hex
         printf("0x%x ", (unsigned char)buffer[0]);
      } else {
         printf("%d ", (unsigned char)buffer[0]);
      }
      fflush(stdout);
      if (count % 20 == 0) {
         printf("\n");
      }
   }

   return NULL;
}



//////////////////////////////
//
// errormessage -- Print an error message.
//

void errormessage(const char *format, ...) {
   va_list ap;
   va_start(ap, format);
   vfprintf(stderr, format, ap);
   va_end(ap);
   putc('\n', stderr);
}


