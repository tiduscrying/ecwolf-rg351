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

#pragma once

#include <SDL.h>

#if !SDL_VERSION_ATLEAST(2,0,7)
#define SDL_NewAudioStream Mix_NewAudioStream
#define SDL_AudioStreamPut Mix_AudioStreamPut
#define SDL_AudioStreamGet Mix_AudioStreamGet
#define SDL_AudioStreamAvailable Mix_AudioStreamAvailable
#define SDL_AudioStreamFlush Mix_AudioStreamFlush
#define SDL_AudioStreamClear Mix_AudioStreamClear
#define SDL_FreeAudioStream Mix_FreeAudioStream

#define SDL_Convert_S8_to_F32 Mix_Convert_S8_to_F32
#define SDL_Convert_U8_to_F32 Mix_Convert_U8_to_F32
#define SDL_Convert_S16_to_F32 Mix_Convert_S16_to_F32
#define SDL_Convert_U16_to_F32 Mix_Convert_U16_to_F32
#define SDL_Convert_S32_to_F32 Mix_Convert_S32_to_F32
#define SDL_Convert_F32_to_S8 Mix_Convert_F32_to_S8
#define SDL_Convert_F32_to_U8 Mix_Convert_F32_to_U8
#define SDL_Convert_F32_to_S16 Mix_Convert_F32_to_S16
#define SDL_Convert_F32_to_U16 Mix_Convert_F32_to_U16
#define SDL_Convert_F32_to_S32 Mix_Convert_F32_to_S32


/**
 *  \brief Audio format flags.
 *
 *  These are what the 16 bits in SDL_AudioFormat currently mean...
 *  (Unspecified bits are always zero).
 *
 *  \verbatim
    ++-----------------------sample is signed if set
    ||
    ||       ++-----------sample is bigendian if set
    ||       ||
    ||       ||          ++---sample is float if set
    ||       ||          ||
    ||       ||          || +---sample bit size---+
    ||       ||          || |                     |
    15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
    \endverbatim
 *
 *  There are macros in SDL 2.0 and later to query these bits.
 */
typedef Uint16 SDL_AudioFormat;

/**
 *  \name Audio flags
 */
/* @{ */

#define SDL_AUDIO_MASK_BITSIZE       (0xFF)
#define SDL_AUDIO_MASK_DATATYPE      (1<<8)
#define SDL_AUDIO_MASK_ENDIAN        (1<<12)
#define SDL_AUDIO_MASK_SIGNED        (1<<15)
#define SDL_AUDIO_BITSIZE(x)         (x & SDL_AUDIO_MASK_BITSIZE)
#define SDL_AUDIO_ISFLOAT(x)         (x & SDL_AUDIO_MASK_DATATYPE)
#define SDL_AUDIO_ISBIGENDIAN(x)     (x & SDL_AUDIO_MASK_ENDIAN)
#define SDL_AUDIO_ISSIGNED(x)        (x & SDL_AUDIO_MASK_SIGNED)
#define SDL_AUDIO_ISINT(x)           (!SDL_AUDIO_ISFLOAT(x))
#define SDL_AUDIO_ISLITTLEENDIAN(x)  (!SDL_AUDIO_ISBIGENDIAN(x))
#define SDL_AUDIO_ISUNSIGNED(x)      (!SDL_AUDIO_ISSIGNED(x))

/**
 *  \name Audio format flags
 *
 *  Defaults to LSB byte order.
 */
/* @{ */
#define AUDIO_U8        0x0008  /**< Unsigned 8-bit samples */
#define AUDIO_S8        0x8008  /**< Signed 8-bit samples */
#define AUDIO_U16LSB    0x0010  /**< Unsigned 16-bit samples */
#define AUDIO_S16LSB    0x8010  /**< Signed 16-bit samples */
#define AUDIO_U16MSB    0x1010  /**< As above, but big-endian byte order */
#define AUDIO_S16MSB    0x9010  /**< As above, but big-endian byte order */
#define AUDIO_U16       AUDIO_U16LSB
#define AUDIO_S16       AUDIO_S16LSB
/* @} */

/**
 *  \name int32 support
 */
/* @{ */
#define AUDIO_S32LSB    0x8020  /**< 32-bit integer samples */
#define AUDIO_S32MSB    0x9020  /**< As above, but big-endian byte order */
#define AUDIO_S32       AUDIO_S32LSB
/* @} */

/**
 *  \name float32 support
 */
/* @{ */
#define AUDIO_F32LSB    0x8120  /**< 32-bit floating point samples */
#define AUDIO_F32MSB    0x9120  /**< As above, but big-endian byte order */
#define AUDIO_F32       AUDIO_F32LSB
/* @} */

/**
 *  \name Native audio byte ordering
 */
/* @{ */
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#define AUDIO_U16SYS    AUDIO_U16LSB
#define AUDIO_S16SYS    AUDIO_S16LSB
#define AUDIO_S32SYS    AUDIO_S32LSB
#define AUDIO_F32SYS    AUDIO_F32LSB
#else
#define AUDIO_U16SYS    AUDIO_U16MSB
#define AUDIO_S16SYS    AUDIO_S16MSB
#define AUDIO_S32SYS    AUDIO_S32MSB
#define AUDIO_F32SYS    AUDIO_F32MSB
#endif
/* @} */


typedef void (SDLCALL * SDL_AudioFilter) (struct SDL_AudioCVT * cvt,
                                          SDL_AudioFormat format);

