#include "pygl.h"

//
// Phase 7: the GL 2.0 / 2.1 program pipeline calls that move strings or
// variable-length arrays. The scalar entry points (CreateShader, CompileShader,
// LinkProgram, UseProgram, Uniform1f, ...) and the plain integer queries
// (GetShaderiv, GetProgramiv) are already generated - only the awkward ones
// live here.
//

// ****************************************************************************
// helpers  (pygl_alloc_* live in pygl_util.h)
// ****************************************************************************

// Turn a (buffer, length) pair filled by a glGet*(...InfoLog|Source) call into
// a Python str.
static PyObject *
pygl_text(const char *buf, GLsizei written) {
    return PyUnicode_FromStringAndSize(buf, written);
}

// ****************************************************************************
// shader source and logs
// ****************************************************************************

static PyObject * PyGL_ShaderSource(PyObject *self, PyObject *pyoArgs) {
    GLuint    shader;
    PyObject *pyoSource;
    if(!PyArg_ParseTuple(pyoArgs, "IO", &shader, &pyoSource)) {
        return NULL;
    }
    if(PyUnicode_Check(pyoSource)) {
        const char *source = PyUnicode_AsUTF8(pyoSource);
        if(NULL == source) {
            return NULL;
        }
        glShaderSource(shader, 1, &source, NULL);
        Py_RETURN_NONE;
    }
    PyObject *fast = PySequence_Fast(pyoSource, "expected str or a sequence of str");
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
    glShaderSource(shader, (GLsizei)n, strings, NULL);
    PyMem_Free(strings);
    Py_DECREF(fast);
    Py_RETURN_NONE;
}

static PyObject * PyGL_GetShaderInfoLog(PyObject *self, PyObject *pyoArg) {
    GLuint shader = (GLuint)PyLong_AsUnsignedLong(pyoArg);
    if(PyErr_Occurred()) {
        return NULL;
    }
    GLint length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    if(length <= 0) {
        return PyUnicode_FromString("");
    }
    PyObject *buf = PyBytes_FromStringAndSize(NULL, length);
    if(NULL == buf) {
        return NULL;
    }
    GLsizei written = 0;
    glGetShaderInfoLog(shader, length, &written, PyBytes_AS_STRING(buf));
    PyObject *text = pygl_text(PyBytes_AS_STRING(buf), written);
    Py_DECREF(buf);
    return text;
}

static PyObject * PyGL_GetProgramInfoLog(PyObject *self, PyObject *pyoArg) {
    GLuint program = (GLuint)PyLong_AsUnsignedLong(pyoArg);
    if(PyErr_Occurred()) {
        return NULL;
    }
    GLint length = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    if(length <= 0) {
        return PyUnicode_FromString("");
    }
    PyObject *buf = PyBytes_FromStringAndSize(NULL, length);
    if(NULL == buf) {
        return NULL;
    }
    GLsizei written = 0;
    glGetProgramInfoLog(program, length, &written, PyBytes_AS_STRING(buf));
    PyObject *text = pygl_text(PyBytes_AS_STRING(buf), written);
    Py_DECREF(buf);
    return text;
}

static PyObject * PyGL_GetShaderSource(PyObject *self, PyObject *pyoArg) {
    GLuint shader = (GLuint)PyLong_AsUnsignedLong(pyoArg);
    if(PyErr_Occurred()) {
        return NULL;
    }
    GLint length = 0;
    glGetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &length);
    if(length <= 0) {
        return PyUnicode_FromString("");
    }
    PyObject *buf = PyBytes_FromStringAndSize(NULL, length);
    if(NULL == buf) {
        return NULL;
    }
    GLsizei written = 0;
    glGetShaderSource(shader, length, &written, PyBytes_AS_STRING(buf));
    PyObject *text = pygl_text(PyBytes_AS_STRING(buf), written);
    Py_DECREF(buf);
    return text;
}

// ****************************************************************************
// program introspection
// ****************************************************************************

