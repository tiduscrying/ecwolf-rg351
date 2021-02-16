#!/bin/bash
cd /roms/ports/ecwolf
export SDL_GAMECONTROLLERCONFIG="$(cat gamecontrollerdb.txt)"
./ecwolf --data  sod
unset SDL_GAMECONTROLLERCONFIG