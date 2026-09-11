#include "pygl.h"

//
// GL 3.0 - 4.6 core entry points that cannot be generated mechanically: sync
// objects, the direct-state-access buffer / texture calls, the string and
// variable-length-array program introspection, indexed / instanced / indirect
// drawing, the debug-output log and callback, and the 64-bit / unsigned
// uniform-array setters.
//
// Everything past GL 1.1 reaches the build through GLEW, which #defines each
// such entry point as a macro, so every wrapper and its method-table row is
// wrapped in "#ifdef gl<Name>". A GLEW too old to know a call simply yields a
// smaller module.
//
// This unit is #included after data.c and shaders.c, so it reuses their file
// -local helpers: pygl_bufptr()/pygl_retain() (data.c) and the PYGL_UNIFORM_V /
// PYGL_UNIFORM_MAT macros (shaders.c).
//

// ****************************************************************************
// sync objects  (a GLsync is passed to Python as the integer value of its
// pointer, the same convention glu.c uses for a GLUquadric)
// ****************************************************************************

#ifdef glFenceSync
static PyObject * PyGL_FenceSync(PyObject *self, PyObject *pyoArgs) {
    GLenum     condition;
    GLbitfield flags;
    if(!PyArg_ParseTuple(pyoArgs, "II", &condition, &flags)) {
        return NULL;
    }
    GLsync sync = glFenceSync(condition, flags);
    return PyLong_FromVoidPtr((void *)sync);
}
#endif

#ifdef glDeleteSync
static PyObject * PyGL_DeleteSync(PyObject *self, PyObject *pyoArg) {
    GLsync sync = (GLsync)PyLong_AsVoidPtr(pyoArg);
    if(PyErr_Occurred()) {
        return NULL;
    }
    glDeleteSync(sync);
    Py_RETURN_NONE;
}
#endif

#ifdef glClientWaitSync
static PyObject * PyGL_ClientWaitSync(PyObject *self, PyObject *pyoArgs) {
    PyObject      *pyoSync;
    GLbitfield     flags;
    unsigned long long timeout;
    if(!PyArg_ParseTuple(pyoArgs, "OIK", &pyoSync, &flags, &timeout)) {
        return NULL;
    }
    GLsync sync = (GLsync)PyLong_AsVoidPtr(pyoSync);
    if(PyErr_Occurred()) {
        return NULL;
    }
    GLenum result = glClientWaitSync(sync, flags, (GLuint64)timeout);
    return Py_BuildValue("I", result);
}
#endif

#ifdef glWaitSync
static PyObject * PyGL_WaitSync(PyObject *self, PyObject *pyoArgs) {
    PyObject      *pyoSync;
    GLbitfield     flags;
    unsigned long long timeout;
    if(!PyArg_ParseTuple(pyoArgs, "OIK", &pyoSync, &flags, &timeout)) {
        return NULL;
    }
    GLsync sync = (GLsync)PyLong_AsVoidPtr(pyoSync);
    if(PyErr_Occurred()) {
        return NULL;
    }
    glWaitSync(sync, flags, (GLuint64)timeout);
    Py_RETURN_NONE;
}
#endif

#ifdef glIsSync
static PyObject * PyGL_IsSync(PyObject *self, PyObject *pyoArg) {
    GLsync sync = (GLsync)PyLong_AsVoidPtr(pyoArg);
    if(PyErr_Occurred()) {
        return NULL;
    }
    if(glIsSync(sync)) { Py_RETURN_TRUE; } else { Py_RETURN_FALSE; }
}
#endif

#ifdef glGetSynciv
static PyObject * PyGL_GetSynciv(PyObject *self, PyObject *pyoArgs) {
    PyObject  *pyoSync;
    GLenum     pname;
    Py_ssize_t count = 1;
    if(!PyArg_ParseTuple(pyoArgs, "OI|n", &pyoSync, &pname, &count)) {
        return NULL;
    }
    GLsync sync = (GLsync)PyLong_AsVoidPtr(pyoSync);
    if(PyErr_Occurred()) {
        return NULL;
    }
    if(count < 1 || count > PYGL_MAX_QUERY) {
        PyErr_SetString(PyExc_ValueError, "invalid count");
        return NULL;
    }
    GLint   values[PYGL_MAX_QUERY];
    GLsizei written = 0;
    glGetSynciv(sync, pname, (GLsizei)count, &written, values);
    return pygl_build_ints(values, written ? written : count);
}
#endif

// ****************************************************************************
// buffer storage and direct-state-access buffer data
// ****************************************************************************

// (target, data-or-size, flags)
#ifdef glBufferStorage
static PyObject * PyGL_BufferStorage(PyObject *self, PyObject *pyoArgs) {
    GLenum     target;
    PyObject  *pyoData;
    GLbitfield flags;
    if(!PyArg_ParseTuple(pyoArgs, "IOI", &target, &pyoData, &flags)) {
        return NULL;
    }
    if(PyLong_Check(pyoData)) {
        GLsizeiptr size = (GLsizeiptr)PyLong_AsSsize_t(pyoData);
        if(PyErr_Occurred()) {
            return NULL;
        }
        glBufferStorage(target, size, NULL, flags);
        Py_RETURN_NONE;
    }
    Py_buffer view;
    if(PyObject_GetBuffer(pyoData, &view, PyBUF_SIMPLE) < 0) {
        return NULL;
    }
    glBufferStorage(target, (GLsizeiptr)view.len, view.buf, flags);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}
#endif

#ifdef glNamedBufferStorage
static PyObject * PyGL_NamedBufferStorage(PyObject *self, PyObject *pyoArgs) {
    GLuint     buffer;
    PyObject  *pyoData;
    GLbitfield flags;
    if(!PyArg_ParseTuple(pyoArgs, "IOI", &buffer, &pyoData, &flags)) {
        return NULL;
    }
    if(PyLong_Check(pyoData)) {
        GLsizeiptr size = (GLsizeiptr)PyLong_AsSsize_t(pyoData);
        if(PyErr_Occurred()) {
            return NULL;
        }
        glNamedBufferStorage(buffer, size, NULL, flags);
        Py_RETURN_NONE;
    }
    Py_buffer view;
    if(PyObject_GetBuffer(pyoData, &view, PyBUF_SIMPLE) < 0) {
        return NULL;
    }
    glNamedBufferStorage(buffer, (GLsizeiptr)view.len, view.buf, flags);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}
#endif

#ifdef glNamedBufferData
static PyObject * PyGL_NamedBufferData(PyObject *self, PyObject *pyoArgs) {
    GLuint    buffer;
    PyObject *pyoData;
    GLenum    usage;
    if(!PyArg_ParseTuple(pyoArgs, "IOI", &buffer, &pyoData, &usage)) {
        return NULL;
    }
    if(PyLong_Check(pyoData)) {
        GLsizeiptr size = (GLsizeiptr)PyLong_AsSsize_t(pyoData);
        if(PyErr_Occurred()) {
            return NULL;
        }
        glNamedBufferData(buffer, size, NULL, usage);
        Py_RETURN_NONE;
    }
    Py_buffer view;
    if(PyObject_GetBuffer(pyoData, &view, PyBUF_SIMPLE) < 0) {
        return NULL;
    }
    glNamedBufferData(buffer, (GLsizeiptr)view.len, view.buf, usage);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}
#endif

#ifdef glNamedBufferSubData
static PyObject * PyGL_NamedBufferSubData(PyObject *self, PyObject *pyoArgs) {
    GLuint     buffer;
    Py_ssize_t offset;
    PyObject  *pyoData;
    if(!PyArg_ParseTuple(pyoArgs, "InO", &buffer, &offset, &pyoData)) {
        return NULL;
    }
    Py_buffer view;
    if(PyObject_GetBuffer(pyoData, &view, PyBUF_SIMPLE) < 0) {
        return NULL;
    }
    glNamedBufferSubData(buffer, (GLintptr)offset, (GLsizeiptr)view.len, view.buf);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}
#endif

#ifdef glGetNamedBufferSubData
static PyObject * PyGL_GetNamedBufferSubData(PyObject *self, PyObject *pyoArgs) {
    GLuint     buffer;
    Py_ssize_t offset;
    Py_ssize_t size;
    if(!PyArg_ParseTuple(pyoArgs, "Inn", &buffer, &offset, &size)) {
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
    glGetNamedBufferSubData(buffer, (GLintptr)offset, (GLsizeiptr)size, PyBytes_AS_STRING(pyoBytes));
    return pyoBytes;
}
#endif

// ****************************************************************************
// buffer mapping  (returns a memoryview over the mapped range)
// ****************************************************************************

#ifdef glMapBufferRange
static PyObject * PyGL_MapBufferRange(PyObject *self, PyObject *pyoArgs) {
    GLenum     target;
    Py_ssize_t offset;
    Py_ssize_t length;
    GLbitfield access;
    if(!PyArg_ParseTuple(pyoArgs, "InnI", &target, &offset, &length, &access)) {
        return NULL;
    }
    void *ptr = glMapBufferRange(target, (GLintptr)offset, (GLsizeiptr)length, access);
    if(NULL == ptr) {
        Py_RETURN_NONE;
    }
    int flags = (access & GL_MAP_WRITE_BIT) ? PyBUF_WRITE : PyBUF_READ;
    return PyMemoryView_FromMemory(ptr, length, flags);
}
#endif

#ifdef glMapNamedBuffer
static PyObject * PyGL_MapNamedBuffer(PyObject *self, PyObject *pyoArgs) {
    GLuint buffer;
    GLenum access;
    if(!PyArg_ParseTuple(pyoArgs, "II", &buffer, &access)) {
        return NULL;
    }
    GLint size = 0;
    glGetNamedBufferParameteriv(buffer, GL_BUFFER_SIZE, &size);
    void *ptr = glMapNamedBuffer(buffer, access);
    if(NULL == ptr) {
        Py_RETURN_NONE;
    }
    int flags = (GL_READ_ONLY == access) ? PyBUF_READ : PyBUF_WRITE;
    return PyMemoryView_FromMemory(ptr, size, flags);
}
#endif

#ifdef glMapNamedBufferRange
static PyObject * PyGL_MapNamedBufferRange(PyObject *self, PyObject *pyoArgs) {
    GLuint     buffer;
    Py_ssize_t offset;
    Py_ssize_t length;
    GLbitfield access;
    if(!PyArg_ParseTuple(pyoArgs, "InnI", &buffer, &offset, &length, &access)) {
        return NULL;
    }
    void *ptr = glMapNamedBufferRange(buffer, (GLintptr)offset, (GLsizeiptr)length, access);
    if(NULL == ptr) {
        Py_RETURN_NONE;
    }
    int flags = (access & GL_MAP_WRITE_BIT) ? PyBUF_WRITE : PyBUF_READ;
    return PyMemoryView_FromMemory(ptr, length, flags);
}
#endif

// ****************************************************************************
// integer / long vertex-attribute arrays  (retained until ReleaseArrays)
// ****************************************************************************

#define PYGL_ATTRIB_POINTER(PY)                                                \
static PyObject * PyGL_##PY(PyObject *self, PyObject *pyoArgs) {                \
    GLuint    index;                                                           \
    GLint     size;                                                            \
    GLenum    type;                                                            \
    GLsizei   stride;                                                          \
    PyObject *pyoPtr;                                                          \
    if(!PyArg_ParseTuple(pyoArgs, "IiIiO", &index, &size, &type, &stride, &pyoPtr)) { \
        return NULL;                                                           \
    }                                                                         \
    Py_buffer view;                                                            \
    const void *ptr;                                                           \
    if(pygl_bufptr(pyoPtr, &view, &ptr) < 0) {                                 \
        return NULL;                                                           \
    }                                                                         \
    gl##PY(index, size, type, stride, ptr);                                    \
    if(pygl_retain(&view) < 0) {                                               \
        PyBuffer_Release(&view);                                               \
        return NULL;                                                           \
    }                                                                         \
    Py_RETURN_NONE;                                                            \
}

