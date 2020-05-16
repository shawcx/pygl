#include "pygl.h"

static PyModuleDef pygl_module = {
    PyModuleDef_HEAD_INIT,
    "pygl",
    NULL,
    -1,
    PyGL_methods
};

PyMODINIT_FUNC PyInit_pygl(void) {
    PyObject *mod;
    mod = PyModule_Create(&pygl_module);
    if(NULL == mod) {
        return NULL;
    }
    add_constants(mod);
    return mod;
}

static PyObject * PyGL_glewInit(PyObject *self, PyObject *pyo) {
    if(0 == glewInit()) { Py_RETURN_TRUE; } else { Py_RETURN_FALSE; }
}

// ****************************************************************************
// management functions
// ****************************************************************************

static PyObject * PyGL_Enable(PyObject *self, PyObject *pyoCap) {
    glEnable(PyLong_AsLong(pyoCap));
    Py_RETURN_NONE;
}

static PyObject * PyGL_Disable(PyObject *self, PyObject *pyoCap) {
    glDisable(PyLong_AsLong(pyoCap));
    Py_RETURN_NONE;
}

static PyObject * PyGL_IsEnabled(PyObject *self, PyObject *pyoCap) {
    GLboolean isEnabled = glIsEnabled(PyLong_AsLong(pyoCap));
    if(isEnabled) { Py_RETURN_TRUE; } else { Py_RETURN_FALSE; }
}

static PyObject * PyGL_Hint(PyObject *self, PyObject *pyoArgs) {
    GLenum target = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 0));
    GLenum mode   = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 1));
    glHint(target, mode);
    Py_RETURN_NONE;
}

static PyObject * PyGL_BlendFunc(PyObject *self, PyObject *pyoArgs) {
    GLenum src = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 0));
    GLenum dst = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 1));
    glBlendFunc(src, dst);
    Py_RETURN_NONE;
}

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

static PyObject * PyGL_GetIntegerv(PyObject * self, PyObject * pyoArgs) {
    GLenum name = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 0));
    Py_ssize_t size = 1;
    if(1 < PyTuple_Size(pyoArgs)) {
        size = PyLong_AsSsize_t(PyTuple_GetItem(pyoArgs, 1));
    }
    GLint *values = (GLint *)alloca(sizeof(GLint) * size);
    glGetIntegerv(name, values);
    PyObject *pyValues = PyList_New(size);
    for(int idx = 0; idx < size; ++idx) {
        PyList_SetItem(pyValues, idx, PyLong_FromLong(values[idx]));
    }
    return pyValues;
}

// ****************************************************************************
// matrix functions
// ****************************************************************************

static PyObject * PyGL_PushMatrix(PyObject *self, PyObject *pyo) {
    glPushMatrix();
    Py_RETURN_NONE;
}

static PyObject * PyGL_PopMatrix(PyObject *self, PyObject *pyo) {
    glPopMatrix();
    Py_RETURN_NONE;
}

static PyObject * PyGL_MatrixMode(PyObject *self, PyObject *pyoMode) {
    glMatrixMode(PyLong_AsUnsignedLong(pyoMode));
    Py_RETURN_NONE;
}

static PyObject * PyGL_LoadIdentity(PyObject *self, PyObject *pyo) {
    glLoadIdentity();
    Py_RETURN_NONE;
}

static PyObject * PyGL_Finish(PyObject *self, PyObject *pyo) {
    glFinish();
    Py_RETURN_NONE;
}

static PyObject * PyGL_Flush(PyObject *self, PyObject *pyo) {
    glFlush();
    Py_RETURN_NONE;
}

// ****************************************************************************
// camera functions
// ****************************************************************************

static PyObject * PyGL_Viewport(PyObject *self, PyObject *pyoArgs) {
    GLint x = (GLint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 0));
    GLint y = (GLint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 1));
    GLsizei w = (GLsizei)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 2));
    GLsizei h = (GLsizei)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 3));
    glViewport(x, y, w, h);
    Py_RETURN_NONE;
}

static PyObject * PyGL_Ortho(PyObject *self, PyObject *pyoArgs) {
    GLdouble l = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 0));
    GLdouble r = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 1));
    GLdouble b = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 2));
    GLdouble t = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 3));
    GLdouble n = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 4));
    GLdouble f = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 5));
    glOrtho(l, r, b, t, n, f);
    Py_RETURN_NONE;
}

