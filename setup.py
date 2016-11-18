#!/usr/bin/env python3

import sys
import platform

import setuptools

defines = []
include_dirs = ['/usr/local/include']
library_dirs = ['/usr/local/lib']
libs = ['GL', 'GLU', 'GLEW']

setuptools.setup(
    name             = 'GL',
    version          = '0.1.0',
    author           = 'Matthew Oertle',
    author_email     = 'moertle@gmail.com',
    url              = 'https://github.com/moertle/pygl',
    license          = 'MIT',
    description      = 'Python bindings for OpenGL',
    long_description = open('README.rst').read(),
    ext_modules = [
        setuptools.Extension(
            'GL',
            ['src/pygl.c'],
            define_macros = defines,
            include_dirs = include_dirs,
            library_dirs = library_dirs,
            libraries = libs
            )
        ],
    classifiers = [
        'Development Status :: 3 - Alpha',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 3',
        ],
    )