#ifdef glVertexAttribIPointer
PYGL_ATTRIB_POINTER( VertexAttribIPointer )
#endif
#ifdef glVertexAttribLPointer
PYGL_ATTRIB_POINTER( VertexAttribLPointer )
#endif

// ****************************************************************************
// base-vertex / instanced / indirect drawing  (indices consumed immediately)
// ****************************************************************************

#ifdef glDrawElementsBaseVertex
static PyObject * PyGL_DrawElementsBaseVertex(PyObject *self, PyObject *pyoArgs) {
    GLenum    mode, type;
    GLsizei   count;
    GLint     basevertex;
    PyObject *pyoIndices;
    if(!PyArg_ParseTuple(pyoArgs, "IiIOi", &mode, &count, &type, &pyoIndices, &basevertex)) {
        return NULL;
    }
    Py_buffer view;
    const void *indices;
    if(pygl_bufptr(pyoIndices, &view, &indices) < 0) {
        return NULL;
    }
    glDrawElementsBaseVertex(mode, count, type, (void *)indices, basevertex);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}
#endif

#ifdef glDrawRangeElementsBaseVertex
static PyObject * PyGL_DrawRangeElementsBaseVertex(PyObject *self, PyObject *pyoArgs) {
    GLenum    mode, type;
    GLuint    start, end;
    GLsizei   count;
    GLint     basevertex;
    PyObject *pyoIndices;
    if(!PyArg_ParseTuple(pyoArgs, "IIIiIOi", &mode, &start, &end, &count, &type,
                         &pyoIndices, &basevertex)) {
        return NULL;
    }
    Py_buffer view;
    const void *indices;
    if(pygl_bufptr(pyoIndices, &view, &indices) < 0) {
        return NULL;
    }
    glDrawRangeElementsBaseVertex(mode, start, end, count, type, (void *)indices, basevertex);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}
#endif

#ifdef glDrawElementsInstanced
static PyObject * PyGL_DrawElementsInstanced(PyObject *self, PyObject *pyoArgs) {
    GLenum    mode, type;
    GLsizei   count, instancecount;
    PyObject *pyoIndices;
    if(!PyArg_ParseTuple(pyoArgs, "IiIOi", &mode, &count, &type, &pyoIndices, &instancecount)) {
        return NULL;
    }
    Py_buffer view;
    const void *indices;
    if(pygl_bufptr(pyoIndices, &view, &indices) < 0) {
        return NULL;
    }
    glDrawElementsInstanced(mode, count, type, indices, instancecount);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}
#endif

#ifdef glDrawElementsInstancedBaseVertex
static PyObject * PyGL_DrawElementsInstancedBaseVertex(PyObject *self, PyObject *pyoArgs) {
    GLenum    mode, type;
    GLsizei   count, instancecount;
    GLint     basevertex;
    PyObject *pyoIndices;
    if(!PyArg_ParseTuple(pyoArgs, "IiIOii", &mode, &count, &type, &pyoIndices,
                         &instancecount, &basevertex)) {
        return NULL;
    }
    Py_buffer view;
    const void *indices;
    if(pygl_bufptr(pyoIndices, &view, &indices) < 0) {
        return NULL;
    }
    glDrawElementsInstancedBaseVertex(mode, count, type, (void *)indices, instancecount, basevertex);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}
#endif

#ifdef glDrawElementsInstancedBaseInstance
static PyObject * PyGL_DrawElementsInstancedBaseInstance(PyObject *self, PyObject *pyoArgs) {
    GLenum    mode, type;
    GLsizei   count, instancecount;
    GLuint    baseinstance;
    PyObject *pyoIndices;
    if(!PyArg_ParseTuple(pyoArgs, "IiIOiI", &mode, &count, &type, &pyoIndices,
                         &instancecount, &baseinstance)) {
        return NULL;
    }
    Py_buffer view;
    const void *indices;
    if(pygl_bufptr(pyoIndices, &view, &indices) < 0) {
        return NULL;
    }
    glDrawElementsInstancedBaseInstance(mode, count, type, indices, instancecount, baseinstance);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}
#endif

#ifdef glDrawElementsInstancedBaseVertexBaseInstance
static PyObject * PyGL_DrawElementsInstancedBaseVertexBaseInstance(PyObject *self, PyObject *pyoArgs) {
    GLenum    mode, type;
    GLsizei   count, instancecount;
    GLint     basevertex;
    GLuint    baseinstance;
    PyObject *pyoIndices;
    if(!PyArg_ParseTuple(pyoArgs, "IiIOiiI", &mode, &count, &type, &pyoIndices,
                         &instancecount, &basevertex, &baseinstance)) {
        return NULL;
    }
    Py_buffer view;
    const void *indices;
    if(pygl_bufptr(pyoIndices, &view, &indices) < 0) {
        return NULL;
    }
    glDrawElementsInstancedBaseVertexBaseInstance(mode, count, type, (void *)indices,
                                                  instancecount, basevertex, baseinstance);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}
#endif

#ifdef glDrawArraysIndirect
static PyObject * PyGL_DrawArraysIndirect(PyObject *self, PyObject *pyoArgs) {
    GLenum    mode;
    PyObject *pyoIndirect;
    if(!PyArg_ParseTuple(pyoArgs, "IO", &mode, &pyoIndirect)) {
        return NULL;
    }
    Py_buffer view;
    const void *indirect;
    if(pygl_bufptr(pyoIndirect, &view, &indirect) < 0) {
        return NULL;
    }
    glDrawArraysIndirect(mode, indirect);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}
#endif

#ifdef glDrawElementsIndirect
static PyObject * PyGL_DrawElementsIndirect(PyObject *self, PyObject *pyoArgs) {
    GLenum    mode, type;
    PyObject *pyoIndirect;
    if(!PyArg_ParseTuple(pyoArgs, "IIO", &mode, &type, &pyoIndirect)) {
        return NULL;
    }
    Py_buffer view;
    const void *indirect;
    if(pygl_bufptr(pyoIndirect, &view, &indirect) < 0) {
        return NULL;
    }
    glDrawElementsIndirect(mode, type, indirect);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}
#endif

#ifdef glMultiDrawArrays
static PyObject * PyGL_MultiDrawArrays(PyObject *self, PyObject *pyoArgs) {
    GLenum    mode;
    PyObject *pyoFirst, *pyoCount;
    if(!PyArg_ParseTuple(pyoArgs, "IOO", &mode, &pyoFirst, &pyoCount)) {
        return NULL;
    }
    Py_ssize_t nf, nc;
    GLint *first = pygl_alloc_ints(pyoFirst, &nf);
    if(NULL == first) {
        return NULL;
    }
    GLint *count = pygl_alloc_ints(pyoCount, &nc);
    if(NULL == count) {
        PyMem_Free(first);
        return NULL;
    }
    if(nf != nc) {
        PyMem_Free(first);
        PyMem_Free(count);
        PyErr_SetString(PyExc_ValueError, "first and count differ in length");
        return NULL;
    }
    glMultiDrawArrays(mode, first, (const GLsizei *)count, (GLsizei)nf);
    PyMem_Free(first);
    PyMem_Free(count);
    Py_RETURN_NONE;
}
#endif

#ifdef glMultiDrawElements
static PyObject * PyGL_MultiDrawElements(PyObject *self, PyObject *pyoArgs) {
    GLenum    mode, type;
    PyObject *pyoCount, *pyoIndices;
    if(!PyArg_ParseTuple(pyoArgs, "IOIO", &mode, &pyoCount, &type, &pyoIndices)) {
        return NULL;
    }
    Py_ssize_t nc;
    GLint *count = pygl_alloc_ints(pyoCount, &nc);
    if(NULL == count) {
        return NULL;
    }
    PyObject *fast = PySequence_Fast(pyoIndices, "expected a sequence of offsets");
    if(NULL == fast) {
        PyMem_Free(count);
        return NULL;
    }
    Py_ssize_t ni = PySequence_Fast_GET_SIZE(fast);
    const void **indices = PyMem_Malloc(sizeof(void *) * (ni > 0 ? ni : 1));
    if(NULL == indices) {
        Py_DECREF(fast);
        PyMem_Free(count);
        return PyErr_NoMemory();
    }
    for(Py_ssize_t idx = 0; idx < ni; ++idx) {
        indices[idx] = (const void *)(intptr_t)PyLong_AsSsize_t(PySequence_Fast_GET_ITEM(fast, idx));
    }
    Py_DECREF(fast);
    if(PyErr_Occurred() || nc != ni) {
        PyMem_Free(indices);
        PyMem_Free(count);
        if(!PyErr_Occurred()) {
            PyErr_SetString(PyExc_ValueError, "count and indices differ in length");
        }
        return NULL;
    }
    glMultiDrawElements(mode, (const GLsizei *)count, type, indices, (GLsizei)ni);
    PyMem_Free(indices);
    PyMem_Free(count);
    Py_RETURN_NONE;
}
#endif

// ****************************************************************************
// transform feedback varyings
// ****************************************************************************

#ifdef glTransformFeedbackVaryings
static PyObject * PyGL_TransformFeedbackVaryings(PyObject *self, PyObject *pyoArgs) {
    GLuint    program;
    PyObject *pyoVaryings;
    GLenum    bufferMode;
    if(!PyArg_ParseTuple(pyoArgs, "IOI", &program, &pyoVaryings, &bufferMode)) {
        return NULL;
    }
    PyObject *fast = PySequence_Fast(pyoVaryings, "expected a sequence of str");
    if(NULL == fast) {
        return NULL;
    }
    Py_ssize_t n = PySequence_Fast_GET_SIZE(fast);
    const char **names = PyMem_Malloc(sizeof(char *) * (n > 0 ? n : 1));
    if(NULL == names) {
        Py_DECREF(fast);
        return PyErr_NoMemory();
    }
    for(Py_ssize_t idx = 0; idx < n; ++idx) {
        names[idx] = PyUnicode_AsUTF8(PySequence_Fast_GET_ITEM(fast, idx));
        if(NULL == names[idx]) {
            PyMem_Free(names);
            Py_DECREF(fast);
            return NULL;
        }
    }
    glTransformFeedbackVaryings(program, (GLsizei)n, names, bufferMode);
    PyMem_Free(names);
    Py_DECREF(fast);
    Py_RETURN_NONE;
}
#endif

#ifdef glGetTransformFeedbackVarying
static PyObject * PyGL_GetTransformFeedbackVarying(PyObject *self, PyObject *pyoArgs) {
    GLuint program, index;
    if(!PyArg_ParseTuple(pyoArgs, "II", &program, &index)) {
        return NULL;
    }
    GLint maxlen = 0;
    glGetProgramiv(program, GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH, &maxlen);
    if(maxlen <= 0) {
        maxlen = 256;
    }
    PyObject *buf = PyBytes_FromStringAndSize(NULL, maxlen);
    if(NULL == buf) {
        return NULL;
    }
    GLsizei written = 0, size = 0;
    GLenum  type = 0;
    glGetTransformFeedbackVarying(program, index, maxlen, &written, &size, &type,
                                 PyBytes_AS_STRING(buf));
    PyObject *name = PyUnicode_FromStringAndSize(PyBytes_AS_STRING(buf), written);
    Py_DECREF(buf);
    if(NULL == name) {
        return NULL;
    }
    return Py_BuildValue("(NiI)", name, size, type);
}
#endif

