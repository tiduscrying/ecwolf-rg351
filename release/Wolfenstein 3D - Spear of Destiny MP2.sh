#!/bin/bash
cd /roms/ports/ecwolf
export SDL_GAMECONTROLLERCONFIG="$(cat gamecontrollerdb.txt)"
./ecwolf --data sd2
unset SDL_GAMECONTROLLERCONFIG
