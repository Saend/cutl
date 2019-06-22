project(
  'cutl', 'c', version : '2.0.0',
  default_options : ['c_std=c99'],
  meson_version: '>= 0.50.0',
)

pkg = import('pkgconfig')


# CONFIGURATION

cc = meson.get_compiler('c')
version = meson.project_version().split('.')
auto_color = not get_option('auto_color').disabled()

config_dat = configuration_data({
  'VERSION' : meson.project_version(),
  'VERSION_MAJOR' : version[0],
  'VERSION_MINOR' : version[1],
  'VERSION_PATCH' : version[2],
  'CUTL_SHARED' : get_option('default_library') != 'static',
  'CUTL_USE_ISATTY' : cc.has_function('isatty') and auto_color,
  'CUTL_USE_FILENO' : cc.has_function('fileno') and auto_color,
})

config_h = configure_file(
  input : 'include/cutl_config.h.in', output : 'cutl_config.h',
  configuration : config_dat, install_dir : 'include'
)

headers = [config_h]



# INCLUDES

include_dir = include_directories('include', '.')



# CUTL TARGET

add_project_arguments('-DCUTL_BUILDING', language: 'c')

cutl_lib = library(
  'cutl', 'src/cutl.c', include_directories : include_dir, install : true,
  version : meson.project_version(), gnu_symbol_visibility : 'hidden'
)

cutl_dep = declare_dependency(
  include_directories : include_dir, link_with : cutl_lib
)

pkg.generate(cutl_lib, description : 'C unit testing library')

headers += 'include/cutl.h'



# LUTL TARGET

lua_dep = dependency('lua', required : get_option('lutl'))

if lua_dep.found()
  lutl_lib = library(
    'lutl', 'src/lutl.c', include_directories : include_dir,  install : true,
    version : meson.project_version(), gnu_symbol_visibility : 'hidden',
    dependencies : [lua_dep, cutl_dep]
  )

  lutl_dep = declare_dependency(
    include_directories : include_dir, link_with : lutl_lib,
    dependencies : cutl_dep
  )

  pkg.generate(lutl_lib, description : 'Lua unit testing library')

  headers += 'include/lutl.h'
endif



# DOCUMENTATION

doxygen = find_program('doxygen', required: false)

if doxygen.found()
  doxyfile = configure_file(
    input: 'Doxyfile.in', output: 'Doxyfile', install : false,
    configuration: {
      'VERSION' : meson.project_version(),
      'SRCDIR' : meson.source_root(),
      'BUILDDIR' : meson.current_build_dir(),
    }
  )

  custom_target(
    'doc', input : headers, output : ['html'], command : [doxygen, doxyfile],
    install_dir : get_option('datadir') / 'doc/cutl', install : true
  )
endif



# INSTALL

install_headers('include/cutl.h')

if lua_dep.found()
  install_headers('include/lutl.h')
endif

install_data('LICENSE',  install_dir : get_option('datadir') / 'licenses/cutl')
install_subdir(
  'example', install_dir : get_option('datadir') / 'cutl',
)