// ****************************************************************************
// fragment-data and uniform-block introspection
// ****************************************************************************

#ifdef glBindFragDataLocation
static PyObject * PyGL_BindFragDataLocation(PyObject *self, PyObject *pyoArgs) {
    GLuint      program, color;
    const char *name;
    if(!PyArg_ParseTuple(pyoArgs, "IIs", &program, &color, &name)) {
        return NULL;
    }
    glBindFragDataLocation(program, color, name);
    Py_RETURN_NONE;
}
#endif

#ifdef glBindFragDataLocationIndexed
static PyObject * PyGL_BindFragDataLocationIndexed(PyObject *self, PyObject *pyoArgs) {
    GLuint      program, colorNumber, index;
    const char *name;
    if(!PyArg_ParseTuple(pyoArgs, "IIIs", &program, &colorNumber, &index, &name)) {
        return NULL;
    }
    glBindFragDataLocationIndexed(program, colorNumber, index, name);
    Py_RETURN_NONE;
}
#endif

#ifdef glGetFragDataLocation
static PyObject * PyGL_GetFragDataLocation(PyObject *self, PyObject *pyoArgs) {
    GLuint      program;
    const char *name;
    if(!PyArg_ParseTuple(pyoArgs, "Is", &program, &name)) {
        return NULL;
    }
    return Py_BuildValue("i", glGetFragDataLocation(program, name));
}
#endif

#ifdef glGetFragDataIndex
static PyObject * PyGL_GetFragDataIndex(PyObject *self, PyObject *pyoArgs) {
    GLuint      program;
    const char *name;
    if(!PyArg_ParseTuple(pyoArgs, "Is", &program, &name)) {
        return NULL;
    }
    return Py_BuildValue("i", glGetFragDataIndex(program, name));
}
#endif

#ifdef glGetUniformBlockIndex
static PyObject * PyGL_GetUniformBlockIndex(PyObject *self, PyObject *pyoArgs) {
    GLuint      program;
    const char *name;
    if(!PyArg_ParseTuple(pyoArgs, "Is", &program, &name)) {
        return NULL;
    }
    return Py_BuildValue("I", glGetUniformBlockIndex(program, name));
}
#endif

#ifdef glGetActiveUniformBlockName
static PyObject * PyGL_GetActiveUniformBlockName(PyObject *self, PyObject *pyoArgs) {
    GLuint program, index;
    if(!PyArg_ParseTuple(pyoArgs, "II", &program, &index)) {
        return NULL;
    }
    GLint maxlen = 0;
    glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &maxlen);
    if(maxlen <= 0) {
        maxlen = 256;
    }
    PyObject *buf = PyBytes_FromStringAndSize(NULL, maxlen);
    if(NULL == buf) {
        return NULL;
    }
    GLsizei written = 0;
    glGetActiveUniformBlockName(program, index, maxlen, &written, PyBytes_AS_STRING(buf));
    PyObject *name = PyUnicode_FromStringAndSize(PyBytes_AS_STRING(buf), written);
    Py_DECREF(buf);
    return name;
}
#endif

#ifdef glGetActiveUniformBlockiv
static PyObject * PyGL_GetActiveUniformBlockiv(PyObject *self, PyObject *pyoArgs) {
    GLuint     program, index;
    GLenum     pname;
    Py_ssize_t count = 1;
    if(!PyArg_ParseTuple(pyoArgs, "III|n", &program, &index, &pname, &count)) {
        return NULL;
    }
    if(count < 1 || count > PYGL_MAX_QUERY) {
        PyErr_SetString(PyExc_ValueError, "invalid count");
        return NULL;
    }
    GLint values[PYGL_MAX_QUERY];
    glGetActiveUniformBlockiv(program, index, pname, values);
    return pygl_build_ints(values, count);
}
#endif

#ifdef glGetActiveUniformName
static PyObject * PyGL_GetActiveUniformName(PyObject *self, PyObject *pyoArgs) {
    GLuint program, index;
    if(!PyArg_ParseTuple(pyoArgs, "II", &program, &index)) {
        return NULL;
    }
    GLint maxlen = 0;
    glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxlen);
    if(maxlen <= 0) {
        maxlen = 256;
    }
    PyObject *buf = PyBytes_FromStringAndSize(NULL, maxlen);
    if(NULL == buf) {
        return NULL;
    }
    GLsizei written = 0;
    glGetActiveUniformName(program, index, maxlen, &written, PyBytes_AS_STRING(buf));
    PyObject *name = PyUnicode_FromStringAndSize(PyBytes_AS_STRING(buf), written);
    Py_DECREF(buf);
    return name;
}
#endif

#ifdef glGetUniformIndices
static PyObject * PyGL_GetUniformIndices(PyObject *self, PyObject *pyoArgs) {
    GLuint    program;
    PyObject *pyoNames;
    if(!PyArg_ParseTuple(pyoArgs, "IO", &program, &pyoNames)) {
        return NULL;
    }
    PyObject *fast = PySequence_Fast(pyoNames, "expected a sequence of str");
    if(NULL == fast) {
        return NULL;
    }
    Py_ssize_t n = PySequence_Fast_GET_SIZE(fast);
    const char **names = PyMem_Malloc(sizeof(char *) * (n > 0 ? n : 1));
    GLuint      *out   = PyMem_Malloc(sizeof(GLuint)  * (n > 0 ? n : 1));
    if(NULL == names || NULL == out) {
        PyMem_Free(names);
        PyMem_Free(out);
        Py_DECREF(fast);
        return PyErr_NoMemory();
    }
    for(Py_ssize_t idx = 0; idx < n; ++idx) {
        names[idx] = PyUnicode_AsUTF8(PySequence_Fast_GET_ITEM(fast, idx));
        if(NULL == names[idx]) {
            PyMem_Free(names);
            PyMem_Free(out);
            Py_DECREF(fast);
            return NULL;
        }
    }
    glGetUniformIndices(program, (GLsizei)n, names, out);
    PyObject *list = pygl_list_uints(out, n);
    PyMem_Free(names);
    PyMem_Free(out);
    Py_DECREF(fast);
    return list;
}
#endif

#ifdef glGetActiveUniformsiv
static PyObject * PyGL_GetActiveUniformsiv(PyObject *self, PyObject *pyoArgs) {
    GLuint    program;
    PyObject *pyoIndices;
    GLenum    pname;
    if(!PyArg_ParseTuple(pyoArgs, "IOI", &program, &pyoIndices, &pname)) {
        return NULL;
    }
    Py_ssize_t n;
    GLuint *indices = pygl_alloc_uints(pyoIndices, &n);
    if(NULL == indices) {
        return NULL;
    }
    GLint *out = PyMem_Malloc(sizeof(GLint) * (n > 0 ? n : 1));
    if(NULL == out) {
        PyMem_Free(indices);
        return PyErr_NoMemory();
    }
    glGetActiveUniformsiv(program, (GLsizei)n, indices, pname, out);
    PyObject *list = pygl_build_ints(out, n ? n : 1);
    PyMem_Free(indices);
    PyMem_Free(out);
    return list;
}
#endif

// ****************************************************************************
// program binary and separable programs
// ****************************************************************************

#ifdef glGetProgramBinary
static PyObject * PyGL_GetProgramBinary(PyObject *self, PyObject *pyoArg) {
    GLuint program = (GLuint)PyLong_AsUnsignedLong(pyoArg);
    if(PyErr_Occurred()) {
        return NULL;
    }
    GLint length = 0;
    glGetProgramiv(program, GL_PROGRAM_BINARY_LENGTH, &length);
    PyObject *buf = PyBytes_FromStringAndSize(NULL, length > 0 ? length : 0);
    if(NULL == buf) {
        return NULL;
    }
    GLsizei written = 0;
    GLenum  format  = 0;
    glGetProgramBinary(program, length, &written, &format, PyBytes_AS_STRING(buf));
    if(written != length) {
        _PyBytes_Resize(&buf, written);
        if(NULL == buf) {
            return NULL;
        }
    }
    return Py_BuildValue("(IN)", format, buf);
}
#endif

#ifdef glProgramBinary
static PyObject * PyGL_ProgramBinary(PyObject *self, PyObject *pyoArgs) {
    GLuint    program;
    GLenum    format;
    PyObject *pyoBinary;
    if(!PyArg_ParseTuple(pyoArgs, "IIO", &program, &format, &pyoBinary)) {
        return NULL;
    }
    Py_buffer view;
    if(PyObject_GetBuffer(pyoBinary, &view, PyBUF_SIMPLE) < 0) {
        return NULL;
    }
    glProgramBinary(program, format, view.buf, (GLsizei)view.len);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}
#endif

#ifdef glCreateShaderProgramv
static PyObject * PyGL_CreateShaderProgramv(PyObject *self, PyObject *pyoArgs) {
    GLenum    type;
    PyObject *pyoStrings;
    if(!PyArg_ParseTuple(pyoArgs, "IO", &type, &pyoStrings)) {
        return NULL;
    }
    if(PyUnicode_Check(pyoStrings)) {
        const char *source = PyUnicode_AsUTF8(pyoStrings);
        if(NULL == source) {
            return NULL;
        }
        return Py_BuildValue("I", glCreateShaderProgramv(type, 1, &source));
    }
    PyObject *fast = PySequence_Fast(pyoStrings, "expected str or a sequence of str");
    if(NULL == fast) {
        return NULL;
    }
    Py_ssize_t n = PySequence_Fast_GET_SIZE(fast);
    const char **strings = PyMem_Malloc(sizeof(char *) * (n > 0 ? n : 1));
    if(NULL == strings) {
        Py_DECREF(fast);
        return PyErr_NoMemory();
    }
    for(Py_ssize_t idx = 0; idx < n; ++idx) {
        strings[idx] = PyUnicode_AsUTF8(PySequence_Fast_GET_ITEM(fast, idx));
        if(NULL == strings[idx]) {
            PyMem_Free(strings);
            Py_DECREF(fast);
            return NULL;
        }
    }
    GLuint program = glCreateShaderProgramv(type, (GLsizei)n, strings);
    PyMem_Free(strings);
    Py_DECREF(fast);
    return Py_BuildValue("I", program);
}
#endif

#ifdef glGetShaderPrecisionFormat
static PyObject * PyGL_GetShaderPrecisionFormat(PyObject *self, PyObject *pyoArgs) {
    GLenum shadertype, precisiontype;
    if(!PyArg_ParseTuple(pyoArgs, "II", &shadertype, &precisiontype)) {
        return NULL;
    }
    GLint range[2] = {0, 0};
    GLint precision = 0;
    glGetShaderPrecisionFormat(shadertype, precisiontype, range, &precision);
    return Py_BuildValue("(iii)", range[0], range[1], precision);
}
#endif

// ****************************************************************************
// direct-state-access texture sub-images
// ****************************************************************************

#ifdef glTextureSubImage1D
static PyObject * PyGL_TextureSubImage1D(PyObject *self, PyObject *pyoArgs) {
    GLuint  texture;
    GLint   level, xoffset;
    GLsizei width;
    GLenum  format, type;
    PyObject *pyoPixels;
    if(!PyArg_ParseTuple(pyoArgs, "IiiiIIO", &texture, &level, &xoffset, &width,
                         &format, &type, &pyoPixels)) {
        return NULL;
    }
    Py_buffer view;
    const void *pixels;
    if(pygl_bufptr(pyoPixels, &view, &pixels) < 0) {
        return NULL;
    }
    glTextureSubImage1D(texture, level, xoffset, width, format, type, pixels);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}
