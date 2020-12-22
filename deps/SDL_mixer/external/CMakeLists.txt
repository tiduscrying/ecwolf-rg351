include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckTypeSize)

set(HEADER_LIST
	alloca.h
	byteswap.h
	cpuid.h
	dlfcn.h
	inttypes.h
	memory.h
	stdint.h
	stdlib.h
	strings.h
	string.h
	sys/ioctl.h
	sys/param.h
	sys/stat.h
	sys/types.h
	termios.h
	unistd.h
	x86intrin.h
)
foreach(HEADER_CHECK ${HEADER_LIST})
	string(TOUPPER "${HEADER_CHECK}" HEADER_VAR_NAME)
	string(REPLACE "/" "_" HEADER_VAR_NAME "${HEADER_VAR_NAME}")
	string(REPLACE "." "_" HEADER_VAR_NAME "${HEADER_VAR_NAME}")
	check_include_file(${HEADER_CHECK} HAVE_${HEADER_VAR_NAME})
endforeach()

set(CMAKE_REQUIRED_LIBRARIES m)
set(FUNCTION_LIST
	clock_gettime
	getopt_long
	iconv
	lround
	setenv
	sinf
)
foreach(FN ${FUNCTION_LIST})
	string(TOUPPER "${FN}" FN_VAR_NAME)
	check_function_exists("${FN}" HAVE_${FN_VAR_NAME})
endforeach()

function(configure_autoheader FILENAME OUTFILENAME)
	file(READ ${FILENAME} CONFIG_H_DATA)
	string(REGEX REPLACE "#undef ([a-zA-Z0-9_]+)" "#cmakedefine \\1 @\\1@" CONFIG_H_DATA "${CONFIG_H_DATA}")
	file(WRITE ${OUTFILENAME}.in "${CONFIG_H_DATA}")
	configure_file(${OUTFILENAME}.in ${OUTFILENAME})
endfunction()

# libogg -----------------------------------------------------------------------

set(INCLUDE_INTTYPES_H ${HAVE_INTTYPES_H})
set(INCLUDE_STDINT_H ${HAVE_STDINT_H})
set(INCLUDE_SYS_TYPES_H ${HAVE_SYS_TYPES_H})

set(SIZE16 int16_t)
set(USIZE16 uint16_t)
set(SIZE32 int32_t)
set(USIZE32 uint32_t)
set(SIZE64 int64_t)

set(LIBOGG libogg-1.3.2)

configure_file(${LIBOGG}/include/ogg/config_types.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/ogg/config_types.h)

add_library(ogg STATIC
	${LIBOGG}/src/bitwise.c
	${LIBOGG}/src/framing.c
)

target_include_directories(ogg
PUBLIC
	${LIBOGG}/include
	${CMAKE_CURRENT_BINARY_DIR}/include
)

set(OGG_LIBRARY ogg PARENT_SCOPE)
set(OGG_INCLUDE_DIR "" PARENT_SCOPE)

# libvorbis --------------------------------------------------------------------

set(LIBVORBIS libvorbis-1.3.5)

if(WIN32)
	set(HAVE_ALLOCA 1)
else()
	set(HAVE_ALLOCA ${HAVE_ALLOCA_H})
endif()
set(STDC_HEADERS 1)

configure_autoheader(${LIBVORBIS}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/vorbis/private/config.h)

add_library(vorbis STATIC
	${LIBVORBIS}/lib/mdct.c
	${LIBVORBIS}/lib/smallft.c
	${LIBVORBIS}/lib/block.c
	${LIBVORBIS}/lib/envelope.c
	${LIBVORBIS}/lib/window.c
	${LIBVORBIS}/lib/lsp.c
	${LIBVORBIS}/lib/lpc.c
	${LIBVORBIS}/lib/analysis.c
	${LIBVORBIS}/lib/synthesis.c
	${LIBVORBIS}/lib/psy.c
	${LIBVORBIS}/lib/info.c
	${LIBVORBIS}/lib/floor1.c
	${LIBVORBIS}/lib/floor0.c
	${LIBVORBIS}/lib/res0.c
	${LIBVORBIS}/lib/mapping0.c
	${LIBVORBIS}/lib/registry.c
	${LIBVORBIS}/lib/codebook.c
	${LIBVORBIS}/lib/sharedbook.c
	${LIBVORBIS}/lib/lookup.c
	${LIBVORBIS}/lib/bitrate.c
)

