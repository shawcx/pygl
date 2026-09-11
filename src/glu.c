#include "pygl.h"

#ifdef __APPLE__
 #include <OpenGL/glu.h>
#else
 #include <GL/glu.h>
#endif

//
// GLU wrappers. GLU is not described by the OpenGL registry, so these stay
// hand-written. The Python names carry a "u" prefix (uPerspective, uLookAt)
// to keep them out of the GL namespace. A GLUquadric is passed around as the
// integer value of its pointer, the same way GL object names are handled.
//

// ****************************************************************************
// viewing
// ****************************************************************************

static PyObject *PyGLu_Ortho2D(PyObject *self, PyObject *pyoArgs) {
    GLdouble l, r, b, t;
    if(!PyArg_ParseTuple(pyoArgs, "dddd", &l, &r, &b, &t)) {
        return NULL;
    }
    gluOrtho2D(l, r, b, t);
    Py_RETURN_NONE;
}

static PyObject *PyGLu_Perspective(PyObject *self, PyObject *pyoArgs) {
    GLdouble fovy, aspect, near, far;
    if(!PyArg_ParseTuple(pyoArgs, "dddd", &fovy, &aspect, &near, &far)) {
        return NULL;
    }
    gluPerspective(fovy, aspect, near, far);
    Py_RETURN_NONE;
}

static PyObject *PyGLu_LookAt(PyObject *self, PyObject *pyoArgs) {
    GLdouble ex, ey, ez, cx, cy, cz, ux, uy, uz;
    if(!PyArg_ParseTuple(pyoArgs, "ddddddddd", &ex, &ey, &ez, &cx, &cy, &cz, &ux, &uy, &uz)) {
        return NULL;
    }
    gluLookAt(ex, ey, ez, cx, cy, cz, ux, uy, uz);
    Py_RETURN_NONE;
}

static PyObject *PyGLu_PickMatrix(PyObject *self, PyObject *pyoArgs) {
    GLdouble  x, y, delx, dely;
    PyObject *pyoView;
    if(!PyArg_ParseTuple(pyoArgs, "ddddO", &x, &y, &delx, &dely, &pyoView)) {
        return NULL;
    }
    GLint view[4];
    if(pygl_ints(pyoView, view, 4) < 0) {
        return NULL;
    }
    gluPickMatrix(x, y, delx, dely, view);
    Py_RETURN_NONE;
}

// ****************************************************************************
// coordinate projection
// ****************************************************************************

static PyObject *PyGLu_Project(PyObject *self, PyObject *pyoArgs) {
    GLdouble  ox, oy, oz;
    PyObject *pyoModel, *pyoProj, *pyoView;
    if(!PyArg_ParseTuple(pyoArgs, "dddOOO", &ox, &oy, &oz, &pyoModel, &pyoProj, &pyoView)) {
        return NULL;
    }
    GLdouble model[16], proj[16];
    GLint    view[4];
    if(pygl_doubles(pyoModel, model, 16) < 0
    || pygl_doubles(pyoProj,  proj,  16) < 0
    || pygl_ints(pyoView,     view,   4) < 0) {
        return NULL;
    }
    GLdouble wx, wy, wz;
    if(GL_TRUE != gluProject(ox, oy, oz, model, proj, view, &wx, &wy, &wz)) {
        Py_RETURN_NONE;
    }
    return Py_BuildValue("(ddd)", wx, wy, wz);
}

static PyObject *PyGLu_UnProject(PyObject *self, PyObject *pyoArgs) {
    GLdouble  wx, wy, wz;
    PyObject *pyoModel, *pyoProj, *pyoView;
    if(!PyArg_ParseTuple(pyoArgs, "dddOOO", &wx, &wy, &wz, &pyoModel, &pyoProj, &pyoView)) {
        return NULL;
    }
    GLdouble model[16], proj[16];
    GLint    view[4];
    if(pygl_doubles(pyoModel, model, 16) < 0
    || pygl_doubles(pyoProj,  proj,  16) < 0
    || pygl_ints(pyoView,     view,   4) < 0) {
        return NULL;
    }
    GLdouble ox, oy, oz;
    if(GL_TRUE != gluUnProject(wx, wy, wz, model, proj, view, &ox, &oy, &oz)) {
        Py_RETURN_NONE;
    }
    return Py_BuildValue("(ddd)", ox, oy, oz);
}

