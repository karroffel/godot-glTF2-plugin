#!python

import os, subprocess, platform


def add_sources(sources, dir, extension):
  for f in os.listdir(dir):
      if f.endswith('.' + extension):
          sources.append(dir + '/' + f)


env = Environment()
host_platform = platform.system()
target_platform = ARGUMENTS.get('p', ARGUMENTS.get('platform', 'linux'))
target_arch = ARGUMENTS.get('a', ARGUMENTS.get('arch', '64'))
# default to debug build, must be same setting as used for cpp_bindings
target = ARGUMENTS.get('target', 'debug')
# Local dependency paths, adapt them to your setup
result_path = 'addons/de.karroffel.gltf2/bin'
result_name = ARGUMENTS.get('n', ARGUMENTS.get('name', os.path.relpath('.', '..')))


if target_platform == 'linux':
    result_name += '.linux.' + target_arch

    if ARGUMENTS.get('use_llvm', 'no') == 'yes':
        env['CXX'] = 'clang++'

    env.Append(CCFLAGS = [ '-fPIC', '-g', '-std=c++14', '-Wwrite-strings' ])
    env.Append(LINKFLAGS = [ '-Wl,-R,\'$$ORIGIN\'' ])

    if target == 'debug':
        env.Append(CCFLAGS = ['-O0'])
    else:
        env.Append(CCFLAGS = ['-O3'])
	
    if target_arch == '32':
        env.Append(CCFLAGS = [ '-m32' ])
        env.Append(LINKFLAGS = [ '-m32' ])
    elif target_arch == '64':
        env.Append(CCFLAGS = [ '-m64' ])
        env.Append(LINKFLAGS = [ '-m64' ])

elif target_platform == 'windows':
    # This makes sure to keep the session environment variables on windows,
    # that way you can run scons in a vs 2017 prompt and it will find all the required tools
    if (target_arch == '64'):
        env = Environment(ENV = os.environ, TARGET_ARCH='amd64')
    else:
        env = Environment(ENV = os.environ, TARGET_ARCH='x86')

    result_name += '.windows.' + target_arch

    if host_platform == 'Windows':
        result_name += '.lib'

        env.Append(LINKFLAGS = [ '/WX' ])
        if target == 'debug':
            env.Append(CCFLAGS = ['/EHsc', '/D_DEBUG', '/MDd' ])
        else:
            env.Append(CCFLAGS = ['/O2', '/EHsc', '/DNDEBUG', '/MD' ])
    else:
        if target_arch == '32':
            env['CXX']='i686-w64-mingw32-g++'
        elif target_arch == '64':
            env['CXX']='x86_64-w64-mingw32-g++'

        env.Append(CCFLAGS = [ '-g', '-O3', '-std=c++14', '-Wwrite-strings' ])
        env.Append(LINKFLAGS = [ '--static', '-Wl,--no-undefined', '-static-libgcc', '-static-libstdc++' ])

elif target_platform == 'osx':
    if ARGUMENTS.get('use_llvm', 'no') == 'yes':
        env['CXX'] = 'clang++'

    # Only 64-bits is supported for OS X
    target_arch = '64'
    result_name += '.osx.' + target_arch

    env.Append(CCFLAGS = [ '-g','-O3', '-std=c++14', '-arch', 'x86_64' ])
    env.Append(LINKFLAGS = [ '-arch', 'x86_64', '-framework', 'Cocoa', '-Wl,-undefined,dynamic_lookup' ])


env.Append(CPPPATH=['src', 'godot-cpp/godot_headers/', 'godot-cpp/include', 'godot-cpp/include/core'])

sources = []
add_sources(sources, 'src', 'cpp')


# add tinygltf thirdparty library

env.Append(CPPPATH=['thirdparty/tinygltf'])

env.Append(LIBPATH=['godot-cpp/bin'])
env.Append(LIBS=['godot-cpp.linux.64'])

library = env.SharedLibrary(target=result_path + '/' + result_name, source=sources)
Default(library)
