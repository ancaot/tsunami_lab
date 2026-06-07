##
# @author Alexander Breuer (alex.breuer AT uni-jena.de)
#
# @section DESCRIPTION
# Entry-point for builds.
##
import os

print( '####################################' )
print( '### Tsunami Lab                  ###' )
print( '###                              ###' )
print( '### https://scalable.uni-jena.de ###' )
print( '####################################' )
print()
print( 'runnning build script' )

vars = Variables()
vars.AddVariables(
  EnumVariable( 'mode',
                'compile modes, option \'san\' enables address and undefined behavior sanitizers',
                'release',
                allowed_values=( 'release', 'debug', 'release+san', 'debug+san' ) ),
  EnumVariable( 'opt',
                'optimization level for release builds',
                'O3',
                allowed_values=( 'O0', 'O1', 'O2', 'O3', 'Ofast' ) ),
  BoolVariable( 'use_report',
                'enable compiler optimization reports',
                False ),
  EnumVariable( 'netcdf',
                'enable netCDF output support (auto/on/off)',
                'on',
                allowed_values=( 'auto', 'on', 'off' ) ),
  ( 'netcdf_include',
    'optional include directory containing netcdf.h',
    '' ),
  ( 'netcdf_libdir',
    'optional library directory containing netCDF, HDF5 and zlib libraries',
    '' )
)

if vars.UnknownVariables():
  print( "build configuration corrupted, don't know what to do with: " + str( vars.UnknownVariables().keys() ) )
  exit( 1 )

env = Environment( variables = vars,
                   ENV = os.environ.copy() )

if 'CXX' in os.environ:
  env.Replace( CXX = os.environ['CXX'] )

l_isMsvc = 'msvc' in env['TOOLS']
l_compilerName = os.path.basename( env.subst( '$CXX' ) ).lower()

print( 'selected C++ compiler: ' + env.subst( '$CXX' ) )

if env['netcdf_include']:
  env.Append( CPPPATH = [ env['netcdf_include'] ] )

if env['netcdf_libdir']:
  env.Append( LIBPATH = [ env['netcdf_libdir'] ] )

Help( vars.GenerateHelpText( env ) )

if l_isMsvc:
  env.Append( CXXFLAGS = [ '/std:c++17',
                           '/W4',
                           '/EHsc' ] )
else:
  env.Append( CXXFLAGS = [ '-std=c++17',
                           '-Wall',
                           '-Wextra',
                           '-Wpedantic' ] )

if 'debug' in env['mode']:
  if l_isMsvc:
    env.Append( CXXFLAGS = [ '/Zi',
                             '/Od' ] )
    env.Append( LINKFLAGS = [ '/DEBUG' ] )
  else:
    env.Append( CXXFLAGS = [ '-g',
                             '-O0' ] )
else:
  if l_isMsvc:
    env.Append( CXXFLAGS = [ '/O2' ] )
  else:
    env.Append( CXXFLAGS = [ '-' + env['opt'] ] )

if env['use_report']:
  if 'clang' in l_compilerName:
    env.Append( CXXFLAGS = [ '-Rpass=.*',
                             '-Rpass-missed=.*',
                             '-Rpass-analysis=.*' ] )
    print( 'optimization reports: enabled for Clang' )
  elif 'g++' in l_compilerName or 'gcc' in l_compilerName:
    env.Append( CXXFLAGS = [ '-fopt-info-vec-optimized',
                             '-fopt-info-vec-missed',
                             '-fopt-info-inline-optimized',
                             '-fopt-info-inline-missed' ] )
    print( 'optimization reports: enabled for GCC' )
  elif l_isMsvc:
    print( 'warning: optimization reports are not configured for MSVC' )
  else:
    print( 'warning: unknown compiler, no optimization report flags added' )

if 'san' in env['mode']:
  if l_isMsvc:
    print( 'warning: sanitizers are not enabled for MSVC builds' )
  else:
    env.Append( CXXFLAGS = [ '-g',
                             '-fsanitize=float-divide-by-zero',
                             '-fsanitize=bounds',
                             '-fsanitize=address',
                             '-fsanitize=undefined',
                             '-fno-omit-frame-pointer' ] )
    env.Append( LINKFLAGS = [ '-g',
                              '-fsanitize=address',
                              '-fsanitize=undefined' ] )
else:
  if not l_isMsvc:
    env.Append( CXXFLAGS = [ '-Werror' ] )

env.Append( CPPPATH = [ '#/submodules/Catch2/single_include',
                        '#/submodules/json/single_include' ] )

env['USE_NETCDF'] = False
if env['netcdf'] != 'off':
  l_hasNetcdf = False
  l_hasZlib = False
  l_hasHdf5 = False
  l_hasHdf5Serial = False

  if l_isMsvc and env['netcdf_include'] and env['netcdf_libdir']:
    l_header = os.path.join( env['netcdf_include'], 'netcdf.h' )
    l_netcdfLib = os.path.join( env['netcdf_libdir'], 'netcdf.lib' )
    l_zlibLib = os.path.join( env['netcdf_libdir'], 'zlib.lib' )
    l_hdf5Lib = os.path.join( env['netcdf_libdir'], 'hdf5.lib' )
    l_hdf5SerialLib = os.path.join( env['netcdf_libdir'], 'hdf5_serial.lib' )

    l_hasNetcdf = os.path.exists( l_header ) and os.path.exists( l_netcdfLib )
    l_hasZlib = os.path.exists( l_zlibLib )
    l_hasHdf5 = os.path.exists( l_hdf5Lib )
    l_hasHdf5Serial = os.path.exists( l_hdf5SerialLib )
  else:
    l_conf = Configure( env )
    l_hasNetcdf = l_conf.CheckLibWithHeader( 'netcdf',
                                             'netcdf.h',
                                             'c' )
    l_hasZlib = l_conf.CheckLib( 'zlib' if l_isMsvc else 'z' )
    l_hasHdf5 = l_conf.CheckLib( 'hdf5' )
    l_hasHdf5Serial = l_conf.CheckLib( 'hdf5_serial' )
    env = l_conf.Finish()

  if l_hasNetcdf:
    env['USE_NETCDF'] = True
    env.AppendUnique( CPPDEFINES = [ 'USE_NETCDF' ] )
    env.AppendUnique( LIBS = [ 'netcdf' ] )
    if l_hasHdf5:
      env.AppendUnique( LIBS = [ 'hdf5' ] )
    if l_hasHdf5Serial:
      env.AppendUnique( LIBS = [ 'hdf5_serial' ] )
    if l_hasZlib:
      env.AppendUnique( LIBS = [ 'zlib' if l_isMsvc else 'z' ] )
    print( 'netCDF support: enabled' )
  else:
    if env['netcdf'] == 'on':
      print( 'ERROR: netCDF support requested but libnetcdf/netcdf.h not found.' )
      Exit( 1 )
    print( 'netCDF support: disabled (libraries not found)' )

env.Append(LIBS=['stdc++fs'])

VariantDir( variant_dir = 'build/src',
            src_dir     = 'src' )

env.sources = []
env.tests = []

Export( 'env' )
SConscript( 'build/src/SConscript' )
Import( 'env' )

env.Program( target = 'build/tsunami_lab',
             source = env.sources + env.standalone )

env.Program( target = 'build/tests',
             source = env.sources + env.tests )
