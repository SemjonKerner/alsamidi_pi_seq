/* midiroute.c by Matthias Nagorni */

#include "midi1.h"

int open_seq(snd_seq_t **seq_handle, int in_ports[], int out_ports[], int num_in, int num_out)
{
  int l1;
  char portname[64];

  if (snd_seq_open(seq_handle, "default", SND_SEQ_OPEN_DUPLEX, 0) < 0) {
    fprintf(stderr, "Error opening ALSA sequencer.\n");
    return(-1);
  }
  snd_seq_set_client_name(*seq_handle, "MIDI Router");
  for (l1 = 0; l1 < num_in; l1++) {
    sprintf(portname, "MIDI Router IN %d", l1);
    if ((in_ports[l1] = snd_seq_create_simple_port(*seq_handle, portname,
              SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE,
              SND_SEQ_PORT_TYPE_APPLICATION)) < 0) {
      fprintf(stderr, "Error creating sequencer port.\n");
      return(-1);
    }
  }
  for (l1 = 0; l1 < num_out; l1++) {
    sprintf(portname, "MIDI Router OUT %d", l1);
    if ((out_ports[l1] = snd_seq_create_simple_port(*seq_handle, portname,
              SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ,
              SND_SEQ_PORT_TYPE_APPLICATION)) < 0) {
      fprintf(stderr, "Error creating sequencer port.\n");
      return(-1);
    }
  }
  return(0);
}

void midi_route(snd_seq_t *seq_handle, int out_ports[], int split_point) {

  snd_seq_event_t *ev;

  do {
    snd_seq_event_input(seq_handle, &ev);
    snd_seq_ev_set_subs(ev);  
    snd_seq_ev_set_direct(ev);
    if ((ev->type == SND_SEQ_EVENT_NOTEON)||(ev->type == SND_SEQ_EVENT_NOTEOFF)) {
      if (ev->data.note.note < split_point) {
        snd_seq_ev_set_source(ev, out_ports[0]);
      } else {
        snd_seq_ev_set_source(ev, out_ports[1]);
      }
      snd_seq_event_output_direct(seq_handle, ev);
    } else {
      snd_seq_ev_set_source(ev, out_ports[0]);
      snd_seq_event_output_direct(seq_handle, ev);
      snd_seq_ev_set_source(ev, out_ports[1]);
      snd_seq_event_output_direct(seq_handle, ev);
    }
    snd_seq_free_event(ev);
  } while (snd_seq_event_input_pending(seq_handle, 0) > 0);
}

int main(int argc, char *argv[]) {

  snd_seq_t *seq_handle;
  int in_ports[MAX_MIDI_PORTS], out_ports[MAX_MIDI_PORTS];
  int npfd, split_point;
  struct pollfd *pfd;
  
  if (argc < 2) {
    fprintf(stderr, "\nmidiroute <split_point>\n\n");
    exit(1);
  } else {
    split_point = atoi(argv[1]);  
  }  
  if (open_seq(&seq_handle, in_ports, out_ports, 1, 2) < 0) {
    fprintf(stderr, "ALSA Error.\n");
    exit(1);
  }
  npfd = snd_seq_poll_descriptors_count(seq_handle, POLLIN);
  pfd = (struct pollfd *)alloca(npfd * sizeof(struct pollfd));
  snd_seq_poll_descriptors(seq_handle, pfd, npfd, POLLIN);
  while (1) {
    if (poll(pfd, npfd, 100000) > 0) {
      midi_route(seq_handle, out_ports, split_point);
    }  
  }
}