static PyObject * PyGL_Frustum(PyObject *self, PyObject *pyoArgs) {
    GLdouble l = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 0));
    GLdouble r = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 1));
    GLdouble b = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 2));
    GLdouble t = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 3));
    GLdouble n = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 4));
    GLdouble f = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 5));
    glFrustum(l, r, b, t, n, f);
    Py_RETURN_NONE;
}

static PyObject * PyGL_DepthRange(PyObject *self, PyObject *pyoArgs) {
    GLclampd n = (GLclampd)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 0));
    GLclampd f = (GLclampd)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 1));
    glDepthRange(n, f);
    Py_RETURN_NONE;
}

// ****************************************************************************
// buffers
// ****************************************************************************

static PyObject * PyGL_Clear(PyObject *self, PyObject *pyoFlags) {
    glClear(PyLong_AsLong(pyoFlags));
    Py_RETURN_NONE;
}

static PyObject * PyGL_ClearColor(PyObject *self, PyObject *pyoArgs) {
    float r, g, b, a;
    int ok = PyArg_ParseTuple(pyoArgs, "ffff", &r, &g, &b, &a);
    if(!ok) {
        return NULL;
    }
    glClearColor(r, g, b, a);
    Py_RETURN_NONE;
}

static PyObject * PyGL_ClearDepth(PyObject *self, PyObject *pyoDepth) {
    glClearDepth(PyFloat_AsDouble(pyoDepth));
    Py_RETURN_NONE;
}

// ****************************************************************************
// call list functions
// ****************************************************************************

static PyObject * PyGL_GenLists(PyObject *self, PyObject *pyoRange) {
    GLsizei size = (GLsizei)PyLong_AsUnsignedLong(pyoRange);
    GLuint range = glGenLists(size);
    return Py_BuildValue("I", range);
}

static PyObject * PyGL_NewList(PyObject *self, PyObject *pyoArgs) {
    GLuint idx = (GLuint)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 0));
    GLenum mode;
    if(2 == PyTuple_Size(pyoArgs)) {
        mode = (GLuint)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 1));
    }
    else {
        mode = GL_COMPILE;
    }
    glNewList(idx, mode);
    Py_RETURN_NONE;
}

static PyObject * PyGL_EndList(PyObject *self, PyObject *pyo) {
    glEndList();
    Py_RETURN_NONE;
}

static PyObject * PyGL_CallList(PyObject *self, PyObject *pyoList) {
    GLuint idx = (GLuint)PyLong_AsUnsignedLong(pyoList);
    glCallList(idx);
    Py_RETURN_NONE;
}

static PyObject * PyGL_IsList(PyObject *self, PyObject *pyoList) {
    GLuint idx = (GLuint)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoList, 0));
    if(glIsList(idx)) { Py_RETURN_TRUE; } else { Py_RETURN_FALSE; }
}

static PyObject * PyGL_DeleteLists(PyObject *self, PyObject *pyoArgs) {
    GLuint  idx   = (GLuint)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 0));
    GLsizei range = (GLsizei)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 1));
    glDeleteLists(idx, range);
    Py_RETURN_NONE;
}

// ****************************************************************************
// drawing functions
// ****************************************************************************

static PyObject * PyGL_Begin(PyObject *self, PyObject *pyoMode) {
    glBegin(PyLong_AsUnsignedLong(pyoMode));
    Py_RETURN_NONE;
}

static PyObject * PyGL_End(PyObject *self, PyObject *pyo) {
    glEnd();
    Py_RETURN_NONE;
}

static PyObject * PyGL_PointSize(PyObject *self, PyObject *pyoSize) {
    glPointSize((GLfloat)PyFloat_AsDouble(pyoSize));
    Py_RETURN_NONE;
}

static PyObject * PyGL_LineWidth(PyObject *self, PyObject *pyoWidth) {
    glLineWidth((GLfloat)PyFloat_AsDouble(pyoWidth));
    Py_RETURN_NONE;
}

// ****************************************************************************
// vertex functions
// ****************************************************************************

static PyObject * PyGL_Vertex2f(PyObject *self, PyObject *pyoArgs) {
    GLfloat x = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 0));
    GLfloat y = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 1));
    glVertex2f(x, y);
    Py_RETURN_NONE;
}

