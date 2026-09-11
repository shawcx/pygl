import json
import os

import pytest

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
MANIFEST = json.load(open(os.path.join(ROOT, 'tools', 'manifest.json')))


def test_module_surface():
    import pygl
    # wrappers past GL 1.1 are #ifdef-guarded on the GLEW entry point, so a
    # generated name may be absent if the build's GLEW predates it
    gated = set(MANIFEST.get('gated', []))
    gen = {n for names in MANIFEST['generated'].values() for n in names}
    for name in gen - gated:
        assert hasattr(pygl, name), name
    # hand-written GL 3.x+ wrappers (modern.c) are #ifdef-guarded the same way
    for name in set(MANIFEST['manual']) - gated:
        assert hasattr(pygl, name), name
    # the modern core that any current GLEW provides
    for name in ('GenVertexArrays', 'BindVertexArray', 'GetStringi', 'TexStorage2D',
                 'DrawArraysInstanced', 'BlitFramebuffer', 'MemoryBarrier'):
        assert hasattr(pygl, name), name


def test_constants_present():
    import pygl
    for name in ('COLOR_BUFFER_BIT', 'DEPTH_BUFFER_BIT', 'TRIANGLES', 'QUADS',
                 'MODELVIEW', 'PROJECTION', 'LIGHT0', 'TEXTURE_2D', 'RGBA',
                 'ARRAY_BUFFER', 'FRAGMENT_SHADER', 'COMPILE_STATUS'):
        assert isinstance(getattr(pygl, name), int)


def test_scalar_calls(gl):
    gl.ClearColor(0.1, 0.2, 0.3, 1.0)
    gl.Clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT)
    gl.Enable(gl.DEPTH_TEST)
    assert gl.IsEnabled(gl.DEPTH_TEST) is True
    gl.Disable(gl.DEPTH_TEST)
    assert gl.IsEnabled(gl.DEPTH_TEST) is False
    gl.Viewport(0, 0, 64, 64)
    gl.MatrixMode(gl.PROJECTION)
    gl.LoadIdentity()
    gl.Ortho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0)
    gl.Rectf(-0.5, -0.5, 0.5, 0.5)
    gl.Finish()


def test_immediate_mode(gl):
    gl.Begin(gl.TRIANGLES)
    gl.Color3f(1.0, 0.0, 0.0)
    gl.Vertex3f(0.0, 1.0, 0.0)
    gl.Vertex3f(-1.0, -1.0, 0.0)
    gl.Vertex3f(1.0, -1.0, 0.0)
    gl.End()


def test_vector_calls(gl):
    gl.Lightfv(gl.LIGHT0, gl.POSITION, [1.0, 1.0, 1.0, 0.0])
    gl.Materialfv(gl.FRONT, gl.AMBIENT_AND_DIFFUSE, [0.2, 0.4, 0.8, 1.0])
    gl.LoadMatrixf([1.0, 0.0, 0.0, 0.0,
                    0.0, 1.0, 0.0, 0.0,
                    0.0, 0.0, 1.0, 0.0,
                    0.0, 0.0, 0.0, 1.0])
    gl.Color3fv([0.5, 0.5, 0.5])


def test_query_scalar_and_list(gl):
    assert gl.GetError() == 0
    depth_bits = gl.GetIntegerv(gl.DEPTH_BITS)
    assert isinstance(depth_bits, int)
    vp = gl.GetIntegerv(gl.VIEWPORT, 4)
    assert isinstance(vp, list) and len(vp) == 4
    matrix = gl.GetFloatv(gl.MODELVIEW_MATRIX, 16)
    assert len(matrix) == 16


def test_display_list(gl):
    base = gl.GenLists(1)
    assert base > 0
    gl.NewList(base, gl.COMPILE)
    gl.Begin(gl.POINTS)
    gl.Vertex3f(0.0, 0.0, 0.0)
    gl.End()
    gl.EndList()
    assert gl.IsList(base) is True
    gl.CallList(base)
    gl.DeleteLists(base, 1)


def test_textures_gen_delete(gl):
    names = gl.GenTextures(3)
    assert isinstance(names, list) and len(names) == 3
    gl.BindTexture(gl.TEXTURE_2D, names[0])
    assert gl.IsTexture(names[0]) is True
    gl.DeleteTextures(names)
    assert gl.IsTexture(names[0]) is False


def test_buffers_roundtrip(gl):
    if not hasattr(gl, 'GenBuffers'):
        pytest.skip('VBOs unavailable')
    buf = gl.GenBuffers(1)[0]
    gl.BindBuffer(gl.ARRAY_BUFFER, buf)
    # BufferData is phase 6; just exercise gen/bind/delete here
    gl.BindBuffer(gl.ARRAY_BUFFER, 0)
    gl.DeleteBuffers([buf])


def test_error_on_bad_enum(gl):
    gl.Begin(0xFFFF)          # invalid primitive -> GL_INVALID_ENUM
    assert gl.GetError() == gl.INVALID_ENUM