// which: 0 = attribute, 1 = uniform
static PyObject * pygl_active_var(GLuint program, GLuint index, int uniform) {
    GLint maxlen = 0;
    glGetProgramiv(program, uniform ? GL_ACTIVE_UNIFORM_MAX_LENGTH
                                    : GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxlen);
    if(maxlen <= 0) {
        maxlen = 256;
    }
    PyObject *buf = PyBytes_FromStringAndSize(NULL, maxlen);
    if(NULL == buf) {
        return NULL;
    }
    GLsizei written = 0;
    GLint   size    = 0;
    GLenum  type    = 0;
    if(uniform) {
        glGetActiveUniform(program, index, maxlen, &written, &size, &type, PyBytes_AS_STRING(buf));
    }
    else {
        glGetActiveAttrib(program, index, maxlen, &written, &size, &type, PyBytes_AS_STRING(buf));
    }
    PyObject *name = PyUnicode_FromStringAndSize(PyBytes_AS_STRING(buf), written);
    Py_DECREF(buf);
    if(NULL == name) {
        return NULL;
    }
    return Py_BuildValue("(NiI)", name, size, type);
}

static PyObject * PyGL_GetActiveAttrib(PyObject *self, PyObject *pyoArgs) {
    GLuint program, index;
    if(!PyArg_ParseTuple(pyoArgs, "II", &program, &index)) {
        return NULL;
    }
    return pygl_active_var(program, index, 0);
}

static PyObject * PyGL_GetActiveUniform(PyObject *self, PyObject *pyoArgs) {
    GLuint program, index;
    if(!PyArg_ParseTuple(pyoArgs, "II", &program, &index)) {
        return NULL;
    }
    return pygl_active_var(program, index, 1);
}

static PyObject * PyGL_GetAttachedShaders(PyObject *self, PyObject *pyoArg) {
    GLuint program = (GLuint)PyLong_AsUnsignedLong(pyoArg);
    if(PyErr_Occurred()) {
        return NULL;
    }
    GLint count = 0;
    glGetProgramiv(program, GL_ATTACHED_SHADERS, &count);
    if(count <= 0) {
        return PyList_New(0);
    }
    GLuint *shaders = PyMem_Malloc(sizeof(GLuint) * count);
    if(NULL == shaders) {
        return PyErr_NoMemory();
    }
    GLsizei written = 0;
    glGetAttachedShaders(program, count, &written, shaders);
    PyObject *list = pygl_list_uints(shaders, written);
    PyMem_Free(shaders);
    return list;
}

static PyObject * PyGL_BindAttribLocation(PyObject *self, PyObject *pyoArgs) {
    GLuint      program, index;
    const char *name;
    if(!PyArg_ParseTuple(pyoArgs, "IIs", &program, &index, &name)) {
        return NULL;
    }
    glBindAttribLocation(program, index, name);
    Py_RETURN_NONE;
}

static PyObject * PyGL_GetAttribLocation(PyObject *self, PyObject *pyoArgs) {
    GLuint      program;
    const char *name;
    if(!PyArg_ParseTuple(pyoArgs, "Is", &program, &name)) {
        return NULL;
    }
    return Py_BuildValue("i", glGetAttribLocation(program, name));
}

static PyObject * PyGL_GetUniformLocation(PyObject *self, PyObject *pyoArgs) {
    GLuint      program;
    const char *name;
    if(!PyArg_ParseTuple(pyoArgs, "Is", &program, &name)) {
        return NULL;
    }
    return Py_BuildValue("i", glGetUniformLocation(program, name));
}

static PyObject * PyGL_GetUniformfv(PyObject *self, PyObject *pyoArgs) {
    GLuint     program;
    GLint      location;
    Py_ssize_t count = 1;
    if(!PyArg_ParseTuple(pyoArgs, "Ii|n", &program, &location, &count)) {
        return NULL;
    }
    if(count < 1 || count > PYGL_MAX_QUERY) {
        PyErr_SetString(PyExc_ValueError, "invalid count");
        return NULL;
    }
    GLfloat values[PYGL_MAX_QUERY];
    glGetUniformfv(program, location, values);
    return pygl_build_floats(values, count);
}

static PyObject * PyGL_GetUniformiv(PyObject *self, PyObject *pyoArgs) {
    GLuint     program;
    GLint      location;
    Py_ssize_t count = 1;
    if(!PyArg_ParseTuple(pyoArgs, "Ii|n", &program, &location, &count)) {
        return NULL;
    }
    if(count < 1 || count > PYGL_MAX_QUERY) {
        PyErr_SetString(PyExc_ValueError, "invalid count");
        return NULL;
    }
    GLint values[PYGL_MAX_QUERY];
    glGetUniformiv(program, location, values);
    return pygl_build_ints(values, count);
}

