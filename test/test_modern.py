import pytest


def test_modern_surface():
    import pygl
    for name in ('FenceSync', 'DeleteSync', 'ClientWaitSync', 'GetSynciv',
                 'MapBufferRange', 'NamedBufferData', 'BufferStorage',
                 'VertexAttribIPointer', 'DrawElementsInstanced',
                 'DrawElementsBaseVertex', 'DrawArraysIndirect',
                 'MultiDrawArrays', 'MultiDrawElements',
                 'TransformFeedbackVaryings', 'GetUniformBlockIndex',
                 'GetActiveUniformBlockName', 'GetUniformIndices',
                 'GetProgramBinary', 'ProgramBinary', 'CreateShaderProgramv',
                 'ClearBufferfv', 'ClearBufferiv', 'ClearBufferuiv',
                 'BindTextures', 'ViewportArrayv', 'ScissorArrayv',
                 'DebugMessageControl', 'DebugMessageInsert',
                 'DebugMessageCallback', 'GetDebugMessageLog', 'ObjectLabel',
                 'Uniform4uiv', 'Uniform3dv', 'UniformMatrix4dv',
                 'ProgramUniform4fv', 'ProgramUniformMatrix4fv',
                 'GetTextureImage', 'TextureSubImage2D', 'ReadnPixels'):
        assert hasattr(pygl, name), name


def _gl_version(gl):
    '''(major, minor) parsed from GL_VERSION - reliable on every context,
    unlike GL_MAJOR_VERSION which is an invalid enum before GL 3.0.'''
    text = gl.GetString(gl.VERSION) or '0.0'
    try:
        major, minor = text.split()[0].split('.')[:2]
        return int(major), int(minor)
    except ValueError:
        return 0, 0


@pytest.fixture
def gl3(gl):
    '''gl, but skipped unless the live context is GL 3.0 or newer.'''
    version = _gl_version(gl)
    if version < (3, 0):
        pytest.skip(f'GL {version[0]}.{version[1]} context, need 3.0+')
    gl.GetError()
    return gl


def test_vertex_array_object(gl3):
    vao = gl3.GenVertexArrays(1)[0]
    assert vao > 0
    gl3.BindVertexArray(vao)
    assert gl3.IsVertexArray(vao) is True
    gl3.BindVertexArray(0)
    gl3.DeleteVertexArrays([vao])


def test_sync_object(gl3):
    sync = gl3.FenceSync(gl3.SYNC_GPU_COMMANDS_COMPLETE, 0)
    assert isinstance(sync, int) and sync != 0
    assert gl3.IsSync(sync) is True
    state = gl3.ClientWaitSync(sync, gl3.SYNC_FLUSH_COMMANDS_BIT, 10_000_000)
    assert state in (gl3.ALREADY_SIGNALED, gl3.CONDITION_SATISFIED,
                     gl3.TIMEOUT_EXPIRED, gl3.WAIT_FAILED)
    status = gl3.GetSynciv(sync, gl3.SYNC_STATUS)
    assert isinstance(status, int)
    gl3.DeleteSync(sync)
    assert gl3.IsSync(sync) is False


def test_map_buffer_range(gl3):
    buf = gl3.GenBuffers(1)[0]
    gl3.BindBuffer(gl3.ARRAY_BUFFER, buf)
    payload = bytes(range(64))
    gl3.BufferData(gl3.ARRAY_BUFFER, payload, gl3.STATIC_DRAW)
    view = gl3.MapBufferRange(gl3.ARRAY_BUFFER, 0, 64, gl3.MAP_READ_BIT)
    assert bytes(view) == payload
    gl3.UnmapBuffer(gl3.ARRAY_BUFFER)
    gl3.BindBuffer(gl3.ARRAY_BUFFER, 0)
    gl3.DeleteBuffers([buf])


def test_buffer_storage(gl3):
    if not hasattr(gl3, 'BufferStorage'):
        pytest.skip('GL 4.4 BufferStorage unavailable')
    if _gl_version(gl3) < (4, 4):
        pytest.skip('need GL 4.4')
    buf = gl3.GenBuffers(1)[0]
    gl3.BindBuffer(gl3.ARRAY_BUFFER, buf)
    gl3.BufferStorage(gl3.ARRAY_BUFFER, 128, gl3.MAP_READ_BIT)
    assert gl3.GetBufferParameteriv(gl3.ARRAY_BUFFER, gl3.BUFFER_SIZE) == 128
    gl3.BindBuffer(gl3.ARRAY_BUFFER, 0)
    gl3.DeleteBuffers([buf])


def test_uniform_uint_and_double_arrays(gl3):
    # a compile-free surface check: bad location is silently ignored by GL,
    # the point is that the marshalling accepts the Python sequences
    gl3.UseProgram(0)
    gl3.Uniform4uiv(-1, [1, 2, 3, 4])
    gl3.Uniform3dv(-1, [0.0, 0.5, 1.0])
    gl3.UniformMatrix4dv(-1, False, [0.0] * 16)
    gl3.GetError()   # -1 location without a program is a no-op, clear anything


def test_clear_buffer(gl3):
    gl3.ClearBufferfv(gl3.COLOR, 0, [0.2, 0.3, 0.4, 1.0])
    gl3.ClearBufferfi(gl3.DEPTH_STENCIL, 0, 1.0, 0)
    gl3.Finish()


def test_transform_feedback_varyings(gl3):
    prog = gl3.CreateProgram()
    gl3.TransformFeedbackVaryings(prog, ['gl_Position'], gl3.INTERLEAVED_ATTRIBS)
    gl3.DeleteProgram(prog)
