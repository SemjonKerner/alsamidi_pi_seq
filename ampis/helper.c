#include "ampis.h"

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
            if (strcmp("MIDISTART", sub_name)) p[0] = card;
            if (strstr(sub_name, "Rocket") != NULL) p[1] = card;

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
