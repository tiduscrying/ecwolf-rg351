#!/bin/bash
cd /roms/ports/ecwolf
export SDL_GAMECONTROLLERCONFIG="$(cat gamecontrollerdb.txt)"
./ecwolf --data sd3
unset SDL_GAMECONTROLLERCONFIG