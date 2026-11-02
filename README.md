# GalaxyBook Firmware Features Controller

`galaxyctl` is a simple command-line utility for managing the features present in the samsung-galaxybook firmware, that is mainline since linux kernel 6.15 and up.

## Usage
the base command `galaxyctl` will give an overview of the current features implenented into the CLI. 

It can be run without sudo for features that doesn't write to a file. Permission warnings will show for cammonds requiring sudo.

After `galaxyctl ` you can run the following commands:

### recording
Manages the 'Block Recording' firmware attribute present in the firmware.

`recording` gives the recording help overview showing the following

  -  `recording` gives the current status of the 'Block Recording'

  -  `recording block` enables 'Block Recording', thus disabling the webcam

  -  `recording unblock` disables 'Block Recording', thus enabling the webcam

  -  `recording toggle` toggles 'Block Recording'


### battery
Manages the Battery features from the firmware

`battery` Manage battery features

   - `battery` Show general battery information

   - `battery health` show battery health and technology

   - `battery limit` Show current battery limit

    - `battery limit <60-100>` Set battery charge limit from 60% to 100%


### powerlid
Manages the 'Power on lid open' firmware attribute present in the firmware.

`powerlid` gives the powerlid help overview showing the following

  -  `powerlid` Shows the current status of 'power on lid open'

  -  `powerlid enable` enables 'power on lid open'

  -  `powerlid disable` disables 'power on lid open'

  -  `powerlid toggle` toggles 'Power on lid open'


### Performance mode profile
Manage power profile through cli (becaus linux mint doesn't show low-power (fan off) modus)

`profile` Manage Performace mode profile

   - `profile` Shows current power profile

   - `help | --help | -h` shows command usage and power profile options 

   - `profile <option>` Set performace profile (with autocomplete for name)


## Installation

`galaxyctl` requires Linux 6.15 or newer with the `samsung-galaxybook` driver loaded. Check with:

```bash
lsmod | grep samsung_galaxybook
```

Ready-made packages for Debian/Ubuntu/Linux Mint and Arch Linux are attached to every [release](https://github.com/Ryan-Diazz/galaxyctl/releases). Bash tab completion is included in both packages.

### Debian / Ubuntu / Linux Mint

1. Download `galaxyctl_<version>_amd64.deb` from the [latest release](https://github.com/Ryan-Diazz/galaxyctl/releases/latest).
2. Install it:

   ```bash
   sudo apt install ./galaxyctl_<version>_amd64.deb
   ```

To uninstall, run `sudo apt remove galaxyctl`.

### Arch Linux

1. Download `galaxyctl-<version>-1-x86_64.pkg.tar.zst` from the [latest release](https://github.com/Ryan-Diazz/galaxyctl/releases/latest).
2. Install it:

   ```bash
   sudo pacman -U galaxyctl-<version>-1-x86_64.pkg.tar.zst
   ```

To uninstall, run `sudo pacman -R galaxyctl`.

### Building it yourself

You only need `make` and a C compiler (`gcc` or `clang`):

```bash
# Debian / Ubuntu / Linux Mint
sudo apt install build-essential git
# Arch Linux
sudo pacman -S --needed base-devel git
```

Then clone, build and install:

```bash
git clone https://github.com/Ryan-Diazz/galaxyctl.git
cd galaxyctl
make all
sudo make install
```

This installs the binary to `/usr/local/bin/galaxyctl` and the bash completion to `/usr/local/share/bash-completion/completions/galaxyctl`. Open a new terminal for tab completion to work.

- Install somewhere else with `sudo make install PREFIX=/usr`.
- Uninstall with `sudo make uninstall`, using the same `PREFIX` if you changed it.
- Run it without installing with `./output/galaxyctl`.

# License
This project is licensed under the terms of the MIT license. check the [LICENCE](LICENCE) File for more information

# About
my [original](https://github.com/Ryan-Diazz/GalaxyBook-Firmware-Driver-CLI) version was written in python. Which i didn't really like. So now it is in c :p

I should have all the important features implemented, but since this project is open source on GitHub, feel free to contribute! If something is missing, broken, or could be improved, don’t hesitate to open a pull request.

Have a wonderful day and enjoy your galaxybook,

btw, i can't be the only one getting yellow spots in my screen :/

Ryan.