// ****************************************************************************
// strings
// ****************************************************************************

static PyObject *PyGLu_ErrorString(PyObject *self, PyObject *pyoError) {
    GLenum error = (GLenum)PyLong_AsUnsignedLong(pyoError);
    if(PyErr_Occurred()) {
        return NULL;
    }
    const char *string = (const char *)gluErrorString(error);
    return string ? PyUnicode_FromString(string) : PyUnicode_FromString("");
}

static PyObject *PyGLu_GetString(PyObject *self, PyObject *pyoName) {
    GLenum name = (GLenum)PyLong_AsUnsignedLong(pyoName);
    if(PyErr_Occurred()) {
        return NULL;
    }
    const char *string = (const char *)gluGetString(name);
    return string ? PyUnicode_FromString(string) : PyUnicode_FromString("");
}

// ****************************************************************************
// mipmap and image helpers
// ****************************************************************************

static PyObject *PyGLu_Build1DMipmaps(PyObject *self, PyObject *pyoArgs) {
    GLenum    target, format, type;
    GLint     internalformat;
    GLsizei   width;
    PyObject *pyoData;
    if(!PyArg_ParseTuple(pyoArgs, "IiiIIO", &target, &internalformat, &width,
                         &format, &type, &pyoData)) {
        return NULL;
    }
    Py_buffer view;
    if(PyObject_GetBuffer(pyoData, &view, PyBUF_SIMPLE) < 0) {
        return NULL;
    }
    GLint result = gluBuild1DMipmaps(target, internalformat, width, format, type, view.buf);
    PyBuffer_Release(&view);
    return Py_BuildValue("i", result);
}

static PyObject *PyGLu_Build2DMipmaps(PyObject *self, PyObject *pyoArgs) {
    GLenum    target, format, type;
    GLint     internalformat;
    GLsizei   width, height;
    PyObject *pyoData;
    if(!PyArg_ParseTuple(pyoArgs, "IiiiIIO", &target, &internalformat, &width, &height,
                         &format, &type, &pyoData)) {
        return NULL;
    }
    Py_buffer view;
    if(PyObject_GetBuffer(pyoData, &view, PyBUF_SIMPLE) < 0) {
        return NULL;
    }
    GLint result = gluBuild2DMipmaps(target, internalformat, width, height, format, type, view.buf);
    PyBuffer_Release(&view);
    return Py_BuildValue("i", result);
}

static PyObject *PyGLu_ScaleImage(PyObject *self, PyObject *pyoArgs) {
    GLenum    format, type_in, type_out;
    GLsizei   w_in, h_in, w_out, h_out;
    PyObject *pyoData;
    if(!PyArg_ParseTuple(pyoArgs, "IiiIOiiI", &format, &w_in, &h_in, &type_in,
                         &pyoData, &w_out, &h_out, &type_out)) {
        return NULL;
    }
    Py_buffer view;
    if(PyObject_GetBuffer(pyoData, &view, PyBUF_SIMPLE) < 0) {
        return NULL;
    }
    Py_ssize_t out_size = (Py_ssize_t)pygl_image_size(w_out, h_out, 1, format, type_out);
    PyObject *pyoOut = PyBytes_FromStringAndSize(NULL, out_size);
    if(NULL == pyoOut) {
        PyBuffer_Release(&view);
        return NULL;
    }
    GLint result = gluScaleImage(format, w_in, h_in, type_in, view.buf,
                                 w_out, h_out, type_out, PyBytes_AS_STRING(pyoOut));
    PyBuffer_Release(&view);
    if(0 != result) {
        Py_DECREF(pyoOut);
        PyErr_Format(PyExc_RuntimeError, "gluScaleImage failed (%d)", result);
        return NULL;
    }
    return pyoOut;
}

// ****************************************************************************
// quadrics
// ****************************************************************************

static PyObject *PyGLu_NewQuadric(PyObject *self, PyObject *pyo) {
    GLUquadric *quad = gluNewQuadric();
    if(NULL == quad) {
        PyErr_SetString(PyExc_RuntimeError, "gluNewQuadric failed");
        return NULL;
    }
    return PyLong_FromVoidPtr(quad);
}

