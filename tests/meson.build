# SOURCES

cutl_tests_src = [
  'tests.c', 'cutl_tests.c',
  'cutl_parse_args_tests.c', 'cutl_message_tests.c', 'cutl_run_tests.c',
  'cutl_summary_tests.c', 'cutl_get_tests.c',
]


lutl_tests_src = [
  'tests.c', 'lutl_tests.c',
]


# TARGETS

cutl_tests = executable('cutl_tests', cutl_tests_src, dependencies : [cutl_dep])

test('cutl_tests', cutl_tests)


if lua_dep.found()
  lutl_tests = executable(
    'lutl_tests', lutl_tests_src, dependencies : [cutl_dep, lutl_dep]
  )

  test('lutl_tests', lutl_tests, workdir : meson.source_root())
endif