target_include_directories(vorbis
PUBLIC
	${LIBVORBIS}/include
PRIVATE
	${CMAKE_CURRENT_BINARY_DIR}/include/vorbis/private
)

target_compile_definitions(vorbis PRIVATE "-DHAVE_CONFIG_H")
if(MSVC)
	target_compile_options(vorbis PRIVATE "/wd4244" "/wd4267" "/wd4305" "/wd4996")
endif()

target_link_libraries(vorbis ogg)

set(VORBIS_LIBRARY vorbis PARENT_SCOPE)
set(VORBIS_INCLUDE_DIR "" PARENT_SCOPE)

add_library(vorbisfile STATIC
	${LIBVORBIS}/lib/vorbisfile.c
)

target_include_directories(vorbisfile PUBLIC ${LIBVORBIS}/include/vorbis)
if(MSVC)
	target_compile_options(vorbisfile PRIVATE "/wd4244" "/wd4267" "/wd4305" "/wd4996")
endif()

target_link_libraries(vorbisfile vorbis ogg)

set(VORBISFILE_LIBRARY vorbisfile PARENT_SCOPE)
set(VORBISFILE_INCLUDE_DIR "" PARENT_SCOPE)

# libFLAC ----------------------------------------------------------------------

set(LIBFLAC flac-1.3.2)

set(PACKAGE_VERSION "\"1.3.2\"")
set(FLAC__HAS_OGG "(0)") # Don't need OGG support here
set(FLAC__NO_ASM 1)
set(FLAC__HAS_X86INTRIN ${HAVE_X86INTRIN_H})
set(FLAC__USE_AVX 1)

# Hack
set(FLAC__CPU_IA32 "\n#if !defined __i386__ && !defined _M_IX86\n#undef FLAC__CPU_IA32\n#endif")
set(FLAC__CPU_X86_64 "\n#if !defined __x86_64__ && !defined _M_X64\n#undef FLAC__CPU_X86_64\n#endif")
set(FLAC__CPU_PPC "\n#if !defined __powerpc__ && !defined _M_PPC\n#undef FLAC__CPU_PPC\n#endif")
set(CPU_IS_BIG_ENDIAN "\n#if !defined __powerpc__ && !defined _M_PPC\n#undef CPU_IS_BIG_ENDIAN\n#endif")
set(CPU_IS_LITTLE_ENDIAN "\n#if defined __powerpc__ || defined _M_PPC\n#undef CPU_IS_LITTLE_ENDIAN\n#endif")

configure_autoheader(${LIBFLAC}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/FLAC/private/config.h)

if(WIN32)
	set(FLAC_EXTRA_SOURCES ${LIBFLAC}/src/libFLAC/windows_unicode_filenames.c)
else()
	set(FLAC_EXTRA_SOURCES "")
endif()

