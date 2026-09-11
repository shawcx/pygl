#ifndef __PYGL_H__
#define __PYGL_H__

#include <Python.h>
#include <structmember.h>

#ifdef WIN32
 #include <Windows.h>
#endif // WIN32

#include <GL/glew.h>

#define  DOC_MOD  "Python wrapper for OpenGL."

// Upper bounds for the fixed stack buffers the wrappers marshal through.
#define  PYGL_MAX_VEC     16   // components in an immediate-mode vector call
#define  PYGL_MAX_QUERY  256   // values returned by a glGet* query
#define  PYGL_MAX_NAMES 4096   // object names generated or deleted at once

// Constants: the generated GL 1.0 - 4.6 core set, plus GLU's overlapping range.
void gl_add_constants(PyObject *mod);
void glu_add_constants(PyObject *mod);

#include "pygl_util.h"

#endif // __PYGL_H__
