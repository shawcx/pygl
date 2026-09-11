#include "pygl.h"

//
// Phase 6: calls that move bulk data across the boundary - buffer objects,
// pixel transfer and client vertex arrays. The trailing data argument is an
// int offset into the bound buffer object, an object exposing the buffer
// protocol, or None; see pygl_bufptr() in pygl_util.h.
//
// Vertex-array pointers are read by the driver later, at draw time, so the
// backing buffer is retained until ReleaseArrays() is called. Every other
// call here consumes its data before returning and releases immediately.
//

// ****************************************************************************
// retained client-array buffers
// ****************************************************************************

static Py_buffer  *pygl_retained     = NULL;
static Py_ssize_t  pygl_retained_n   = 0;
static Py_ssize_t  pygl_retained_cap = 0;

static int pygl_retain(Py_buffer *view) {
    if(NULL == view->obj) {
        return 0;               // an int offset - nothing to hold on to
    }
    if(pygl_retained_n == pygl_retained_cap) {
        Py_ssize_t cap = pygl_retained_cap ? pygl_retained_cap * 2 : 8;
        Py_buffer *grown = PyMem_Realloc(pygl_retained, cap * sizeof(Py_buffer));
        if(NULL == grown) {
            PyErr_NoMemory();
            return -1;
        }
        pygl_retained     = grown;
        pygl_retained_cap = cap;
    }
    pygl_retained[pygl_retained_n++] = *view;
    return 0;
}

static PyObject * PyGL_ReleaseArrays(PyObject *self, PyObject *pyo) {
    for(Py_ssize_t idx = 0; idx < pygl_retained_n; ++idx) {
        PyBuffer_Release(&pygl_retained[idx]);
    }
    pygl_retained_n = 0;
    Py_RETURN_NONE;
}

// ****************************************************************************
// buffer objects
// ****************************************************************************

static PyObject * PyGL_BufferData(PyObject *self, PyObject *pyoArgs) {
    GLenum    target;
    PyObject *pyoData;
    GLenum    usage;
    if(!PyArg_ParseTuple(pyoArgs, "IOI", &target, &pyoData, &usage)) {
        return NULL;
    }
    if(PyLong_Check(pyoData)) {
        GLsizeiptr size = (GLsizeiptr)PyLong_AsSsize_t(pyoData);
        if(PyErr_Occurred()) {
            return NULL;
        }
        glBufferData(target, size, NULL, usage);
        Py_RETURN_NONE;
    }
    Py_buffer view;
    if(PyObject_GetBuffer(pyoData, &view, PyBUF_SIMPLE) < 0) {
        return NULL;
    }
    glBufferData(target, (GLsizeiptr)view.len, view.buf, usage);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}

static PyObject * PyGL_BufferSubData(PyObject *self, PyObject *pyoArgs) {
    GLenum    target;
    Py_ssize_t offset;
    PyObject *pyoData;
    if(!PyArg_ParseTuple(pyoArgs, "InO", &target, &offset, &pyoData)) {
        return NULL;
    }
    Py_buffer view;
    if(PyObject_GetBuffer(pyoData, &view, PyBUF_SIMPLE) < 0) {
        return NULL;
    }
    glBufferSubData(target, (GLintptr)offset, (GLsizeiptr)view.len, view.buf);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}

static PyObject * PyGL_GetBufferSubData(PyObject *self, PyObject *pyoArgs) {
    GLenum     target;
    Py_ssize_t offset;
    Py_ssize_t size;
    if(!PyArg_ParseTuple(pyoArgs, "Inn", &target, &offset, &size)) {
        return NULL;
    }
    if(size < 0) {
        PyErr_SetString(PyExc_ValueError, "invalid size");
        return NULL;
    }
    PyObject *pyoBytes = PyBytes_FromStringAndSize(NULL, size);
    if(NULL == pyoBytes) {
        return NULL;
    }
    glGetBufferSubData(target, (GLintptr)offset, (GLsizeiptr)size, PyBytes_AS_STRING(pyoBytes));
    return pyoBytes;
}

// ****************************************************************************
// texture images
// ****************************************************************************

