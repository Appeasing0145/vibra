if(NOT EMSCRIPTEN)
    message(FATAL_ERROR "The JavaScript/WebAssembly bindings require Emscripten")
endif()

add_executable(vibra_wasm wasm.cpp)

target_include_directories(vibra_wasm PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(vibra_wasm PRIVATE vibra_static)

target_link_options(vibra_wasm PRIVATE
    "SHELL:-s EXPORTED_FUNCTIONS=['_GetWavSignature','_GetSignedPcmSignature','_GetFloatPcmSignature','_GetFingerprint','_GetSampleMs','_FreeFingerprint','_malloc','_free']"
    "SHELL:-s EXPORTED_RUNTIME_METHODS=['ccall','cwrap']"
    "SHELL:-s ALLOW_MEMORY_GROWTH=1"
    "SHELL:-s WASM=1"
    "SHELL:-s MODULARIZE=1"
    "SHELL:-s EXPORT_ES6=1"
    "SHELL:-s EXPORT_NAME=createModule"
    "SHELL:-s ENVIRONMENT=web,worker"
)

set_target_properties(vibra_wasm PROPERTIES
    OUTPUT_NAME "vibra"
)
