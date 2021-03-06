#!/bin/bash
cd "$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )/ecwolf"
export SDL_GAMECONTROLLERCONFIG="$(cat gamecontrollerdb.txt)"
./ecwolf --config ./ecwolf.cfg --data sd3 2>&1 | tee -a ./log_sd3.txt
unset SDL_GAMECONTROLLERCONFIG