a small "inspirational" wisdom printer for whenever you open the terminal, written in c!

requirements:
- a linux distribution
- a notification handler (that supports libnotify)
- gcc
- glib

dependencies (for debian and derivatives):
```sh
apt install build-essential libnotify-bin glibc-source
```

dependencies (for arch and derivatives):
```sh
pacman -S gcc glib2 libnotify
```

to compile the program:
```sh
git clone https://github.com/flippyshoe1/cwisdomgen.git
cd cwisdomgen
make
```

modify the "quotes" array to add any quotes you want!

