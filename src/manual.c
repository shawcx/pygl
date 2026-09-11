#include "pygl.h"

//
// Wrappers that cannot be generated mechanically and do not belong with the
// bulk-data code in data.c / shaders.c / modern.c: the string query and the
// two-vector rectangle calls.
//

// ****************************************************************************
// string queries
// ****************************************************************************

static PyObject * PyGL_GetString(PyObject *self, PyObject *pyoName) {
    GLenum name = (GLenum)PyLong_AsUnsignedLong(pyoName);
    const char *string = (const char *)glGetString(name);
    if(string) {
        return PyUnicode_FromString(string);
    }
    else {
        Py_RETURN_NONE;
    }
}

static PyObject * PyGL_GetStringi(PyObject *self, PyObject *pyoArgs) {
    GLenum name;
    GLuint index;
    if(!PyArg_ParseTuple(pyoArgs, "II", &name, &index)) {
        return NULL;
    }
    const char *string = (const char *)glGetStringi(name, index);
    if(NULL == string) {
        Py_RETURN_NONE;
    }
    return PyUnicode_FromString(string);
}

// ****************************************************************************
// two-vector rectangles
// ****************************************************************************

#define PYGL_RECTV(PY, CTYPE, HELPER, GLCALL)                                  \
static PyObject * PyGL_##PY(PyObject *self, PyObject *pyoArgs) {                \
    PyObject *pyoV1, *pyoV2;                                                   \
    if(!PyArg_ParseTuple(pyoArgs, "OO", &pyoV1, &pyoV2)) {                     \
        return NULL;                                                           \
    }                                                                         \
    CTYPE a[2], b[2];                                                          \
    if(HELPER(pyoV1, a, 2) < 2 || HELPER(pyoV2, b, 2) < 2) {                   \
        if(!PyErr_Occurred()) {                                                \
            PyErr_SetString(PyExc_ValueError, "expected two 2-element vectors"); \
        }                                                                     \
        return NULL;                                                           \
    }                                                                         \
    GLCALL(a[0], a[1], b[0], b[1]);                                            \
    Py_RETURN_NONE;                                                            \
}

PYGL_RECTV( Rectdv, GLdouble, pygl_doubles, glRectd )
PYGL_RECTV( Rectfv, GLfloat,  pygl_floats,  glRectf )
PYGL_RECTV( Rectiv, GLint,    pygl_ints,    glRecti )
PYGL_RECTV( Rectsv, GLshort,  pygl_shorts,  glRects )

// ****************************************************************************

static PyMethodDef PyGL_manual_methods[] = {
    { "GetString",  PyGL_GetString,  METH_O,       "GetString(name)" },
    { "GetStringi", PyGL_GetStringi, METH_VARARGS, "GetStringi(name, index)" },

    { "Rectdv",     PyGL_Rectdv,     METH_VARARGS, "Rectdv(v1, v2)" },
    { "Rectfv",     PyGL_Rectfv,     METH_VARARGS, "Rectfv(v1, v2)" },
    { "Rectiv",     PyGL_Rectiv,     METH_VARARGS, "Rectiv(v1, v2)" },
    { "Rectsv",     PyGL_Rectsv,     METH_VARARGS, "Rectsv(v1, v2)" },

    { NULL }
};
