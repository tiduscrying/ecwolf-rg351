#!/bin/bash
cd /roms/ports/ecwolf
export SDL_GAMECONTROLLERCONFIG="$(cat gamecontrollerdb.txt)"
./ecwolf -data wl6
unset SDL_GAMECONTROLLERCONFIG