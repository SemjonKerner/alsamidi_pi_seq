#include "ampis.h"

// ++ HELPER ++

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

            DEBUG("hw:%d,%d %s\n", card, device, sub_name);
            if (strstr(sub_name, "MIDISTART") != NULL) {
                printf("1. IF: %s,%d\n", sub_name, card);
                p[0] = card;    
            }
            if (strstr(sub_name, "Rocket") != NULL){
                printf("2. IF: %s,%d\n", sub_name, card);
                p[1] = card;
            }

            snd_ctl_rawmidi_next_device(ctl, &device);
        }
        snd_ctl_close(ctl);
        if (snd_card_next(&card) < 0) break;
    } 

    DEBUG("ports[0] = %d, ports[1] = %d\n", p[0], p[1]);
    return;
}

void setbpm (int bpm) 
{
    sleeptime = (int)(2500000 / bpm);
    DEBUG("BPM = %f, sleeping = %d\n",
    (double)(2500000 / sleeptime), sleeptime);
}

// ++ RECORDER ++

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

void free_link(step_link_t* start)
{
    step_link_t *act, *next;
    act = start;

    while(1) {
        DEBUG("%lf: 0x%c, %c, %c\n",
        (double)act->t.tv_sec +
        (double)act->t.tv_nsec / 1000000000,
        act->midi[0], act->midi[1], act->midi[2]);

        next = act->next;
        free(act);
        act = next;
        if (act == NULL) break;
    }
}

int play_link(ampis_recorder_t* r, char *midi)
{
    memcpy(midi, r->actual->midi, 3);

    if (r->actual == r->last)
        r->actual = r->first;
    else 
        r->actual = r->actual->next;

    if (r->actual == r->first)
        return 0;

    struct timespec temp;
    if ((r->actual->t.tv_sec - r->actual->prev->t.tv_sec) < 0){
        temp.tv_sec = (r->actual->t.tv_sec -
                       r->actual->prev->t.tv_sec) - 1;
    } else {
        temp.tv_nsec = 1000000000 + (r->actual->t.tv_nsec -
                       r->actual->prev->t.tv_nsec);
    }
    printf("S:%d - US: %d\n", temp.tv_sec, temp.tv_nsec);
    int usec = (int)(temp.tv_nsec / 1000) + (temp.tv_sec * 1000);
    printf("USEC:%d\n", usec);

    return usec;
}

void init_recorder(ampis_recorder_t* r)
{
    r->actual = (step_link_t *)malloc(sizeof(step_link_t));
    r->first = r->actual;
    r->last = r->actual;
    r->actual->prev = NULL;
    r->actual->next = NULL;
}












