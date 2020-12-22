#pragma once
// Selectively back-port features to old SDL

#include <SDL.h>

#if !SDL_VERSION_ATLEAST(2,0,0)
// SDL 1.2 support is a bit of a hack since we need to ignore a lot of features

#include <assert.h>
#define SDL_assert assert
#define SDL_CloseAudioDevice(a) SDL_CloseAudio()
#define SDL_LockAudioDevice(a) SDL_LockAudio()
#define SDL_UnlockAudioDevice(a) SDL_UnlockAudio()
#define SDL_PauseAudioDevice(a,b) SDL_PauseAudio((b))
#define SDL_GetHintBoolean(a, b) (b)
#define SDL_MixAudioFormat(a,b,c,d,e) SDL_MixAudio((a),(b),(d),(e))
#define SDL_Log printf
#define SDL_ceil ceil
#define SDL_pow pow
#define SDL_sinf sinf
#define SDL_sqrt sqrt

#define SDL_zero(x) SDL_memset(&(x), 0, sizeof((x)))
#define SDL_zerop(x) SDL_memset((x), 0, sizeof(*(x)))

#define SDL_AUDIO_ALLOW_FREQUENCY_CHANGE 0
#define SDL_AUDIO_ALLOW_CHANNELS_CHANGE 0

static int SDL_InvalidParamError(const char *param) {
    SDL_SetError("Invalid parameter: %s", param);
    return -1;
}

static int SDL_OutOfMemoryNew() {
    SDL_OutOfMemory();
    return -1;
}
#undef SDL_OutOfMemory
#define SDL_OutOfMemory SDL_OutOfMemoryNew
#endif

#if !SDL_VERSION_ATLEAST(2,0,5)
extern void *Mix_LoadFile_RW(SDL_RWops * src, size_t *datasize, int freesrc);
#define SDL_LoadFile_RW Mix_LoadFile_RW

#if SDL_VERSION_ATLEAST(2,0,0)
static SDL_bool SDL_GetHintBoolean(const char* name, SDL_bool default_value) {
    const char *hint = SDL_GetHint(name);
    if (!hint || !*hint) {
        return default_value;
    }
    if (*hint == '0' || SDL_strcasecmp(hint, "false") == 0) {
        return SDL_FALSE;
    }
    return SDL_TRUE;
}
#endif
#endif

#if !SDL_VERSION_ATLEAST(2,0,7)
#include "SDL_audiocvt.h"

// Use newer resampler even though this function is nominally available
#define SDL_BuildAudioCVT Mix_BuildAudioCVT
#endif

#ifndef SDL_MIN_SINT16
#define SDL_MIN_SINT16 -0x8000
#endif
#ifndef SDL_MAX_SINT16
#define SDL_MAX_SINT16 0x7FFF
#endif
#ifndef SDL_MAX_UINT64
#define SDL_MAX_UINT64 0xFFFFFFFFFFFFFFFFu
#endif
