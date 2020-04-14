#!/bin/bash

# if needed to create debug or other builds replace 'release' in the following
result=${PWD##*/}
if [ $result == "scripts" ]; then
  cd ..
fi
(
  mkdir -p ./bin
  cd ./bin
  cp -r ../../externalResources/samples .
  mkdir -p soundOutput
  mkdir -p presets
  if [ $(uname -s) == "Linux" ]; then
    cp -r ../external/Resources .
    cp -r ../../externalResources/Fonts ./Resources
  fi
  cd ..

  # Build LIBSAMPLERATE if it doesnt exist../external/libsamplerate/build
  if [ ! -d ./external/libsamplerate/build ]; then
    cmake -E make_directory external/libsamplerate/build
    cd external/libsamplerate/build
    cmake ..
    make
    cd ../../../
  fi

  mkdir -p build
  cd build
  mkdir -p debug
  cd debug
  if [ $(uname -s) == "Linux" ]; then
    cmake -DCMAKE_BUILD_TYPE=Debug -Wno-deprecated -DBUILD_EXAMPLES=0 -DRTAUDIO_API_JACK=1 -DRTMIDI_API_JACK=1 ../..
  fi

  if [ $(uname -s) == "Darwin" ]; then
    cmake -DCMAKE_BUILD_TYPE=Release -Wno-deprecated -DBUILD_EXAMPLES=0 -DRTAUDIO_API_JACK=0 -DRTMIDI_API_JACK=0 ../..
  fi

)