add_library(FLAC STATIC
	${FLAC_EXTRA_SOURCES}

	${LIBFLAC}/src/libFLAC/bitmath.c
	${LIBFLAC}/src/libFLAC/bitreader.c
	${LIBFLAC}/src/libFLAC/bitwriter.c
	${LIBFLAC}/src/libFLAC/cpu.c
	${LIBFLAC}/src/libFLAC/crc.c
	${LIBFLAC}/src/libFLAC/fixed.c
	${LIBFLAC}/src/libFLAC/fixed_intrin_sse2.c
	${LIBFLAC}/src/libFLAC/fixed_intrin_ssse3.c
	${LIBFLAC}/src/libFLAC/float.c
	${LIBFLAC}/src/libFLAC/format.c
	${LIBFLAC}/src/libFLAC/lpc.c
	${LIBFLAC}/src/libFLAC/lpc_intrin_sse.c
	${LIBFLAC}/src/libFLAC/lpc_intrin_sse2.c
	${LIBFLAC}/src/libFLAC/lpc_intrin_sse41.c
	${LIBFLAC}/src/libFLAC/lpc_intrin_avx2.c
	${LIBFLAC}/src/libFLAC/md5.c
	${LIBFLAC}/src/libFLAC/memory.c
	${LIBFLAC}/src/libFLAC/metadata_iterators.c
	${LIBFLAC}/src/libFLAC/metadata_object.c
	${LIBFLAC}/src/libFLAC/stream_decoder.c
	${LIBFLAC}/src/libFLAC/stream_encoder.c
	${LIBFLAC}/src/libFLAC/stream_encoder_intrin_sse2.c
	${LIBFLAC}/src/libFLAC/stream_encoder_intrin_ssse3.c
	${LIBFLAC}/src/libFLAC/stream_encoder_intrin_avx2.c
	${LIBFLAC}/src/libFLAC/stream_encoder_framing.c
	${LIBFLAC}/src/libFLAC/window.c
)

target_compile_definitions(FLAC PRIVATE "-DHAVE_CONFIG_H" PUBLIC "-DFLAC__NO_DLL")
if(MSVC)
	target_compile_options(FLAC PRIVATE "/wd4244" "/wd4267" "/wd4334" "/wd4996")
elseif(MINGW)
	# Needed for non-w64 MinGW
	target_compile_definitions(FLAC PRIVATE "-D__MSVCRT_VERSION__=0x0601")
endif()

target_include_directories(FLAC
PUBLIC
	${LIBFLAC}/include
PRIVATE
	${LIBFLAC}/src/libFLAC/include
	${CMAKE_CURRENT_BINARY_DIR}/include/FLAC/private
)

set(FLAC_LIBRARY FLAC PARENT_SCOPE)
set(FLAC_INCLUDE_DIR "" PARENT_SCOPE)

# libmodplug -------------------------------------------------------------------

set(LIBMODPLUG libmodplug-0.8.9.0)

configure_autoheader(${LIBMODPLUG}/src/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/libmodplug/private/config.h)

add_library(modplug STATIC
	${LIBMODPLUG}/src/sndmix.cpp
	${LIBMODPLUG}/src/sndfile.cpp
	${LIBMODPLUG}/src/snd_fx.cpp
	${LIBMODPLUG}/src/snd_flt.cpp
	${LIBMODPLUG}/src/snd_dsp.cpp
	${LIBMODPLUG}/src/fastmix.cpp
	${LIBMODPLUG}/src/mmcmp.cpp
	${LIBMODPLUG}/src/load_xm.cpp
	${LIBMODPLUG}/src/load_wav.cpp
	${LIBMODPLUG}/src/load_umx.cpp
	${LIBMODPLUG}/src/load_ult.cpp
	${LIBMODPLUG}/src/load_stm.cpp
 	${LIBMODPLUG}/src/load_s3m.cpp
	${LIBMODPLUG}/src/load_ptm.cpp
	${LIBMODPLUG}/src/load_okt.cpp
	${LIBMODPLUG}/src/load_mtm.cpp
	${LIBMODPLUG}/src/load_mod.cpp
	${LIBMODPLUG}/src/load_med.cpp
	${LIBMODPLUG}/src/load_mdl.cpp
	${LIBMODPLUG}/src/load_it.cpp
	${LIBMODPLUG}/src/load_far.cpp
	${LIBMODPLUG}/src/load_dsm.cpp
	${LIBMODPLUG}/src/load_dmf.cpp
	${LIBMODPLUG}/src/load_dbm.cpp
	${LIBMODPLUG}/src/load_ams.cpp
	${LIBMODPLUG}/src/load_amf.cpp
	${LIBMODPLUG}/src/load_669.cpp
	${LIBMODPLUG}/src/load_j2b.cpp
	${LIBMODPLUG}/src/load_mt2.cpp
	${LIBMODPLUG}/src/load_psm.cpp
	${LIBMODPLUG}/src/load_abc.cpp
	${LIBMODPLUG}/src/load_mid.cpp
	${LIBMODPLUG}/src/load_pat.cpp
	${LIBMODPLUG}/src/modplug.cpp
)