static PyObject * PyGL_TexImage1D(PyObject *self, PyObject *pyoArgs) {
    GLenum target, format, type;
    GLint  level, internalformat, border;
    GLsizei width;
    PyObject *pyoPixels;
    if(!PyArg_ParseTuple(pyoArgs, "IiiiiIIO", &target, &level, &internalformat,
                         &width, &border, &format, &type, &pyoPixels)) {
        return NULL;
    }
    Py_buffer view;
    const void *pixels;
    if(pygl_bufptr(pyoPixels, &view, &pixels) < 0) {
        return NULL;
    }
    glTexImage1D(target, level, internalformat, width, border, format, type, pixels);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}

static PyObject * PyGL_TexImage2D(PyObject *self, PyObject *pyoArgs) {
    GLenum target, format, type;
    GLint  level, internalformat, border;
    GLsizei width, height;
    PyObject *pyoPixels;
    if(!PyArg_ParseTuple(pyoArgs, "IiiiiiIIO", &target, &level, &internalformat,
                         &width, &height, &border, &format, &type, &pyoPixels)) {
        return NULL;
    }
    Py_buffer view;
    const void *pixels;
    if(pygl_bufptr(pyoPixels, &view, &pixels) < 0) {
        return NULL;
    }
    glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}

static PyObject * PyGL_TexImage3D(PyObject *self, PyObject *pyoArgs) {
    GLenum target, format, type;
    GLint  level, internalformat, border;
    GLsizei width, height, depth;
    PyObject *pyoPixels;
    if(!PyArg_ParseTuple(pyoArgs, "IiiiiiiIIO", &target, &level, &internalformat,
                         &width, &height, &depth, &border, &format, &type, &pyoPixels)) {
        return NULL;
    }
    Py_buffer view;
    const void *pixels;
    if(pygl_bufptr(pyoPixels, &view, &pixels) < 0) {
        return NULL;
    }
    glTexImage3D(target, level, internalformat, width, height, depth, border, format, type, pixels);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}

static PyObject * PyGL_TexSubImage1D(PyObject *self, PyObject *pyoArgs) {
    GLenum target, format, type;
    GLint  level, xoffset;
    GLsizei width;
    PyObject *pyoPixels;
    if(!PyArg_ParseTuple(pyoArgs, "IiiiIIO", &target, &level, &xoffset,
                         &width, &format, &type, &pyoPixels)) {
        return NULL;
    }
    Py_buffer view;
    const void *pixels;
    if(pygl_bufptr(pyoPixels, &view, &pixels) < 0) {
        return NULL;
    }
    glTexSubImage1D(target, level, xoffset, width, format, type, pixels);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}

static PyObject * PyGL_TexSubImage2D(PyObject *self, PyObject *pyoArgs) {
    GLenum target, format, type;
    GLint  level, xoffset, yoffset;
    GLsizei width, height;
    PyObject *pyoPixels;
    if(!PyArg_ParseTuple(pyoArgs, "IiiiiiIIO", &target, &level, &xoffset, &yoffset,
                         &width, &height, &format, &type, &pyoPixels)) {
        return NULL;
    }
    Py_buffer view;
    const void *pixels;
    if(pygl_bufptr(pyoPixels, &view, &pixels) < 0) {
        return NULL;
    }
    glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}

static PyObject * PyGL_TexSubImage3D(PyObject *self, PyObject *pyoArgs) {
    GLenum target, format, type;
    GLint  level, xoffset, yoffset, zoffset;
    GLsizei width, height, depth;
    PyObject *pyoPixels;
    if(!PyArg_ParseTuple(pyoArgs, "IiiiiiiiIIO", &target, &level, &xoffset, &yoffset,
                         &zoffset, &width, &height, &depth, &format, &type, &pyoPixels)) {
        return NULL;
    }
    Py_buffer view;
    const void *pixels;
    if(pygl_bufptr(pyoPixels, &view, &pixels) < 0) {
        return NULL;
    }
    glTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}

static PyObject * PyGL_GetTexImage(PyObject *self, PyObject *pyoArgs) {
    GLenum     target, format, type;
    GLint      level;
    Py_ssize_t size;
    if(!PyArg_ParseTuple(pyoArgs, "IiIIn", &target, &level, &format, &type, &size)) {
        return NULL;
    }
    if(size < 0) {
        PyErr_SetString(PyExc_ValueError, "invalid size");
        return NULL;
    }
    PyObject *pyoBytes = PyBytes_FromStringAndSize(NULL, size);
    if(NULL == pyoBytes) {
        return NULL;
    }
    glGetTexImage(target, level, format, type, PyBytes_AS_STRING(pyoBytes));
    return pyoBytes;
}

