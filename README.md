# EmissionControl2

EmissionControl2 (EC2) is a new standalone interactive real-time application for granular synthesis and sound file granulation. It is available for OSX, Linux, and Windows. 

![](docs/EC2_lightmode.png "This is what EC2 looks like")

Features include:

- Granulation of multiple sound files simultaneously
- Multiple simultaneous grain streams
- Synchronous and asynchronous grain emission
- Intermittency control
- Per-grain signal processing (envelope, waveform, amplitude, frequency, spatial position, filter center frequency and resonance)
- Unique filter design optimized for per-grain synthesis
- Matrix modulation control of all granulation parameters with six LFOs
- Real-time display of peak amplitude, grain counter, waveform, and scan range
- Scalable GUI and font size
- MIDI Learn enables mapping to any MIDI continuous controller.
- Code is open source and available at GitHub
- Maximal "Grain Integrity" (tm)

See ![releases](https://github.com/jackkilgore/EmissionControl2/releases) page to download the latest version for your operating system.

## Mailing Lists

### New releases, major bug fixes, and other big news (low-volume list)
https://lists.sr.ht/~rondo/emissioncontrol2-announce

To subscribe, send a blank email to: ~rondo/emissioncontrol2-announce+subscribe@lists.sr.ht

### Discussion, questions, ideas, tips, sounds and video, and other general postings (higher-volume list; anyone can post here)
https://lists.sr.ht/~rondo/emissioncontrol2-discuss

To subscribe, send a blank email to: ~rondo/emissioncontrol2-discuss+subscribe@lists.sr.ht

To post to this list, send your post to: ~rondo/emissioncontrol2-discuss@lists.sr.ht

## Building
### Debian

- This project uses cmake to build so if you don't have cmake then install it (Minimum version: 3.13) and make sure your c and c++ compilers are defined in your environment.

- Run the following in a terminal to install the necessary libraries for building:

`sudo apt install libgtk-3-dev libasound2-dev libsndfile1-dev libfftw3-dev libjack-dev`
 
- git clone the repository and run some the scripts that automate the configure/build/install process.

`git clone https://github.com/EmissionControl2/EmissionControl2.git`

`cd EmissionControl2/ecSource`

`./scripts/configure.sh`

`./scripts/build.sh`

`sudo ./scripts/install_linux.sh` (this will put the binary in /usr/bin and set up resources in the paths where EC2 expects them to be)

To undo the files installed with install_linux.sh script, I've included the `scripts/uninstall_linux.sh` script as well.

### Arch Linux

- Must have cmake (3.13 or later), git, and base-devel packages installed.

`git clone https://github.com/EmissionControl2/EmissionControl2.git`

`cd EmissionControl2/ecSource`

`./scripts/configure.sh`

`./scripts/build.sh`

`sudo ./scripts/install_linux.sh` (this will put the binary in /usr/bin and set up resources in the paths where EC2 expects them to be)

To undo the files installed with install_linux.sh script, I've included the `scripts/uninstall_linux.sh` script as well.


### OS X
You must have cmake installed (version 3.10 or later), and Xcode (hopefully we can get rid of this dependency soon).

- First, clone the repo:

`git clone https://github.com/EmissionControl2/EmissionControl2.git`

- cd into EmissionControl2/ecSource

`cd EmissionControl2/ecSource`

- run configure script:

`./scripts/configure.sh`

- run build script:

`./scripts/build.sh`

### Windows
Coming soon...