add_custom_command(TARGET modplug POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/${LIBMODPLUG}/src/modplug.h" "${CMAKE_CURRENT_BINARY_DIR}/include/libmodplug/modplug.h"
)

target_compile_definitions(modplug PUBLIC "-DMODPLUG_STATIC" PRIVATE "-DHAVE_CONFIG_H")
if(MSVC)
	target_compile_options(modplug PRIVATE "/wd4018" "/wd4244" "/wd4267" "/wd4996")
endif()

target_include_directories(modplug
INTERFACE
	${CMAKE_CURRENT_BINARY_DIR}/include
PRIVATE
	${LIBMODPLUG}/src/libmodplug
	${CMAKE_CURRENT_BINARY_DIR}/include/libmodplug/private
)

set(MODPLUG_LIBRARY modplug PARENT_SCOPE)
set(MODPLUG_INCLUDE_DIR "" PARENT_SCOPE)

# opus -------------------------------------------------------------------------

set(LIBOPUS opus-1.2.1)

set(OPUS_BUILD 1)
if(WIN32)
	set(USE_ALLOCA 1)
else()
	set(USE_ALLOCA ${HAVE_ALLOCA_H})
endif()

# Hack
set(OPUS_HAVE_RTCD 1)
set(OPUS_X86_MAY_HAVE_SSE "\n#if !defined __i386__ && !defined _M_IX86\n#undef OPUS_X86_MAY_HAVE_SSE\n#endif")
set(OPUS_X86_PRESUME_SSE "\n#if !defined __x86_64__ && !defined _M_X64\n#undef OPUS_X86_PRESUME_SSE\n#endif")
set(OPUS_X86_MAY_HAVE_SSE2 "\n#if (!defined __i386__ && !defined _M_IX86) || (defined _MSVC_VER || _MSVC_VER < 1500)\n#undef OPUS_X86_MAY_HAVE_SSE2\n#endif")
set(OPUS_X86_PRESUME_SSE2 "\n#if !defined __x86_64__ && !defined _M_X64\n#undef OPUS_X86_PRESUME_SSE2\n#endif")
set(OPUS_X86_MAY_HAVE_SSE4_1 "\n#if (!defined __i386__ && !defined _M_IX86 && !defined __x86_64__ && !defined _M_X64) || (defined _MSVC_VER || _MSVC_VER < 1500)\n#undef OPUS_X86_MAY_HAVE_SSE4_1\n#endif")
set(OPUS_X86_MAY_HAVE_AVX "\n#if !defined __i386__ && !defined _M_IX86 && !defined __x86_64__ && !defined _M_X64\n#undef OPUS_X86_MAY_HAVE_AVX\n#endif")

configure_autoheader(${LIBOPUS}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/opus/private/config.h)

set(OPUS_SSE_SOURCES
	# Celt SSE
	${LIBOPUS}/celt/x86/x86cpu.c
	${LIBOPUS}/celt/x86/x86_celt_map.c
	${LIBOPUS}/celt/x86/pitch_sse.c
)
set(OPUS_SSE2_SOURCES
	# Celt SSE2
	${LIBOPUS}/celt/x86/pitch_sse2.c
	${LIBOPUS}/celt/x86/vq_sse2.c
)
set(OPUS_SSE41_SOURCES
	# Celt SSE4.1
	${LIBOPUS}/celt/x86/celt_lpc_sse.c
	${LIBOPUS}/celt/x86/pitch_sse4_1.c

	# Silk SSE4.1
	#${LIBOPUS}/silk/x86/NSQ_sse.c
	#${LIBOPUS}/silk/x86/NSQ_del_dec_sse.c
	#${LIBOPUS}/silk/x86/x86_silk_map.c
	#${LIBOPUS}/silk/x86/VAD_sse.c
	#${LIBOPUS}/silk/x86/VQ_WMat_EC_sse.c
)

