/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2018 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#include "SDL.h"

#if !SDL_VERSION_ATLEAST(2,0,5)
/* Load all the data from an SDL data stream */
void *
Mix_LoadFile_RW(SDL_RWops * src, size_t *datasize, int freesrc)
{
    const int FILE_CHUNK_SIZE = 1024;
    Sint64 size;
    size_t size_read, size_total;
    void *data = NULL, *newdata;

    if (!src) {
        SDL_SetError("SDL_LoadFile_RW invalid src");
        return NULL;
    }

    SDL_RWseek(src, 0, RW_SEEK_END);
    size = SDL_RWtell(src);
	SDL_RWseek(src, 0, RW_SEEK_SET);
    if (size < 0) {
        size = FILE_CHUNK_SIZE;
    }
    data = SDL_malloc((size_t)(size + 1));

    size_total = 0;
    for (;;) {
        if ((((Sint64)size_total) + FILE_CHUNK_SIZE) > size) {
            size = (size_total + FILE_CHUNK_SIZE);
            newdata = SDL_realloc(data, (size_t)(size + 1));
            if (!newdata) {
                SDL_free(data);
                data = NULL;
                SDL_OutOfMemory();
                goto done;
            }
            data = newdata;
        }

        size_read = SDL_RWread(src, (char *)data+size_total, 1, (size_t)(size-size_total));
        if (size_read == 0) {
            break;
        }
        size_total += size_read;
    }

    if (datasize) {
        *datasize = size_total;
    }
    ((char *)data)[size_total] = '\0';

done:
    if (freesrc && src) {
        SDL_RWclose(src);
    }
    return data;
}
#endif
