#!/usr/bin/env python3
#
# A tour of the pieces of pygl that are not plain 1:1 scalar wrappers:
#
#   * GLSL program pipeline  - CreateShader / ShaderSource / CompileShader,
#     GetShaderiv / GetShaderInfoLog, LinkProgram, GetActiveAttrib /
#     GetActiveUniform, Get{Attrib,Uniform}Location
#   * buffer objects         - GenBuffers, BufferData (buffer protocol),
#     BufferSubData, MapBuffer / UnmapBuffer, GetBufferParameteriv
#   * client draw state       - VertexAttribPointer with integer offsets,
#     EnableVertexAttribArray, DrawElements against a bound index buffer
#   * textures                - TexImage2D / uBuild2DMipmaps from a bytes blob
#   * uniforms                - Uniform1i / Uniform1f / Uniform4fv,
#     UniformMatrix4fv with a matrix built in Python
#   * GLU                     - uPerspective / uLookAt on the fixed pipeline,
#     a uNewQuadric wire sphere, uProject, uErrorString
#   * readback                - ReadPixels -> bytes, saved as a PPM screenshot
#
# Needs the SDL2 bindings (github.com/shawcx/sdl2) for the window and context.
#
#   python example/shaders.py            keys:  R spin on/off   S screenshot   ESC quit
#

import math
import os
import struct
import sys
import time

import SDL2
import pygl as GL

WIDTH, HEIGHT = 800, 600

# PYGL_FRAMES=N renders N frames into a hidden window and exits - used to
# smoke-test the demo without a display.
MAX_FRAMES = int(os.environ.get('PYGL_FRAMES', '0'))

VERTEX_SHADER = '''
#version 120
uniform mat4 mvp;
uniform float time;
attribute vec3 position;
attribute vec2 texcoord;
varying vec2 uv;
void main() {
    uv = texcoord;
    vec3 p = position;
    p.z += 0.15 * sin(6.0 * position.x + time);   // gentle ripple
    gl_Position = mvp * vec4(p, 1.0);
}
'''

FRAGMENT_SHADER = '''
#version 120
uniform sampler2D tex;
uniform vec4 tint;
varying vec2 uv;
void main() {
    gl_FragColor = texture2D(tex, uv) * tint;
}
'''


# ---------------------------------------------------------------------------
# 4x4 column-major matrix helpers (OpenGL order), as flat lists of 16 floats
# ---------------------------------------------------------------------------

def m_mul(a, b):
    out = [0.0] * 16
    for col in range(4):
        for row in range(4):
            out[col * 4 + row] = sum(a[k * 4 + row] * b[col * 4 + k] for k in range(4))
    return out


def m_perspective(fovy_deg, aspect, near, far):
    f = 1.0 / math.tan(math.radians(fovy_deg) / 2.0)
    return [f / aspect, 0.0, 0.0,                          0.0,
            0.0,        f,   0.0,                          0.0,
            0.0,        0.0, (far + near) / (near - far), -1.0,
            0.0,        0.0, (2 * far * near) / (near - far), 0.0]


def m_translate(x, y, z):
    return [1.0, 0.0, 0.0, 0.0,
            0.0, 1.0, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            x,   y,   z,   1.0]


def m_rotate_x(rad):
    c, s = math.cos(rad), math.sin(rad)
    return [1.0, 0.0, 0.0, 0.0,
            0.0, c,   s,   0.0,
            0.0, -s,  c,   0.0,
            0.0, 0.0, 0.0, 1.0]


def m_rotate_y(rad):
    c, s = math.cos(rad), math.sin(rad)
    return [c,   0.0, -s,  0.0,
            0.0, 1.0, 0.0, 0.0,
            s,   0.0, c,   0.0,
            0.0, 0.0, 0.0, 1.0]


# ---------------------------------------------------------------------------
# shader helpers
# ---------------------------------------------------------------------------

def compile_shader(kind, source):
    shader = GL.CreateShader(kind)
    GL.ShaderSource(shader, source)
    GL.CompileShader(shader)
    if GL.GetShaderiv(shader, GL.COMPILE_STATUS) != GL.TRUE:
        raise RuntimeError(GL.GetShaderInfoLog(shader))
    return shader


def link_program(*shaders):
    program = GL.CreateProgram()
    for shader in shaders:
        GL.AttachShader(program, shader)
    GL.LinkProgram(program)
    if GL.GetProgramiv(program, GL.LINK_STATUS) != GL.TRUE:
        raise RuntimeError(GL.GetProgramInfoLog(program))
    return program


def describe_program(program):
    print('  attached shaders :', GL.GetAttachedShaders(program))
    for i in range(GL.GetProgramiv(program, GL.ACTIVE_ATTRIBUTES)):
        name, size, gltype = GL.GetActiveAttrib(program, i)
        print(f'  attribute {GL.GetAttribLocation(program, name):>2}   {name} (size {size}, type 0x{gltype:04x})')
    for i in range(GL.GetProgramiv(program, GL.ACTIVE_UNIFORMS)):
        name, size, gltype = GL.GetActiveUniform(program, i)
        print(f'  uniform   {GL.GetUniformLocation(program, name):>2}   {name} (size {size}, type 0x{gltype:04x})')


