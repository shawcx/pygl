#!/usr/bin/env python3

import sys
import platform

import setuptools

defines = []
include_dirs = ['/usr/local/include','/usr/include']
library_dirs = ['/usr/local/lib','/usr/lib','/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries']
libs = ['GL', 'GLU', 'GLEW']


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