static PyObject *PyGLu_DeleteQuadric(PyObject *self, PyObject *pyoQuad) {
    GLUquadric *quad = PyLong_AsVoidPtr(pyoQuad);
    if(NULL == quad && PyErr_Occurred()) {
        return NULL;
    }
    gluDeleteQuadric(quad);
    Py_RETURN_NONE;
}

#define PYGLU_QUADRIC_ENUM(PY, GLUCALL)                                        \
static PyObject *PyGLu_##PY(PyObject *self, PyObject *pyoArgs) {                \
    PyObject *pyoQuad;                                                         \
    GLenum    value;                                                           \
    if(!PyArg_ParseTuple(pyoArgs, "OI", &pyoQuad, &value)) {                   \
        return NULL;                                                           \
    }                                                                         \
    GLUquadric *quad = PyLong_AsVoidPtr(pyoQuad);                              \
    if(NULL == quad && PyErr_Occurred()) {                                     \
        return NULL;                                                           \
    }                                                                         \
    GLUCALL(quad, value);                                                      \
    Py_RETURN_NONE;                                                            \
}

PYGLU_QUADRIC_ENUM( QuadricNormals,     gluQuadricNormals     )
PYGLU_QUADRIC_ENUM( QuadricTexture,     gluQuadricTexture     )
PYGLU_QUADRIC_ENUM( QuadricOrientation, gluQuadricOrientation )
PYGLU_QUADRIC_ENUM( QuadricDrawStyle,   gluQuadricDrawStyle   )

static PyObject *PyGLu_Sphere(PyObject *self, PyObject *pyoArgs) {
    PyObject *pyoQuad;
    GLdouble  radius;
    GLint     slices, stacks;
    if(!PyArg_ParseTuple(pyoArgs, "Odii", &pyoQuad, &radius, &slices, &stacks)) {
        return NULL;
    }
    GLUquadric *quad = PyLong_AsVoidPtr(pyoQuad);
    if(NULL == quad && PyErr_Occurred()) {
        return NULL;
    }
    gluSphere(quad, radius, slices, stacks);
    Py_RETURN_NONE;
}

static PyObject *PyGLu_Cylinder(PyObject *self, PyObject *pyoArgs) {
    PyObject *pyoQuad;
    GLdouble  base, top, height;
    GLint     slices, stacks;
    if(!PyArg_ParseTuple(pyoArgs, "Odddii", &pyoQuad, &base, &top, &height, &slices, &stacks)) {
        return NULL;
    }
    GLUquadric *quad = PyLong_AsVoidPtr(pyoQuad);
    if(NULL == quad && PyErr_Occurred()) {
        return NULL;
    }
    gluCylinder(quad, base, top, height, slices, stacks);
    Py_RETURN_NONE;
}

static PyObject *PyGLu_Disk(PyObject *self, PyObject *pyoArgs) {
    PyObject *pyoQuad;
    GLdouble  inner, outer;
    GLint     slices, loops;
    if(!PyArg_ParseTuple(pyoArgs, "Oddii", &pyoQuad, &inner, &outer, &slices, &loops)) {
        return NULL;
    }
    GLUquadric *quad = PyLong_AsVoidPtr(pyoQuad);
    if(NULL == quad && PyErr_Occurred()) {
        return NULL;
    }
    gluDisk(quad, inner, outer, slices, loops);
    Py_RETURN_NONE;
}

static PyObject *PyGLu_PartialDisk(PyObject *self, PyObject *pyoArgs) {
    PyObject *pyoQuad;
    GLdouble  inner, outer, start, sweep;
    GLint     slices, loops;
    if(!PyArg_ParseTuple(pyoArgs, "Oddiidd", &pyoQuad, &inner, &outer, &slices, &loops, &start, &sweep)) {
        return NULL;
    }
    GLUquadric *quad = PyLong_AsVoidPtr(pyoQuad);
    if(NULL == quad && PyErr_Occurred()) {
        return NULL;
    }
    gluPartialDisk(quad, inner, outer, slices, loops, start, sweep);
    Py_RETURN_NONE;
}

// ****************************************************************************

