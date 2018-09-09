#include "ampis.h"

/* ++ HELPER ++ */

int read_midi(snd_rawmidi_t *in, char buf[3])
{
    int i;
    for (i = 0; i < 3; i++) {
        int e = snd_rawmidi_read(in, &buf[i], 1);
        if (e == -EAGAIN)
            /* print byte count without incrementing*/
            break;
        if (e == -EBUSY || e < 0) {
            /* serious error */
            DEBUG("Problem reading MIDI input\n");
            return -1;
        }
    }
    return i;
}

void setbpm (int b) 
{
    sleeptime = (int)(2500000 / b);
    bpm = b;
    DEBUG("BPM = %f, sleeping = %d\n",
    (double)(2500000 / sleeptime), sleeptime);
}

int check_port(int *p)
{
    int card = -1;
    int device = -1;
    snd_ctl_t *ctl;
    char name[32];
    const char *sub_name;
    snd_rawmidi_info_t *info;

    snd_card_next(&card);

    p[0] = -1;
    p[1] = -1;

    while (card >= 0) {
        sprintf(name, "hw:%d", card);
        snd_ctl_open(&ctl, name, 0);
        snd_ctl_rawmidi_next_device(ctl, &device);
        while (device >= 0) {
            snd_rawmidi_info_alloca(&info);
            snd_rawmidi_info_set_device(info, device);
            snd_ctl_rawmidi_info(ctl, info);
            sub_name = snd_rawmidi_info_get_subdevice_name(info);

            DEBUG("hw:%d,%d %s\n", card, device, sub_name);
            if (strstr(sub_name, "MIDISTART") != NULL) {
                p[0] = card;    
            }
            if (strstr(sub_name, "Rocket") != NULL){
                p[1] = card;
            }

            snd_ctl_rawmidi_next_device(ctl, &device);
        }
        snd_ctl_close(ctl);
        if (snd_card_next(&card) < 0) break;
    } 
    
    if (p[0] < 0)
        return -1;
    if (p[1] < 0)
        return -2;

    DEBUG("Music25 port = %d, Rocket_port = %d\n", p[0], p[1]);
    return 0;
}

int midi_ports_init(snd_rawmidi_t *midichan[2])
{
    int ports[2];
    int status;
    char portname[10];

    midichan[0] = NULL;
    midichan[1] = NULL;

    if((status = check_port(ports)) < 0)
        return status;

    sprintf(portname, "hw:%d,0,0\n", (char)(ports[0] % 10));
    if ((snd_rawmidi_open(&midichan[0], NULL, portname,
        SND_RAWMIDI_NONBLOCK)) < 0) {
        DEBUG("Problem opening MIDI input\n");
        return -3;
    }

    sprintf(portname, "hw:%d,0,0\n", (char)(ports[1] % 10));
    if (snd_rawmidi_open(NULL, &midichan[1], portname,
        SND_RAWMIDI_SYNC) < 0) {
        DEBUG("Problem opening MIDI output\n");
        return -4;
    }

    return 0;
}

/* ++ RECORDER ++ */

void quantize(step_link_t *act, struct timespec *start, int steps)
{
    struct timespec temp;

    /* Calculate difference between timespecs */
    if ((act->t.tv_nsec - start->tv_nsec) < 0) {
        temp.tv_sec = (act->t.tv_sec - start->tv_sec) - 1;
        temp.tv_nsec = 1000000000 + (act->t.tv_nsec - start->tv_nsec);

    } else {
        temp.tv_sec = act->t.tv_sec - start->tv_sec;
        temp.tv_nsec = act->t.tv_nsec - start->tv_nsec;
    }

    float s = (float)temp.tv_sec + ((float)temp.tv_nsec / 1000000000);
    act->step = round(s / (((float)bpm / 60) / 16));
    DEBUG("Step #%d at %f s\n", act->step, s);
}

void record_link(char midi[3], ampis_recorder_t* r)
{
    struct timespec time;

    clock_gettime(CLOCK_REALTIME, &time);

    r->actual->next = (step_link_t *)malloc(sizeof(step_link_t));
    r->actual->next->prev = r->actual;
    r->last = r->actual->next;
    r->actual = r->actual->next;
    r->actual->next = NULL;

    r->actual->t = time;
    memcpy(r->actual->midi, midi, 3);
}

//TODO:
int play_link(ampis_recorder_t* r, char *midi)
{
    memcpy(midi, r->actual->midi, 3);

    if (r->actual == r->first){
        r->actual = r->actual->next;
        return 0;
    }

    int ret = 0;

    if (r->actual->step == r->actual->prev->step)
        ret = 0;
    else
        ret = (((float)(r->actual->step - r->actual->prev->step) *
              (((float)bpm / 60) / 16)) * 1000000);

    if (r->actual == r->last)
        r->actual = r->first;
    else 
        r->actual = r->actual->next;

    printf("Step #%d in %d ns\n", r->actual->step, ret);

    return ret;
}

void init_recorder(ampis_recorder_t* r)
{
    r->actual = (step_link_t *)malloc(sizeof(step_link_t));
    r->first = r->actual;
    r->last = r->actual;
    r->actual->prev = NULL;
    r->actual->next = NULL;
}

/* ++ INTERFACE ++ */

int get_ampis_mode()
{
    char readoption[20];

    DEBUG("Reading...\n");
    fgets(readoption, 19, stdin);

    if (strstr(readoption, "rec") != NULL)
            mode.rec = 1;
    else if (strstr(readoption, "play") != NULL)
            mode.rec = 2;
    else
            mode.rec = 0;

    return 0;
}