static PyObject * PyGL_Vertex3d(PyObject *self, PyObject *pyoArgs) {
    GLdouble x = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 0));
    GLdouble y = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 1));
    GLdouble z = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 2));
    glVertex3d(x, y, z);
    Py_RETURN_NONE;
}

static PyObject * PyGL_Vertex3f(PyObject *self, PyObject *pyoArgs) {
    GLfloat x = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 0));
    GLfloat y = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 1));
    GLfloat z = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 2));
    glVertex3f(x, y, z);
    Py_RETURN_NONE;
}

// ****************************************************************************
// normal functions
// ****************************************************************************

static PyObject * PyGL_Normal3f(PyObject *self, PyObject *pyoArgs) {
    GLfloat x = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 0));
    GLfloat y = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 1));
    GLfloat z = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 2));
    glNormal3f(x, y, z);
    Py_RETURN_NONE;
}


// ****************************************************************************

static PyObject * PyGL_PolygonMode(PyObject *self, PyObject *pyoArgs) {
    GLenum face = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 0));
    GLenum mode = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 1));
    glPolygonMode(face, mode);
    Py_RETURN_NONE;
}

static PyObject * PyGL_FrontFace(PyObject *self, PyObject *pyoMode) {
    glFrontFace(PyLong_AsUnsignedLong(pyoMode));
    Py_RETURN_NONE;
}

static PyObject * PyGL_CullFace(PyObject *self, PyObject *pyoMode) {
    glCullFace(PyLong_AsUnsignedLong(pyoMode));
    Py_RETURN_NONE;
}

static PyObject * PyGL_EdgeFlag(PyObject *self, PyObject *pyoBool) {
    glEdgeFlag((GLboolean)PyLong_AsUnsignedLong(pyoBool));
    Py_RETURN_NONE;
}

// ****************************************************************************
// transform functions
// ****************************************************************************

static PyObject * PyGL_Rotated(PyObject *self, PyObject *pyoArgs) {
    GLdouble a = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 0));
    GLdouble x = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 1));
    GLdouble y = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 2));
    GLdouble z = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 3));
    glRotated(a, x, y, z);
    Py_RETURN_NONE;
}

static PyObject * PyGL_Rotatef(PyObject *self, PyObject *pyoArgs) {
    GLfloat a = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 0));
    GLfloat x = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 1));
    GLfloat y = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 2));
    GLfloat z = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 3));
    glRotatef(a, x, y, z);
    Py_RETURN_NONE;
}

static PyObject * PyGL_Translated(PyObject *self, PyObject *pyoArgs) {
    GLdouble x = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 0));
    GLdouble y = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 1));
    GLdouble z = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 2));
    glTranslated(x, y, z);
    Py_RETURN_NONE;
}

static PyObject * PyGL_Translatef(PyObject *self, PyObject *pyoArgs) {
    GLfloat x = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 0));
    GLfloat y = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 1));
    GLfloat z = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 2));
    glTranslatef(x, y, z);
    Py_RETURN_NONE;
}

static PyObject * PyGL_Scaled(PyObject *self, PyObject *pyoArgs) {
    GLdouble x = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 0));
    GLdouble y = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 1));
    GLdouble z = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 2));
    glScaled(x, y, z);
    Py_RETURN_NONE;
}

static PyObject * PyGL_Scalef(PyObject *self, PyObject *pyoArgs) {
    GLfloat x = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 0));
    GLfloat y = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 1));
    GLfloat z = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 2));
    glScalef(x, y, z);
    Py_RETURN_NONE;
}

// ****************************************************************************
// shape functions
// ****************************************************************************

static PyObject * PyGL_Rectd(PyObject *self, PyObject *pyoArgs) {
    GLdouble x1 = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 0));
    GLdouble y1 = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 1));
    GLdouble x2 = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 2));
    GLdouble y2 = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 2));
    glRectd(x1, y1, x2, y2);
    Py_RETURN_NONE;
}

static PyObject * PyGL_Rectf(PyObject *self, PyObject *pyoArgs) {
    GLfloat x1 = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 0));
    GLfloat y1 = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 1));
    GLfloat x2 = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 2));
    GLfloat y2 = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 2));
    glRectf(x1, y1, x2, y2);
    Py_RETURN_NONE;
}

// ****************************************************************************
// color functions
// ****************************************************************************

