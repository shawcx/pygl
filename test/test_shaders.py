import struct

import pytest

VERT = """
#version 120
uniform float scale;
attribute vec2 pos;
void main() { gl_Position = vec4(pos * scale, 0.0, 1.0); }
"""

FRAG = """
#version 120
uniform vec4 tint;
void main() { gl_FragColor = tint; }
"""


@pytest.fixture
def program(gl):
    if not hasattr(gl, 'CreateShader'):
        pytest.skip('GLSL unavailable')

    vs = gl.CreateShader(gl.VERTEX_SHADER)
    gl.ShaderSource(vs, VERT)
    gl.CompileShader(vs)
    assert gl.GetShaderiv(vs, gl.COMPILE_STATUS) == gl.TRUE, gl.GetShaderInfoLog(vs)

    fs = gl.CreateShader(gl.FRAGMENT_SHADER)
    gl.ShaderSource(fs, [FRAG])                 # list-of-str form
    gl.CompileShader(fs)
    assert gl.GetShaderiv(fs, gl.COMPILE_STATUS) == gl.TRUE, gl.GetShaderInfoLog(fs)

    prog = gl.CreateProgram()
    gl.AttachShader(prog, vs)
    gl.AttachShader(prog, fs)
    gl.BindAttribLocation(prog, 0, 'pos')
    gl.LinkProgram(prog)
    assert gl.GetProgramiv(prog, gl.LINK_STATUS) == gl.TRUE, gl.GetProgramInfoLog(prog)

    yield gl, prog

    gl.DeleteProgram(prog)
    gl.DeleteShader(vs)
    gl.DeleteShader(fs)


def test_compile_link(program):
    gl, prog = program
    assert isinstance(gl.GetProgramInfoLog(prog), str)
    assert gl.GetShaderiv.__doc__


def test_shader_source_roundtrip(program):
    gl, prog = program
    shaders = gl.GetAttachedShaders(prog)
    assert isinstance(shaders, list) and len(shaders) == 2
    src = gl.GetShaderSource(shaders[0])
    assert 'gl_Position' in src or 'gl_FragColor' in src


def test_uniforms(program):
    gl, prog = program
    gl.UseProgram(prog)

    loc_scale = gl.GetUniformLocation(prog, 'scale')
    loc_tint = gl.GetUniformLocation(prog, 'tint')
    assert loc_scale >= 0 and loc_tint >= 0
    assert gl.GetUniformLocation(prog, 'missing') == -1

    gl.Uniform1f(loc_scale, 2.0)
    assert abs(gl.GetUniformfv(prog, loc_scale) - 2.0) < 1e-6

    gl.Uniform4fv(loc_tint, [0.25, 0.5, 0.75, 1.0])
    tint = gl.GetUniformfv(prog, loc_tint, 4)
    assert len(tint) == 4 and abs(tint[2] - 0.75) < 1e-6

    with pytest.raises(ValueError):
        gl.Uniform4fv(loc_tint, [1.0, 2.0, 3.0])     # not a multiple of 4

    gl.UseProgram(0)


def test_active_uniform(program):
    gl, prog = program
    count = gl.GetProgramiv(prog, gl.ACTIVE_UNIFORMS)
    names = {gl.GetActiveUniform(prog, i)[0] for i in range(count)}
    assert 'scale' in names and 'tint' in names
    name, size, typ = gl.GetActiveUniform(prog, 0)
    assert isinstance(name, str) and size >= 1 and isinstance(typ, int)


def test_uniform_matrix(program):
    gl, prog = program
    gl.UseProgram(prog)
    ident = [1.0, 0.0, 0.0, 0.0,
             0.0, 1.0, 0.0, 0.0,
             0.0, 0.0, 1.0, 0.0,
             0.0, 0.0, 0.0, 1.0]
    # no mat4 uniform in this program; just exercise the marshalling path
    gl.UniformMatrix4fv(-1, False, ident)
    gl.UseProgram(0)
