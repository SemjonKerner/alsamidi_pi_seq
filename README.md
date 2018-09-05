# This is Ampis (Alsa Midi PI Sequencer)

With this project i am on the way of providing a midi step sequenzer written in c for Raspberry Pi.
The focus is to be simplistic and more and more modular, so that in future versions adaptations to different interfaces or midi devices will become trivial.

This Project is based on Raspberry Pi 1 B+ and uses some additional libraries.
Setup and building Tutorial will be published when the code base is done.

## current status

- [X] midi devices are hardcoded
- [X] midi clock is created internally, but the bpm may be changed by a midi controller
- [X] midi signals are passed thru to a synth

## next steps

### code base
- [ ] recording patterns is about to be finished
- [ ] quantisation needs a bit more overthinking
- [ ] using gpio inputs to connect a midi device (and perhaps even registering it with alsa)
- [ ] building, testing, debugging gpio/i2c interface

### future work
- [ ] writing a coherent tutorial for interested people
- [ ] messing around with patterns (timey wimey stuff and also harmonic stuff)
- [ ] modularity
  - [ ] different sequencer versions
  - [ ] perhaps a library
- [ ] migration to either a cheaper plattform or a newer raspi model
