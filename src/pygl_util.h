#ifndef __PYGL_UTIL_H__
#define __PYGL_UTIL_H__

//
// Marshalling helpers shared by the generated and hand-written wrappers.
// Everything here is static and lives in one translation unit, so the
// unused-function attribute keeps the build quiet when a helper is only
// reached from generated code that a given scope does not emit.
//

#if defined(__GNUC__)
 #define PYGL_UNUSED __attribute__((unused))
#else
 #define PYGL_UNUSED
#endif

// ****************************************************************************
// Python sequence -> C array. Returns the count, or -1 with an exception set.
// ****************************************************************************

#define PYGL_SEQ(NAME, CTYPE, CONV)                                            \
static PYGL_UNUSED Py_ssize_t                                                  \
NAME(PyObject *seq, CTYPE *out, Py_ssize_t max) {                              \
    PyObject *fast = PySequence_Fast(seq, "expected a sequence of numbers");   \
    if(NULL == fast) {                                                         \
        return -1;                                                             \
    }                                                                         \
    Py_ssize_t n = PySequence_Fast_GET_SIZE(fast);                             \
    if(n > max) {                                                              \
        Py_DECREF(fast);                                                       \
        PyErr_SetString(PyExc_ValueError, "sequence too long");                \
        return -1;                                                             \
    }                                                                         \
    for(Py_ssize_t idx = 0; idx < n; ++idx) {                                  \
        out[idx] = (CTYPE)CONV(PySequence_Fast_GET_ITEM(fast, idx));           \
    }                                                                          \
    Py_DECREF(fast);                                                           \
    if(PyErr_Occurred()) {                                                     \
        return -1;                                                             \
    }                                                                          \
    return n;                                                                  \
}

PYGL_SEQ( pygl_floats,  GLfloat,  PyFloat_AsDouble       )
PYGL_SEQ( pygl_doubles, GLdouble, PyFloat_AsDouble       )
PYGL_SEQ( pygl_ints,    GLint,    PyLong_AsLong          )
PYGL_SEQ( pygl_uints,   GLuint,   PyLong_AsUnsignedLong  )
PYGL_SEQ( pygl_shorts,  GLshort,  PyLong_AsLong          )
PYGL_SEQ( pygl_ushorts, GLushort, PyLong_AsUnsignedLong  )
PYGL_SEQ( pygl_bytes,   GLbyte,   PyLong_AsLong          )
PYGL_SEQ( pygl_ubytes,  GLubyte,  PyLong_AsUnsignedLong  )

// ****************************************************************************
// C array -> Python. A single element comes back as a scalar, more as a list.
// ****************************************************************************

#define PYGL_BUILD(NAME, CTYPE, CONV)                                          \
static PYGL_UNUSED PyObject *                                                  \
NAME(const CTYPE *values, Py_ssize_t n) {                                      \
    if(1 == n) {                                                              \
        return CONV(values[0]);                                               \
    }                                                                         \
    PyObject *list = PyList_New(n);                                            \
    if(NULL == list) {                                                        \
        return NULL;                                                          \
    }                                                                         \
    for(Py_ssize_t idx = 0; idx < n; ++idx) {                                  \
        PyList_SET_ITEM(list, idx, CONV(values[idx]));                        \
    }                                                                         \
    return list;                                                              \
}

PYGL_BUILD( pygl_build_floats,  GLfloat,   PyFloat_FromDouble           )
PYGL_BUILD( pygl_build_doubles, GLdouble,  PyFloat_FromDouble           )
PYGL_BUILD( pygl_build_ints,    GLint,     PyLong_FromLong              )
PYGL_BUILD( pygl_build_uints,   GLuint,    PyLong_FromUnsignedLong      )
PYGL_BUILD( pygl_build_bools,   GLboolean, PyBool_FromLong              )
PYGL_BUILD( pygl_build_int64,   GLint64,   PyLong_FromLongLong          )
PYGL_BUILD( pygl_build_uint64,  GLuint64,  PyLong_FromUnsignedLongLong  )

// Heap variant for calls whose array is unbounded (uniform arrays, name
// lists). PyMem_Malloc'd - free with PyMem_Free; NULL on error / empty.
#define PYGL_ALLOC(NAME, CTYPE, CONV)                                          \
static PYGL_UNUSED CTYPE *                                                     \
NAME(PyObject *seq, Py_ssize_t *n_out) {                                       \
    PyObject *fast = PySequence_Fast(seq, "expected a sequence of numbers");   \
    if(NULL == fast) {                                                         \
        return NULL;                                                           \
    }                                                                         \
    Py_ssize_t n = PySequence_Fast_GET_SIZE(fast);                             \
    CTYPE *buf = PyMem_Malloc(sizeof(CTYPE) * (n > 0 ? n : 1));                \
    if(NULL == buf) {                                                          \
        Py_DECREF(fast);                                                       \
        PyErr_NoMemory();                                                      \
        return NULL;                                                           \
    }                                                                         \
    for(Py_ssize_t idx = 0; idx < n; ++idx) {                                  \
        buf[idx] = (CTYPE)CONV(PySequence_Fast_GET_ITEM(fast, idx));           \
    }                                                                          \
    Py_DECREF(fast);                                                           \
    if(PyErr_Occurred()) {                                                     \
        PyMem_Free(buf);                                                       \
        return NULL;                                                           \
    }                                                                         \
    *n_out = n;                                                               \
    return buf;                                                               \
}