#endif

#ifdef glTextureSubImage2D
static PyObject * PyGL_TextureSubImage2D(PyObject *self, PyObject *pyoArgs) {
    GLuint  texture;
    GLint   level, xoffset, yoffset;
    GLsizei width, height;
    GLenum  format, type;
    PyObject *pyoPixels;
    if(!PyArg_ParseTuple(pyoArgs, "IiiiiiIIO", &texture, &level, &xoffset, &yoffset,
                         &width, &height, &format, &type, &pyoPixels)) {
        return NULL;
    }
    Py_buffer view;
    const void *pixels;
    if(pygl_bufptr(pyoPixels, &view, &pixels) < 0) {
        return NULL;
    }
    glTextureSubImage2D(texture, level, xoffset, yoffset, width, height, format, type, pixels);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}
#endif

#ifdef glTextureSubImage3D
static PyObject * PyGL_TextureSubImage3D(PyObject *self, PyObject *pyoArgs) {
    GLuint  texture;
    GLint   level, xoffset, yoffset, zoffset;
    GLsizei width, height, depth;
    GLenum  format, type;
    PyObject *pyoPixels;
    if(!PyArg_ParseTuple(pyoArgs, "IiiiiiiiIIO", &texture, &level, &xoffset, &yoffset,
                         &zoffset, &width, &height, &depth, &format, &type, &pyoPixels)) {
        return NULL;
    }
    Py_buffer view;
    const void *pixels;
    if(pygl_bufptr(pyoPixels, &view, &pixels) < 0) {
        return NULL;
    }
    glTextureSubImage3D(texture, level, xoffset, yoffset, zoffset, width, height, depth,
                        format, type, pixels);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}
#endif

#ifdef glCompressedTextureSubImage1D
static PyObject * PyGL_CompressedTextureSubImage1D(PyObject *self, PyObject *pyoArgs) {
    GLuint  texture;
    GLint   level, xoffset;
    GLsizei width;
    GLenum  format;
    PyObject *pyoData;
    if(!PyArg_ParseTuple(pyoArgs, "IiiiIO", &texture, &level, &xoffset, &width,
                         &format, &pyoData)) {
        return NULL;
    }
    Py_buffer view;
    if(PyObject_GetBuffer(pyoData, &view, PyBUF_SIMPLE) < 0) {
        return NULL;
    }
    glCompressedTextureSubImage1D(texture, level, xoffset, width, format,
                                  (GLsizei)view.len, view.buf);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}
#endif

#ifdef glCompressedTextureSubImage2D
static PyObject * PyGL_CompressedTextureSubImage2D(PyObject *self, PyObject *pyoArgs) {
    GLuint  texture;
    GLint   level, xoffset, yoffset;
    GLsizei width, height;
    GLenum  format;
    PyObject *pyoData;
    if(!PyArg_ParseTuple(pyoArgs, "IiiiiiIO", &texture, &level, &xoffset, &yoffset,
                         &width, &height, &format, &pyoData)) {
        return NULL;
    }
    Py_buffer view;
    if(PyObject_GetBuffer(pyoData, &view, PyBUF_SIMPLE) < 0) {
        return NULL;
    }
    glCompressedTextureSubImage2D(texture, level, xoffset, yoffset, width, height, format,
                                  (GLsizei)view.len, view.buf);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}
#endif

#ifdef glCompressedTextureSubImage3D
static PyObject * PyGL_CompressedTextureSubImage3D(PyObject *self, PyObject *pyoArgs) {
    GLuint  texture;
    GLint   level, xoffset, yoffset, zoffset;
    GLsizei width, height, depth;
    GLenum  format;
    PyObject *pyoData;
    if(!PyArg_ParseTuple(pyoArgs, "IiiiiiiiIO", &texture, &level, &xoffset, &yoffset,
                         &zoffset, &width, &height, &depth, &format, &pyoData)) {
        return NULL;
    }
    Py_buffer view;
    if(PyObject_GetBuffer(pyoData, &view, PyBUF_SIMPLE) < 0) {
        return NULL;
    }
    glCompressedTextureSubImage3D(texture, level, xoffset, yoffset, zoffset,
                                  width, height, depth, format, (GLsizei)view.len, view.buf);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}
#endif

// ****************************************************************************
// sized image readbacks  (all return a bytes of the requested size)
// ****************************************************************************

#ifdef glGetTextureImage
static PyObject * PyGL_GetTextureImage(PyObject *self, PyObject *pyoArgs) {
    GLuint     texture;
    GLint      level;
    GLenum     format, type;
    Py_ssize_t bufSize;
    if(!PyArg_ParseTuple(pyoArgs, "IiIIn", &texture, &level, &format, &type, &bufSize)) {
        return NULL;
    }
    if(bufSize < 0) {
        PyErr_SetString(PyExc_ValueError, "invalid size");
        return NULL;
    }
    PyObject *pyoBytes = PyBytes_FromStringAndSize(NULL, bufSize);
    if(NULL == pyoBytes) {
        return NULL;
    }
    glGetTextureImage(texture, level, format, type, (GLsizei)bufSize, PyBytes_AS_STRING(pyoBytes));
    return pyoBytes;
}
#endif

#ifdef glGetTextureSubImage
static PyObject * PyGL_GetTextureSubImage(PyObject *self, PyObject *pyoArgs) {
    GLuint     texture;
    GLint      level, xoffset, yoffset, zoffset;
    GLsizei    width, height, depth;
    GLenum     format, type;
    Py_ssize_t bufSize;
    if(!PyArg_ParseTuple(pyoArgs, "IiiiiiiiIIn", &texture, &level, &xoffset, &yoffset,
                         &zoffset, &width, &height, &depth, &format, &type, &bufSize)) {
        return NULL;
    }
    if(bufSize < 0) {
        PyErr_SetString(PyExc_ValueError, "invalid size");
        return NULL;
    }
    PyObject *pyoBytes = PyBytes_FromStringAndSize(NULL, bufSize);
    if(NULL == pyoBytes) {
        return NULL;
    }
    glGetTextureSubImage(texture, level, xoffset, yoffset, zoffset, width, height, depth,
                         format, type, (GLsizei)bufSize, PyBytes_AS_STRING(pyoBytes));
    return pyoBytes;
}
#endif

#ifdef glGetCompressedTextureImage
static PyObject * PyGL_GetCompressedTextureImage(PyObject *self, PyObject *pyoArgs) {
    GLuint     texture;
    GLint      level;
    Py_ssize_t bufSize;
    if(!PyArg_ParseTuple(pyoArgs, "Iin", &texture, &level, &bufSize)) {
        return NULL;
    }
    if(bufSize < 0) {
        PyErr_SetString(PyExc_ValueError, "invalid size");
        return NULL;
    }
    PyObject *pyoBytes = PyBytes_FromStringAndSize(NULL, bufSize);
    if(NULL == pyoBytes) {
        return NULL;
    }
    glGetCompressedTextureImage(texture, level, (GLsizei)bufSize, PyBytes_AS_STRING(pyoBytes));
    return pyoBytes;
}
#endif

#ifdef glGetnTexImage
static PyObject * PyGL_GetnTexImage(PyObject *self, PyObject *pyoArgs) {
    GLenum     target;
    GLint      level;
    GLenum     format, type;
    Py_ssize_t bufSize;
    if(!PyArg_ParseTuple(pyoArgs, "IiIIn", &target, &level, &format, &type, &bufSize)) {
        return NULL;
    }
    if(bufSize < 0) {
        PyErr_SetString(PyExc_ValueError, "invalid size");
        return NULL;
    }
    PyObject *pyoBytes = PyBytes_FromStringAndSize(NULL, bufSize);
    if(NULL == pyoBytes) {
        return NULL;
    }
    glGetnTexImage(target, level, format, type, (GLsizei)bufSize, PyBytes_AS_STRING(pyoBytes));
    return pyoBytes;
}
#endif

#ifdef glReadnPixels
static PyObject * PyGL_ReadnPixels(PyObject *self, PyObject *pyoArgs) {
    GLint      x, y;
    GLsizei    width, height;
    GLenum     format, type;
    Py_ssize_t bufSize = -1;
    if(!PyArg_ParseTuple(pyoArgs, "iiiiII|n", &x, &y, &width, &height, &format, &type, &bufSize)) {
        return NULL;
    }
    if(bufSize < 0) {
        bufSize = (Py_ssize_t)pygl_image_size(width, height, 1, format, type);
    }
    PyObject *pyoBytes = PyBytes_FromStringAndSize(NULL, bufSize);
    if(NULL == pyoBytes) {
        return NULL;
    }
    glReadnPixels(x, y, width, height, format, type, (GLsizei)bufSize, PyBytes_AS_STRING(pyoBytes));
    return pyoBytes;
}
#endif

// ****************************************************************************
// buffer / texture region clears
// ****************************************************************************

#ifdef glClearBufferData
static PyObject * PyGL_ClearBufferData(PyObject *self, PyObject *pyoArgs) {
    GLenum target, internalformat, format, type;
    PyObject *pyoData;
    if(!PyArg_ParseTuple(pyoArgs, "IIIIO", &target, &internalformat, &format, &type, &pyoData)) {
        return NULL;
    }
    Py_buffer view;
    const void *data;
    if(pygl_bufptr(pyoData, &view, &data) < 0) {
        return NULL;
    }
    glClearBufferData(target, internalformat, format, type, data);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}
#endif

#ifdef glClearBufferSubData
static PyObject * PyGL_ClearBufferSubData(PyObject *self, PyObject *pyoArgs) {
    GLenum     target, internalformat, format, type;
    Py_ssize_t offset, size;
    PyObject  *pyoData;
    if(!PyArg_ParseTuple(pyoArgs, "IInnIIO", &target, &internalformat, &offset, &size,
                         &format, &type, &pyoData)) {
        return NULL;
    }
    Py_buffer view;
    const void *data;
    if(pygl_bufptr(pyoData, &view, &data) < 0) {
        return NULL;
    }
    glClearBufferSubData(target, internalformat, (GLintptr)offset, (GLsizeiptr)size,
                         format, type, data);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}
#endif

#ifdef glClearNamedBufferData
static PyObject * PyGL_ClearNamedBufferData(PyObject *self, PyObject *pyoArgs) {
    GLuint    buffer;
    GLenum    internalformat, format, type;
    PyObject *pyoData;
    if(!PyArg_ParseTuple(pyoArgs, "IIIIO", &buffer, &internalformat, &format, &type, &pyoData)) {
        return NULL;
    }
    Py_buffer view;
    const void *data;
    if(pygl_bufptr(pyoData, &view, &data) < 0) {
        return NULL;
    }
    glClearNamedBufferData(buffer, internalformat, format, type, data);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}
#endif

#ifdef glClearNamedBufferSubData
static PyObject * PyGL_ClearNamedBufferSubData(PyObject *self, PyObject *pyoArgs) {
    GLuint     buffer;
    GLenum     internalformat, format, type;
    Py_ssize_t offset, size;
    PyObject  *pyoData;
    if(!PyArg_ParseTuple(pyoArgs, "IInnIIO", &buffer, &internalformat, &offset, &size,
                         &format, &type, &pyoData)) {
        return NULL;
    }
    Py_buffer view;
    const void *data;
    if(pygl_bufptr(pyoData, &view, &data) < 0) {
        return NULL;
    }
    glClearNamedBufferSubData(buffer, internalformat, (GLintptr)offset, (GLsizeiptr)size,
                              format, type, data);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}
