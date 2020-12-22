cmake_minimum_required(VERSION 2.8.7)
project(SDL2_mixer)

if(POLICY CMP0053)
	cmake_policy(SET CMP0053 NEW)
endif()

# Being built as part of the ECWolf build process?
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
	set(VENDORED_LIB OFF)
	find_package(SDL REQUIRED)
else()
	set(VENDORED_LIB ON)
	set(SDL_LIBRARY SDL2::SDL2)
	set(SDL_INCLUDE_DIR "") # Blank this out since the target will provide it
endif()

if(NOT VENDORED_LIB)
	option(ANDROID "Build for Android" OFF)
endif()

# Visual Studio 2005 compatibility for legacy builds
if(MSVC_VERSION EQUAL 1400)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/vs2005compat)
	add_definitions(-Dinline=__inline)
	add_definitions(-D_USE_MATH_DEFINES)
endif()

# This could be broken out into individual options, but the testing overhead
# for that is too high right now. So until we have a reason to do so we'll just
# do all or nothing.
option(INTERNAL_SDL_MIXER_CODECS "Force build with internal codecs" OFF)
if(INTERNAL_SDL_MIXER_CODECS)
	set(INTERNAL_VORBIS ON)
	set(INTERNAL_VORBISFILE ON)
	set(INTERNAL_FLAC ON)
	set(INTERNAL_OPUS ON)
	set(INTERNAL_OPUSFILE ON)
	set(INTERNAL_OGG ON)
	set(INTERNAL_MODPLUG ON)
endif()

set(MISSING_CODEC NO)
if(NOT INTERNAL_VORBIS)
	find_path(VORBIS_INCLUDE_DIR vorbis/codec.h)
	find_library(VORBIS_LIBRARY vorbis)
	if(NOT VORBIS_LIBRARY AND NOT VORBIS_INCLUDE_DIR)
		message("Could not locate vorbis library and include directory. Internal codecs will be forced.")
		set(MISSING_CODEC YES)
	endif()
endif()
if(NOT INTERNAL_VORBISFILE)
	find_path(VORBISFILE_INCLUDE_DIR vorbis/vorbisfile.h)
	find_library(VORBISFILE_LIBRARY vorbisfile)
	if(NOT VORBISFILE_LIBRARY AND NOT VORBISFILE_INCLUDE_DIR)
		message("Could not locate vorbisfile library and include directory. Internal codecs will be forced.")
		set(MISSING_CODEC YES)
	endif()
endif()
if(NOT INTERNAL_FLAC)
	find_path(FLAC_INCLUDE_DIR FLAC/stream_decoder.h)
	find_library(FLAC_LIBRARY FLAC)
	if(NOT FLAC_LIBRARY AND NOT FLAC_INCLUDE_DIR)
		message("Could not locate flac library and include directory. Internal codecs will be forced.")
		set(MISSING_CODEC YES)
	endif()
endif()
if(NOT INTERNAL_OGG)
	find_path(OGG_INCLUDE_DIR ogg/ogg.h)
	find_library(OGG_LIBRARY ogg)
	if(NOT OGG_LIBRARY AND NOT OGG_INCLUDE_DIR)
		message("Could not locate ogg library and include directory. Internal codecs will be forced.")
		set(MISSING_CODEC YES)
	endif()
endif()
if(NOT INTERNAL_OPUSFILE)
	find_path(OPUSFILE_INCLUDE_DIR opus/opusfile.h)
	find_library(OPUSFILE_LIBRARY opusfile)
	if(NOT OPUSFILE_LIBRARY AND NOT OPUSFILE_INCLUDE_DIR)
		message("Could not locate opusfile library and include directory. Internal codecs will be forced.")
		set(MISSING_CODEC YES)
	endif()
endif()
if(NOT INTERNAL_OPUS)
	find_path(OPUS_INCLUDE_DIR opus.h PATH_SUFFIXES opus)
	find_library(OPUS_LIBRARY opus)
	if(NOT OPUS_LIBRARY AND NOT OPUS_INCLUDE_DIR)
		message("Could not locate opus library and include directory. Internal codecs will be forced.")
		set(MISSING_CODEC YES)
	endif()
