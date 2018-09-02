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

void init_recorder(ampis_recorder_t* r)
{
    r->first = NULL;
    r->actual = NULL;
    r->last = NULL;
    r->clock_prio = 1;
    r->rec = 0;
}












