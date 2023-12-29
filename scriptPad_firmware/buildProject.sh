#/bin/bash

#Build webPage
./buildFrontEnd.sh

#Build completed project

folder_name="build"

# Create build folder if it doesn't exists
if [ ! -d "$folder_name" ]; then
    mkdir "$folder_name"
fi

cd "$folder_name"

# Build project
if cmake ..; then
    make -j4
fi