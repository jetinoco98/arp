gcc first.c -o first
gcc second.c -o second
gcc third.c -o third

konsole  -e ./second &
konsole  -e ./first &
konsole  -e ./third &
