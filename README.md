## introduction

- Look for PNG pictures in `/usr/share/background` and show it in full screen
- Use font in `/usr/share/fonts` to show times
- Click the mouse will end the program


## requirements

- More than four PNG pictures in `/usr/share/backgrounds`
- Have fonts in `/usr/share/fonts`
```bash
$ sudo apt-get install libsdl2-dev
$ sudo apt install gcovr
```

## build
```bash
$ mkdir cmake-build-debug
$ cmake --build cmake-build-debug --target demo -- -j 12
```

## run

```bash
$ cd cmake-build-debug
$ ./demo
```