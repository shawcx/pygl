import pytest


def test_error_and_version_strings(gl):
    assert gl.uErrorString(gl.INVALID_ENUM).lower().find('enum') >= 0
    assert isinstance(gl.uGetString(gl.VERSION), str)


def test_project_roundtrip(gl):
    gl.MatrixMode(gl.PROJECTION)
    gl.LoadIdentity()
    gl.uPerspective(60.0, 1.0, 1.0, 100.0)
    gl.MatrixMode(gl.MODELVIEW)
    gl.LoadIdentity()
    gl.Translatef(0.0, 0.0, -10.0)

    model = gl.GetDoublev(gl.MODELVIEW_MATRIX, 16)
    proj = gl.GetDoublev(gl.PROJECTION_MATRIX, 16)
    view = gl.GetIntegerv(gl.VIEWPORT, 4)

    win = gl.uProject(1.0, 2.0, -10.0, model, proj, view)
    assert win is not None and len(win) == 3
    obj = gl.uUnProject(win[0], win[1], win[2], model, proj, view)
    assert obj is not None
    assert abs(obj[0] - 1.0) < 1e-3
    assert abs(obj[1] - 2.0) < 1e-3
    assert abs(obj[2] + 10.0) < 1e-3


def test_quadric_sphere(gl):
    quad = gl.uNewQuadric()
    assert isinstance(quad, int) and quad != 0
    gl.uQuadricNormals(quad, gl.SMOOTH)
    gl.uQuadricTexture(quad, gl.TRUE)
    lst = gl.GenLists(1)
    gl.NewList(lst, gl.COMPILE)
    gl.uSphere(quad, 1.0, 16, 16)
    gl.uCylinder(quad, 1.0, 0.5, 2.0, 12, 3)
    gl.uDisk(quad, 0.2, 1.0, 12, 2)
    gl.EndList()
    gl.uDeleteQuadric(quad)
    gl.DeleteLists(lst, 1)


def test_build_2d_mipmaps(gl):
    tex = gl.GenTextures(1)[0]
    gl.BindTexture(gl.TEXTURE_2D, tex)
    pixels = bytes([128]) * (4 * 4 * 3)
    rc = gl.uBuild2DMipmaps(gl.TEXTURE_2D, gl.RGB, 4, 4, gl.RGB, gl.UNSIGNED_BYTE, pixels)
    assert rc == 0
    gl.DeleteTextures([tex])


def test_scale_image(gl):
    src = bytes(range(0, 64))            # 4x4 RGBA
    out = gl.uScaleImage(gl.RGBA, 4, 4, gl.UNSIGNED_BYTE, src, 2, 2, gl.UNSIGNED_BYTE)
    assert isinstance(out, bytes) and len(out) == 2 * 2 * 4
