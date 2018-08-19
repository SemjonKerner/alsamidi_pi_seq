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

void *record_thread(void *arg)
{
    return NULL;
}

int main(int argc, char *argv[])
{
    puts("This is ampis");
    puts("=============\n");

    int mode = SND_RAWMIDI_SYNC;
    snd_rawmidi_t* midiin = NULL;
    snd_rawmidi_t* midiout = NULL;
    int ports[2];
    end = 1;
    char portin[9];
    char portout[9];
    char buffer[3];
    pthread_t midiclkthread;
    ampis_recorder_t recorder;

    puts("initializing...");

    // ++ INITIALIZATION ++
    pthread_mutex_init(&mutex, NULL);
    setbpm(120);
    check_port(ports);
    init_recorder(&recorder);
    clock_gettime(CLOCK_REALTIME, &recorder.first->t);

    sprintf(portin, "hw:%d,0,0", ports[0]);
    sprintf(portout, "hw:%d,0,0", ports[1]);
    DEBUG("in: %s, out %s\n", portin, portout);

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

    puts("running...");

    // ++ MAIN LOOP ++
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

    puts("finalizing...");

    // ++ DEINITIALIZATION ++
    pthread_mutex_unlock(&mutex);
    end = 0;
    pthread_join(midiclkthread, NULL);

    snd_rawmidi_close(midiin);
    snd_rawmidi_close(midiout);
    midiin  = NULL;
    midiout = NULL;

    puts("\n=============");
    puts("  Good  Bye  ");

    return 0;
}