if((NOT ANDROID OR CMAKE_ANDROID_ARCH STREQUAL "x86" OR CMAKE_ANDROID_ARCH STREQUAL "x86_64") AND (NOT APPLE OR NOT CMAKE_OSX_ARCHITECTURES MATCHES ppc))
	set(OPUS_ARCH_SOURCES ${OPUS_SSE_SOURCES})
	# MSVC 2005 doesn't support SSE4.1 for obvious reasons
	# SSE2 disabled there as well due to some missing intrinsics
	if(NOT MSVC OR MSVC_VERSION GREATER 1499)
		set(OPUS_ARCH_SOURCES ${OPUS_ARCH_SOURCES} ${OPUS_SSE2_SOURCES} ${OPUS_SSE41_SOURCES})
	endif()
else()
	set(OPUS_ARCH_SOURCES "")
endif()

add_library(opus STATIC
	${LIBOPUS}/src/opus.c
	${LIBOPUS}/src/opus_decoder.c
	${LIBOPUS}/src/opus_encoder.c
	${LIBOPUS}/src/opus_multistream.c
	${LIBOPUS}/src/opus_multistream_encoder.c
	${LIBOPUS}/src/opus_multistream_decoder.c
	${LIBOPUS}/src/repacketizer.c

	# Opus float
	${LIBOPUS}/src/analysis.c
	${LIBOPUS}/src/mlp.c
	${LIBOPUS}/src/mlp_data.c

	# Celt
	${LIBOPUS}/celt/bands.c
	${LIBOPUS}/celt/celt.c
	${LIBOPUS}/celt/celt_encoder.c
	${LIBOPUS}/celt/celt_decoder.c
	${LIBOPUS}/celt/cwrs.c
	${LIBOPUS}/celt/entcode.c
	${LIBOPUS}/celt/entdec.c
	${LIBOPUS}/celt/entenc.c
	${LIBOPUS}/celt/kiss_fft.c
	${LIBOPUS}/celt/laplace.c
	${LIBOPUS}/celt/mathops.c
	${LIBOPUS}/celt/mdct.c
	${LIBOPUS}/celt/modes.c
	${LIBOPUS}/celt/pitch.c
	${LIBOPUS}/celt/celt_lpc.c
	${LIBOPUS}/celt/quant_bands.c
	${LIBOPUS}/celt/rate.c
	${LIBOPUS}/celt/vq.c

	# Silk
	${LIBOPUS}/silk/CNG.c
	${LIBOPUS}/silk/code_signs.c
	${LIBOPUS}/silk/init_decoder.c
	${LIBOPUS}/silk/decode_core.c
	${LIBOPUS}/silk/decode_frame.c
	${LIBOPUS}/silk/decode_parameters.c
	${LIBOPUS}/silk/decode_indices.c
	${LIBOPUS}/silk/decode_pulses.c
	${LIBOPUS}/silk/decoder_set_fs.c
	${LIBOPUS}/silk/dec_API.c
	${LIBOPUS}/silk/enc_API.c
	${LIBOPUS}/silk/encode_indices.c
	${LIBOPUS}/silk/encode_pulses.c
	${LIBOPUS}/silk/gain_quant.c
	${LIBOPUS}/silk/interpolate.c
	${LIBOPUS}/silk/LP_variable_cutoff.c
	${LIBOPUS}/silk/NLSF_decode.c
	${LIBOPUS}/silk/NSQ.c
	${LIBOPUS}/silk/NSQ_del_dec.c
	${LIBOPUS}/silk/PLC.c
	${LIBOPUS}/silk/shell_coder.c
	${LIBOPUS}/silk/tables_gain.c
	${LIBOPUS}/silk/tables_LTP.c
	${LIBOPUS}/silk/tables_NLSF_CB_NB_MB.c
	${LIBOPUS}/silk/tables_NLSF_CB_WB.c
	${LIBOPUS}/silk/tables_other.c
	${LIBOPUS}/silk/tables_pitch_lag.c
	${LIBOPUS}/silk/tables_pulses_per_block.c
	${LIBOPUS}/silk/VAD.c
	${LIBOPUS}/silk/control_audio_bandwidth.c
	${LIBOPUS}/silk/quant_LTP_gains.c
	${LIBOPUS}/silk/VQ_WMat_EC.c
	${LIBOPUS}/silk/HP_variable_cutoff.c
	${LIBOPUS}/silk/NLSF_encode.c
	${LIBOPUS}/silk/NLSF_VQ.c
	${LIBOPUS}/silk/NLSF_unpack.c
	${LIBOPUS}/silk/NLSF_del_dec_quant.c
	${LIBOPUS}/silk/process_NLSFs.c
	${LIBOPUS}/silk/stereo_LR_to_MS.c
	${LIBOPUS}/silk/stereo_MS_to_LR.c
	${LIBOPUS}/silk/check_control_input.c
	${LIBOPUS}/silk/control_SNR.c
	${LIBOPUS}/silk/init_encoder.c
	${LIBOPUS}/silk/control_codec.c
	${LIBOPUS}/silk/A2NLSF.c
	${LIBOPUS}/silk/ana_filt_bank_1.c
	${LIBOPUS}/silk/biquad_alt.c
	${LIBOPUS}/silk/bwexpander_32.c
	${LIBOPUS}/silk/bwexpander.c
	${LIBOPUS}/silk/debug.c
	${LIBOPUS}/silk/decode_pitch.c
	${LIBOPUS}/silk/inner_prod_aligned.c
	${LIBOPUS}/silk/lin2log.c
	${LIBOPUS}/silk/log2lin.c
	${LIBOPUS}/silk/LPC_analysis_filter.c
	${LIBOPUS}/silk/LPC_inv_pred_gain.c
	${LIBOPUS}/silk/table_LSF_cos.c
	${LIBOPUS}/silk/NLSF2A.c
	${LIBOPUS}/silk/NLSF_stabilize.c
	${LIBOPUS}/silk/NLSF_VQ_weights_laroia.c
	${LIBOPUS}/silk/pitch_est_tables.c
	${LIBOPUS}/silk/resampler.c
	${LIBOPUS}/silk/resampler_down2_3.c
	${LIBOPUS}/silk/resampler_down2.c
	${LIBOPUS}/silk/resampler_private_AR2.c
	${LIBOPUS}/silk/resampler_private_down_FIR.c
	${LIBOPUS}/silk/resampler_private_IIR_FIR.c
	${LIBOPUS}/silk/resampler_private_up2_HQ.c
	${LIBOPUS}/silk/resampler_rom.c
	${LIBOPUS}/silk/sigm_Q15.c
	${LIBOPUS}/silk/sort.c
	${LIBOPUS}/silk/sum_sqr_shift.c
	${LIBOPUS}/silk/stereo_decode_pred.c
	${LIBOPUS}/silk/stereo_encode_pred.c
	${LIBOPUS}/silk/stereo_find_predictor.c
	${LIBOPUS}/silk/stereo_quant_pred.c
	${LIBOPUS}/silk/LPC_fit.c

	# Silk float
	${LIBOPUS}/silk/float/apply_sine_window_FLP.c
	${LIBOPUS}/silk/float/corrMatrix_FLP.c
	${LIBOPUS}/silk/float/encode_frame_FLP.c
	${LIBOPUS}/silk/float/find_LPC_FLP.c
	${LIBOPUS}/silk/float/find_LTP_FLP.c
	${LIBOPUS}/silk/float/find_pitch_lags_FLP.c
	${LIBOPUS}/silk/float/find_pred_coefs_FLP.c
	${LIBOPUS}/silk/float/LPC_analysis_filter_FLP.c
	${LIBOPUS}/silk/float/LTP_analysis_filter_FLP.c
	${LIBOPUS}/silk/float/LTP_scale_ctrl_FLP.c
	${LIBOPUS}/silk/float/noise_shape_analysis_FLP.c
	${LIBOPUS}/silk/float/process_gains_FLP.c
	${LIBOPUS}/silk/float/regularize_correlations_FLP.c
	${LIBOPUS}/silk/float/residual_energy_FLP.c
	${LIBOPUS}/silk/float/warped_autocorrelation_FLP.c
	${LIBOPUS}/silk/float/wrappers_FLP.c
	${LIBOPUS}/silk/float/autocorrelation_FLP.c
	${LIBOPUS}/silk/float/burg_modified_FLP.c
	${LIBOPUS}/silk/float/bwexpander_FLP.c
	${LIBOPUS}/silk/float/energy_FLP.c
	${LIBOPUS}/silk/float/inner_product_FLP.c
	${LIBOPUS}/silk/float/k2a_FLP.c
	${LIBOPUS}/silk/float/LPC_inv_pred_gain_FLP.c
	${LIBOPUS}/silk/float/pitch_analysis_core_FLP.c
	${LIBOPUS}/silk/float/scale_copy_vector_FLP.c
	${LIBOPUS}/silk/float/scale_vector_FLP.c
	${LIBOPUS}/silk/float/schur_FLP.c
	${LIBOPUS}/silk/float/sort_FLP.c

	${OPUS_ARCH_SOURCES}
)

