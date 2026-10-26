# libFuzzer targets — Clang only. Enabled via COLORCPP_BUILD_FUZZERS=ON and
# driven by the time-boxed `fuzz` CI job (.github/workflows/ci.yml).
#
# Local run:
#   cmake -G Ninja -B build-fuzz -S . -DCMAKE_C_COMPILER=clang \
#     -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Debug \
#     -DCOLORCPP_BUILD_FUZZERS=ON
#   cmake --build build-fuzz
#   ./build-fuzz/fuzz/fuzz_parse_css_color -max_total_time=60
#
# Windows note: copy clang_rt.asan_dynamic-x86_64.dll from
# <llvm>/lib/clang/<ver>/lib/windows/ next to the built exes before running;
# the LLVM installer does not put it on PATH.

# Compile without the libFuzzer runtime, link with it plus ASan/UBSan so
# memory errors and UB abort the fuzzer instead of silently corrupting state.
set(COLORCPP_FUZZ_COMPILE_OPTIONS
  -fsanitize=fuzzer-no-link,address,undefined
  -fno-omit-frame-pointer
  -g
)
set(COLORCPP_FUZZ_LINK_OPTIONS
  -fsanitize=fuzzer,address,undefined
)

foreach(fuzz_target IN ITEMS fuzz_parse_css_color fuzz_read_cube)
  add_executable(${fuzz_target} ${fuzz_target}.cpp)
  target_link_libraries(${fuzz_target} PRIVATE colorcpp colorcpp_warnings)
  target_compile_options(${fuzz_target} PRIVATE ${COLORCPP_FUZZ_COMPILE_OPTIONS})
  target_link_options(${fuzz_target} PRIVATE ${COLORCPP_FUZZ_LINK_OPTIONS})
  if(WIN32)
    # Prebuilt LLVM Windows compiler-rt fuzz runtime (clang_rt.fuzzer) is
    # MT_StaticRelease with STL container annotations off. Under
    # -fsanitize=address the MSVC STL auto-activates annotate_string/vector,
    # which mismatches clang_rt.fuzzer at link — disable both. Static release
    # CRT in every config keeps the object format identical.
    set_property(TARGET ${fuzz_target} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
    target_compile_definitions(${fuzz_target} PRIVATE
      _DISABLE_STRING_ANNOTATION
      _DISABLE_VECTOR_ANNOTATION
    )
  endif()
  set_target_properties(${fuzz_target} PROPERTIES FOLDER "fuzz")
endforeach()
