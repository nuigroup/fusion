#################################################################
# Command line options to controll build
#################################################################
AddOption( '--clean-contrib',
  help='force rebuilding of contrib libraries',
  dest='clean-contrib', action="store_true"
)


#################################################################
# Source files list for each target we are building
#################################################################

#source files for the daemon ####################################
daemon_src = [
  'src/fusion.cpp',
  'contrib/cJSON/cJSON.c'
]

#source files for ccx, core+modules ########################
ccx_src = [
  'ccx/ccxDaemon.cpp',
  'ccx/ccxDataGenericContainer.cpp',
  'ccx/ccxDataStream.cpp',
  'ccx/ccxFactory.cpp',
  'ccx/ccxLog.cpp',
  'ccx/ccxModule.cpp',
  'ccx/ccxPipeline.cpp',
  'ccx/ccxProperty.cpp',
  'ccx/ccxThread.cpp',
  'ccx/ccxUtils.cpp',
  'src/modules/ccxDebugModule.cpp'
]

#################################################################
# Check some arguments in command line
#################################################################
mode = ARGUMENTS.get('mode', 'normal')
if mode not in ('normal', 'debug'):
  print 'Invalid mode <%s>, fallback to normal' % mode
  mode = 'normal'
print 'Doing compilation in %s mode' % mode

#################################################################
# Build contrib and configure env for linking against deps
#################################################################
env = SConscript('ccx/contrib/SConscript')

#################################################################
# Platform specific settings for build env and OpenCV flags
#################################################################
import sys, os

# WIN32 #########################################################
if sys.platform == 'win32':
  #on widnows daemon also needs XgetOpt source file
  daemon_src.append('contrib/XgetOpt/XgetOpt.cxx')

  #gotta set up msvc compiler and linker for list of options see:
  #see http://msdn.microsoft.com/en-us/library/fwkeyyhe(v=VS.71).aspx
  #and http://msdn.microsoft.com/en-us/library/y0zzbyt4(VS.80).aspx
  env.Append(
    CPPDEFINES = ['WIN32'],
    CCFLAGS = ['/O2', '/Oi', '/GL', '/EHsc', '/MD'], #mainly optimization
    LIBS = ['ws2_32.lib', 'user32.lib'], #ws_32.lib is needed buy libevent
    LINKFLAGS = ['/LTCG', '/OPT:REF', '/OPT:ICF']) #mainly optimization



# UNIX #######################################################
else:

  #set the compiler if set in ENV, used e.g. to force 32bit by setting to g++ -m32
  if os.environ.get('CC'): env.Replace(CC=os.environ['CC'])
  if os.environ.get('CXX'): env.Replace(CXX=os.environ['CXX'])

  if mode == 'debug':
    env.Append(CCFLAGS = ['-ggdb', '-O0'])




#################################################################
# Build Rules for ccx and fusion program
#################################################################
ccx = env.Library('ccx', ccx_src )
env.Append(CPPPATH = ['ccx'])
env.Append(CPPPATH = 'src/modules')
env.Append(CPPPATH = 'contrib/cJSON')
env.Program('fusion', daemon_src + [ccx])
