#include <alsa/asoundlib.h>

int main(int argc, char *argv[]) {
    int card = -1;
    snd_ctl_t *ctl;
    char name[32];
    int device = -1;
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
            printf("hw:%d,%d %s\n", card, device, sub_name);
            snd_ctl_rawmidi_next_device(ctl, &device);
        }
        snd_ctl_close(ctl);
        if (snd_card_next(&card) < 0) break;
    } 
    return 0;
}