// ****************************************************************************
// compressed texture images
// ****************************************************************************

static PyObject * PyGL_CompressedTexImage1D(PyObject *self, PyObject *pyoArgs) {
    GLenum  target, internalformat;
    GLint   level, border;
    GLsizei width;
    PyObject *pyoData;
    if(!PyArg_ParseTuple(pyoArgs, "IiIiiO", &target, &level, &internalformat,
                         &width, &border, &pyoData)) {
        return NULL;
    }
    Py_buffer view;
    if(PyObject_GetBuffer(pyoData, &view, PyBUF_SIMPLE) < 0) {
        return NULL;
    }
    glCompressedTexImage1D(target, level, internalformat, width, border,
                           (GLsizei)view.len, view.buf);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}

static PyObject * PyGL_CompressedTexImage2D(PyObject *self, PyObject *pyoArgs) {
    GLenum  target, internalformat;
    GLint   level, border;
    GLsizei width, height;
    PyObject *pyoData;
    if(!PyArg_ParseTuple(pyoArgs, "IiIiiiO", &target, &level, &internalformat,
                         &width, &height, &border, &pyoData)) {
        return NULL;
    }
    Py_buffer view;
    if(PyObject_GetBuffer(pyoData, &view, PyBUF_SIMPLE) < 0) {
        return NULL;
    }
    glCompressedTexImage2D(target, level, internalformat, width, height, border,
                           (GLsizei)view.len, view.buf);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}

static PyObject * PyGL_CompressedTexImage3D(PyObject *self, PyObject *pyoArgs) {
    GLenum  target, internalformat;
    GLint   level, border;
    GLsizei width, height, depth;
    PyObject *pyoData;
    if(!PyArg_ParseTuple(pyoArgs, "IiIiiiiO", &target, &level, &internalformat,
                         &width, &height, &depth, &border, &pyoData)) {
        return NULL;
    }
    Py_buffer view;
    if(PyObject_GetBuffer(pyoData, &view, PyBUF_SIMPLE) < 0) {
        return NULL;
    }
    glCompressedTexImage3D(target, level, internalformat, width, height, depth, border,
                           (GLsizei)view.len, view.buf);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}

static PyObject * PyGL_CompressedTexSubImage1D(PyObject *self, PyObject *pyoArgs) {
    GLenum  target, format;
    GLint   level, xoffset;
    GLsizei width;
    PyObject *pyoData;
    if(!PyArg_ParseTuple(pyoArgs, "IiiiIO", &target, &level, &xoffset,
                         &width, &format, &pyoData)) {
        return NULL;
    }
    Py_buffer view;
    if(PyObject_GetBuffer(pyoData, &view, PyBUF_SIMPLE) < 0) {
        return NULL;
    }
    glCompressedTexSubImage1D(target, level, xoffset, width, format,
                              (GLsizei)view.len, view.buf);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}

static PyObject * PyGL_CompressedTexSubImage2D(PyObject *self, PyObject *pyoArgs) {
    GLenum  target, format;
    GLint   level, xoffset, yoffset;
    GLsizei width, height;
    PyObject *pyoData;
    if(!PyArg_ParseTuple(pyoArgs, "IiiiiiIO", &target, &level, &xoffset, &yoffset,
                         &width, &height, &format, &pyoData)) {
        return NULL;
    }
    Py_buffer view;
    if(PyObject_GetBuffer(pyoData, &view, PyBUF_SIMPLE) < 0) {
        return NULL;
    }
    glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format,
                              (GLsizei)view.len, view.buf);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}

