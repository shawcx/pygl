#include "pygl.h"

// ****************************************************************************
// GLEW
// ****************************************************************************

static PyObject * PyGL_glewInit(PyObject *self, PyObject *pyo) {
    if(0 == glewInit()) { Py_RETURN_TRUE; } else { Py_RETURN_FALSE; }
}

static PyMethodDef PyGL_core_methods[] = {
    { "glewInit", PyGL_glewInit, METH_NOARGS, "glewInit()" },
    { NULL }
};

// ****************************************************************************
// wrapper translation units, compiled as one unit through pygl.c
// ****************************************************************************

#include "generated/wrappers.c"
#include "manual.c"
#include "data.c"
#include "shaders.c"
#include "modern.c"
#include "glu.c"
#include "generated/constants.c"

// ****************************************************************************
// module
// ****************************************************************************

static struct PyModuleDef pygl_module = {
    PyModuleDef_HEAD_INIT,
    "pygl",
    DOC_MOD,
    -1,
    NULL
};

PyMODINIT_FUNC PyInit_pygl(void) {
    PyObject *mod = PyModule_Create(&pygl_module);
    if(NULL == mod) {
        return NULL;
    }

    if(PyModule_AddFunctions(mod, PyGL_core_methods)
    || PyModule_AddFunctions(mod, PyGL_generated_methods)
    || PyModule_AddFunctions(mod, PyGL_manual_methods)
    || PyModule_AddFunctions(mod, PyGL_data_methods)
    || PyModule_AddFunctions(mod, PyGL_shader_methods)
    || PyModule_AddFunctions(mod, PyGL_modern_methods)
    || PyModule_AddFunctions(mod, PyGL_glu_methods)) {
        Py_DECREF(mod);
        return NULL;
    }

    gl_add_constants(mod);
    glu_add_constants(mod);

    return mod;
}