PYGL_ALLOC( pygl_alloc_floats,  GLfloat,  PyFloat_AsDouble      )
PYGL_ALLOC( pygl_alloc_doubles, GLdouble, PyFloat_AsDouble      )
PYGL_ALLOC( pygl_alloc_ints,    GLint,    PyLong_AsLong         )
PYGL_ALLOC( pygl_alloc_uints,   GLuint,   PyLong_AsUnsignedLong )

// Object names always come back as a list, even a single one.
static PYGL_UNUSED PyObject *
pygl_list_uints(const GLuint *values, Py_ssize_t n) {
    PyObject *list = PyList_New(n);
    if(NULL == list) {
        return NULL;
    }
    for(Py_ssize_t idx = 0; idx < n; ++idx) {
        PyList_SET_ITEM(list, idx, PyLong_FromUnsignedLong(values[idx]));
    }
    return list;
}

// ****************************************************************************
// Bulk-data arguments (phase 6). The trailing "pointer" of a vertex-array,
// draw or pixel-transfer call is either an integer offset into the bound
// buffer object or an object exposing the buffer protocol.
// ****************************************************************************

// Fills *out with the data pointer. When obj is a buffer, view is initialised
// and the caller must PyBuffer_Release it (or hand it to pygl_retain); when
// obj is an int offset or None, view->obj is left NULL. Returns 0 / -1.
static PYGL_UNUSED int
pygl_bufptr(PyObject *obj, Py_buffer *view, const void **out) {
    view->obj = NULL;
    if(NULL == obj || Py_None == obj) {
        *out = NULL;
        return 0;
    }
    if(PyLong_Check(obj)) {
        *out = (const void *)(intptr_t)PyLong_AsSsize_t(obj);
        return PyErr_Occurred() ? -1 : 0;
    }
    if(PyObject_GetBuffer(obj, view, PyBUF_SIMPLE) < 0) {
        return -1;
    }
    *out = view->buf;
    return 0;
}

// bytes in one component of a pixel-transfer type
static PYGL_UNUSED size_t
pygl_type_size(GLenum type) {
    switch(type) {
        case GL_UNSIGNED_BYTE_3_3_2: case GL_UNSIGNED_BYTE_2_3_3_REV:
            return 1;
        case GL_UNSIGNED_SHORT_5_6_5: case GL_UNSIGNED_SHORT_5_6_5_REV:
        case GL_UNSIGNED_SHORT_4_4_4_4: case GL_UNSIGNED_SHORT_4_4_4_4_REV:
        case GL_UNSIGNED_SHORT_5_5_5_1: case GL_UNSIGNED_SHORT_1_5_5_5_REV:
            return 2;
        case GL_UNSIGNED_INT_8_8_8_8: case GL_UNSIGNED_INT_8_8_8_8_REV:
        case GL_UNSIGNED_INT_10_10_10_2: case GL_UNSIGNED_INT_2_10_10_10_REV:
            return 4;
        case GL_BYTE: case GL_UNSIGNED_BYTE:
            return 1;
        case GL_SHORT: case GL_UNSIGNED_SHORT: case GL_HALF_FLOAT:
            return 2;
        default:  // GL_INT, GL_UNSIGNED_INT, GL_FLOAT, ...
            return 4;
    }
}

// worst-case byte size of a width x height image, assuming the default
// GL_PACK_ALIGNMENT of 4. Packed types count as a single component.
static PYGL_UNUSED size_t
pygl_image_size(GLsizei w, GLsizei h, GLsizei d, GLenum format, GLenum type) {
    size_t comps;
    switch(type) {
        case GL_UNSIGNED_BYTE_3_3_2: case GL_UNSIGNED_BYTE_2_3_3_REV:
        case GL_UNSIGNED_SHORT_5_6_5: case GL_UNSIGNED_SHORT_5_6_5_REV:
        case GL_UNSIGNED_SHORT_4_4_4_4: case GL_UNSIGNED_SHORT_4_4_4_4_REV:
        case GL_UNSIGNED_SHORT_5_5_5_1: case GL_UNSIGNED_SHORT_1_5_5_5_REV:
        case GL_UNSIGNED_INT_8_8_8_8: case GL_UNSIGNED_INT_8_8_8_8_REV:
        case GL_UNSIGNED_INT_10_10_10_2: case GL_UNSIGNED_INT_2_10_10_10_REV:
            comps = 1;
            break;
        default:
            switch(format) {
                case GL_RED: case GL_GREEN: case GL_BLUE: case GL_ALPHA:
                case GL_LUMINANCE: case GL_INTENSITY:
                case GL_DEPTH_COMPONENT: case GL_STENCIL_INDEX:
                    comps = 1; break;
                case GL_LUMINANCE_ALPHA: case GL_RG:
                    comps = 2; break;
                case GL_RGB: case GL_BGR:
                    comps = 3; break;
                default:  // GL_RGBA, GL_BGRA and anything unknown
                    comps = 4; break;
            }
    }
    size_t row = comps * pygl_type_size(type) * (size_t)(w > 0 ? w : 0);
    row = (row + 3) & ~(size_t)3;
    size_t layer = row * (size_t)(h > 0 ? h : 1);
    return layer * (size_t)(d > 0 ? d : 1);
}

#endif // __PYGL_UTIL_H__
