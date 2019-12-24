simde_test_x86_sources = [
  'isa.c'
  # 'sse.c',
  # 'sse2.c',
  # 'sse3.c',
  # 'ssse3.c',
  # 'sse4.1.c',
  # 'sse4.2.c',
  # 'avx.c',
  # 'avx2.c',
  # 'fma.c'
]

foreach src : simde_test_x86_sources
  simde_test_x86_sources += configure_file(input: src, output: src + 'pp', copy: true)
endforeach

simde_tests_x86_emul = static_library('simde-tests-x86-emul', simde_test_x86_sources,
  c_args: '-DSIMDE_NO_NATIVE',
  cpp_args: '-DSIMDE_NO_NATIVE')

simde_tests_x86_native = static_library('simde-tests-x86-native', simde_test_x86_sources,
  c_args: simde_native_c_flags,
  cpp_args: simde_native_cxx_flags)

simde_tests_x86 = static_library('simde-tests-x86', 'test-x86.c',
  link_with: [simde_tests_x86_emul, simde_tests_x86_native])
