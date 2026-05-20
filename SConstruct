##
# @author Alexander Breuer (alex.breuer AT uni-jena.de)
#
# @section DESCRIPTION
# Entry-point for builds.
##
import SCons
import os

print( '####################################' )
print( '### Tsunami Lab                  ###' )
print( '###                              ###' )
print( '### https://scalable.uni-jena.de ###' )
print( '####################################' )
print()
print('runnning build script')

# configuration
vars = Variables()

vars.AddVariables(
  EnumVariable( 'mode',
                'compile modes, option \'san\' enables address and undefined behavior sanitizers',
                'release',
                allowed_values=('release', 'debug', 'release+san', 'debug+san' )
              )
)

vars.AddVariables(
  EnumVariable( 'netcdf',
                'enable netCDF output support (auto/on/off)',
                'auto',
                allowed_values=('auto', 'on', 'off' )
              ),
  ( 'netcdf_include',
    'optional include directory containing netcdf.h',
    ''
  ),
  ( 'netcdf_libdir',
    'optional library directory containing netCDF, HDF5 and zlib libraries',
    ''
  )
)

# exit in the case of unknown variables
if vars.UnknownVariables():
  print( "build configuration corrupted, don't know what to do with: " + str(vars.UnknownVariables().keys()) )
  exit(1)

# create environment
# Pass through the full process environment so spawned compiler processes
# can resolve toolchain executables from PATH on Windows.
env = Environment( variables = vars,
                   ENV = os.environ.copy() )

l_isMsvc = 'msvc' in env['TOOLS']

if env['netcdf_include']:
  env.Append( CPPPATH = [ env['netcdf_include'] ] )

if env['netcdf_libdir']:
  env.Append( LIBPATH = [ env['netcdf_libdir'] ] )

# generate help message
Help( vars.GenerateHelpText( env ) )

# add default flags
if l_isMsvc:
  env.Append( CXXFLAGS = [ '/std:c++17',
                           '/W4',
                           '/EHsc' ] )
else:
  env.Append( CXXFLAGS = [ '-std=c++17',
                           '-Wall',
                           '-Wextra',
                           '-Wpedantic' ] )

# set optimization mode
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
    env.Append( CXXFLAGS = [ '-O2' ] )

# add sanitizers
if 'san' in  env['mode']:
  if l_isMsvc:
    print( 'warning: sanitizers are not enabled for MSVC builds' )
  else:
    env.Append( CXXFLAGS =  [ '-g',
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

# add Catch2
env.Append( CPPPATH = [ '#/submodules/Catch2/single_include' ] )

# add pugixml for XML runtime configuration
env.pugixml_sources = []
l_pugixmlHeader = File( 'libs/pugixml-1.15/src/pugixml.hpp' )
l_pugixmlConfig = File( 'libs/pugixml-1.15/src/pugiconfig.hpp' )
l_pugixmlSource = File( 'libs/pugixml-1.15/src/pugixml.cpp' )

if os.path.isfile( str( l_pugixmlHeader ) ) and \
   os.path.isfile( str( l_pugixmlConfig ) ) and \
   os.path.isfile( str( l_pugixmlSource ) ):
  env['USE_PUGIXML'] = True
  env.AppendUnique( CPPPATH = [ '#/src' ] )
  env.AppendUnique( CPPDEFINES = [ 'USE_PUGIXML' ] )
  env.pugixml_sources = [ l_pugixmlSource ]
  print( 'pugixml support: enabled from src/pugixml.cpp' )
else:
  print( 'pugixml support: disabled (expected src/pugixml.hpp, src/pugiconfig.hpp and src/pugixml.cpp)' )

# add C++17 filesystem support for MinGW
env.Append( LIBS = [ 'stdc++fs' ] )
# optionally add pugixml for XML runtime configuration
env['USE_PUGIXML'] = False
l_confXml = Configure( env )
l_hasPugixmlHeader = l_confXml.CheckCXXHeader( 'pugixml.hpp' )
l_hasPugixmlLib = l_confXml.CheckLib( 'pugixml' )
env = l_confXml.Finish()

if l_hasPugixmlHeader and l_hasPugixmlLib:
  env['USE_PUGIXML'] = True
  env.Append( CPPDEFINES = [ 'USE_PUGIXML' ] )
  env.Append( LIBS = [ 'pugixml' ] )
  print( 'pugixml support: enabled' )
else:
  print( 'pugixml support: disabled (falling back to lightweight XML parser)' )

# optionally enable netCDF support
env['USE_NETCDF'] = False
if env['netcdf'] != 'off':
  l_conf = Configure( env )

  l_hasNetcdf = l_conf.CheckLibWithHeader( 'netcdf',
                                           'netcdf.h',
                                           'c',
                                           'nc_open(0, 0, 0);' )
  l_hasZlib = l_conf.CheckLib( 'z' )
  l_hasHdf5 = l_conf.CheckLib( 'hdf5' )
  l_hasHdf5Hl = l_conf.CheckLib( 'hdf5_hl' )

  env = l_conf.Finish()

  if l_hasNetcdf:
    env['USE_NETCDF'] = True
    env.Append( CPPDEFINES = [ 'USE_NETCDF' ] )

    env.AppendUnique( LIBS = [ 'netcdf' ] )

    # static netCDF builds often require explicit dependency libraries.
    if l_hasHdf5Hl:
      env.AppendUnique( LIBS = [ 'hdf5_hl' ] )
    if l_hasHdf5:
      env.AppendUnique( LIBS = [ 'hdf5' ] )
    if l_hasZlib:
      env.AppendUnique( LIBS = [ 'z' ] )
    print( 'netCDF support: enabled' )
  else:
    if env['netcdf'] == 'on':
      print( 'ERROR: netCDF support requested but libnetcdf/netcdf.h not found.' )
      Exit( 1 )
    print( 'netCDF support: disabled (libraries not found)' )

# get source files
VariantDir( variant_dir = 'build/src',
            src_dir     = 'src' )

env.sources = []
env.tests = []

Export('env')
SConscript( 'build/src/SConscript' )
Import('env')
env.sources += env.pugixml_sources

env.Program( target = 'build/tsunami_lab',
             source = env.sources + env.standalone_main )

env.Program( target = 'build/symmetric_station_study',
             source = env.sources + env.standalone_symmetry )

env.Program( target = 'build/tests',
             source = env.sources + env.tests )