/**
 *  \brief Upper limit of filters in SDL_AudioCVT
 *
 *  The maximum number of SDL_AudioFilter functions in SDL_AudioCVT is
 *  currently limited to 9. The SDL_AudioCVT.filters array has 10 pointers,
 *  one of which is the terminating NULL pointer.
 */
#define SDL_AUDIOCVT_MAX_FILTERS 9

/* SDL_AudioStream is a new audio conversion interface.
   The benefits vs SDL_AudioCVT:
    - it can handle resampling data in chunks without generating
      artifacts, when it doesn't have the complete buffer available.
    - it can handle incoming data in any variable size.
    - You push data as you have it, and pull it when you need it
 */
/* this is opaque to the outside world. */
struct _SDL_AudioStream;
typedef struct _SDL_AudioStream SDL_AudioStream;

/**
 *  Create a new audio stream
 *
 *  \param src_format The format of the source audio
 *  \param src_channels The number of channels of the source audio
 *  \param src_rate The sampling rate of the source audio
 *  \param dst_format The format of the desired audio output
 *  \param dst_channels The number of channels of the desired audio output
 *  \param dst_rate The sampling rate of the desired audio output
 *  \return 0 on success, or -1 on error.
 *
 *  \sa SDL_AudioStreamPut
 *  \sa SDL_AudioStreamGet
 *  \sa SDL_AudioStreamAvailable
 *  \sa SDL_AudioStreamFlush
 *  \sa SDL_AudioStreamClear
 *  \sa SDL_FreeAudioStream
 */
extern DECLSPEC SDL_AudioStream * SDLCALL SDL_NewAudioStream(const SDL_AudioFormat src_format,
                                           const Uint8 src_channels,
                                           const int src_rate,
                                           const SDL_AudioFormat dst_format,
                                           const Uint8 dst_channels,
                                           const int dst_rate);

/**
 *  Add data to be converted/resampled to the stream
 *
 *  \param stream The stream the audio data is being added to
 *  \param buf A pointer to the audio data to add
 *  \param len The number of bytes to write to the stream
 *  \return 0 on success, or -1 on error.
 *
 *  \sa SDL_NewAudioStream
 *  \sa SDL_AudioStreamGet
 *  \sa SDL_AudioStreamAvailable
 *  \sa SDL_AudioStreamFlush
 *  \sa SDL_AudioStreamClear
 *  \sa SDL_FreeAudioStream
 */
extern DECLSPEC int SDLCALL SDL_AudioStreamPut(SDL_AudioStream *stream, const void *buf, int len);

/**
 *  Get converted/resampled data from the stream
 *
 *  \param stream The stream the audio is being requested from
 *  \param buf A buffer to fill with audio data
 *  \param len The maximum number of bytes to fill
 *  \return The number of bytes read from the stream, or -1 on error
 *
 *  \sa SDL_NewAudioStream
 *  \sa SDL_AudioStreamPut
 *  \sa SDL_AudioStreamAvailable
 *  \sa SDL_AudioStreamFlush
 *  \sa SDL_AudioStreamClear
 *  \sa SDL_FreeAudioStream
 */
extern DECLSPEC int SDLCALL SDL_AudioStreamGet(SDL_AudioStream *stream, void *buf, int len);

/**
 * Get the number of converted/resampled bytes available. The stream may be
 *  buffering data behind the scenes until it has enough to resample
 *  correctly, so this number might be lower than what you expect, or even
 *  be zero. Add more data or flush the stream if you need the data now.
 *
 *  \sa SDL_NewAudioStream
 *  \sa SDL_AudioStreamPut
 *  \sa SDL_AudioStreamGet
 *  \sa SDL_AudioStreamFlush
 *  \sa SDL_AudioStreamClear
 *  \sa SDL_FreeAudioStream
 */
extern DECLSPEC int SDLCALL SDL_AudioStreamAvailable(SDL_AudioStream *stream);

/**
 * Tell the stream that you're done sending data, and anything being buffered
 *  should be converted/resampled and made available immediately.
 *
 * It is legal to add more data to a stream after flushing, but there will
 *  be audio gaps in the output. Generally this is intended to signal the
 *  end of input, so the complete output becomes available.
 *
 *  \sa SDL_NewAudioStream
 *  \sa SDL_AudioStreamPut
 *  \sa SDL_AudioStreamGet
 *  \sa SDL_AudioStreamAvailable
 *  \sa SDL_AudioStreamClear
 *  \sa SDL_FreeAudioStream
 */
extern DECLSPEC int SDLCALL SDL_AudioStreamFlush(SDL_AudioStream *stream);

/**
 *  Clear any pending data in the stream without converting it
 *
 *  \sa SDL_NewAudioStream
 *  \sa SDL_AudioStreamPut
 *  \sa SDL_AudioStreamGet
 *  \sa SDL_AudioStreamAvailable
 *  \sa SDL_AudioStreamFlush
 *  \sa SDL_FreeAudioStream
 */
extern DECLSPEC void SDLCALL SDL_AudioStreamClear(SDL_AudioStream *stream);

/**
 * Free an audio stream
 *
 *  \sa SDL_NewAudioStream
 *  \sa SDL_AudioStreamPut
 *  \sa SDL_AudioStreamGet
 *  \sa SDL_AudioStreamAvailable
 *  \sa SDL_AudioStreamFlush
 *  \sa SDL_AudioStreamClear
 */
extern DECLSPEC void SDLCALL SDL_FreeAudioStream(SDL_AudioStream *stream);

#endif