endif()
if(NOT INTERNAL_MODPLUG)
	find_path(MODPLUG_INCLUDE_DIR modplug.h PATH_SUFFIXES libmodplug)
	find_library(MODPLUG_LIBRARY modplug)
	if(NOT MODPLUG_LIBRARY AND NOT MODPLUG_INCLUDE_DIR)
		message("Could not locate modplug library and include directory. Internal codecs will be forced.")
		set(MISSING_CODEC YES)
	endif()
endif()
if(NOT APPLE AND NOT WIN32 AND NOT ANDROID)
	find_path(FLUIDSYNTH_INCLUDE_DIR fluidsynth.h)
	find_library(FLUIDSYNTH_LIBRARY fluidsynth)
else()
	set(FLUIDSYNTH_LIBRARY "")
	set(FLUIDSYNTH_INCLUDE_DIR "")
endif()

if(MISSING_CODEC)
	set(INTERNAL_SDL_MIXER_CODECS ON)
	set(INTERNAL_VORBIS ON)
	set(INTERNAL_VORBISFILE ON)
	set(INTERNAL_FLAC ON)
	set(INTERNAL_OPUS ON)
	set(INTERNAL_OPUSFILE ON)
	set(INTERNAL_OGG ON)
	set(INTERNAL_MODPLUG ON)
endif()

if(INTERNAL_FLAC)
	message(STATUS "Using internal FLAC")
else()
	message(STATUS "Using system FLAC")
endif()
if(INTERNAL_MODPLUG)
	message(STATUS "Using internal modplug")
else()
	message(STATUS "Using system modplug")
endif()
if(INTERNAL_OGG)
	message(STATUS "Using internal ogg")
else()
	message(STATUS "Using system ogg")
endif()
if(INTERNAL_OPUS)
	message(STATUS "Using internal opus")
else()
	message(STATUS "Using system opus")
endif()
if(INTERNAL_OPUSFILE)
	message(STATUS "Using internal opusfile")
else()
	message(STATUS "Using system opusfile")
endif()
if(INTERNAL_VORBIS)
	message(STATUS "Using internal vorbis")
else()
	message(STATUS "Using system vorbis")
endif()
if(INTERNAL_VORBISFILE)
	message(STATUS "Using internal vorbisfile")
else()
	message(STATUS "Using system vorbisfile")
endif()

if(INTERNAL_SDL_MIXER_CODECS)
	add_subdirectory(external)
endif()

if("${SDL_VERSION_STRING}" VERSION_LESS "2.0.0")
	set(MIXER_LIB_NAME "SDL_mixer")
else()
	set(MIXER_LIB_NAME "SDL2_mixer")
endif()
if("${SDL_VERSION_STRING}" VERSION_LESS "2.0.6")
	list(APPEND EXTRA_SOURCES sdl_resample/SDL_rwops.c)
endif()
if("${SDL_VERSION_STRING}" VERSION_LESS "2.0.7")
	list(APPEND EXTRA_SOURCES
		sdl_resample/SDL_audiocvt.c
		sdl_resample/SDL_audiotypecvt.c
		sdl_resample/SDL_dataqueue.c
	)
endif()

if(NOT APPLE AND NOT WIN32 AND NOT ANDROID)
	if(VENDORED_LIB)
		set(DYN_FLUIDSYNTH ON)
	else()
		option(DYN_FLUIDSYNTH "Dynamically load fluidsynth" ON)
	endif()
	if(DYN_FLUIDSYNTH)
		add_definitions("-DFLUIDSYNTH_DYNAMIC=\"libfluidsynth.so.1\"")
	endif()
endif()

if (WIN32 OR APPLE)
	add_definitions(-DMUSIC_MID_NATIVE)
	set(MIDI_SOURCES music_nativemidi.c native_midi/native_midi_common.c)
	if (APPLE)
		set(MIDI_SOURCES ${MIDI_SOURCES} native_midi/native_midi_macosx.c)
		find_library(AUDIOTOOLBOX AudioToolbox)
		find_library(AUDIOUNIT AudioUnit)
		find_library(MIDI_LIBRARIES CoreMIDI)
		set(MIDI_LIBRARIES ${MIDI_LIBRARIES} ${AUDIOTOOLBOX} ${AUDIOUNIT})
	else()
		set(MIDI_SOURCES ${MIDI_SOURCES} native_midi/native_midi_win32.c)
		set(MIDI_LIBRARIES winmm)
	endif()
