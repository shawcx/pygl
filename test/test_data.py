import struct

import pytest


def test_buffer_object_roundtrip(gl):
    if not hasattr(gl, 'GenBuffers'):
        pytest.skip('VBOs unavailable')
    buf = gl.GenBuffers(1)[0]
    gl.BindBuffer(gl.ARRAY_BUFFER, buf)

    payload = struct.pack('<8f', *range(8))
    gl.BufferData(gl.ARRAY_BUFFER, payload, gl.STATIC_DRAW)
    assert gl.GetBufferParameteriv(gl.ARRAY_BUFFER, gl.BUFFER_SIZE) == len(payload)

    gl.BufferSubData(gl.ARRAY_BUFFER, 4, struct.pack('<f', 99.0))
    back = gl.GetBufferSubData(gl.ARRAY_BUFFER, 0, len(payload))
    assert isinstance(back, bytes) and len(back) == len(payload)
    assert struct.unpack('<f', back[4:8])[0] == 99.0

    gl.BufferData(gl.ARRAY_BUFFER, 64, gl.DYNAMIC_DRAW)   # size-only allocation
    assert gl.GetBufferParameteriv(gl.ARRAY_BUFFER, gl.BUFFER_SIZE) == 64

    gl.BindBuffer(gl.ARRAY_BUFFER, 0)
    gl.DeleteBuffers([buf])


def test_tex_image_upload_and_readback(gl):
    tex = gl.GenTextures(1)[0]
    gl.BindTexture(gl.TEXTURE_2D, tex)
    gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST)
    gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST)

    pixels = bytes([10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120])  # 2x2 RGB
    gl.TexImage2D(gl.TEXTURE_2D, 0, gl.RGB, 2, 2, 0, gl.RGB, gl.UNSIGNED_BYTE, pixels)

    # allocate-only upload with None
    gl.TexImage2D(gl.TEXTURE_2D, 1, gl.RGB, 1, 1, 0, gl.RGB, gl.UNSIGNED_BYTE, None)
    gl.TexSubImage2D(gl.TEXTURE_2D, 0, 0, 0, 1, 1, gl.RGB, gl.UNSIGNED_BYTE, bytes([1, 2, 3]))

    out = gl.GetTexImage(gl.TEXTURE_2D, 0, gl.RGB, gl.UNSIGNED_BYTE, 2 * 2 * 3)
    assert isinstance(out, bytes) and len(out) == 12
    assert out[:3] == bytes([1, 2, 3])

    gl.DeleteTextures([tex])


def test_read_pixels(gl):
    gl.ClearColor(1.0, 0.0, 0.0, 1.0)
    gl.Clear(gl.COLOR_BUFFER_BIT)
    data = gl.ReadPixels(0, 0, 4, 4, gl.RGBA, gl.UNSIGNED_BYTE)
    assert isinstance(data, bytes) and len(data) == 4 * 4 * 4
    assert data[0] == 255 and data[1] == 0 and data[2] == 0

    sized = gl.ReadPixels(0, 0, 4, 4, gl.RGB, gl.UNSIGNED_BYTE, 4 * 4 * 3)
    assert len(sized) == 48


def test_draw_elements_client_indices(gl):
    gl.EnableClientState(gl.VERTEX_ARRAY)
    verts = struct.pack('<9f',
                        0.0, 1.0, 0.0,
                        -1.0, -1.0, 0.0,
                        1.0, -1.0, 0.0)
    gl.VertexPointer(3, gl.FLOAT, 0, verts)
    gl.DrawElements(gl.TRIANGLES, 3, gl.UNSIGNED_INT, struct.pack('<3I', 0, 1, 2))
    gl.DrawArrays(gl.TRIANGLES, 0, 3)
    gl.DisableClientState(gl.VERTEX_ARRAY)
    gl.ReleaseArrays()


def test_vertex_pointer_offset(gl):
    if not hasattr(gl, 'GenBuffers'):
        pytest.skip('VBOs unavailable')
    buf = gl.GenBuffers(1)[0]
    gl.BindBuffer(gl.ARRAY_BUFFER, buf)
    gl.BufferData(gl.ARRAY_BUFFER, struct.pack('<9f', *([0.0] * 9)), gl.STATIC_DRAW)
    gl.EnableClientState(gl.VERTEX_ARRAY)
    gl.VertexPointer(3, gl.FLOAT, 0, 0)          # integer offset, no retain
    gl.DrawArrays(gl.POINTS, 0, 3)
    gl.DisableClientState(gl.VERTEX_ARRAY)
    gl.BindBuffer(gl.ARRAY_BUFFER, 0)
    gl.DeleteBuffers([buf])


def test_release_arrays_is_safe_when_empty(gl):
    gl.ReleaseArrays()
    gl.ReleaseArrays()


def test_bad_buffer_arg_raises(gl):
    with pytest.raises(TypeError):
        gl.BufferSubData(gl.ARRAY_BUFFER, 0, object())