//static PyObject * PyGL_Color3b ( PyObject * self, PyObject * pyoColors );
//static PyObject * PyGL_Color3s ( PyObject * self, PyObject * pyoColors );
//static PyObject * PyGL_Color3i ( PyObject * self, PyObject * pyoColors );

static PyObject * PyGL_Color3d(PyObject * self, PyObject * pyoColors) {
    GLdouble r = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoColors, 0));
    GLdouble g = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoColors, 1));
    GLdouble b = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoColors, 2));
    glColor3d(r, g, b);
    Py_RETURN_NONE;
}

static PyObject * PyGL_Color3f(PyObject * self, PyObject * pyoColors) {
    GLfloat r = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoColors, 0));
    GLfloat g = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoColors, 1));
    GLfloat b = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoColors, 2));
    glColor3f(r, g, b);
    Py_RETURN_NONE;
}

static PyObject * PyGL_Color4f(PyObject * self, PyObject * pyoColors) {
    GLfloat r = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoColors, 0));
    GLfloat g = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoColors, 1));
    GLfloat b = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoColors, 2));
    GLfloat a = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoColors, 3));
    glColor4f(r, g, b, a);
    Py_RETURN_NONE;
}

static PyObject * PyGL_ShadeModel(PyObject *self, PyObject *pyoShade) {
    glShadeModel(PyLong_AsUnsignedLong(pyoShade));
    Py_RETURN_NONE;
}

// ****************************************************************************
// lighting functions
// ****************************************************************************

static PyObject * PyGL_Lighti(PyObject *self, PyObject *pyoArgs) {
    GLenum light = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 0));
    GLenum name  = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 1));
    GLint param = (GLint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 2));
    glLighti(light, name, param);
    Py_RETURN_NONE;
}

static PyObject * PyGL_Lightf(PyObject *self, PyObject *pyoArgs) {
    GLenum light = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 0));
    GLenum name  = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 1));
    GLfloat param = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 2));
    glLightf(light, name, param);
    Py_RETURN_NONE;
}

static PyObject * PyGL_Lightiv(PyObject *self, PyObject *pyoArgs) {
    GLenum light = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 0));
    GLenum name  = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 1));
    PyObject *pyoList = PyTuple_GetItem(pyoArgs, 2);
    Py_ssize_t size = PyList_Size(pyoList);
    GLint *args = (GLint *)alloca(sizeof(GLint) * size);
    for(int idx = 0; idx < size; ++idx) {
        args[idx] = (GLint)PyLong_AsLong(PyList_GetItem(pyoList, idx));
    }
    glLightiv(light, name, args);
    Py_RETURN_NONE;
}

static PyObject * PyGL_Lightfv(PyObject *self, PyObject *pyoArgs) {
    GLenum light = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 0));
    GLenum name  = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 1));
    PyObject *pyoList = PyTuple_GetItem(pyoArgs, 2);
    Py_ssize_t size = PyList_Size(pyoList);
    GLfloat *args = (GLfloat *)alloca(sizeof(GLfloat) * size);
    for(int idx = 0; idx < size; ++idx) {
        args[idx] = (GLfloat)PyFloat_AsDouble(PyList_GetItem(pyoList, idx));
    }
    glLightfv(light, name, args);
    Py_RETURN_NONE;
}

// ****************************************************************************
// material functions
// ****************************************************************************

static PyObject * PyGL_Materiali(PyObject *self, PyObject *pyoArgs) {
    GLenum face = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 0));
    GLenum name = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 1));
    GLint param = (GLint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 2));
    glMateriali(face, name, param);
    Py_RETURN_NONE;
}

static PyObject * PyGL_Materialf(PyObject *self, PyObject *pyoArgs) {
    GLenum face = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 0));
    GLenum name = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 1));
    GLfloat param = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 2));
    glMaterialf(face, name, param);
    Py_RETURN_NONE;
}

static PyObject * PyGL_Materialiv(PyObject *self, PyObject *pyoArgs) {
    GLenum face = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 0));
    GLenum name = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 1));
    PyObject *pyoList = PyTuple_GetItem(pyoArgs, 2);
    Py_ssize_t size = PyList_Size(pyoList);
    GLint *args = (GLint *)alloca(sizeof(GLint) * size);
    for(int idx = 0; idx < size; ++idx) {
        args[idx] = (GLint)PyLong_AsLong(PyList_GetItem(pyoList, idx));
    }
    glMaterialiv(face, name, args);
    Py_RETURN_NONE;
}

