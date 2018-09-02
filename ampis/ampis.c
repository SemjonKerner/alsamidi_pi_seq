#include "ampis.h"

void exit_handler(int dummy)
{
   end = 0;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void *clk_thread(void *arg)
{
/* this thread should either get a clk from midi
 * or generate its own clock */
    snd_rawmidi_t** mfd = (snd_rawmidi_t **)arg;
    char midiclk[1] = {0xF8};
    int try = 0;

/* TODO: Option: Clk Thru */

/* Option: Generator */
    while (end) {
        do {
            if (end == 0)
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
stopclk:
    end = 0;
    return NULL;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void *play_thread(void *arg)
{
/* this thread either plays a recording
 * or passes midi signals thru */
    char buffer[3];
    snd_rawmidi_t** mfd  = (snd_rawmidi_t **)arg;
    int try = 0;

    ampis_recorder_t recorder;
    init_recorder(&recorder);

    while (end) {
        /* Option: recorder */
        // ließ und speichere daten in liste
        if (rec == 1) {
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
        // spiele liste ab
        } else if (rec == 2) {
            usleep(play_link(&recorder, buffer));
            
            if (snd_rawmidi_write(mfd[1], buffer, 3) < 0) {
                puts("Problem writing MIDI output");
                pthread_mutex_unlock(&mutex);
                break;
            }
        /* Option: midi thru */
        } else if (rec == 0) {
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
                if (end == 0)
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
    end = 0;
    return NULL;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int main(int argc, char *argv[])
{
    puts("This is ampis");
    puts("=============\n");

    int ports[2];
    char portin[9];
    char portout[9];
    snd_rawmidi_t* midichan[2];
    midichan[0] = NULL;
    midichan[1] = NULL;
    pthread_t midiclkthread;
    pthread_t playthread;
    char readoption[20];

    /* ++ INITIALIZATION ++ */
    puts("initializing...");
    //signal(SIGINT, exit_handler);

    end = 1;
    rec = 0;

    pthread_mutex_init(&mutex, NULL);
    setbpm(120);
    check_port(ports);

    sprintf(portin, "hw:%d,0,0\n", ports[0]);
    if ((snd_rawmidi_open(&midichan[0], NULL, portin,  
        SND_RAWMIDI_SYNC)) < 0) {
        puts("Problem opening MIDI input");
        exit(1);
    }

    sprintf(portout, "hw:%d,0,0\n", ports[1]);
    if (snd_rawmidi_open(NULL, &midichan[1], portout,
        SND_RAWMIDI_SYNC) < 0) {
        puts("Problem opening MIDI output");
        exit(1);
    }

    if (pthread_create(&midiclkthread, NULL,
        clk_thread, midichan) < 0) {
        puts("Problem starting clk thread");
        exit(1);
    }

    if (pthread_create(&playthread, NULL,
        play_thread, midichan) < 0) {
        puts("Problem starting play thread");
        exit(1);
    }

    /* ++ MAIN LOOP ++ */
    puts("running...");
    while (end) {
        if(rec == 1)
            getc(stdin);
            rec = 2;

        puts("Reading...");
        fgets(readoption, 19, stdin);

        if (strstr(readoption, "rec") != NULL)
            rec = 1;
        else if (strstr(readoption, "play") != NULL)
            rec = 2;
        else
            rec = 0;
    };

    /* ++ DEINITIALIZATION ++ */
    puts("finalizing...");
    end = 0;

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
