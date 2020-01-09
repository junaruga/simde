simde_test_js_sources = [
  'int32x4.c'
]

foreach src : simde_test_js_sources
  simde_test_js_sources += configure_file(input: src, output: src + 'pp', copy: true)
endforeach

simde_tests_js_emul = static_library('simde-tests-js-emul', simde_test_js_sources,
  c_args: '-DSIMDE_NO_NATIVE',
  cpp_args: '-DSIMDE_NO_NATIVE')

simde_tests_js_native = static_library('simde-tests-js-native', simde_test_js_sources,
  c_args: simde_native_c_flags,
  cpp_args: simde_native_cxx_flags)

simde_tests_js = static_library('simde-tests-js', 'test-js.c',
  link_with: [simde_tests_js_emul, simde_tests_js_native])