static PyObject * PyGL_CompressedTexSubImage3D(PyObject *self, PyObject *pyoArgs) {
    GLenum  target, format;
    GLint   level, xoffset, yoffset, zoffset;
    GLsizei width, height, depth;
    PyObject *pyoData;
    if(!PyArg_ParseTuple(pyoArgs, "IiiiiiiiIO", &target, &level, &xoffset, &yoffset,
                         &zoffset, &width, &height, &depth, &format, &pyoData)) {
        return NULL;
    }
    Py_buffer view;
    if(PyObject_GetBuffer(pyoData, &view, PyBUF_SIMPLE) < 0) {
        return NULL;
    }
    glCompressedTexSubImage3D(target, level, xoffset, yoffset, zoffset,
                              width, height, depth, format,
                              (GLsizei)view.len, view.buf);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}

static PyObject * PyGL_GetCompressedTexImage(PyObject *self, PyObject *pyoArgs) {
    GLenum     target;
    GLint      level;
    Py_ssize_t size;
    if(!PyArg_ParseTuple(pyoArgs, "Iin", &target, &level, &size)) {
        return NULL;
    }
    if(size < 0) {
        PyErr_SetString(PyExc_ValueError, "invalid size");
        return NULL;
    }
    PyObject *pyoBytes = PyBytes_FromStringAndSize(NULL, size);
    if(NULL == pyoBytes) {
        return NULL;
    }
    glGetCompressedTexImage(target, level, PyBytes_AS_STRING(pyoBytes));
    return pyoBytes;
}

// ****************************************************************************
// raster pixel transfer
// ****************************************************************************

static PyObject * PyGL_DrawPixels(PyObject *self, PyObject *pyoArgs) {
    GLsizei width, height;
    GLenum  format, type;
    PyObject *pyoPixels;
    if(!PyArg_ParseTuple(pyoArgs, "iiIIO", &width, &height, &format, &type, &pyoPixels)) {
        return NULL;
    }
    Py_buffer view;
    const void *pixels;
    if(pygl_bufptr(pyoPixels, &view, &pixels) < 0) {
        return NULL;
    }
    glDrawPixels(width, height, format, type, pixels);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}

static PyObject * PyGL_ReadPixels(PyObject *self, PyObject *pyoArgs) {
    GLint      x, y;
    GLsizei    width, height;
    GLenum     format, type;
    Py_ssize_t size = -1;
    if(!PyArg_ParseTuple(pyoArgs, "iiiiII|n", &x, &y, &width, &height, &format, &type, &size)) {
        return NULL;
    }
    if(size < 0) {
        size = (Py_ssize_t)pygl_image_size(width, height, 1, format, type);
    }
    PyObject *pyoBytes = PyBytes_FromStringAndSize(NULL, size);
    if(NULL == pyoBytes) {
        return NULL;
    }
    glReadPixels(x, y, width, height, format, type, PyBytes_AS_STRING(pyoBytes));
    return pyoBytes;
}

static PyObject * PyGL_Bitmap(PyObject *self, PyObject *pyoArgs) {
    GLsizei width, height;
    GLfloat xorig, yorig, xmove, ymove;
    PyObject *pyoBitmap;
    if(!PyArg_ParseTuple(pyoArgs, "iiffffO", &width, &height, &xorig, &yorig,
                         &xmove, &ymove, &pyoBitmap)) {
        return NULL;
    }
    Py_buffer view;
    const void *bitmap;
    if(pygl_bufptr(pyoBitmap, &view, &bitmap) < 0) {
        return NULL;
    }
    glBitmap(width, height, xorig, yorig, xmove, ymove, bitmap);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}

static PyObject * PyGL_PolygonStipple(PyObject *self, PyObject *pyoArg) {
    Py_buffer view;
    if(PyObject_GetBuffer(pyoArg, &view, PyBUF_SIMPLE) < 0) {
        return NULL;
    }
    if(view.len < 128) {
        PyBuffer_Release(&view);
        PyErr_SetString(PyExc_ValueError, "stipple mask must be at least 128 bytes");
        return NULL;
    }
    glPolygonStipple(view.buf);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}

static PyObject * PyGL_GetPolygonStipple(PyObject *self, PyObject *pyo) {
    PyObject *pyoBytes = PyBytes_FromStringAndSize(NULL, 128);
    if(NULL == pyoBytes) {
        return NULL;
    }
    glGetPolygonStipple((GLubyte *)PyBytes_AS_STRING(pyoBytes));
    return pyoBytes;
}

// ****************************************************************************
// buffer mapping
// ****************************************************************************

