# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

`pygl` is a single CPython C extension module that exposes a thin binding over
the **OpenGL 1.0 - 4.6 compatibility profile** — the fixed-function /
immediate-mode API and modern core (VAOs, sync objects, DSA, debug output, …)
in one module, since the generator ignores the registry `<remove>` blocks. The
wrappers are a direct 1:1 mapping over the GL entry points — one C function per
GL call, no abstraction. For general use, PyOpenGL is the real option.

Most of the surface is **generated** from the Khronos OpenGL registry; the rest
is hand-written. The build produces one shared object importable as `import pygl`
— there is no Python package.

Coverage is ~92% of the GL 1.0-4.6 command set and ~99% of its enums
(`python3 tools/coverage.py`). The unwrapped remainder (`tools/coverage.py`
`WONTFIX`, `tools/manifest.json` `unimplemented`) is evaluators, pixel-transfer
maps, selection/feedback mode, program-interface/subroutine introspection,
SPIR-V, the `glGetn*` robustness getters and the deep-cut `ProgramUniform*` /
multi-draw-indirect variants — little practical use from Python.

**GLEW version portability**: every entry point past GL 1.1 reaches the build
through GLEW, which `#define`s each one as a macro. Generated *and* hand-written
wrappers for those calls are wrapped in `#ifdef gl<Name>` (constants in
`#ifdef GL_<NAME>`), so an older GLEW simply yields a smaller module instead of
a broken build. GL 1.0/1.1 (`BASELINE` in the generator) come from the base
`gl.h` as plain declarations and are never guarded. A name whose wrapper is
compiled out is listed in `tools/manifest.json` `gated`; the tests tolerate its
absence. macOS caps a compatibility context at 2.1 and a core context at 4.1,
so the dev machine can only exercise the 2.1 subset at runtime (CI's llvmpipe
gives 4.5).

## Build

```sh
pip install .                          # normal install
python setup.py build_ext --inplace    # build pygl.<abi>.so into the cwd
```

Native dependency: **GLEW**, plus GL and GLU (linked as `GL`, `GLU`, `GLEW`).

- Ubuntu: `apt-get install libglew-dev`
- macOS: `brew install glew` (setup.py searches `/opt/homebrew` and `/usr/local`;
  system GL/GLU come from the OpenGL framework; `GL_SILENCE_DEPRECATION` is set)

`build/` holds stale per-OS/per-Python artifacts and is gitignored.

## Tooling (`tools/`)

`src/generated/` and `pygl.pyi` are checked in but produced from `tools/gl.xml`.
After changing `tools/generate.py`, the type maps, or vendoring a newer registry:

```sh
python3 tools/generate.py            # rewrites src/generated/*, tools/manifest.json
python3 tools/generate.py --report   # print the classification without writing
python3 tools/coverage.py --enums    # API coverage vs the registry (--strict for CI)
python3 tools/stubgen.py > pygl.pyi  # regenerate the type stub from the built module
```

Then rebuild and run the tests. `setup.py` lists every `src/**` file in
`depends`, so `build_ext` rebuilds when an `#include`d `.c` changes. CI
(`.github/workflows/ci.yml`) checks generator drift, runs `coverage --strict`,
and tests on Linux (Xvfb + llvmpipe) and macOS.

## Tests

```sh
python -m pytest test/
```

`test/conftest.py` opens a hidden SDL2 window for a current GL context (2.1
compat on macOS), so the suite needs the separate **`SDL2`** module
(github.com/shawcx/sdl2); without it the context-dependent tests skip. An
autouse fixture fails any test that leaves `glGetError()` non-zero. Files:
`test_wrappers.py` (generated surface), `test_data.py` (buffer protocol),
`test_shaders.py` (GLSL pipeline), `test_glu.py` (GLU), `test_modern.py` (GL
3/4 — its `gl3` fixture parses `GL_VERSION` and skips on a pre-3.0 context, so
the runtime half only runs where the driver is new enough).

`example/gears.py` is the end-to-end demo (also needs `SDL2`):

```sh
python example/gears.py            # spawns a GL window per display
python example/gears.py -info      # dump VENDOR/RENDERER/VERSION/extensions
```

## Code layout

Everything compiles as **one translation unit**: `src/pygl.c` includes `pygl.h`
then `#include`s the other `.c` files directly.

- **`src/pygl.c`** — `PyInit_pygl`, `glewInit`, and the module assembly. Builds
  the module by calling `PyModule_AddFunctions` once per method table
  (`PyGL_core_methods`, `PyGL_generated_methods`, `PyGL_manual_methods`,
  `PyGL_data_methods`, `PyGL_shader_methods`, `PyGL_modern_methods`,
  `PyGL_glu_methods`), then `gl_add_constants` + `glu_add_constants`. It
  `#include`s the wrapper `.c` files in dependency order — `modern.c` after
  `data.c`/`shaders.c` because it reuses their file-local helpers and macros.
- **`src/pygl.h`** — includes, `PYGL_MAX_*` buffer bounds, declarations. No
  method table (each `.c` file owns its own).
- **`src/pygl_util.h`** — marshalling helpers used by generated and manual code:
  `pygl_floats`/`pygl_ints`/… (Python sequence → C array) and
  `pygl_build_floats`/… (C array → scalar-or-list). Generated by `PYGL_SEQ` /
  `PYGL_BUILD` macros.
- **`src/generated/wrappers.c`** — ~790 wrappers + `PyGL_generated_methods[]`,
  each post-1.1 one `#ifdef gl<Name>` guarded.