// ****************************************************************************
// uniform array and matrix setters
// ****************************************************************************

#define PYGL_UNIFORM_V(PY, COMPONENTS, ALLOC, FREE_T)                          \
static PyObject * PyGL_##PY(PyObject *self, PyObject *pyoArgs) {                \
    GLint     location;                                                        \
    PyObject *pyoValue;                                                        \
    if(!PyArg_ParseTuple(pyoArgs, "iO", &location, &pyoValue)) {               \
        return NULL;                                                           \
    }                                                                         \
    Py_ssize_t n;                                                              \
    FREE_T *value = ALLOC(pyoValue, &n);                                       \
    if(NULL == value) {                                                        \
        return NULL;                                                           \
    }                                                                         \
    if(0 != n % COMPONENTS) {                                                  \
        PyMem_Free(value);                                                     \
        PyErr_SetString(PyExc_ValueError, "length is not a multiple of " #COMPONENTS); \
        return NULL;                                                           \
    }                                                                         \
    gl##PY(location, (GLsizei)(n / COMPONENTS), value);                        \
    PyMem_Free(value);                                                         \
    Py_RETURN_NONE;                                                            \
}

PYGL_UNIFORM_V( Uniform1fv, 1, pygl_alloc_floats, GLfloat )
PYGL_UNIFORM_V( Uniform2fv, 2, pygl_alloc_floats, GLfloat )
PYGL_UNIFORM_V( Uniform3fv, 3, pygl_alloc_floats, GLfloat )
PYGL_UNIFORM_V( Uniform4fv, 4, pygl_alloc_floats, GLfloat )
PYGL_UNIFORM_V( Uniform1iv, 1, pygl_alloc_ints,   GLint   )
PYGL_UNIFORM_V( Uniform2iv, 2, pygl_alloc_ints,   GLint   )
PYGL_UNIFORM_V( Uniform3iv, 3, pygl_alloc_ints,   GLint   )
PYGL_UNIFORM_V( Uniform4iv, 4, pygl_alloc_ints,   GLint   )

#define PYGL_UNIFORM_MAT(PY, ELEMENTS, ALLOC, T)                               \
static PyObject * PyGL_##PY(PyObject *self, PyObject *pyoArgs) {                \
    GLint     location;                                                        \
    GLboolean transpose;                                                       \
    PyObject *pyoValue;                                                        \
    if(!PyArg_ParseTuple(pyoArgs, "ibO", &location, &transpose, &pyoValue)) {  \
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
    gl##PY(location, (GLsizei)(n / ELEMENTS), transpose, value);               \
    PyMem_Free(value);                                                         \
    Py_RETURN_NONE;                                                            \
}

PYGL_UNIFORM_MAT( UniformMatrix2fv,    4,  pygl_alloc_floats, GLfloat )
PYGL_UNIFORM_MAT( UniformMatrix3fv,    9,  pygl_alloc_floats, GLfloat )
PYGL_UNIFORM_MAT( UniformMatrix4fv,    16, pygl_alloc_floats, GLfloat )
PYGL_UNIFORM_MAT( UniformMatrix2x3fv,  6,  pygl_alloc_floats, GLfloat )
PYGL_UNIFORM_MAT( UniformMatrix3x2fv,  6,  pygl_alloc_floats, GLfloat )
PYGL_UNIFORM_MAT( UniformMatrix2x4fv,  8,  pygl_alloc_floats, GLfloat )
PYGL_UNIFORM_MAT( UniformMatrix4x2fv,  8,  pygl_alloc_floats, GLfloat )
PYGL_UNIFORM_MAT( UniformMatrix3x4fv,  12, pygl_alloc_floats, GLfloat )
PYGL_UNIFORM_MAT( UniformMatrix4x3fv,  12, pygl_alloc_floats, GLfloat )

// ****************************************************************************
// draw buffers
// ****************************************************************************

static PyObject * PyGL_DrawBuffers(PyObject *self, PyObject *pyoArg) {
    GLuint bufs[PYGL_MAX_VEC];
    Py_ssize_t n = pygl_uints(pyoArg, bufs, PYGL_MAX_VEC);
    if(n < 0) {
        return NULL;
    }
    glDrawBuffers((GLsizei)n, (const GLenum *)bufs);
    Py_RETURN_NONE;
}

// ****************************************************************************

