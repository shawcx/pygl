====
pygl
====

Thin Python bindings for the **OpenGL 1.0 - 4.6 compatibility profile** — the
fixed-function / immediate-mode API and modern core (VAOs, sync objects,
direct state access, debug output, …) in one module. One C function per GL
call, no abstraction layer. If you want a maintained, complete binding use
`PyOpenGL <http://pyopengl.sourceforge.net/>`_ instead.

Most of the surface is generated from the Khronos OpenGL registry; the calls
that move strings or bulk data are hand-written. Coverage is ~92% of the
1.0 - 4.6 command set and ~99% of its enums. Everything past GL 1.1 is compiled
only when the installed GLEW knows it, so an old GLEW just yields a smaller
module. A macOS legacy context still tops out at 2.1 (4.1 for a core context).

Install
=======

Requires **GLEW** and the GL / GLU libraries.

Ubuntu::

    apt-get install libglew-dev libglu1-mesa-dev libgl1-mesa-dev
    pip install .

macOS::

    brew install glew
    pip install .

The build produces a single extension module::

    import pygl

Using it
========

``pygl`` does no windowing - a GL context must already be current before any
call works. Create one with your toolkit of choice (the examples use the
companion `SDL2 <https://github.com/shawcx/sdl2>`_ bindings), then::

    import pygl as GL
    GL.glewInit()
    GL.ClearColor(0.1, 0.1, 0.1, 1.0)
    GL.Clear(GL.COLOR_BUFFER_BIT | GL.DEPTH_BUFFER_BIT)

Naming
------

* Functions drop the ``gl`` prefix: ``glClearColor`` -> ``GL.ClearColor``.
* GLU keeps a ``u`` prefix: ``gluPerspective`` -> ``GL.uPerspective``.
* GL constants drop ``GL_``: ``GL_COLOR_BUFFER_BIT`` -> ``GL.COLOR_BUFFER_BIT``
  (a leading digit becomes ``_``: ``GL_3_BYTES`` -> ``GL._3_BYTES``).
* GLU constants keep a ``u`` prefix (their values overlap GL's):
  ``GLU_LINE`` -> ``GL.uLINE``.

Conventions
-----------

* ``Gen*`` returns a list of names; ``Delete*`` takes one.
* ``Get*`` queries take an optional trailing count (default 1) and return a
  scalar for count 1, a list otherwise.
* Vector calls (``Color3fv``, ``Lightfv``, ``LoadMatrixf`` ...) take any
  sequence of numbers.
* Bulk-data calls (``BufferData``, ``Tex*Image*``, ``DrawElements``,
  ``ReadPixels`` ...) take an int offset into the bound buffer object, any
  object supporting the buffer protocol, or ``None``. Client-side vertex-array
  pointers are retained until ``GL.ReleaseArrays()``.
* ``MapBuffer`` / ``MapBufferRange`` / ``MapNamedBuffer*`` return a
  ``memoryview`` over the mapping.
* A ``GLsync`` (and a ``GLUquadric``) is passed as the integer value of its
  pointer. ``DebugMessageCallback`` takes a Python callable (or ``None``).

A ``pygl.pyi`` stub with argument names ships alongside the source.

Development
==========

``src/generated/`` is checked in but produced from ``tools/gl.xml``::

    python3 tools/generate.py          # rewrite src/generated/*, tools/manifest.json
    python3 tools/coverage.py --enums  # report API coverage
    python3 tools/stubgen.py > pygl.pyi

Tests need a GL context (hidden SDL2 window; they skip without the ``SDL2``
module)::

    python -m pytest test/

Examples (need ``SDL2``)::

    python example/gears.py            # the classic gears demo, fixed pipeline
    python example/shaders.py          # GLSL program, VBOs, textures, GLU quadric
                                       #   R spin   S screenshot   ESC quit