- **`src/generated/constants.c`** — `gl_add_constants()`, one
  `#ifdef`-guarded `PyModule_AddIntConstant` per GL 1.0-4.6 enum.
- **`src/manual.c`** — `GetString` / `GetStringi` (string returns) and the
  two-vector `Rect*v` calls. Everything mechanical (including the FBO/RBO
  objects, now correct GL 3.0 versions) is generated.
- **`src/data.c`** — bulk data through the buffer protocol (`pygl_bufptr` in
  `pygl_util.h`): buffer objects, `Tex[Sub]Image*D` + compressed variants,
  `DrawPixels`/`ReadPixels`/`GetTexImage`, `Draw[Range]Elements`, `CallLists`,
  `Bitmap`/`PolygonStipple`, `MapBuffer` (returns a `memoryview`), and the
  client vertex-array pointers. The trailing data arg is an int offset into the
  bound buffer object, a buffer-protocol object, or `None`. Vertex-array
  pointers are read at draw time, so their backing buffer is retained until
  `pygl.ReleaseArrays()`; every other call releases immediately.
- **`src/shaders.c`** — GL 2.0/2.1 program pipeline calls that move strings or
  variable-length arrays: `ShaderSource`, `Get{Shader,Program}InfoLog`,
  `GetActive{Attrib,Uniform}`, `Get{Attrib,Uniform}Location`, `Uniform{1..4}{f,i}v`,
  `UniformMatrix*fv`, `DrawBuffers`. Owns the `PYGL_UNIFORM_V` /
  `PYGL_UNIFORM_MAT` macros that `modern.c` reuses for the `ui`/`d` variants.
  The `PYGL_ALLOC` / `pygl_alloc_*` heap-array helpers live in `pygl_util.h`.
  The scalar pipeline calls (`CreateShader`, `LinkProgram`, `Uniform1f`,
  `GetShaderiv`, …) are generated.
- **`src/modern.c`** — hand-written GL 3.0-4.6 core that the classifier cannot
  handle: sync objects (`GLsync` passed as an int, like a `GLUquadric`), the
  DSA buffer/texture calls, `MapBufferRange` family (returns a `memoryview`),
  `VertexAttribIPointer`/`LPointer` (retained like the `data.c` arrays),
  base-vertex / instanced / indirect / multi-draw, transform-feedback
  varyings, uniform-block and frag-data introspection, program binaries,
  `CreateShaderProgramv`, the per-draw-buffer / region clears, the multi-bind
  and viewport-array calls, debug output (`DebugMessageCallback` installs a
  Python trampoline), and the `ui`/`d` uniform-array and `ProgramUniform*`
  setters (via the `PYGL_UNIFORM_*` macros from `shaders.c`). Every wrapper and
  method row is `#ifdef gl<Name>` guarded.
- **`src/glu.c`** — GLU wrappers (hand-written; GLU is not in the registry). A
  `GLUquadric` is passed as the integer value of its pointer. Owns
  `glu_add_constants()` for the `u`-prefixed GLU enums.

## Generator classification (`tools/generate.py`)

Each registry command for GL 1.0-4.6 is sorted into one shape:

- **scalar** — all args are scalars/enums, return is void or a scalar. Emits a
  `PyArg_ParseTuple` wrapper. Covers ~270 calls including the non-`v` uniform
  and vertex-attrib setters.
- **vector** — trailing `const T*` numeric array, everything before it scalar.
  Reads a Python sequence into a `PYGL_MAX_VEC` buffer (`glLightfv`,
  `glLoadMatrixf`, `glColor3fv`, `glClipPlane`).
- **gen / delete** — `glGen*(n, T*)` → returns a list; `glDelete*(n, const T*)`
  → takes a list.
- **getter** — `glGet*(… , T* out)` with an optional trailing `count`
  (default 1). Returns a scalar when count is 1, else a list.

Anything else lands in `manifest.json` `unimplemented`. The generator discovers
hand-written wrappers automatically: `hand_written_names()` scans every
`src/*.c` method table for `{ "Name", PyGL…` and skips those commands, so just
adding a row to `modern.c` (or any `.c`) is enough — no list to maintain.
`hand_written_gated()` likewise finds `#ifdef`-guarded hand-written rows and
adds them to `manifest['gated']`. `FORCE_SKIP` / `COUNT_HINTS` at the top of the
generator are the remaining hand-tuning knobs.

## Naming conventions

- Functions drop the `gl` prefix (`glClearColor` → `pygl.ClearColor`). GLU keeps
  a `u` prefix (`gluPerspective` → `pygl.uPerspective`).
- GL constants drop `GL_` (`GL_COLOR_BUFFER_BIT` → `pygl.COLOR_BUFFER_BIT`);
  names that would start with a digit get a `_` (`GL_3_BYTES` → `pygl._3_BYTES`).
- GLU constants keep a `u` prefix because their enum range overlaps GL's
  (`GLU_LINE` 100011 ≠ `GL_LINE`): `pygl.uLINE`, `pygl.uSMOOTH`, `pygl.uNONE`,
  `pygl.uFILL`, `pygl.uOUTSIDE`, `pygl.uVERSION`, … (in `src/glu.c`).

## Style

- C: aligned columns in declarations, method tables, and parameter lists; Yoda
  conditions (`0 == foo`, `NULL == mod`); `PyGL_` prefix on GL wrappers,
  `PyGLu_` on GLU. Generated code matches this so it reads like the manual code.
- Python: 4-space indent, single quotes, f-strings, column-aligned assignments.