static PyMethodDef PyGL_shader_methods[] = {
    { "ShaderSource",        PyGL_ShaderSource,        METH_VARARGS, "ShaderSource(shader, source)" },
    { "GetShaderInfoLog",    PyGL_GetShaderInfoLog,    METH_O,       "GetShaderInfoLog(shader)" },
    { "GetProgramInfoLog",   PyGL_GetProgramInfoLog,   METH_O,       "GetProgramInfoLog(program)" },
    { "GetShaderSource",     PyGL_GetShaderSource,     METH_O,       "GetShaderSource(shader)" },

    { "GetActiveAttrib",     PyGL_GetActiveAttrib,     METH_VARARGS, "GetActiveAttrib(program, index) -> (name, size, type)" },
    { "GetActiveUniform",    PyGL_GetActiveUniform,    METH_VARARGS, "GetActiveUniform(program, index) -> (name, size, type)" },
    { "GetAttachedShaders",  PyGL_GetAttachedShaders,  METH_O,       "GetAttachedShaders(program)" },
    { "BindAttribLocation",  PyGL_BindAttribLocation,  METH_VARARGS, "BindAttribLocation(program, index, name)" },
    { "GetAttribLocation",   PyGL_GetAttribLocation,   METH_VARARGS, "GetAttribLocation(program, name)" },
    { "GetUniformLocation",  PyGL_GetUniformLocation,  METH_VARARGS, "GetUniformLocation(program, name)" },
    { "GetUniformfv",        PyGL_GetUniformfv,        METH_VARARGS, "GetUniformfv(program, location, count=1)" },
    { "GetUniformiv",        PyGL_GetUniformiv,        METH_VARARGS, "GetUniformiv(program, location, count=1)" },

    { "Uniform1fv",          PyGL_Uniform1fv,          METH_VARARGS, "Uniform1fv(location, value)" },
    { "Uniform2fv",          PyGL_Uniform2fv,          METH_VARARGS, "Uniform2fv(location, value)" },
    { "Uniform3fv",          PyGL_Uniform3fv,          METH_VARARGS, "Uniform3fv(location, value)" },
    { "Uniform4fv",          PyGL_Uniform4fv,          METH_VARARGS, "Uniform4fv(location, value)" },
    { "Uniform1iv",          PyGL_Uniform1iv,          METH_VARARGS, "Uniform1iv(location, value)" },
    { "Uniform2iv",          PyGL_Uniform2iv,          METH_VARARGS, "Uniform2iv(location, value)" },
    { "Uniform3iv",          PyGL_Uniform3iv,          METH_VARARGS, "Uniform3iv(location, value)" },
    { "Uniform4iv",          PyGL_Uniform4iv,          METH_VARARGS, "Uniform4iv(location, value)" },

    { "UniformMatrix2fv",    PyGL_UniformMatrix2fv,    METH_VARARGS, "UniformMatrix2fv(location, transpose, value)" },
    { "UniformMatrix3fv",    PyGL_UniformMatrix3fv,    METH_VARARGS, "UniformMatrix3fv(location, transpose, value)" },
    { "UniformMatrix4fv",    PyGL_UniformMatrix4fv,    METH_VARARGS, "UniformMatrix4fv(location, transpose, value)" },
    { "UniformMatrix2x3fv",  PyGL_UniformMatrix2x3fv,  METH_VARARGS, "UniformMatrix2x3fv(location, transpose, value)" },
    { "UniformMatrix3x2fv",  PyGL_UniformMatrix3x2fv,  METH_VARARGS, "UniformMatrix3x2fv(location, transpose, value)" },
    { "UniformMatrix2x4fv",  PyGL_UniformMatrix2x4fv,  METH_VARARGS, "UniformMatrix2x4fv(location, transpose, value)" },
    { "UniformMatrix4x2fv",  PyGL_UniformMatrix4x2fv,  METH_VARARGS, "UniformMatrix4x2fv(location, transpose, value)" },
    { "UniformMatrix3x4fv",  PyGL_UniformMatrix3x4fv,  METH_VARARGS, "UniformMatrix3x4fv(location, transpose, value)" },
    { "UniformMatrix4x3fv",  PyGL_UniformMatrix4x3fv,  METH_VARARGS, "UniformMatrix4x3fv(location, transpose, value)" },

    { "DrawBuffers",         PyGL_DrawBuffers,         METH_O,       "DrawBuffers(bufs)" },

    { NULL }
};