static PyObject * PyGL_Materialfv(PyObject *self, PyObject *pyoArgs) {
    GLenum face = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 0));
    GLenum name = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 1));
    PyObject *pyoList = PyTuple_GetItem(pyoArgs, 2);
    Py_ssize_t size = PyList_Size(pyoList);
    GLfloat *args = (GLfloat *)alloca(sizeof(GLfloat) * size);
    for(int idx = 0; idx < size; ++idx) {
        args[idx] = (GLfloat)PyFloat_AsDouble(PyList_GetItem(pyoList, idx));
    }
    glMaterialfv(face, name, args);
    Py_RETURN_NONE;
}

// ****************************************************************************
// texture functions
// ****************************************************************************

static PyObject * PyGL_GenTextures(PyObject *self, PyObject *pyoNum) {
    int nSize = PyLong_AsLong(pyoNum);
    GLuint *textures = (GLuint *)alloca(sizeof(GLuint) * nSize);
    glGenTextures(nSize, textures);
    PyObject *pyoList = PyList_New(nSize);
    for(int idx = 0; idx < nSize; ++idx) {
        PyList_SetItem(pyoList, idx, PyLong_FromLong(textures[idx]));
    }
    return pyoList;
}

static PyObject * PyGL_DeleteTextures(PyObject *self, PyObject *pyoList) {
    Py_ssize_t size = PyList_Size(pyoList);
    GLuint *textures = (GLuint *)alloca(sizeof(GLuint) * size);
    for(int idx = 0; idx < size; ++idx) {
        textures[idx] = PyLong_AsLong(PyList_GetItem(pyoList, idx));
    }
    glDeleteTextures((GLsizei)size, textures);
    Py_RETURN_NONE;
}

static PyObject * PyGL_IsTexture(PyObject *self, PyObject *pyoTexture) {
    GLuint idx = (GLuint)PyLong_AsUnsignedLong(pyoTexture);
    if(glIsTexture(idx)) { Py_RETURN_TRUE; } else { Py_RETURN_FALSE; }
}

static PyObject * PyGL_BindTexture(PyObject *self, PyObject *pyoArgs) {
    GLenum target  = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 0));
    GLuint texture = (GLuint)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 1));
    glBindTexture(target, texture);
    Py_RETURN_NONE;
}

static PyObject * PyGL_TexCoord2i(PyObject *self, PyObject *pyoArgs) {
    GLint s = (GLint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 0));
    GLint t = (GLint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 1));
    glTexCoord2i(s, t);
    Py_RETURN_NONE;
}

static PyObject * PyGL_TexImage2D(PyObject *self, PyObject *pyoArgs) {
    int target, level, intfmt, w, h, border, fmt, type, size;
    uint8_t *pixels;
    int ok = PyArg_ParseTuple(pyoArgs, "iiiiiiiis#", &target, &level, &intfmt, &w, &h, &border, &fmt, &type, &pixels, &size);
    if(!ok) {
        return NULL;
    }
    glTexImage2D(target, level, intfmt, w, h, border, fmt, type, pixels);
    Py_RETURN_NONE;
}

static PyObject * PyGL_TexSubImage2D(PyObject *self, PyObject *pyoArgs) {
    GLenum  target = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 0));
    GLint   level  = (GLint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 1));
    GLint   x      = (GLint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 2));
    GLint   y      = (GLint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 3));
    GLsizei w      = (GLsizei)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 4));
    GLsizei h      = (GLsizei)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 5));
    GLenum  fmt    = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 6));
    GLenum  type   = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 7));
    uint8_t * pixels = (uint8_t *)PyBytes_AsString(PyTuple_GetItem(pyoArgs, 8));
    glTexSubImage2D(target, level, x, y, w, h, fmt, type, pixels);
    Py_RETURN_NONE;
}

static PyObject * PyGL_TexParameterf(PyObject *self, PyObject *pyoArgs) {
    int target, name;
    float param;
    int ok = PyArg_ParseTuple(pyoArgs, "iif", &target, &name, &param);
    if(!ok) {
        return NULL;
    }
    glTexParameterf(target, name, param);
    Py_RETURN_NONE;
}