target_compile_definitions(opus PRIVATE "-DHAVE_CONFIG_H")

target_include_directories(opus
PUBLIC
	${LIBOPUS}/include
PRIVATE
	${LIBOPUS}
	${LIBOPUS}/celt
	${LIBOPUS}/silk
	${LIBOPUS}/silk/float
	${CMAKE_CURRENT_BINARY_DIR}/include/opus/private
)

if(MINGW OR (APPLE AND NOT CMAKE_OSX_ARCHITECTURES MATCHES ppc))
	foreach(SSE_SOURCE ${OPUS_SSE_SOURCES})
		set_source_files_properties(${SSE_SOURCE} PROPERTIES COMPILE_FLAGS "-msse")
	endforeach()
	foreach(SSE_SOURCE ${OPUS_SSE2_SOURCES})
		set_source_files_properties(${SSE_SOURCE} PROPERTIES COMPILE_FLAGS "-msse -msse2")
	endforeach()
	foreach(SSE_SOURCE ${OPUS_SSE41_SOURCES})
		set_source_files_properties(${SSE_SOURCE} PROPERTIES COMPILE_FLAGS "-msse -msse2 -msse4.1")
	endforeach()
endif()

set(OPUS_LIBRARY opus PARENT_SCOPE)
set(OPUS_INCLUDE_DIR "" PARENT_SCOPE)

# opusfile ---------------------------------------------------------------------

set(LIBOPUSFILE opusfile-0.9)

configure_autoheader(${LIBOPUSFILE}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/opusfile/private/config.h)

add_library(opusfile STATIC
	${LIBOPUSFILE}/src/info.c
	${LIBOPUSFILE}/src/internal.c
	${LIBOPUSFILE}/src/opusfile.c
	${LIBOPUSFILE}/src/stream.c
)

add_custom_command(TARGET opusfile POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/${LIBOPUSFILE}/include/opusfile.h" "${CMAKE_CURRENT_BINARY_DIR}/include/opus/opusfile.h"
)

target_compile_definitions(opusfile PRIVATE "-DHAVE_CONFIG_H")

target_include_directories(opusfile
PUBLIC
	${LIBOPUSFILE}/include
PRIVATE
	${CMAKE_CURRENT_BINARY_DIR}/include/opusfile/private
)

target_link_libraries(opusfile opus ogg)

set(OPUSFILE_LIBRARY opusfile PARENT_SCOPE)
set(OPUSFILE_INCLUDE_DIR "" PARENT_SCOPE)