# ---------------------------------------------------------------------------
# geometry and texture data
# ---------------------------------------------------------------------------

def checkerboard(size=64, squares=8):
    step = size // squares
    rows = bytearray()
    for y in range(size):
        for x in range(size):
            light = ((x // step) + (y // step)) % 2
            rows += b'\xf0\xf0\xf0' if light else b'\x30\x40\x80'
    return bytes(rows)


# interleaved: position.xyz, texcoord.uv   (5 floats per vertex)
QUAD = struct.pack('<20f',
                   -1.0, -1.0, 0.0,  0.0, 0.0,
                    1.0, -1.0, 0.0,  1.0, 0.0,
                    1.0,  1.0, 0.0,  1.0, 1.0,
                   -1.0,  1.0, 0.0,  0.0, 1.0)
INDICES = struct.pack('<6I', 0, 1, 2, 0, 2, 3)
STRIDE = 5 * 4


def save_ppm(path, width, height):
    GL.PixelStorei(GL.PACK_ALIGNMENT, 1)
    pixels = GL.ReadPixels(0, 0, width, height, GL.RGB, GL.UNSIGNED_BYTE, width * height * 3)
    row = width * 3
    flipped = b''.join(pixels[i * row:(i + 1) * row]
                       for i in range(height - 1, -1, -1))
    with open(path, 'wb') as fh:
        fh.write(b'P6\n%d %d\n255\n' % (width, height))
        fh.write(flipped)
    print('wrote', path)


def main():
    SDL2.Init(SDL2.INIT_VIDEO)
    SDL2.GL_SetAttribute(SDL2.GL_DEPTH_SIZE, 24)
    flags = SDL2.WINDOW_OPENGL
    if MAX_FRAMES:
        flags |= SDL2.WINDOW_HIDDEN
    window = SDL2.Window('pygl shaders', size=(WIDTH, HEIGHT), flags=flags)
    window.GL_CreateContext()
    window.GL_MakeCurrent()
    GL.glewInit()

    print('vendor  :', GL.GetString(GL.VENDOR))
    print('renderer:', GL.GetString(GL.RENDERER))
    print('version :', GL.GetString(GL.VERSION))
    print('glsl    :', GL.GetString(GL.SHADING_LANGUAGE_VERSION))
    print('max texture size:', GL.GetIntegerv(GL.MAX_TEXTURE_SIZE))
    print('max vertex attribs:', GL.GetIntegerv(GL.MAX_VERTEX_ATTRIBS))

    # -- program -----------------------------------------------------------
    vs = compile_shader(GL.VERTEX_SHADER, VERTEX_SHADER)
    fs = compile_shader(GL.FRAGMENT_SHADER, FRAGMENT_SHADER)
    program = link_program(vs, fs)
    print('\nprogram interface:')
    describe_program(program)

    loc_mvp  = GL.GetUniformLocation(program, 'mvp')
    loc_time = GL.GetUniformLocation(program, 'time')
    loc_tint = GL.GetUniformLocation(program, 'tint')
    loc_tex  = GL.GetUniformLocation(program, 'tex')
    a_position = GL.GetAttribLocation(program, 'position')
    a_texcoord = GL.GetAttribLocation(program, 'texcoord')

    # -- buffers ---------------------------------------------------------------
    vbo, ibo = GL.GenBuffers(2)
    GL.BindBuffer(GL.ARRAY_BUFFER, vbo)
    GL.BufferData(GL.ARRAY_BUFFER, QUAD, GL.STATIC_DRAW)
    GL.BindBuffer(GL.ELEMENT_ARRAY_BUFFER, ibo)
    GL.BufferData(GL.ELEMENT_ARRAY_BUFFER, INDICES, GL.STATIC_DRAW)
    print('\nvbo bytes:', GL.GetBufferParameteriv(GL.ARRAY_BUFFER, GL.BUFFER_SIZE))

    # nudge one vertex's u-coord through a mapped pointer, then restore it
    GL.BindBuffer(GL.ARRAY_BUFFER, vbo)
    view = GL.MapBuffer(GL.ARRAY_BUFFER, GL.READ_WRITE)
    if view is not None:
        struct.pack_into('<f', view, 3 * 4, 0.5)      # vertex 0, texcoord.u
        GL.UnmapBuffer(GL.ARRAY_BUFFER)
        GL.BufferSubData(GL.ARRAY_BUFFER, 3 * 4, struct.pack('<f', 0.0))

    # -- texture -------------------------------------------------------------
    texture = GL.GenTextures(1)[0]
    GL.BindTexture(GL.TEXTURE_2D, texture)
    GL.uBuild2DMipmaps(GL.TEXTURE_2D, GL.RGB, 64, 64, GL.RGB, GL.UNSIGNED_BYTE, checkerboard())
    GL.TexParameteri(GL.TEXTURE_2D, GL.TEXTURE_MIN_FILTER, GL.LINEAR_MIPMAP_LINEAR)
    GL.TexParameteri(GL.TEXTURE_2D, GL.TEXTURE_MAG_FILTER, GL.LINEAR)

    # -- a GLU quadric drawn with the fixed pipeline, for contrast ----------
    sphere = GL.uNewQuadric()
    GL.uQuadricDrawStyle(sphere, GL.uLINE)      # GLU enums keep the u prefix
    GL.uQuadricNormals(sphere, GL.uNONE)

    GL.Enable(GL.DEPTH_TEST)
    GL.ClearColor(0.08, 0.09, 0.11, 1.0)

    projection = m_perspective(55.0, WIDTH / HEIGHT, 0.5, 50.0)
    view_mtx   = m_translate(0.0, 0.0, -6.0)

    spinning = True
    angle = 0.0
    start = time.monotonic()
    frame = 0

    while True:
        now = time.monotonic() - start
        if spinning:
            angle += 0.002

        # textured quad sits on the left, tilted and spinning
        model = m_mul(m_translate(-1.7, 0.0, 0.0),
                      m_mul(m_rotate_x(0.4), m_rotate_y(angle)))
        mvp = m_mul(projection, m_mul(view_mtx, model))

        GL.Viewport(0, 0, WIDTH, HEIGHT)
        GL.Clear(GL.COLOR_BUFFER_BIT | GL.DEPTH_BUFFER_BIT)

        # textured quad through the shader
        GL.UseProgram(program)
        GL.ActiveTexture(GL.TEXTURE0)
        GL.BindTexture(GL.TEXTURE_2D, texture)
        GL.Uniform1i(loc_tex, 0)
        GL.Uniform1f(loc_time, now)
        GL.Uniform4fv(loc_tint, [1.0, 0.9, 0.7, 1.0])
        GL.UniformMatrix4fv(loc_mvp, False, mvp)

        GL.BindBuffer(GL.ARRAY_BUFFER, vbo)
        GL.BindBuffer(GL.ELEMENT_ARRAY_BUFFER, ibo)
        GL.EnableVertexAttribArray(a_position)
        GL.EnableVertexAttribArray(a_texcoord)
        GL.VertexAttribPointer(a_position, 3, GL.FLOAT, False, STRIDE, 0)
        GL.VertexAttribPointer(a_texcoord, 2, GL.FLOAT, False, STRIDE, 3 * 4)
        GL.DrawElements(GL.TRIANGLES, 6, GL.UNSIGNED_INT, 0)
        GL.DisableVertexAttribArray(a_position)
        GL.DisableVertexAttribArray(a_texcoord)

        # wire sphere on the right, fixed pipeline, camera set up with GLU
        GL.UseProgram(0)
        GL.MatrixMode(GL.PROJECTION)
        GL.LoadIdentity()
        GL.uPerspective(55.0, WIDTH / HEIGHT, 0.5, 50.0)
        GL.MatrixMode(GL.MODELVIEW)
        GL.LoadIdentity()
        GL.uLookAt(0.0, 0.0, 6.0,  0.0, 0.0, 0.0,  0.0, 1.0, 0.0)
        GL.Translatef(1.8, 0.0, 0.0)
        GL.Rotatef(angle * 57.3, 0.0, 1.0, 0.0)
        GL.Color3f(0.4, 0.7, 0.5)
        GL.uSphere(sphere, 1.4, 24, 16)

        err = GL.GetError()
        if err:
            print('GL error:', GL.uErrorString(err))

        window.GL_SwapWindow()
        frame += 1

        if frame == 1:
            model_view = m_mul(view_mtx, model)
            win = GL.uProject(-1.0, 1.0, 0.0, model_view, projection,
                              GL.GetIntegerv(GL.VIEWPORT, 4))
            print(f'\nquad top-left corner projects to screen pixel {win}')

        if MAX_FRAMES:
            if frame == MAX_FRAMES:
                save_ppm('pygl_screenshot.ppm', WIDTH, HEIGHT)
                break
            continue

        keys = SDL2.GetKeyState()
        if keys[SDL2.SCANCODE_ESCAPE]:
            break
        if keys[SDL2.SCANCODE_S]:
            save_ppm('pygl_screenshot.ppm', WIDTH, HEIGHT)
        if keys[SDL2.SCANCODE_R]:
            spinning = not spinning
            time.sleep(0.15)

        quit_requested = False
        event = SDL2.PollEvent()
        while event:
            if event[0] == SDL2.QUIT:
                quit_requested = True
            event = SDL2.PollEvent()
        if quit_requested:
            break

    # -- teardown ----------------------------------------------------------
    GL.ReleaseArrays()
    GL.uDeleteQuadric(sphere)
    GL.DeleteTextures([texture])
    GL.DeleteBuffers([vbo, ibo])
    GL.DeleteProgram(program)
    GL.DeleteShader(vs)
    GL.DeleteShader(fs)
    SDL2.Quit()


if __name__ == '__main__':
    sys.exit(main())
