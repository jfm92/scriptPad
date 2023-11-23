#/bin/bash

#Build webPage
./buildFrontEnd.sh

#Build completed project
cd build && make -j4
cd ..