// GLU has its own enum range that overlaps the GL names (GLU_LINE != GL_LINE),
// so these keep the "u" prefix: pygl.uLINE, pygl.uSMOOTH, ...
void glu_add_constants(PyObject *mod) {
    PyModule_AddIntConstant( mod, "uPOINT",      GLU_POINT      );
    PyModule_AddIntConstant( mod, "uLINE",       GLU_LINE       );
    PyModule_AddIntConstant( mod, "uFILL",       GLU_FILL       );
    PyModule_AddIntConstant( mod, "uSILHOUETTE", GLU_SILHOUETTE );
    PyModule_AddIntConstant( mod, "uSMOOTH",     GLU_SMOOTH     );
    PyModule_AddIntConstant( mod, "uFLAT",       GLU_FLAT       );
    PyModule_AddIntConstant( mod, "uNONE",       GLU_NONE       );
    PyModule_AddIntConstant( mod, "uOUTSIDE",    GLU_OUTSIDE    );
    PyModule_AddIntConstant( mod, "uINSIDE",     GLU_INSIDE     );
    PyModule_AddIntConstant( mod, "uVERSION",    GLU_VERSION    );
    PyModule_AddIntConstant( mod, "uEXTENSIONS", GLU_EXTENSIONS );
}

static PyMethodDef PyGL_glu_methods[] = {
    { "uOrtho2D",           PyGLu_Ortho2D,        METH_VARARGS, "uOrtho2D(left, right, bottom, top)" },
    { "uPerspective",       PyGLu_Perspective,    METH_VARARGS, "uPerspective(fovy, aspect, zNear, zFar)" },
    { "uLookAt",            PyGLu_LookAt,         METH_VARARGS, "uLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ)" },
    { "uPickMatrix",        PyGLu_PickMatrix,     METH_VARARGS, "uPickMatrix(x, y, delX, delY, viewport)" },

    { "uProject",           PyGLu_Project,        METH_VARARGS, "uProject(objX, objY, objZ, model, proj, view) -> (winX, winY, winZ)" },
    { "uUnProject",         PyGLu_UnProject,      METH_VARARGS, "uUnProject(winX, winY, winZ, model, proj, view) -> (objX, objY, objZ)" },

    { "uErrorString",       PyGLu_ErrorString,    METH_O,       "uErrorString(error)" },
    { "uGetString",         PyGLu_GetString,      METH_O,       "uGetString(name)" },

    { "uBuild1DMipmaps",    PyGLu_Build1DMipmaps, METH_VARARGS, "uBuild1DMipmaps(target, internalformat, width, format, type, data)" },
    { "uBuild2DMipmaps",    PyGLu_Build2DMipmaps, METH_VARARGS, "uBuild2DMipmaps(target, internalformat, width, height, format, type, data)" },
    { "uScaleImage",        PyGLu_ScaleImage,     METH_VARARGS, "uScaleImage(format, wIn, hIn, typeIn, dataIn, wOut, hOut, typeOut) -> bytes" },

    { "uNewQuadric",        PyGLu_NewQuadric,     METH_NOARGS,  "uNewQuadric()" },
    { "uDeleteQuadric",     PyGLu_DeleteQuadric,  METH_O,       "uDeleteQuadric(quad)" },
    { "uQuadricNormals",    PyGLu_QuadricNormals, METH_VARARGS, "uQuadricNormals(quad, normal)" },
    { "uQuadricTexture",    PyGLu_QuadricTexture, METH_VARARGS, "uQuadricTexture(quad, texture)" },
    { "uQuadricOrientation",PyGLu_QuadricOrientation, METH_VARARGS, "uQuadricOrientation(quad, orientation)" },
    { "uQuadricDrawStyle",  PyGLu_QuadricDrawStyle,   METH_VARARGS, "uQuadricDrawStyle(quad, draw)" },
    { "uSphere",            PyGLu_Sphere,         METH_VARARGS, "uSphere(quad, radius, slices, stacks)" },
    { "uCylinder",          PyGLu_Cylinder,       METH_VARARGS, "uCylinder(quad, base, top, height, slices, stacks)" },
    { "uDisk",              PyGLu_Disk,           METH_VARARGS, "uDisk(quad, inner, outer, slices, loops)" },
    { "uPartialDisk",       PyGLu_PartialDisk,    METH_VARARGS, "uPartialDisk(quad, inner, outer, slices, loops, start, sweep)" },

    { NULL }
};