#endif

#ifdef glClearTexImage
static PyObject * PyGL_ClearTexImage(PyObject *self, PyObject *pyoArgs) {
    GLuint    texture;
    GLint     level;
    GLenum    format, type;
    PyObject *pyoData;
    if(!PyArg_ParseTuple(pyoArgs, "IiIIO", &texture, &level, &format, &type, &pyoData)) {
        return NULL;
    }
    Py_buffer view;
    const void *data;
    if(pygl_bufptr(pyoData, &view, &data) < 0) {
        return NULL;
    }
    glClearTexImage(texture, level, format, type, data);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}
#endif

#ifdef glClearTexSubImage
static PyObject * PyGL_ClearTexSubImage(PyObject *self, PyObject *pyoArgs) {
    GLuint    texture;
    GLint     level, xoffset, yoffset, zoffset;
    GLsizei   width, height, depth;
    GLenum    format, type;
    PyObject *pyoData;
    if(!PyArg_ParseTuple(pyoArgs, "IiiiiiiiIIO", &texture, &level, &xoffset, &yoffset,
                         &zoffset, &width, &height, &depth, &format, &type, &pyoData)) {
        return NULL;
    }
    Py_buffer view;
    const void *data;
    if(pygl_bufptr(pyoData, &view, &data) < 0) {
        return NULL;
    }
    glClearTexSubImage(texture, level, xoffset, yoffset, zoffset, width, height, depth,
                       format, type, data);
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}
#endif

// ****************************************************************************
// per-draw-buffer clears  (GL 3.0)  and their DSA framebuffer equivalents
// ****************************************************************************

#define PYGL_CLEAR_BUFFER(PY, T, HELPER)                                       \
static PyObject * PyGL_##PY(PyObject *self, PyObject *pyoArgs) {                \
    GLenum    buffer;                                                          \
    GLint     drawbuffer;                                                      \
    PyObject *pyoValue;                                                        \
    if(!PyArg_ParseTuple(pyoArgs, "IiO", &buffer, &drawbuffer, &pyoValue)) {   \
        return NULL;                                                           \
    }                                                                         \
    T value[4] = {0, 0, 0, 0};                                                 \
    if(HELPER(pyoValue, value, 4) < 0) {                                       \
        return NULL;                                                           \
    }                                                                         \
    gl##PY(buffer, drawbuffer, value);                                         \
    Py_RETURN_NONE;                                                            \
}

#ifdef glClearBufferfv
PYGL_CLEAR_BUFFER( ClearBufferfv, GLfloat, pygl_floats )
#endif
#ifdef glClearBufferiv
PYGL_CLEAR_BUFFER( ClearBufferiv, GLint,   pygl_ints   )
#endif
#ifdef glClearBufferuiv
PYGL_CLEAR_BUFFER( ClearBufferuiv, GLuint, pygl_uints  )
#endif

#define PYGL_CLEAR_NAMED_FB(PY, T, HELPER)                                     \
static PyObject * PyGL_##PY(PyObject *self, PyObject *pyoArgs) {                \
    GLuint    framebuffer;                                                     \
    GLenum    buffer;                                                          \
    GLint     drawbuffer;                                                      \
    PyObject *pyoValue;                                                        \
    if(!PyArg_ParseTuple(pyoArgs, "IIiO", &framebuffer, &buffer, &drawbuffer, &pyoValue)) { \
        return NULL;                                                           \
    }                                                                         \
    T value[4] = {0, 0, 0, 0};                                                 \
    if(HELPER(pyoValue, value, 4) < 0) {                                       \
        return NULL;                                                           \
    }                                                                         \
    gl##PY(framebuffer, buffer, drawbuffer, value);                            \
    Py_RETURN_NONE;                                                            \
}

#ifdef glClearNamedFramebufferfv
PYGL_CLEAR_NAMED_FB( ClearNamedFramebufferfv, GLfloat, pygl_floats )
#endif
#ifdef glClearNamedFramebufferiv
PYGL_CLEAR_NAMED_FB( ClearNamedFramebufferiv, GLint,   pygl_ints   )
#endif
#ifdef glClearNamedFramebufferuiv
PYGL_CLEAR_NAMED_FB( ClearNamedFramebufferuiv, GLuint, pygl_uints  )
#endif

// ****************************************************************************
// framebuffer invalidation and draw-buffer lists
// ****************************************************************************

#ifdef glInvalidateFramebuffer
static PyObject * PyGL_InvalidateFramebuffer(PyObject *self, PyObject *pyoArgs) {
    GLenum    target;
    PyObject *pyoAttachments;
    if(!PyArg_ParseTuple(pyoArgs, "IO", &target, &pyoAttachments)) {
        return NULL;
    }
    GLuint attachments[PYGL_MAX_VEC];
    Py_ssize_t n = pygl_uints(pyoAttachments, attachments, PYGL_MAX_VEC);
    if(n < 0) {
        return NULL;
    }
    glInvalidateFramebuffer(target, (GLsizei)n, (const GLenum *)attachments);
    Py_RETURN_NONE;
}
#endif

#ifdef glInvalidateSubFramebuffer
static PyObject * PyGL_InvalidateSubFramebuffer(PyObject *self, PyObject *pyoArgs) {
    GLenum    target;
    PyObject *pyoAttachments;
    GLint     x, y;
    GLsizei   width, height;
    if(!PyArg_ParseTuple(pyoArgs, "IOiiii", &target, &pyoAttachments, &x, &y, &width, &height)) {
        return NULL;
    }
    GLuint attachments[PYGL_MAX_VEC];
    Py_ssize_t n = pygl_uints(pyoAttachments, attachments, PYGL_MAX_VEC);
    if(n < 0) {
        return NULL;
    }
    glInvalidateSubFramebuffer(target, (GLsizei)n, (const GLenum *)attachments, x, y, width, height);
    Py_RETURN_NONE;
}
#endif

#ifdef glNamedFramebufferDrawBuffers
static PyObject * PyGL_NamedFramebufferDrawBuffers(PyObject *self, PyObject *pyoArgs) {
    GLuint    framebuffer;
    PyObject *pyoBufs;
    if(!PyArg_ParseTuple(pyoArgs, "IO", &framebuffer, &pyoBufs)) {
        return NULL;
    }
    GLuint bufs[PYGL_MAX_VEC];
    Py_ssize_t n = pygl_uints(pyoBufs, bufs, PYGL_MAX_VEC);
    if(n < 0) {
        return NULL;
    }
    glNamedFramebufferDrawBuffers(framebuffer, (GLsizei)n, (const GLenum *)bufs);
    Py_RETURN_NONE;
}
#endif

// ****************************************************************************
// multi-bind  (GL 4.4)
// ****************************************************************************

#define PYGL_MULTIBIND(PY)                                                     \
static PyObject * PyGL_##PY(PyObject *self, PyObject *pyoArgs) {                \
    GLuint    first;                                                           \
    PyObject *pyoNames;                                                        \
    if(!PyArg_ParseTuple(pyoArgs, "IO", &first, &pyoNames)) {                  \
        return NULL;                                                           \
    }                                                                         \
    Py_ssize_t n;                                                              \
    GLuint *names = pygl_alloc_uints(pyoNames, &n);                            \
    if(NULL == names) {                                                        \
        return NULL;                                                           \
    }                                                                         \
    gl##PY(first, (GLsizei)n, names);                                          \
    PyMem_Free(names);                                                         \
    Py_RETURN_NONE;                                                            \
}

#ifdef glBindTextures
PYGL_MULTIBIND( BindTextures )
#endif
#ifdef glBindSamplers
PYGL_MULTIBIND( BindSamplers )
#endif
#ifdef glBindImageTextures
PYGL_MULTIBIND( BindImageTextures )
#endif

#define PYGL_MULTIBIND_TARGET(PY)                                              \
static PyObject * PyGL_##PY(PyObject *self, PyObject *pyoArgs) {                \
    GLenum    target;                                                          \
    GLuint    first;                                                           \
    PyObject *pyoNames;                                                        \
    if(!PyArg_ParseTuple(pyoArgs, "IIO", &target, &first, &pyoNames)) {        \
        return NULL;                                                           \
    }                                                                         \
    Py_ssize_t n;                                                              \
    GLuint *names = pygl_alloc_uints(pyoNames, &n);                            \
    if(NULL == names) {                                                        \
        return NULL;                                                           \
    }                                                                         \
    gl##PY(target, first, (GLsizei)n, names);                                  \
    PyMem_Free(names);                                                         \
    Py_RETURN_NONE;                                                            \
}

#ifdef glBindBuffersBase
PYGL_MULTIBIND_TARGET( BindBuffersBase )
#endif

// ****************************************************************************
// viewport / scissor / depth-range arrays  (GL 4.1)
// ****************************************************************************

#define PYGL_INDEXED_ARRAY(PY, STRIDE, HELPER, T)                              \
static PyObject * PyGL_##PY(PyObject *self, PyObject *pyoArgs) {                \
    GLuint    first;                                                           \
    PyObject *pyoValue;                                                        \
    if(!PyArg_ParseTuple(pyoArgs, "IO", &first, &pyoValue)) {                  \
        return NULL;                                                           \
    }                                                                         \
    Py_ssize_t n;                                                              \
    T *value = HELPER(pyoValue, &n);                                           \
    if(NULL == value) {                                                        \
        return NULL;                                                           \
    }                                                                         \
    if(0 != n % STRIDE) {                                                      \
        PyMem_Free(value);                                                     \
        PyErr_SetString(PyExc_ValueError, "length is not a multiple of " #STRIDE); \
        return NULL;                                                           \
    }                                                                         \
    gl##PY(first, (GLsizei)(n / STRIDE), value);                               \
    PyMem_Free(value);                                                         \
    Py_RETURN_NONE;                                                            \
}

#ifdef glViewportArrayv
PYGL_INDEXED_ARRAY( ViewportArrayv,   4, pygl_alloc_floats,  GLfloat  )
#endif
#ifdef glScissorArrayv
PYGL_INDEXED_ARRAY( ScissorArrayv,    4, pygl_alloc_ints,    GLint    )
#endif
#ifdef glDepthRangeArrayv
PYGL_INDEXED_ARRAY( DepthRangeArrayv, 2, pygl_alloc_doubles, GLdouble )
#endif

// ****************************************************************************
// debug output  (GL 4.3 / KHR_debug)
// ****************************************************************************

#ifdef glDebugMessageControl
static PyObject * PyGL_DebugMessageControl(PyObject *self, PyObject *pyoArgs) {
    GLenum    source, type, severity;
    PyObject *pyoIds;
    GLboolean enabled;
    if(!PyArg_ParseTuple(pyoArgs, "IIIOb", &source, &type, &severity, &pyoIds, &enabled)) {
        return NULL;
    }
    GLuint ids[PYGL_MAX_QUERY];
    Py_ssize_t n = 0;
    if(Py_None != pyoIds) {
        n = pygl_uints(pyoIds, ids, PYGL_MAX_QUERY);
        if(n < 0) {
            return NULL;
        }
    }
    glDebugMessageControl(source, type, severity, (GLsizei)n, n ? ids : NULL, enabled);
    Py_RETURN_NONE;
}
#endif

#ifdef glDebugMessageInsert
static PyObject * PyGL_DebugMessageInsert(PyObject *self, PyObject *pyoArgs) {
    GLenum      source, type, severity;
    GLuint      id;
    const char *buf;
    Py_ssize_t  length;
    if(!PyArg_ParseTuple(pyoArgs, "IIIIs#", &source, &type, &id, &severity, &buf, &length)) {
        return NULL;
    }
    glDebugMessageInsert(source, type, id, severity, (GLsizei)length, buf);
    Py_RETURN_NONE;
}
#endif

