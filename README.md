# Ultra-barebones, Linux-only media-controls for VLC, your web browser, etc.
Designed as a zero-redundancy alternative to [the accepted means of binding multimedia keys to i3](https://faq.i3wm.org/question/3747/enabling-multimedia-keys.1.html)

## !!WIP, Currently only plays/pauses all active players!!

## Currently must be built from src...
Requires libdbus-1 headers:

`sudo apt install libdbus-1-dev`

then...

`./build.sh`

## Currently a WIP, please contribute any missing MPRIS methods you need, or any other relevant features you can think of :)