elseif (ANDROID)
	set(MIDI_SOURCES "")
	set(MIDI_LIBRARIES "")
else ()
	add_definitions(-DMUSIC_MID_FLUIDSYNTH)
	set(MIDI_SOURCES music_fluidsynth.c)
	if(DYN_FLUIDSYNTH)
		set(MIDI_LIBRARIES "")
	else()
		set(MIDI_LIBRARIES ${FLUIDSYNTH_LIBRARY})
	endif()
endif ()

if(VENDORED_LIB)
	set(MAKE_SHARED OFF)
else()
	option(MAKE_SHARED "Build a shared library instead of static." OFF)
endif()
if(MAKE_SHARED)
	set(LIB_TYPE SHARED)
else()
	set(LIB_TYPE STATIC)
endif()

add_library(SDL2_mixer ${LIB_TYPE}
	${EXTRA_SOURCES}
	${MIDI_SOURCES}
	effect_position.c
	effect_stereoreverse.c
	effects_internal.c
	load_aiff.c
	load_voc.c
	mixer.c
	music.c
	music_flac.c
	music_modplug.c
	music_ogg.c
	music_opus.c
	music_wav.c
)

target_compile_definitions(SDL2_mixer PRIVATE
	-DMUSIC_WAV
	-DMUSIC_FLAC
	-DMUSIC_OGG
	-DMUSIC_OPUS
	-DMUSIC_MOD_MODPLUG
)
if(NOT ANDROID)
	target_compile_definitions(SDL2_mixer PRIVATE -DMID_MUSIC)
endif()

target_include_directories(SDL2_mixer
INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}
PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/native_midi
	${SDL_INCLUDE_DIR}
	${VORBISFILE_INCLUDE_DIR}
	${FLAC_INCLUDE_DIR}
	${OGG_INCLUDE_DIR}
	${OPUSFILE_INCLUDE_DIR}
	${OPUS_INCLUDE_DIR}
	${MODPLUG_INCLUDE_DIR}
	${FLUIDSYNTH_INCLUDE_DIR}
)

target_link_libraries(SDL2_mixer
PUBLIC
	${SDL_LIBRARY}
PRIVATE
	${MIDI_LIBRARIES}
	${VORBISFILE_LIBRARY}
	${VORBIS_LIBRARY}
	${OPUSFILE_LIBRARY}
	${OPUS_LIBRARY}
	${FLAC_LIBRARY}
	${MODPLUG_LIBRARY}
	${OGG_LIBRARY}
)

set_target_properties(SDL2_mixer PROPERTIES OUTPUT_NAME ${MIXER_LIB_NAME})

if(ANDROID)
	target_link_libraries(SDL2_mixer PRIVATE m android log)
endif()

if(WIN32)
	set_target_properties(SDL2_mixer PROPERTIES PREFIX "")
	if(NOT MSVC)
		set_property(TARGET SDL2_mixer APPEND PROPERTY LINK_FLAGS "-Wl,--output-def,${MIXER_LIB_NAME}.def")

		# Find tool to make lib
		set(PROGFILESx86 "PROGRAMFILES(X86)")
		find_program(LIB_EXECUTABLE lib.exe HINTS "$ENV{${PROGFILESX86}}\\Microsoft Visual Studio 14.0\\VC\\bin\\amd64")
		if(LIB_EXECUTABLE)
			set(LIB_MACHINE "i386" CACHE STRING "Machine type to generate lib for.")
			add_custom_command(TARGET SDL2_mixer POST_BUILD COMMAND "${LIB_EXECUTABLE}" "/machine:${LIB_MACHINE}" "/def:${MIXER_LIB_NAME}.def" WORKING_DIRECTORY ${CMAKE_BINARY_DIR} COMMENT "Building Visual Studio lib file.")
		endif()
	else()
		target_compile_options(SDL2_mixer PRIVATE "/wd4996")
	endif()
endif()

if(NOT VENDORED_LIB)
	add_executable(mixtest mixtest.c)
	target_link_libraries(mixtest SDL2_mixer ${SDL_LIBRARY} ${MIDI_LIBRARIES})
endif()