#ifdef glObjectLabel
static PyObject * PyGL_ObjectLabel(PyObject *self, PyObject *pyoArgs) {
    GLenum      identifier;
    GLuint      name;
    const char *label;
    Py_ssize_t  length;
    if(!PyArg_ParseTuple(pyoArgs, "IIs#", &identifier, &name, &label, &length)) {
        return NULL;
    }
    glObjectLabel(identifier, name, (GLsizei)length, label);
    Py_RETURN_NONE;
}
#endif

#ifdef glGetObjectLabel
static PyObject * PyGL_GetObjectLabel(PyObject *self, PyObject *pyoArgs) {
    GLenum identifier;
    GLuint name;
    if(!PyArg_ParseTuple(pyoArgs, "II", &identifier, &name)) {
        return NULL;
    }
    char    label[1024];
    GLsizei written = 0;
    glGetObjectLabel(identifier, name, (GLsizei)sizeof(label), &written, label);
    return PyUnicode_FromStringAndSize(label, written);
}
#endif

#ifdef glPushDebugGroup
static PyObject * PyGL_PushDebugGroup(PyObject *self, PyObject *pyoArgs) {
    GLenum      source;
    GLuint      id;
    const char *message;
    Py_ssize_t  length;
    if(!PyArg_ParseTuple(pyoArgs, "IIs#", &source, &id, &message, &length)) {
        return NULL;
    }
    glPushDebugGroup(source, id, (GLsizei)length, message);
    Py_RETURN_NONE;
}
#endif

#ifdef glGetDebugMessageLog
static PyObject * PyGL_GetDebugMessageLog(PyObject *self, PyObject *pyoArg) {
    unsigned long request = PyLong_AsUnsignedLong(pyoArg);
    if(PyErr_Occurred()) {
        return NULL;
    }
    if(request < 1 || request > 64) {
        PyErr_SetString(PyExc_ValueError, "count must be 1..64");
        return NULL;
    }
    GLuint  count = (GLuint)request;
    GLenum  sources[64], types[64], severities[64];
    GLuint  ids[64];
    GLsizei lengths[64];
    GLsizei bufSize  = (GLsizei)(count * 512);
    char   *messages = PyMem_Malloc(bufSize);
    if(NULL == messages) {
        return PyErr_NoMemory();
    }
    GLuint got = glGetDebugMessageLog(count, bufSize, sources, types, ids, severities,
                                      lengths, messages);
    PyObject *list = PyList_New(got);
    if(NULL == list) {
        PyMem_Free(messages);
        return NULL;
    }
    const char *cursor = messages;
    for(GLuint idx = 0; idx < got; ++idx) {
        Py_ssize_t text = lengths[idx] > 0 ? lengths[idx] - 1 : 0;
        PyObject *item = Py_BuildValue("(IIIIs#)", sources[idx], types[idx], ids[idx],
                                       severities[idx], cursor, text);
        if(NULL == item) {
            Py_DECREF(list);
            PyMem_Free(messages);
            return NULL;
        }
        PyList_SET_ITEM(list, idx, item);
        cursor += lengths[idx];
    }
    PyMem_Free(messages);
    return list;
}
#endif

#ifdef glDebugMessageCallback
static PyObject *pygl_debug_callback = NULL;

static void GLAPIENTRY pygl_debug_trampoline(GLenum source, GLenum type, GLuint id,
        GLenum severity, GLsizei length, const GLchar *message, const void *user) {
    (void)user;
    (void)length;
    if(NULL == pygl_debug_callback) {
        return;
    }
    PyGILState_STATE gil = PyGILState_Ensure();
    PyObject *result = PyObject_CallFunction(pygl_debug_callback, "IIIIs",
                                             source, type, id, severity, message);
    if(NULL == result) {
        PyErr_WriteUnraisable(pygl_debug_callback);
    }
    Py_XDECREF(result);
    PyGILState_Release(gil);
}

static PyObject * PyGL_DebugMessageCallback(PyObject *self, PyObject *pyoArg) {
    if(Py_None == pyoArg) {
        glDebugMessageCallback(NULL, NULL);
        Py_CLEAR(pygl_debug_callback);
        Py_RETURN_NONE;
    }
    if(!PyCallable_Check(pyoArg)) {
        PyErr_SetString(PyExc_TypeError, "expected a callable or None");
        return NULL;
    }
    Py_INCREF(pyoArg);
    Py_XSETREF(pygl_debug_callback, pyoArg);
    glDebugMessageCallback(pygl_debug_trampoline, NULL);
    Py_RETURN_NONE;
}
#endif

// ****************************************************************************
// unsigned / double uniform arrays  (via the shaders.c macros)
// ****************************************************************************

#ifdef glUniform1uiv
PYGL_UNIFORM_V( Uniform1uiv, 1, pygl_alloc_uints, GLuint )
#endif
#ifdef glUniform2uiv
PYGL_UNIFORM_V( Uniform2uiv, 2, pygl_alloc_uints, GLuint )
#endif
#ifdef glUniform3uiv
PYGL_UNIFORM_V( Uniform3uiv, 3, pygl_alloc_uints, GLuint )
#endif
#ifdef glUniform4uiv
PYGL_UNIFORM_V( Uniform4uiv, 4, pygl_alloc_uints, GLuint )
#endif
#ifdef glUniform1dv
PYGL_UNIFORM_V( Uniform1dv, 1, pygl_alloc_doubles, GLdouble )
#endif
#ifdef glUniform2dv
PYGL_UNIFORM_V( Uniform2dv, 2, pygl_alloc_doubles, GLdouble )
#endif
#ifdef glUniform3dv
PYGL_UNIFORM_V( Uniform3dv, 3, pygl_alloc_doubles, GLdouble )
#endif
#ifdef glUniform4dv
PYGL_UNIFORM_V( Uniform4dv, 4, pygl_alloc_doubles, GLdouble )
#endif
#ifdef glUniformMatrix2dv
PYGL_UNIFORM_MAT( UniformMatrix2dv, 4,  pygl_alloc_doubles, GLdouble )
#endif
#ifdef glUniformMatrix3dv
PYGL_UNIFORM_MAT( UniformMatrix3dv, 9,  pygl_alloc_doubles, GLdouble )
#endif
#ifdef glUniformMatrix4dv
PYGL_UNIFORM_MAT( UniformMatrix4dv, 16, pygl_alloc_doubles, GLdouble )
#endif

// ****************************************************************************
// separable-program uniform arrays  (GL 4.1)
// ****************************************************************************

#define PYGL_PROGRAM_UNIFORM_V(PY, COMPONENTS, ALLOC, T)                       \
static PyObject * PyGL_##PY(PyObject *self, PyObject *pyoArgs) {                \
    GLuint    program;                                                         \
    GLint     location;                                                        \
    PyObject *pyoValue;                                                        \
    if(!PyArg_ParseTuple(pyoArgs, "IiO", &program, &location, &pyoValue)) {    \
        return NULL;                                                           \
    }                                                                         \
    Py_ssize_t n;                                                              \
    T *value = ALLOC(pyoValue, &n);                                            \
    if(NULL == value) {                                                        \
        return NULL;                                                           \
    }                                                                         \
    if(0 != n % COMPONENTS) {                                                  \
        PyMem_Free(value);                                                     \
        PyErr_SetString(PyExc_ValueError, "length is not a multiple of " #COMPONENTS); \
        return NULL;                                                           \
    }                                                                         \
    gl##PY(program, location, (GLsizei)(n / COMPONENTS), value);               \
    PyMem_Free(value);                                                         \
    Py_RETURN_NONE;                                                            \
}

#define PYGL_PROGRAM_UNIFORM_MAT(PY, ELEMENTS, ALLOC, T)                       \
static PyObject * PyGL_##PY(PyObject *self, PyObject *pyoArgs) {                \
    GLuint    program;                                                         \
    GLint     location;                                                        \
    GLboolean transpose;                                                       \
    PyObject *pyoValue;                                                        \
    if(!PyArg_ParseTuple(pyoArgs, "IibO", &program, &location, &transpose, &pyoValue)) { \
        return NULL;                                                           \
    }                                                                         \
    Py_ssize_t n;                                                              \
    T *value = ALLOC(pyoValue, &n);                                            \
    if(NULL == value) {                                                        \
        return NULL;                                                           \
    }                                                                         \
    if(0 != n % ELEMENTS) {                                                    \
        PyMem_Free(value);                                                     \
        PyErr_SetString(PyExc_ValueError, "length is not a multiple of " #ELEMENTS); \
        return NULL;                                                           \
    }                                                                         \
    gl##PY(program, location, (GLsizei)(n / ELEMENTS), transpose, value);      \
    PyMem_Free(value);                                                         \
    Py_RETURN_NONE;                                                            \
}

#ifdef glProgramUniform1fv
PYGL_PROGRAM_UNIFORM_V( ProgramUniform1fv, 1, pygl_alloc_floats, GLfloat )
#endif
#ifdef glProgramUniform2fv
PYGL_PROGRAM_UNIFORM_V( ProgramUniform2fv, 2, pygl_alloc_floats, GLfloat )
#endif
#ifdef glProgramUniform3fv
PYGL_PROGRAM_UNIFORM_V( ProgramUniform3fv, 3, pygl_alloc_floats, GLfloat )
#endif
#ifdef glProgramUniform4fv
PYGL_PROGRAM_UNIFORM_V( ProgramUniform4fv, 4, pygl_alloc_floats, GLfloat )
#endif
#ifdef glProgramUniform1iv
PYGL_PROGRAM_UNIFORM_V( ProgramUniform1iv, 1, pygl_alloc_ints, GLint )
#endif
#ifdef glProgramUniform2iv
PYGL_PROGRAM_UNIFORM_V( ProgramUniform2iv, 2, pygl_alloc_ints, GLint )
#endif
#ifdef glProgramUniform3iv
PYGL_PROGRAM_UNIFORM_V( ProgramUniform3iv, 3, pygl_alloc_ints, GLint )
#endif
#ifdef glProgramUniform4iv
PYGL_PROGRAM_UNIFORM_V( ProgramUniform4iv, 4, pygl_alloc_ints, GLint )
#endif
#ifdef glProgramUniform1uiv
PYGL_PROGRAM_UNIFORM_V( ProgramUniform1uiv, 1, pygl_alloc_uints, GLuint )
#endif
#ifdef glProgramUniform2uiv
PYGL_PROGRAM_UNIFORM_V( ProgramUniform2uiv, 2, pygl_alloc_uints, GLuint )
#endif
#ifdef glProgramUniform3uiv
PYGL_PROGRAM_UNIFORM_V( ProgramUniform3uiv, 3, pygl_alloc_uints, GLuint )
#endif
#ifdef glProgramUniform4uiv
PYGL_PROGRAM_UNIFORM_V( ProgramUniform4uiv, 4, pygl_alloc_uints, GLuint )
#endif
#ifdef glProgramUniformMatrix2fv
PYGL_PROGRAM_UNIFORM_MAT( ProgramUniformMatrix2fv, 4,  pygl_alloc_floats, GLfloat )
#endif
#ifdef glProgramUniformMatrix3fv
PYGL_PROGRAM_UNIFORM_MAT( ProgramUniformMatrix3fv, 9,  pygl_alloc_floats, GLfloat )
#endif
#ifdef glProgramUniformMatrix4fv
PYGL_PROGRAM_UNIFORM_MAT( ProgramUniformMatrix4fv, 16, pygl_alloc_floats, GLfloat )
#endif