static PyObject * PyGL_MapBuffer(PyObject *self, PyObject *pyoArgs) {
    GLenum target, access;
    if(!PyArg_ParseTuple(pyoArgs, "II", &target, &access)) {
        return NULL;
    }
    GLint size = 0;
    glGetBufferParameteriv(target, GL_BUFFER_SIZE, &size);
    void *ptr = glMapBuffer(target, access);
    if(NULL == ptr) {
        Py_RETURN_NONE;
    }
    int flags = (GL_READ_ONLY == access) ? PyBUF_READ : PyBUF_WRITE;
    return PyMemoryView_FromMemory(ptr, size, flags);
}

// ****************************************************************************
// indexed drawing
// ****************************************************************************

static PyObject * PyGL_DrawElements(PyObject *self, PyObject *pyoArgs) {
    GLenum    mode, type;
    GLsizei   count;
    PyObject *pyoIndices;
    if(!PyArg_ParseTuple(pyoArgs, "IiIO", &mode, &count, &type, &pyoIndices)) {
        return NULL;
    }
    Py_buffer view;
    const void *indices;
    if(pygl_bufptr(pyoIndices, &view, &indices) < 0) {
        return NULL;
    }
    glDrawElements(mode, count, type, indices);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}

static PyObject * PyGL_DrawRangeElements(PyObject *self, PyObject *pyoArgs) {
    GLenum    mode, type;
    GLuint    start, end;
    GLsizei   count;
    PyObject *pyoIndices;
    if(!PyArg_ParseTuple(pyoArgs, "IIIiIO", &mode, &start, &end, &count, &type, &pyoIndices)) {
        return NULL;
    }
    Py_buffer view;
    const void *indices;
    if(pygl_bufptr(pyoIndices, &view, &indices) < 0) {
        return NULL;
    }
    glDrawRangeElements(mode, start, end, count, type, indices);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}

static PyObject * PyGL_CallLists(PyObject *self, PyObject *pyoArgs) {
    GLenum    type;
    PyObject *pyoLists;
    if(!PyArg_ParseTuple(pyoArgs, "IO", &type, &pyoLists)) {
        return NULL;
    }
    Py_buffer view;
    if(PyObject_GetBuffer(pyoLists, &view, PyBUF_SIMPLE) < 0) {
        return NULL;
    }
    size_t elem = pygl_type_size(type);
    glCallLists((GLsizei)(view.len / (elem ? elem : 1)), type, view.buf);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}

// ****************************************************************************
// client vertex arrays  (retained until ReleaseArrays)
// ****************************************************************************

#define PYGL_ARRAY_STS(PY)                                                     \
static PyObject * PyGL_##PY(PyObject *self, PyObject *pyoArgs) {                \
    GLint     size;                                                            \
    GLenum    type;                                                            \
    GLsizei   stride;                                                          \
    PyObject *pyoPtr;                                                          \
    if(!PyArg_ParseTuple(pyoArgs, "iiiO", &size, &type, &stride, &pyoPtr)) {   \
        return NULL;                                                           \
    }                                                                         \
    Py_buffer view;                                                            \
    const void *ptr;                                                           \
    if(pygl_bufptr(pyoPtr, &view, &ptr) < 0) {                                 \
        return NULL;                                                           \
    }                                                                         \
    gl##PY(size, type, stride, ptr);                                           \
    if(pygl_retain(&view) < 0) {                                               \
        PyBuffer_Release(&view);                                               \
        return NULL;                                                           \
    }                                                                         \
    Py_RETURN_NONE;                                                            \
}

#define PYGL_ARRAY_TS(PY)                                                      \
static PyObject * PyGL_##PY(PyObject *self, PyObject *pyoArgs) {                \
    GLenum    type;                                                            \
    GLsizei   stride;                                                          \
    PyObject *pyoPtr;                                                          \
    if(!PyArg_ParseTuple(pyoArgs, "iiO", &type, &stride, &pyoPtr)) {           \
        return NULL;                                                           \
    }                                                                         \
    Py_buffer view;                                                            \
    const void *ptr;                                                           \
    if(pygl_bufptr(pyoPtr, &view, &ptr) < 0) {                                 \
        return NULL;                                                           \
    }                                                                         \
    gl##PY(type, stride, ptr);                                                 \
    if(pygl_retain(&view) < 0) {                                               \
        PyBuffer_Release(&view);                                               \
        return NULL;                                                           \
    }                                                                         \
    Py_RETURN_NONE;                                                            \
}