static PyObject * PyGL_TexParameteri(PyObject *self, PyObject *pyoArgs) {
    int target, name, param;
    int ok = PyArg_ParseTuple(pyoArgs, "iii", &target, &name, &param);
    if(!ok) {
        return NULL;
    }
    glTexParameteri(target, name, param);
    Py_RETURN_NONE;
}

// ****************************************************************************
// raster functions
// ****************************************************************************

static PyObject * PyGL_RasterPos2i(PyObject *self, PyObject *pyoArgs) {
    GLint x = (GLint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 0));
    GLint y = (GLint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 1));
    glRasterPos2i(x, y);
    Py_RETURN_NONE;
}

static PyObject * PyGL_WindowPos2i(PyObject *self, PyObject *pyoArgs) {
    GLint x = (GLint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 0));
    GLint y = (GLint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 1));
    glWindowPos2i(x, y);
    Py_RETURN_NONE;
}

static PyObject * PyGL_WindowPos3i(PyObject *self, PyObject *pyoArgs) {
    GLint x = (GLint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 0));
    GLint y = (GLint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 1));
    GLint z = (GLint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 2));
    glWindowPos3i(x, y, z);
    Py_RETURN_NONE;
}

static PyObject * PyGL_PixelZoom(PyObject *self, PyObject *pyoArgs) {
    GLfloat x = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 0));
    GLfloat y = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 1));
    glPixelZoom(x, y);
    Py_RETURN_NONE;
}

static PyObject * PyGL_PixelStorei(PyObject *self, PyObject *pyoArgs) {
    GLenum name  = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 0));
    GLint  param = (GLint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 1));
    glPixelStorei(name, param);
    Py_RETURN_NONE;
}

static PyObject * PyGL_DrawPixels(PyObject *self, PyObject *pyoArgs) {
    GLsizei w      = (GLsizei)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 0));
    GLsizei h      = (GLsizei)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 1));
    GLenum  fmt    = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 2));
    GLenum  type   = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 3));
    char *  pixels = PyBytes_AS_STRING(PyTuple_GetItem(pyoArgs, 4));
    glDrawPixels(w, h, fmt, type, pixels);
    Py_RETURN_NONE;
}

static PyObject * PyGL_CopyPixels(PyObject *self, PyObject *pyoArgs) {
    GLint   x    = (GLint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 0));
    GLint   y    = (GLint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 1));
    GLsizei w    = (GLsizei)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 2));
    GLsizei h    = (GLsizei)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 3));
    GLenum  type = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 4));
    glCopyPixels(x, y, w, h, type);
    Py_RETURN_NONE;
}

static PyObject * PyGL_ReadPixels(PyObject *self, PyObject *pyoArgs) {
    GLint   x    = (GLint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 0));
    GLint   y    = (GLint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 1));
    GLsizei w    = (GLsizei)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 2));
    GLsizei h    = (GLsizei)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 3));
    GLenum  fmt  = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 4));
    GLenum  type = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 5));
    size_t len;
    if(GL_RGBA == fmt)
        len = w * h * 4;
    else
        len = w * h * 3;
    PyObject *pyoPixels = PyBytes_FromStringAndSize(NULL, len);
    char *pixels = PyBytes_AS_STRING(pyoPixels);
    glReadPixels(x, y, w, h, fmt, type, pixels);
    return pyoPixels;
}

// ****************************************************************************
// frame buffer objects
// ****************************************************************************

static PyObject * PyGL_GenFramebuffers(PyObject *self, PyObject *pyoNum) {
    GLuint fbos[64];
    int nSize = PyLong_AsLong(pyoNum);
    if(0 >= nSize || 64 < nSize) {
        return NULL;
    }
    glGenFramebuffers(nSize, fbos);
    PyObject *pyoList = PyList_New(nSize);
    for(int idx = 0; idx < nSize; ++idx) {
        PyList_SetItem(pyoList, idx, PyLong_FromLong(fbos[idx]));
    }
    return pyoList;
}

static PyObject * PyGL_DeleteFramebuffers(PyObject *self, PyObject *pyoList) {
    GLuint fbos[64];
    Py_ssize_t size = PyList_Size(pyoList);
    if(64 < size) {
        return NULL;
    }
    for(int idx = 0; idx < size; ++idx) {
        fbos[idx] = PyLong_AsLong(PyList_GetItem(pyoList, idx));
    }
    glDeleteFramebuffers((GLsizei)size, fbos);
    Py_RETURN_NONE;
}

