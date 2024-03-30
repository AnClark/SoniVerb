cmake_minimum_required (VERSION 3.20)

project (SONiVerb
    VERSION 0.0.1
    LANGUAGES C CXX
)

add_subdirectory (dep/dpf)

# TODO: May need resampling on final output side, or try to modify _OUTPUT_SAMPLE_RATE param in eas_reverb.c!
add_compile_definitions (
    _SAMPLE_RATE_44100=1
)

include_directories (src)

dpf_add_plugin (${PROJECT_NAME}
    TARGETS vst2 vst3 clap lv2
    FILES_DSP
        plugin/SoniVerb_Plugin.cpp
        src/eas_math.cpp
        src/eas_reverb.cpp
        src/eas_reverbdata.cpp
)
target_include_directories (${PROJECT_NAME} PUBLIC plugin/)