PYGL_ARRAY_STS( VertexPointer          )
PYGL_ARRAY_STS( ColorPointer           )
PYGL_ARRAY_STS( SecondaryColorPointer  )
PYGL_ARRAY_STS( TexCoordPointer        )
PYGL_ARRAY_TS ( NormalPointer          )
PYGL_ARRAY_TS ( IndexPointer           )
PYGL_ARRAY_TS ( FogCoordPointer        )

static PyObject * PyGL_EdgeFlagPointer(PyObject *self, PyObject *pyoArgs) {
    GLsizei   stride;
    PyObject *pyoPtr;
    if(!PyArg_ParseTuple(pyoArgs, "iO", &stride, &pyoPtr)) {
        return NULL;
    }
    Py_buffer view;
    const void *ptr;
    if(pygl_bufptr(pyoPtr, &view, &ptr) < 0) {
        return NULL;
    }
    glEdgeFlagPointer(stride, ptr);
    if(pygl_retain(&view) < 0) {
        PyBuffer_Release(&view);
        return NULL;
    }
    Py_RETURN_NONE;
}

static PyObject * PyGL_InterleavedArrays(PyObject *self, PyObject *pyoArgs) {
    GLenum    format;
    GLsizei   stride;
    PyObject *pyoPtr;
    if(!PyArg_ParseTuple(pyoArgs, "IiO", &format, &stride, &pyoPtr)) {
        return NULL;
    }
    Py_buffer view;
    const void *ptr;
    if(pygl_bufptr(pyoPtr, &view, &ptr) < 0) {
        return NULL;
    }
    glInterleavedArrays(format, stride, ptr);
    if(pygl_retain(&view) < 0) {
        PyBuffer_Release(&view);
        return NULL;
    }
    Py_RETURN_NONE;
}

static PyObject * PyGL_VertexAttribPointer(PyObject *self, PyObject *pyoArgs) {
    GLuint    index;
    GLint     size;
    GLenum    type;
    GLboolean normalized;
    GLsizei   stride;
    PyObject *pyoPtr;
    if(!PyArg_ParseTuple(pyoArgs, "IiIbiO", &index, &size, &type, &normalized,
                         &stride, &pyoPtr)) {
        return NULL;
    }
    Py_buffer view;
    const void *ptr;
    if(pygl_bufptr(pyoPtr, &view, &ptr) < 0) {
        return NULL;
    }
    glVertexAttribPointer(index, size, type, normalized, stride, ptr);
    if(pygl_retain(&view) < 0) {
        PyBuffer_Release(&view);
        return NULL;
    }
    Py_RETURN_NONE;
}

// ****************************************************************************