static PyObject * PyGL_BindFramebuffer(PyObject *self, PyObject *pyoNum) {
    GLuint fbo = (GLuint)PyLong_AsUnsignedLong(pyoNum);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    Py_RETURN_NONE;
}

static PyObject * PyGL_FramebufferRenderbuffer(PyObject *self, PyObject *pyoArgs) {
    GLenum attachment = (GLenum)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 0));
    GLuint rbo        = (GLuint)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 1));
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, rbo);
    Py_RETURN_NONE;
}

// ****************************************************************************
// render buffer objects
// ****************************************************************************

static PyObject * PyGL_GenRenderbuffers(PyObject *self, PyObject *pyoNum) {
    GLuint rbos[64];
    int nSize = PyLong_AsLong(pyoNum);
    if(0 >= nSize || 64 < nSize) {
        return NULL;
    }
    glGenRenderbuffers(nSize, rbos);
    PyObject *pyRenderBuffers = PyList_New(nSize);
    for(int idx = 0; idx < nSize; ++idx) {
        PyList_SetItem(pyRenderBuffers, idx, PyLong_FromLong(rbos[idx]));
    }
    return pyRenderBuffers;
}

static PyObject * PyGL_DeleteRenderbuffers(PyObject *self, PyObject *pyoList) {
    GLuint rbos[64];
    Py_ssize_t size = PyList_Size(pyoList);
    if(64 < size) {
        return NULL;
    }
    for(int idx = 0; idx < size; ++idx) {
        rbos[idx] = PyLong_AsLong(PyList_GetItem(pyoList, idx));
    }
    glDeleteRenderbuffers((GLsizei)size, rbos);
    Py_RETURN_NONE;
}

static PyObject * PyGL_BindRenderbuffer(PyObject *self, PyObject *pyoNum) {
    GLuint rbo = (GLuint)PyLong_AsUnsignedLong(pyoNum);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    Py_RETURN_NONE;
}

static PyObject * PyGL_RenderbufferStorage(PyObject *self, PyObject *pyoArgs) {
    GLenum  fmt = (GLenum)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 0));
    GLsizei w   = (GLsizei)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 1));
    GLsizei h   = (GLsizei)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 2));
    glRenderbufferStorage(GL_RENDERBUFFER, fmt, w, h);
    Py_RETURN_NONE;
}

// ****************************************************************************

static PyObject * PyGL_DepthFunc(PyObject *self, PyObject *pyoFunc) {
    glDepthFunc(PyLong_AsLong(pyoFunc));
    Py_RETURN_NONE;
}

// ****************************************************************************

static PyObject * PyGL_DepthMask(PyObject *self, PyObject *pyoBool) {
    glDepthMask(PyLong_AsLong(pyoBool));
    Py_RETURN_NONE;
}


static PyObject * PyGL_BindBuffer(PyObject *self, PyObject *pyoArgs) {
    GLenum target = (GLenum)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 0));
    GLuint buffer = (GLuint)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 1));
    glBindBuffer(target, buffer);
    Py_RETURN_NONE;
}


// ****************************************************************************
// GLu functions
// ****************************************************************************

static PyObject *PyGLu_Ortho2D(PyObject *self, PyObject *pyoArgs) {
    GLdouble l = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 0));
    GLdouble r = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 1));
    GLdouble b = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 2));
    GLdouble t = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 3));
    gluOrtho2D(l, r, b, t);
    Py_RETURN_NONE;
}

static PyObject *PyGLu_Perspective(PyObject *self, PyObject *pyoArgs) {
    GLdouble v = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 0));
    GLdouble a = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 1));
    GLdouble n = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 2));
    GLdouble f = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 3));
    gluPerspective(v, a, n, f);
    Py_RETURN_NONE;
}

static PyObject *PyGLu_LookAt(PyObject *self, PyObject *pyoArgs) {
    GLdouble ex = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 0));
    GLdouble ey = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 1));
    GLdouble ez = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 2));
    GLdouble cx = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 3));
    GLdouble cy = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 4));
    GLdouble cz = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 5));
    GLdouble ux = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 6));
    GLdouble uy = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 7));
    GLdouble uz = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 8));
    gluLookAt(ex, ey, ez, cx, cy, cz, ux, uy, uz);
    Py_RETURN_NONE;
}
