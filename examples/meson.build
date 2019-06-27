project('cutl examples', 'c')

cc = meson.get_compiler('c')

libm_dep = cc.find_library('m')
cutl_dep = dependency('cutl')
lutl_dep = dependency('cutl')

executable('simple', 'simple.c', dependencies : cutl_dep)
executable('common', 'common.c', dependencies : [cutl_dep, libm_dep])
