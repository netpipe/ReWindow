gcc -shared -o libpreloader.so preloader.c -lX11 -fPIC
LD_PRELOAD=./libpreloader.so /usr/bin/pluma