static PyMethodDef PyGL_data_methods[] = {
    { "ReleaseArrays",             PyGL_ReleaseArrays,           METH_NOARGS,  "ReleaseArrays()" },

    { "Bitmap",                    PyGL_Bitmap,                  METH_VARARGS, "Bitmap(width, height, xorig, yorig, xmove, ymove, bitmap)" },
    { "PolygonStipple",            PyGL_PolygonStipple,          METH_O,       "PolygonStipple(mask)" },
    { "GetPolygonStipple",         PyGL_GetPolygonStipple,       METH_NOARGS,  "GetPolygonStipple()" },
    { "MapBuffer",                 PyGL_MapBuffer,               METH_VARARGS, "MapBuffer(target, access)" },

    { "BufferData",                PyGL_BufferData,              METH_VARARGS, "BufferData(target, data_or_size, usage)" },
    { "BufferSubData",             PyGL_BufferSubData,           METH_VARARGS, "BufferSubData(target, offset, data)" },
    { "GetBufferSubData",          PyGL_GetBufferSubData,        METH_VARARGS, "GetBufferSubData(target, offset, size)" },

    { "TexImage1D",                PyGL_TexImage1D,              METH_VARARGS, "TexImage1D(target, level, internalformat, width, border, format, type, pixels)" },
    { "TexImage2D",                PyGL_TexImage2D,              METH_VARARGS, "TexImage2D(target, level, internalformat, width, height, border, format, type, pixels)" },
    { "TexImage3D",                PyGL_TexImage3D,              METH_VARARGS, "TexImage3D(target, level, internalformat, width, height, depth, border, format, type, pixels)" },
    { "TexSubImage1D",             PyGL_TexSubImage1D,           METH_VARARGS, "TexSubImage1D(target, level, xoffset, width, format, type, pixels)" },
    { "TexSubImage2D",             PyGL_TexSubImage2D,           METH_VARARGS, "TexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels)" },
    { "TexSubImage3D",             PyGL_TexSubImage3D,           METH_VARARGS, "TexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels)" },
    { "GetTexImage",               PyGL_GetTexImage,             METH_VARARGS, "GetTexImage(target, level, format, type, size)" },

    { "CompressedTexImage1D",      PyGL_CompressedTexImage1D,    METH_VARARGS, "CompressedTexImage1D(target, level, internalformat, width, border, data)" },
    { "CompressedTexImage2D",      PyGL_CompressedTexImage2D,    METH_VARARGS, "CompressedTexImage2D(target, level, internalformat, width, height, border, data)" },
    { "CompressedTexImage3D",      PyGL_CompressedTexImage3D,    METH_VARARGS, "CompressedTexImage3D(target, level, internalformat, width, height, depth, border, data)" },
    { "CompressedTexSubImage1D",   PyGL_CompressedTexSubImage1D, METH_VARARGS, "CompressedTexSubImage1D(target, level, xoffset, width, format, data)" },
    { "CompressedTexSubImage2D",   PyGL_CompressedTexSubImage2D, METH_VARARGS, "CompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, data)" },
    { "CompressedTexSubImage3D",   PyGL_CompressedTexSubImage3D, METH_VARARGS, "CompressedTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, data)" },
    { "GetCompressedTexImage",     PyGL_GetCompressedTexImage,   METH_VARARGS, "GetCompressedTexImage(target, level, size)" },

    { "DrawPixels",                PyGL_DrawPixels,              METH_VARARGS, "DrawPixels(width, height, format, type, pixels)" },
    { "ReadPixels",                PyGL_ReadPixels,              METH_VARARGS, "ReadPixels(x, y, width, height, format, type, size=-1)" },

    { "DrawElements",              PyGL_DrawElements,            METH_VARARGS, "DrawElements(mode, count, type, indices)" },
    { "DrawRangeElements",         PyGL_DrawRangeElements,       METH_VARARGS, "DrawRangeElements(mode, start, end, count, type, indices)" },
    { "CallLists",                 PyGL_CallLists,               METH_VARARGS, "CallLists(type, lists)" },

    { "VertexPointer",             PyGL_VertexPointer,           METH_VARARGS, "VertexPointer(size, type, stride, pointer)" },
    { "ColorPointer",              PyGL_ColorPointer,            METH_VARARGS, "ColorPointer(size, type, stride, pointer)" },
    { "SecondaryColorPointer",     PyGL_SecondaryColorPointer,   METH_VARARGS, "SecondaryColorPointer(size, type, stride, pointer)" },
    { "TexCoordPointer",           PyGL_TexCoordPointer,         METH_VARARGS, "TexCoordPointer(size, type, stride, pointer)" },
    { "NormalPointer",             PyGL_NormalPointer,           METH_VARARGS, "NormalPointer(type, stride, pointer)" },
    { "IndexPointer",              PyGL_IndexPointer,            METH_VARARGS, "IndexPointer(type, stride, pointer)" },
    { "FogCoordPointer",           PyGL_FogCoordPointer,         METH_VARARGS, "FogCoordPointer(type, stride, pointer)" },
    { "EdgeFlagPointer",           PyGL_EdgeFlagPointer,         METH_VARARGS, "EdgeFlagPointer(stride, pointer)" },
    { "InterleavedArrays",         PyGL_InterleavedArrays,       METH_VARARGS, "InterleavedArrays(format, stride, pointer)" },
    { "VertexAttribPointer",       PyGL_VertexAttribPointer,     METH_VARARGS, "VertexAttribPointer(index, size, type, normalized, stride, pointer)" },

    { NULL }
};
