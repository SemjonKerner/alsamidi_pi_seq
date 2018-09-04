#include "ampis.h"

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void *interface_thread(void *arg)
{
    return NULL;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void *clk_thread(void *arg)
{
    snd_rawmidi_t** mfd = (snd_rawmidi_t **)arg;
    char midiclk[1] = {0xF8};
    int try = 0;

    while (mode.run) {
    /* Option: Clk Thru */
        if (mode.clock == 1) {
        // Initialisiere Midi Eingang (GPIO)
        // stoppe internes bpm setzen
        // errechne bpm von extern und setze es, wenn es sich ändert
        //
            ;
    /* Option: Clk Generator */
        } else {
            do {
                if (mode.run == 0)
                    goto stopclk; 
                try = pthread_mutex_trylock(&mutex);
            } while (try == EBUSY);
            
            if (snd_rawmidi_write(mfd[1], midiclk, 1) < 0) {
                puts("Problem writing MIDI output");
                pthread_mutex_unlock(&mutex);
                break;
            }
            pthread_mutex_unlock(&mutex);

            usleep(sleeptime);
        }
    }
stopclk:
    mode.run = 0;
    return NULL;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void *play_thread(void *arg)
{
    char buffer[3];
    snd_rawmidi_t** mfd  = (snd_rawmidi_t **)arg;
    int try = 0;

    ampis_recorder_t recorder;
    init_recorder(&recorder);

    while (mode.run) {

    /* Option: recorder */
        if (mode.rec == 1) {
            // INITIALISIERUNG
            // maximale Steps, Stepweite (8, 16, 32)
            // Startzeitpunkt
            //
            // HAUPTSCHLEIFE bis max Steps + (Step_timelength - 1)
            // Midi aufnehmen, Zeitpunkt speichern
            // Quantisierung zu nächstem Step (Vor- oder Rückquantisiert)
            //
            // ENDE
            // ?? sortiere Liste (Steps mit mehreren notes: note off zuerst)
            // gehe in Playmode
            if (snd_rawmidi_read(mfd[0], buffer, 3) < 0) {
                puts("Problem reading MIDI input");
                break;
            }
        
            record_link(buffer, &recorder);
                
            if (snd_rawmidi_write(mfd[1], buffer, 3) < 0) {
                puts("Problem writing MIDI output");
                pthread_mutex_unlock(&mutex);
                break;
            }
        // PLAYMODE spiele liste ab 
        } else if (mode.rec == 2) {
            usleep(play_link(&recorder, buffer));
            
            if (snd_rawmidi_write(mfd[1], buffer, 3) < 0) {
                puts("Problem writing MIDI output");
                pthread_mutex_unlock(&mutex);
                break;
            }

    /* Option: midi thru */
        } else if (mode.rec == 0) {
            if (snd_rawmidi_read(mfd[0], buffer, 3) < 0) {
                puts("Problem reading MIDI input");
                break;
            }

            /* the slider of music25 is set to 0xb0
             * thus it can only interact with the program
             * not the rocket */
            if ((buffer[0] == 0xb0) && (buffer[1] == 2)) {
                setbpm(((int)buffer[2] * 2) + 46);
                continue; /* dont send data to rocket */
            }

            do {
                if (mode.run == 0)
                    goto stopthru;
                try = pthread_mutex_trylock(&mutex);
            } while (try == EBUSY);

            if (snd_rawmidi_write(mfd[1], buffer, 3) < 0) {
                puts("Problem writing MIDI output");
                pthread_mutex_unlock(&mutex);
                break;
            }
            pthread_mutex_unlock(&mutex);
        }
    }

stopthru:
    mode.run = 0;
    return NULL;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int main(int argc, char *argv[])
{
    puts("This is ampis");
    puts("=============\n");

    snd_rawmidi_t* midichan[2];
    pthread_t midiclkthread;
    pthread_t playthread;
//    /pthread_t interfacethread;

    /* ++ INITIALIZATION ++ */
    puts("initializing...");
    mode.run = 1;
    mode.rec = 0;
    setbpm(120);

    if (pthread_mutex_init(&mutex, NULL) != 0)
        exit(1);
    if (midi_ports_init(midichan) < 0)
        exit(1);
    if (pthread_create(&midiclkthread, NULL, clk_thread, midichan) < 0) {
        puts("Problem starting clk thread");
        exit(1);
    }
    if (pthread_create(&playthread, NULL, play_thread, midichan) < 0) {
        puts("Problem starting play thread");
        exit(1);
    }
//    if (pthread_create(&interfacethread, NULL, interface_thread, midichan) < 0) {
//        puts("Problem starting play thread");
//        exit(1);
//    }

    /* ++ MAIN LOOP ++ */
    puts("running...");
    while (mode.run) {
        if (get_ampis_mode() < 0)
            break;
    };

    /* ++ DEINITIALIZATION ++ */
    puts("finalizing...");
    mode.run = 0;

    pthread_join(midiclkthread, NULL);
    pthread_join(playthread, NULL);
    pthread_mutex_destroy(&mutex);

    snd_rawmidi_close(midichan[0]);
    snd_rawmidi_close(midichan[1]);
    midichan[0] = NULL;
    midichan[1] = NULL;

    puts("\n=============");
    puts("  Good  Bye  ");

    return 0;
}
