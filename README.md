# Ultra-barebones, Linux mediaplayer controls for VLC, your web browser, etc.
Designed as a zero-redundancy alternative to [the accepted means of binding multimedia keys to i3](https://faq.i3wm.org/question/3747/enabling-multimedia-keys.1.html), but can also be used for general-purpose media control :D

# Installation

### Download the x86-64 binary, or...
### build from src, its easy c:
Requires libdbus-1 headers: `sudo apt update && sudo apt install -y libdbus-1-dev`

then build... `./build.sh`

optionally install with `ln` or `cp` or whatever, e.g. `cp ./mediactl /usr/bin/mediactl`

# Usage
`mediactl` to list all mediaplayers found over D-Bus

`mediactl <MPRIS Method Name>` to call an mpris method

`mediactl` simply propagates MPRIS methods from the commandline over the D-Bus to all active mediaplayers, so hypothetically, any zero-argument MPRIS method will work normally.

e.g, to play: `mediactl Play`

A list of all MPRIS methods can be found [here](https://specifications.freedesktop.org/mpris-spec/2.2/Player_Interface.html), but keep in mind not everything fully implements it!

# Example `mediactl` usage in i3 config
The original purpose of this program was to enable my keyboard's multimedia keys after installing i3.
My i3 config now has the following lines enabling this behaviour:

```
# bind keyboard media keys to mpris methods
bindsym XF86AudioPlay exec mediactl PlayPause
bindsym XF86AudioNext exec mediactl Next
bindsym XF86AudioPrev exec mediactl Previous
bindsym XF86AudioStop exec mediactl Stop
```


# Disclaimer
This is a WIP and is provided with no guarantees or safety nets, please contribute with PRs and logged issues :)
