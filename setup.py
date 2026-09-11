#!/usr/bin/env python3

import glob
import sys
import platform

import setuptools

# src/pygl.c pulls in the generated and hand-written wrappers as one unit;
# src/ on the include path lets those files resolve "pygl.h".
defines = []
include_dirs = ['src', '/opt/homebrew/include', '/usr/local/include','/usr/include']
library_dirs = ['/opt/homebrew/lib', '/usr/local/lib','/usr/lib','/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries']
libs = ['GL', 'GLU', 'GLEW']

if sys.platform == 'darwin':
    # the framework GL/GLU headers mark the whole 1.x/2.x API deprecated
    defines.append(('GL_SILENCE_DEPRECATION', None))


setuptools.setup(
    name         = 'pygl',
    version      = '0.1.0',
    author       = 'Matthew Shaw',
    author_email = 'mshaw.cx@gmail.com',
    url          = 'https://github.com/shawcx/pygl',
    license      = 'MIT',
    description  = 'Python bindings for OpenGL',
    long_description = open('README.rst').read(),
    ext_modules = [
        setuptools.Extension(
            'pygl',
            ['src/pygl.c'],
            # src/pygl.c #includes the rest; list them so build_ext notices edits
            depends       = sorted(glob.glob('src/*.c') + glob.glob('src/*.h')
                                   + glob.glob('src/generated/*.c')),
            define_macros = defines,
            include_dirs  = include_dirs,
            library_dirs  = library_dirs,
            libraries     = libs
            )
        ],
    classifiers = [
        'Development Status :: 3 - Alpha',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python :: 3',
        ],
    )
