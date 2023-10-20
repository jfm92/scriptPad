#!/bin/bash

export PROJECT_PATH=${PWD}
echo $PROJECT_PATH

if [ ! -f "${PROJECT_PATH}/build/buildFrontEndBin" ]; then
    echo "Compiling tool to convert web Page to .C file"

    gcc -I "${PROJECT_PATH}/libraries/pico-sdk/lib/lwip/src/apps/http/makefsdata" \
        -I "${PROJECT_PATH}/libraries/pico-sdk/lib/lwip/src/include/" \
        -I "${PROJECT_PATH}" \
        "${PROJECT_PATH}/libraries/pico-sdk/lib/lwip/src/apps/http/makefsdata/makefsdata.c" \
        -o "${PROJECT_PATH}/build/buildFrontEndBin"
fi

"${PROJECT_PATH}/build/buildFrontEndBin" "${PROJECT_PATH}/modemUSBManagement/frontEnd" -f:"${PROJECT_PATH}/modemUSBManagement/frontEnd.c"