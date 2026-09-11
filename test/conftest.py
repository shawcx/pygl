import pytest


@pytest.fixture(scope='session')
def gl():
    '''A current OpenGL context backed by a hidden SDL2 window.

    macOS hands back a 2.1 compatibility context, which is exactly the
    surface pygl targets.
    '''
    SDL2 = pytest.importorskip('SDL2')
    import pygl

    if SDL2.Init(SDL2.INIT_VIDEO):
        pytest.skip('SDL2 video init failed (no display)')

    window = SDL2.Window(
        'pygl-test',
        size  = (64, 64),
        flags = SDL2.WINDOW_OPENGL | SDL2.WINDOW_HIDDEN,
        )
    if not window:
        SDL2.Quit()
        pytest.skip(f'could not create GL window: {SDL2.GetError()}')

    window.GL_CreateContext()
    window.GL_MakeCurrent()
    pygl.glewInit()
    # GLEW's own probing leaves a stale GL_INVALID_ENUM on some drivers
    pygl.GetError()

    yield pygl

    del window
    SDL2.Quit()


@pytest.fixture(autouse=True)
def _no_gl_error(request):
    '''Fail any test that leaves the GL error flag set.'''
    yield
    if 'gl' in request.fixturenames:
        import pygl
        err = pygl.GetError()
        assert err == 0, f'GL error 0x{err:04x} left set'
