
#!/bin/sh
./configure --prefix=./ --bindir=bin/ --libdir=lib/ \
    --includedir=../../include \
    --target=android  --with-cxx=arm-none-eabi-c++ --cc=arm-none-eabi-gcc \
    --use-compiler-family=gcc --enable-static