// ****************************************************************************
// method table
// ****************************************************************************

static PyMethodDef PyGL_modern_methods[] = {
#ifdef glFenceSync
    { "FenceSync",             PyGL_FenceSync,             METH_VARARGS, "FenceSync(condition, flags)" },
#endif
#ifdef glDeleteSync
    { "DeleteSync",            PyGL_DeleteSync,            METH_O,       "DeleteSync(sync)" },
#endif
#ifdef glClientWaitSync
    { "ClientWaitSync",        PyGL_ClientWaitSync,        METH_VARARGS, "ClientWaitSync(sync, flags, timeout)" },
#endif
#ifdef glWaitSync
    { "WaitSync",              PyGL_WaitSync,              METH_VARARGS, "WaitSync(sync, flags, timeout)" },
#endif
#ifdef glIsSync
    { "IsSync",                PyGL_IsSync,                METH_O,       "IsSync(sync)" },
#endif
#ifdef glGetSynciv
    { "GetSynciv",             PyGL_GetSynciv,             METH_VARARGS, "GetSynciv(sync, pname, count=1)" },
#endif

#ifdef glBufferStorage
    { "BufferStorage",         PyGL_BufferStorage,         METH_VARARGS, "BufferStorage(target, data_or_size, flags)" },
#endif
#ifdef glNamedBufferStorage
    { "NamedBufferStorage",    PyGL_NamedBufferStorage,    METH_VARARGS, "NamedBufferStorage(buffer, data_or_size, flags)" },
#endif
#ifdef glNamedBufferData
    { "NamedBufferData",       PyGL_NamedBufferData,       METH_VARARGS, "NamedBufferData(buffer, data_or_size, usage)" },
#endif
#ifdef glNamedBufferSubData
    { "NamedBufferSubData",    PyGL_NamedBufferSubData,    METH_VARARGS, "NamedBufferSubData(buffer, offset, data)" },
#endif
#ifdef glGetNamedBufferSubData
    { "GetNamedBufferSubData", PyGL_GetNamedBufferSubData, METH_VARARGS, "GetNamedBufferSubData(buffer, offset, size)" },
#endif

#ifdef glMapBufferRange
    { "MapBufferRange",        PyGL_MapBufferRange,        METH_VARARGS, "MapBufferRange(target, offset, length, access)" },
#endif
#ifdef glMapNamedBuffer
    { "MapNamedBuffer",        PyGL_MapNamedBuffer,        METH_VARARGS, "MapNamedBuffer(buffer, access)" },
#endif
#ifdef glMapNamedBufferRange
    { "MapNamedBufferRange",   PyGL_MapNamedBufferRange,   METH_VARARGS, "MapNamedBufferRange(buffer, offset, length, access)" },
#endif

#ifdef glVertexAttribIPointer
    { "VertexAttribIPointer",  PyGL_VertexAttribIPointer,  METH_VARARGS, "VertexAttribIPointer(index, size, type, stride, pointer)" },
#endif
#ifdef glVertexAttribLPointer
    { "VertexAttribLPointer",  PyGL_VertexAttribLPointer,  METH_VARARGS, "VertexAttribLPointer(index, size, type, stride, pointer)" },
#endif

#ifdef glDrawElementsBaseVertex
    { "DrawElementsBaseVertex", PyGL_DrawElementsBaseVertex, METH_VARARGS, "DrawElementsBaseVertex(mode, count, type, indices, basevertex)" },
#endif
#ifdef glDrawRangeElementsBaseVertex
    { "DrawRangeElementsBaseVertex", PyGL_DrawRangeElementsBaseVertex, METH_VARARGS, "DrawRangeElementsBaseVertex(mode, start, end, count, type, indices, basevertex)" },
#endif
#ifdef glDrawElementsInstanced
    { "DrawElementsInstanced", PyGL_DrawElementsInstanced, METH_VARARGS, "DrawElementsInstanced(mode, count, type, indices, instancecount)" },
#endif
#ifdef glDrawElementsInstancedBaseVertex
    { "DrawElementsInstancedBaseVertex", PyGL_DrawElementsInstancedBaseVertex, METH_VARARGS, "DrawElementsInstancedBaseVertex(mode, count, type, indices, instancecount, basevertex)" },
#endif
#ifdef glDrawElementsInstancedBaseInstance
    { "DrawElementsInstancedBaseInstance", PyGL_DrawElementsInstancedBaseInstance, METH_VARARGS, "DrawElementsInstancedBaseInstance(mode, count, type, indices, instancecount, baseinstance)" },
#endif
#ifdef glDrawElementsInstancedBaseVertexBaseInstance
    { "DrawElementsInstancedBaseVertexBaseInstance", PyGL_DrawElementsInstancedBaseVertexBaseInstance, METH_VARARGS, "DrawElementsInstancedBaseVertexBaseInstance(mode, count, type, indices, instancecount, basevertex, baseinstance)" },
#endif
#ifdef glDrawArraysIndirect
    { "DrawArraysIndirect",    PyGL_DrawArraysIndirect,    METH_VARARGS, "DrawArraysIndirect(mode, indirect)" },
#endif
#ifdef glDrawElementsIndirect
    { "DrawElementsIndirect",  PyGL_DrawElementsIndirect,  METH_VARARGS, "DrawElementsIndirect(mode, type, indirect)" },
#endif
#ifdef glMultiDrawArrays
    { "MultiDrawArrays",       PyGL_MultiDrawArrays,       METH_VARARGS, "MultiDrawArrays(mode, first, count)" },
#endif
#ifdef glMultiDrawElements
    { "MultiDrawElements",     PyGL_MultiDrawElements,     METH_VARARGS, "MultiDrawElements(mode, count, type, indices)" },
#endif

#ifdef glTransformFeedbackVaryings
    { "TransformFeedbackVaryings", PyGL_TransformFeedbackVaryings, METH_VARARGS, "TransformFeedbackVaryings(program, varyings, bufferMode)" },
#endif
#ifdef glGetTransformFeedbackVarying
    { "GetTransformFeedbackVarying", PyGL_GetTransformFeedbackVarying, METH_VARARGS, "GetTransformFeedbackVarying(program, index) -> (name, size, type)" },
#endif

#ifdef glBindFragDataLocation
    { "BindFragDataLocation",  PyGL_BindFragDataLocation,  METH_VARARGS, "BindFragDataLocation(program, color, name)" },
#endif
#ifdef glBindFragDataLocationIndexed
    { "BindFragDataLocationIndexed", PyGL_BindFragDataLocationIndexed, METH_VARARGS, "BindFragDataLocationIndexed(program, colorNumber, index, name)" },
#endif
#ifdef glGetFragDataLocation
    { "GetFragDataLocation",   PyGL_GetFragDataLocation,   METH_VARARGS, "GetFragDataLocation(program, name)" },
#endif
#ifdef glGetFragDataIndex
    { "GetFragDataIndex",      PyGL_GetFragDataIndex,      METH_VARARGS, "GetFragDataIndex(program, name)" },
#endif

#ifdef glGetUniformBlockIndex
    { "GetUniformBlockIndex",  PyGL_GetUniformBlockIndex,  METH_VARARGS, "GetUniformBlockIndex(program, name)" },
#endif
#ifdef glGetActiveUniformBlockName
    { "GetActiveUniformBlockName", PyGL_GetActiveUniformBlockName, METH_VARARGS, "GetActiveUniformBlockName(program, index)" },
#endif
#ifdef glGetActiveUniformBlockiv
    { "GetActiveUniformBlockiv", PyGL_GetActiveUniformBlockiv, METH_VARARGS, "GetActiveUniformBlockiv(program, index, pname, count=1)" },
#endif
#ifdef glGetActiveUniformName
    { "GetActiveUniformName",  PyGL_GetActiveUniformName,  METH_VARARGS, "GetActiveUniformName(program, index)" },
#endif
#ifdef glGetUniformIndices
    { "GetUniformIndices",     PyGL_GetUniformIndices,     METH_VARARGS, "GetUniformIndices(program, names)" },
#endif
#ifdef glGetActiveUniformsiv
    { "GetActiveUniformsiv",   PyGL_GetActiveUniformsiv,   METH_VARARGS, "GetActiveUniformsiv(program, indices, pname)" },
#endif

#ifdef glGetProgramBinary
    { "GetProgramBinary",      PyGL_GetProgramBinary,      METH_O,       "GetProgramBinary(program) -> (binaryFormat, bytes)" },
#endif
#ifdef glProgramBinary
    { "ProgramBinary",         PyGL_ProgramBinary,         METH_VARARGS, "ProgramBinary(program, binaryFormat, binary)" },
#endif
#ifdef glCreateShaderProgramv
    { "CreateShaderProgramv",  PyGL_CreateShaderProgramv,  METH_VARARGS, "CreateShaderProgramv(type, strings)" },
#endif
#ifdef glGetShaderPrecisionFormat
    { "GetShaderPrecisionFormat", PyGL_GetShaderPrecisionFormat, METH_VARARGS, "GetShaderPrecisionFormat(shadertype, precisiontype) -> (low, high, precision)" },
#endif

#ifdef glTextureSubImage1D
    { "TextureSubImage1D",     PyGL_TextureSubImage1D,     METH_VARARGS, "TextureSubImage1D(texture, level, xoffset, width, format, type, pixels)" },
#endif
#ifdef glTextureSubImage2D
    { "TextureSubImage2D",     PyGL_TextureSubImage2D,     METH_VARARGS, "TextureSubImage2D(texture, level, xoffset, yoffset, width, height, format, type, pixels)" },
#endif
#ifdef glTextureSubImage3D
    { "TextureSubImage3D",     PyGL_TextureSubImage3D,     METH_VARARGS, "TextureSubImage3D(texture, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels)" },
#endif
#ifdef glCompressedTextureSubImage1D
    { "CompressedTextureSubImage1D", PyGL_CompressedTextureSubImage1D, METH_VARARGS, "CompressedTextureSubImage1D(texture, level, xoffset, width, format, data)" },
#endif
#ifdef glCompressedTextureSubImage2D
    { "CompressedTextureSubImage2D", PyGL_CompressedTextureSubImage2D, METH_VARARGS, "CompressedTextureSubImage2D(texture, level, xoffset, yoffset, width, height, format, data)" },
#endif
#ifdef glCompressedTextureSubImage3D
    { "CompressedTextureSubImage3D", PyGL_CompressedTextureSubImage3D, METH_VARARGS, "CompressedTextureSubImage3D(texture, level, xoffset, yoffset, zoffset, width, height, depth, format, data)" },
#endif
#ifdef glGetTextureImage
    { "GetTextureImage",       PyGL_GetTextureImage,       METH_VARARGS, "GetTextureImage(texture, level, format, type, bufSize)" },
#endif
#ifdef glGetTextureSubImage
    { "GetTextureSubImage",    PyGL_GetTextureSubImage,    METH_VARARGS, "GetTextureSubImage(texture, level, xoffset, yoffset, zoffset, width, height, depth, format, type, bufSize)" },
#endif
#ifdef glGetCompressedTextureImage
    { "GetCompressedTextureImage", PyGL_GetCompressedTextureImage, METH_VARARGS, "GetCompressedTextureImage(texture, level, bufSize)" },
#endif
#ifdef glGetnTexImage
    { "GetnTexImage",          PyGL_GetnTexImage,          METH_VARARGS, "GetnTexImage(target, level, format, type, bufSize)" },
#endif
#ifdef glReadnPixels
    { "ReadnPixels",           PyGL_ReadnPixels,           METH_VARARGS, "ReadnPixels(x, y, width, height, format, type, bufSize=-1)" },
#endif

#ifdef glClearBufferData
    { "ClearBufferData",       PyGL_ClearBufferData,       METH_VARARGS, "ClearBufferData(target, internalformat, format, type, data)" },
#endif
#ifdef glClearBufferSubData
    { "ClearBufferSubData",    PyGL_ClearBufferSubData,    METH_VARARGS, "ClearBufferSubData(target, internalformat, offset, size, format, type, data)" },
#endif
#ifdef glClearNamedBufferData
    { "ClearNamedBufferData",  PyGL_ClearNamedBufferData,  METH_VARARGS, "ClearNamedBufferData(buffer, internalformat, format, type, data)" },
#endif
#ifdef glClearNamedBufferSubData
    { "ClearNamedBufferSubData", PyGL_ClearNamedBufferSubData, METH_VARARGS, "ClearNamedBufferSubData(buffer, internalformat, offset, size, format, type, data)" },
#endif
#ifdef glClearTexImage
    { "ClearTexImage",         PyGL_ClearTexImage,         METH_VARARGS, "ClearTexImage(texture, level, format, type, data)" },
#endif
#ifdef glClearTexSubImage
    { "ClearTexSubImage",      PyGL_ClearTexSubImage,      METH_VARARGS, "ClearTexSubImage(texture, level, xoffset, yoffset, zoffset, width, height, depth, format, type, data)" },
#endif
#ifdef glClearBufferfv
    { "ClearBufferfv",         PyGL_ClearBufferfv,         METH_VARARGS, "ClearBufferfv(buffer, drawbuffer, value)" },
#endif
#ifdef glClearBufferiv
    { "ClearBufferiv",         PyGL_ClearBufferiv,         METH_VARARGS, "ClearBufferiv(buffer, drawbuffer, value)" },
#endif
#ifdef glClearBufferuiv
    { "ClearBufferuiv",        PyGL_ClearBufferuiv,        METH_VARARGS, "ClearBufferuiv(buffer, drawbuffer, value)" },
#endif
#ifdef glClearNamedFramebufferfv
    { "ClearNamedFramebufferfv", PyGL_ClearNamedFramebufferfv, METH_VARARGS, "ClearNamedFramebufferfv(framebuffer, buffer, drawbuffer, value)" },
#endif
#ifdef glClearNamedFramebufferiv
    { "ClearNamedFramebufferiv", PyGL_ClearNamedFramebufferiv, METH_VARARGS, "ClearNamedFramebufferiv(framebuffer, buffer, drawbuffer, value)" },
#endif
#ifdef glClearNamedFramebufferuiv
    { "ClearNamedFramebufferuiv", PyGL_ClearNamedFramebufferuiv, METH_VARARGS, "ClearNamedFramebufferuiv(framebuffer, buffer, drawbuffer, value)" },
#endif

#ifdef glInvalidateFramebuffer
    { "InvalidateFramebuffer", PyGL_InvalidateFramebuffer, METH_VARARGS, "InvalidateFramebuffer(target, attachments)" },
#endif
#ifdef glInvalidateSubFramebuffer
    { "InvalidateSubFramebuffer", PyGL_InvalidateSubFramebuffer, METH_VARARGS, "InvalidateSubFramebuffer(target, attachments, x, y, width, height)" },
#endif
#ifdef glNamedFramebufferDrawBuffers
    { "NamedFramebufferDrawBuffers", PyGL_NamedFramebufferDrawBuffers, METH_VARARGS, "NamedFramebufferDrawBuffers(framebuffer, bufs)" },
#endif

#ifdef glBindTextures
    { "BindTextures",          PyGL_BindTextures,          METH_VARARGS, "BindTextures(first, textures)" },
#endif
#ifdef glBindSamplers
    { "BindSamplers",          PyGL_BindSamplers,          METH_VARARGS, "BindSamplers(first, samplers)" },
#endif
#ifdef glBindImageTextures
    { "BindImageTextures",     PyGL_BindImageTextures,     METH_VARARGS, "BindImageTextures(first, textures)" },
#endif
#ifdef glBindBuffersBase
    { "BindBuffersBase",       PyGL_BindBuffersBase,       METH_VARARGS, "BindBuffersBase(target, first, buffers)" },
#endif

#ifdef glViewportArrayv
    { "ViewportArrayv",        PyGL_ViewportArrayv,        METH_VARARGS, "ViewportArrayv(first, v)" },
#endif
#ifdef glScissorArrayv
    { "ScissorArrayv",         PyGL_ScissorArrayv,         METH_VARARGS, "ScissorArrayv(first, v)" },
#endif
#ifdef glDepthRangeArrayv
    { "DepthRangeArrayv",      PyGL_DepthRangeArrayv,      METH_VARARGS, "DepthRangeArrayv(first, v)" },
#endif

#ifdef glDebugMessageControl
    { "DebugMessageControl",   PyGL_DebugMessageControl,   METH_VARARGS, "DebugMessageControl(source, type, severity, ids, enabled)" },
#endif
#ifdef glDebugMessageInsert
    { "DebugMessageInsert",    PyGL_DebugMessageInsert,    METH_VARARGS, "DebugMessageInsert(source, type, id, severity, message)" },
#endif
#ifdef glDebugMessageCallback
    { "DebugMessageCallback",  PyGL_DebugMessageCallback,  METH_O,       "DebugMessageCallback(callable_or_None)" },
#endif
#ifdef glGetDebugMessageLog
    { "GetDebugMessageLog",    PyGL_GetDebugMessageLog,    METH_O,       "GetDebugMessageLog(count) -> [(source, type, id, severity, message), ...]" },
#endif
#ifdef glObjectLabel
    { "ObjectLabel",           PyGL_ObjectLabel,           METH_VARARGS, "ObjectLabel(identifier, name, label)" },
#endif
#ifdef glGetObjectLabel
    { "GetObjectLabel",        PyGL_GetObjectLabel,        METH_VARARGS, "GetObjectLabel(identifier, name)" },
#endif
#ifdef glPushDebugGroup
    { "PushDebugGroup",        PyGL_PushDebugGroup,        METH_VARARGS, "PushDebugGroup(source, id, message)" },
#endif

#ifdef glUniform1uiv
    { "Uniform1uiv",           PyGL_Uniform1uiv,           METH_VARARGS, "Uniform1uiv(location, value)" },
#endif
#ifdef glUniform2uiv
    { "Uniform2uiv",           PyGL_Uniform2uiv,           METH_VARARGS, "Uniform2uiv(location, value)" },
#endif
#ifdef glUniform3uiv
    { "Uniform3uiv",           PyGL_Uniform3uiv,           METH_VARARGS, "Uniform3uiv(location, value)" },
#endif
#ifdef glUniform4uiv
    { "Uniform4uiv",           PyGL_Uniform4uiv,           METH_VARARGS, "Uniform4uiv(location, value)" },
#endif
#ifdef glUniform1dv
    { "Uniform1dv",            PyGL_Uniform1dv,            METH_VARARGS, "Uniform1dv(location, value)" },
#endif
#ifdef glUniform2dv
    { "Uniform2dv",            PyGL_Uniform2dv,            METH_VARARGS, "Uniform2dv(location, value)" },
#endif
#ifdef glUniform3dv
    { "Uniform3dv",            PyGL_Uniform3dv,            METH_VARARGS, "Uniform3dv(location, value)" },
#endif
#ifdef glUniform4dv
    { "Uniform4dv",            PyGL_Uniform4dv,            METH_VARARGS, "Uniform4dv(location, value)" },
#endif
#ifdef glUniformMatrix2dv
    { "UniformMatrix2dv",      PyGL_UniformMatrix2dv,      METH_VARARGS, "UniformMatrix2dv(location, transpose, value)" },
#endif
#ifdef glUniformMatrix3dv
    { "UniformMatrix3dv",      PyGL_UniformMatrix3dv,      METH_VARARGS, "UniformMatrix3dv(location, transpose, value)" },
#endif
#ifdef glUniformMatrix4dv
    { "UniformMatrix4dv",      PyGL_UniformMatrix4dv,      METH_VARARGS, "UniformMatrix4dv(location, transpose, value)" },
#endif

#ifdef glProgramUniform1fv
    { "ProgramUniform1fv",     PyGL_ProgramUniform1fv,     METH_VARARGS, "ProgramUniform1fv(program, location, value)" },
#endif
#ifdef glProgramUniform2fv
    { "ProgramUniform2fv",     PyGL_ProgramUniform2fv,     METH_VARARGS, "ProgramUniform2fv(program, location, value)" },
#endif
#ifdef glProgramUniform3fv
    { "ProgramUniform3fv",     PyGL_ProgramUniform3fv,     METH_VARARGS, "ProgramUniform3fv(program, location, value)" },
#endif
#ifdef glProgramUniform4fv
    { "ProgramUniform4fv",     PyGL_ProgramUniform4fv,     METH_VARARGS, "ProgramUniform4fv(program, location, value)" },
#endif
#ifdef glProgramUniform1iv
    { "ProgramUniform1iv",     PyGL_ProgramUniform1iv,     METH_VARARGS, "ProgramUniform1iv(program, location, value)" },
#endif
#ifdef glProgramUniform2iv
    { "ProgramUniform2iv",     PyGL_ProgramUniform2iv,     METH_VARARGS, "ProgramUniform2iv(program, location, value)" },
#endif
#ifdef glProgramUniform3iv
    { "ProgramUniform3iv",     PyGL_ProgramUniform3iv,     METH_VARARGS, "ProgramUniform3iv(program, location, value)" },
#endif
#ifdef glProgramUniform4iv
    { "ProgramUniform4iv",     PyGL_ProgramUniform4iv,     METH_VARARGS, "ProgramUniform4iv(program, location, value)" },
#endif
#ifdef glProgramUniform1uiv
    { "ProgramUniform1uiv",    PyGL_ProgramUniform1uiv,    METH_VARARGS, "ProgramUniform1uiv(program, location, value)" },
#endif
#ifdef glProgramUniform2uiv
    { "ProgramUniform2uiv",    PyGL_ProgramUniform2uiv,    METH_VARARGS, "ProgramUniform2uiv(program, location, value)" },
#endif
#ifdef glProgramUniform3uiv
    { "ProgramUniform3uiv",    PyGL_ProgramUniform3uiv,    METH_VARARGS, "ProgramUniform3uiv(program, location, value)" },
#endif
#ifdef glProgramUniform4uiv
    { "ProgramUniform4uiv",    PyGL_ProgramUniform4uiv,    METH_VARARGS, "ProgramUniform4uiv(program, location, value)" },
#endif
#ifdef glProgramUniformMatrix2fv
    { "ProgramUniformMatrix2fv", PyGL_ProgramUniformMatrix2fv, METH_VARARGS, "ProgramUniformMatrix2fv(program, location, transpose, value)" },
#endif
#ifdef glProgramUniformMatrix3fv
    { "ProgramUniformMatrix3fv", PyGL_ProgramUniformMatrix3fv, METH_VARARGS, "ProgramUniformMatrix3fv(program, location, transpose, value)" },
#endif
#ifdef glProgramUniformMatrix4fv
    { "ProgramUniformMatrix4fv", PyGL_ProgramUniformMatrix4fv, METH_VARARGS, "ProgramUniformMatrix4fv(program, location, transpose, value)" },
#endif

    { NULL }
};
