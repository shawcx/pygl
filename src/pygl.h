#ifndef __PYGL_H__
#define __PYGL_H__

#include <Python.h>
#include <structmember.h>

#ifdef WIN32
 #include <Windows.h>
#endif // WIN32

#include <GL/glew.h>

#define  DOC_MOD  "Python wrapper for OpenGL."

// ****************************************************************************
//
// Init Function
//
// ****************************************************************************

void initpygl(void);

// ****************************************************************************
// extension functions
// ****************************************************************************
static PyObject * PyGL_glewInit ( PyObject * self, PyObject * pyo );

// ****************************************************************************
// management functions
// ****************************************************************************
static PyObject * PyGL_Enable    ( PyObject * self, PyObject * pyoCap  );
static PyObject * PyGL_Disable   ( PyObject * self, PyObject * pyoCap  );
static PyObject * PyGL_IsEnabled ( PyObject * self, PyObject * pyoCap  );
static PyObject * PyGL_Hint      ( PyObject * self, PyObject * pyoArgs );
static PyObject * PyGL_BlendFunc ( PyObject * self, PyObject * pyoArgs );

static PyObject * PyGL_GetString ( PyObject * self, PyObject * pyoArgs );

static PyObject * PyGL_GetIntegerv ( PyObject * self, PyObject * pyoArgs );

// ****************************************************************************
// matrix functions
// ****************************************************************************
static PyObject * PyGL_PushMatrix   ( PyObject * self, PyObject * pyo       );
static PyObject * PyGL_PopMatrix    ( PyObject * self, PyObject * pyo       );
static PyObject * PyGL_MatrixMode   ( PyObject * self, PyObject * pyoMode   );
static PyObject * PyGL_LoadIdentity ( PyObject * self, PyObject * pyo       );
/*
static PyObject * PyGL_LoadMatrixd  ( PyObject * self, PyObject * pyoMatrix );
static PyObject * PyGL_LoadMatrixf  ( PyObject * self, PyObject * pyoMatrix );
static PyObject * PyGL_MultMatrixd  ( PyObject * self, PyObject * pyoMatrix );
static PyObject * PyGL_MultMatrixf  ( PyObject * self, PyObject * pyoMatrix );
*/
static PyObject * PyGL_Flush        ( PyObject * self, PyObject * pyo       );
static PyObject * PyGL_Finish       ( PyObject * self, PyObject * pyo       );

// ****************************************************************************
// camera functions
// ****************************************************************************
static PyObject * PyGL_Viewport   ( PyObject * self, PyObject * pyoArgs );
static PyObject * PyGL_Ortho      ( PyObject * self, PyObject * pyoArgs );
static PyObject * PyGL_Frustum    ( PyObject * self, PyObject * pyoArgs );
static PyObject * PyGL_DepthRange ( PyObject * self, PyObject * pyoArgs );

// ****************************************************************************
// buffers
// ****************************************************************************
static PyObject * PyGL_Clear          ( PyObject * self, PyObject * pyoFlags );
static PyObject * PyGL_ClearColor     ( PyObject * self, PyObject * pyoArgs  );
static PyObject * PyGL_ClearDepth     ( PyObject * self, PyObject * pyoDepth );

// ****************************************************************************
// call list functions
// ****************************************************************************
static PyObject * PyGL_GenLists    ( PyObject * self, PyObject * pyoRange );
static PyObject * PyGL_NewList     ( PyObject * self, PyObject * pyoArgs  );
static PyObject * PyGL_EndList     ( PyObject * self, PyObject * pyo      );
static PyObject * PyGL_CallList    ( PyObject * self, PyObject * pyoList  );
static PyObject * PyGL_IsList      ( PyObject * self, PyObject * pyoList  );
static PyObject * PyGL_DeleteLists ( PyObject * self, PyObject * pyoArgs  );

// ****************************************************************************
// drawing functions
// ****************************************************************************
static PyObject * PyGL_Begin      ( PyObject * self, PyObject * pyoMode  );
static PyObject * PyGL_End        ( PyObject * self, PyObject * pyo      );

static PyObject * PyGL_PointSize  ( PyObject * self, PyObject * pyoSize  );
static PyObject * PyGL_LineWidth  ( PyObject * self, PyObject * pyoWidth );

// ****************************************************************************
// vertex functions
// ****************************************************************************

//static PyObject * PyGL_Vertex2d ( PyObject * self, PyObject * pyoArgs  );
static PyObject * PyGL_Vertex2f ( PyObject * self, PyObject * pyoArgs  );
//static PyObject * PyGL_Vertex2i ( PyObject * self, PyObject * pyoArgs  );
//static PyObject * PyGL_Vertex2s ( PyObject * self, PyObject * pyoArgs  );
static PyObject * PyGL_Vertex3d ( PyObject * self, PyObject * pyoArgs  );
static PyObject * PyGL_Vertex3f ( PyObject * self, PyObject * pyoArgs  );
//static PyObject * PyGL_Vertex3i ( PyObject * self, PyObject * pyoArgs  );
//static PyObject * PyGL_Vertex3s ( PyObject * self, PyObject * pyoArgs  );
//static PyObject * PyGL_Vertex4d ( PyObject * self, PyObject * pyoArgs  );
//static PyObject * PyGL_Vertex4f ( PyObject * self, PyObject * pyoArgs  );
//static PyObject * PyGL_Vertex4i ( PyObject * self, PyObject * pyoArgs  );
//static PyObject * PyGL_Vertex4s ( PyObject * self, PyObject * pyoArgs  );

// ****************************************************************************
// normal functions
// ****************************************************************************

static PyObject * PyGL_Normal3f   ( PyObject * self, PyObject * pyoArgs  );


// ****************************************************************************

static PyObject * PyGL_PolygonMode  ( PyObject * self, PyObject * pyoArgs );
static PyObject * PyGL_FrontFace    ( PyObject * self, PyObject * pyoMode );
static PyObject * PyGL_CullFace     ( PyObject * self, PyObject * pyoMode );
static PyObject * PyGL_EdgeFlag     ( PyObject * self, PyObject * pyoBool );

// ****************************************************************************
// transform functions
// ****************************************************************************
static PyObject * PyGL_Rotated    ( PyObject * self, PyObject * pyoArgs );
static PyObject * PyGL_Rotatef    ( PyObject * self, PyObject * pyoArgs );
static PyObject * PyGL_Translated ( PyObject * self, PyObject * pyoArgs );
static PyObject * PyGL_Translatef ( PyObject * self, PyObject * pyoArgs );
static PyObject * PyGL_Scaled     ( PyObject * self, PyObject * pyoArgs );
static PyObject * PyGL_Scalef     ( PyObject * self, PyObject * pyoArgs );

// ****************************************************************************
// shape functions
// ****************************************************************************
static PyObject * PyGL_Rectd ( PyObject * self, PyObject * pyoArgs );
static PyObject * PyGL_Rectf ( PyObject * self, PyObject * pyoArgs );

// ****************************************************************************
// color functions
// ****************************************************************************
//static PyObject * PyGL_Color3b ( PyObject * self, PyObject * pyoColors );
//static PyObject * PyGL_Color3s ( PyObject * self, PyObject * pyoColors );
//static PyObject * PyGL_Color3i ( PyObject * self, PyObject * pyoColors );
static PyObject * PyGL_Color3d ( PyObject * self, PyObject * pyoColors );
static PyObject * PyGL_Color3f ( PyObject * self, PyObject * pyoColors );
static PyObject * PyGL_Color4f ( PyObject * self, PyObject * pyoColors );

static PyObject * PyGL_ShadeModel ( PyObject * self, PyObject * pyoShade );

// ****************************************************************************
// lighting functions
// ****************************************************************************
static PyObject * PyGL_Lighti  ( PyObject * self, PyObject * pyoArgs );
static PyObject * PyGL_Lightf  ( PyObject * self, PyObject * pyoArgs );
static PyObject * PyGL_Lightiv ( PyObject * self, PyObject * pyoArgs );
static PyObject * PyGL_Lightfv ( PyObject * self, PyObject * pyoArgs );

// ****************************************************************************
// material functions
// ****************************************************************************
static PyObject * PyGL_Materiali  ( PyObject * self, PyObject * pyoArgs );
static PyObject * PyGL_Materialf  ( PyObject * self, PyObject * pyoArgs );
static PyObject * PyGL_Materialiv ( PyObject * self, PyObject * pyoArgs );
static PyObject * PyGL_Materialfv ( PyObject * self, PyObject * pyoArgs );

// ****************************************************************************
// texture functions
// ****************************************************************************
static PyObject * PyGL_GenTextures    ( PyObject * self, PyObject * pyoNum   );
static PyObject * PyGL_DeleteTextures ( PyObject * self, PyObject * pyoList  );
static PyObject * PyGL_IsTexture      ( PyObject * self, PyObject * pyoNum   );
static PyObject * PyGL_BindTexture    ( PyObject * self, PyObject * pyoNum   );
static PyObject * PyGL_TexCoord2i     ( PyObject * self, PyObject * pyoArgs  );
static PyObject * PyGL_TexImage2D     ( PyObject * self, PyObject * pyoArgs  );
static PyObject * PyGL_TexSubImage2D  ( PyObject * self, PyObject * pyoArgs  );
static PyObject * PyGL_TexParameterf  ( PyObject * self, PyObject * pyoArgs  );
static PyObject * PyGL_TexParameteri  ( PyObject * self, PyObject * pyoArgs  );

// ****************************************************************************
// raster functions
// ****************************************************************************
static PyObject * PyGL_RasterPos2i ( PyObject * self, PyObject * pyoArgs );
static PyObject * PyGL_WindowPos2i ( PyObject * self, PyObject * pyoArgs );
static PyObject * PyGL_WindowPos3i ( PyObject * self, PyObject * pyoArgs );
static PyObject * PyGL_PixelZoom   ( PyObject * self, PyObject * pyoArgs );
static PyObject * PyGL_PixelStorei ( PyObject * self, PyObject * pyoArgs );
static PyObject * PyGL_DrawPixels  ( PyObject * self, PyObject * pyoArgs );
static PyObject * PyGL_CopyPixels  ( PyObject * self, PyObject * pyoArgs );
static PyObject * PyGL_ReadPixels  ( PyObject * self, PyObject * pyoArgs );

// ****************************************************************************
// frame buffer objects
// ****************************************************************************
static PyObject * PyGL_GenFramebuffers         ( PyObject * self, PyObject * pyoNum  );
static PyObject * PyGL_DeleteFramebuffers      ( PyObject * self, PyObject * pyoList );
static PyObject * PyGL_BindFramebuffer         ( PyObject * self, PyObject * pyoArgs );
static PyObject * PyGL_FramebufferRenderbuffer ( PyObject * self, PyObject * pyoArgs );

// ****************************************************************************
// render buffer objects
// ****************************************************************************
static PyObject * PyGL_GenRenderbuffers    ( PyObject * self, PyObject * pyoNum  );
static PyObject * PyGL_DeleteRenderbuffers ( PyObject * self, PyObject * pyoList );
static PyObject * PyGL_BindRenderbuffer    ( PyObject * self, PyObject * pyoNum  );
static PyObject * PyGL_RenderbufferStorage ( PyObject * self, PyObject * pyoArgs );

// ****************************************************************************

static PyObject * PyGL_DepthFunc      ( PyObject * self, PyObject * pyoFunc  );

static PyObject * PyGL_DepthMask      ( PyObject * self, PyObject * pyoFunc  );
static PyObject * PyGL_BindBuffer     ( PyObject * self, PyObject * pyoFunc  );

// ****************************************************************************
// GLU functions
// ****************************************************************************

static PyObject * PyGLu_Ortho2D     ( PyObject * self, PyObject * pyoArgs );
static PyObject * PyGLu_Perspective ( PyObject * self, PyObject * pyoArgs );
static PyObject * PyGLu_LookAt      ( PyObject * self, PyObject * pyoArgs );

static PyMethodDef PyGL_methods[] = {
    { "glewInit",  PyGL_glewInit, METH_NOARGS, "" },

// management functions
    { "Enable",    PyGL_Enable,    METH_O,       "" },
    { "Disable",   PyGL_Disable,   METH_O,       "" },
    { "IsEnabled", PyGL_IsEnabled, METH_O,       "" },
    { "Hint",      PyGL_Hint,      METH_VARARGS, "" },
    { "BlendFunc", PyGL_BlendFunc, METH_VARARGS,
    "BlendFunc(srcfactor, destfactor)" },
    { "GetString", PyGL_GetString, METH_O,       "" },

//
    { "GetIntegerv", PyGL_GetIntegerv, METH_O, "" },

// matrix functions
    { "PushMatrix",   PyGL_PushMatrix,   METH_NOARGS,  "" },
    { "PopMatrix",    PyGL_PopMatrix,    METH_NOARGS,  "" },
    { "MatrixMode",   PyGL_MatrixMode,   METH_O,       "" },
    { "LoadIdentity", PyGL_LoadIdentity, METH_NOARGS,  "" },
/*
    { "LoadMatrixd",  PyGL_LoadMatrixd,  METH_VARARGS, "" },
    { "LoadMatrixf",  PyGL_LoadMatrixf,  METH_VARARGS, "" },
    { "MultMatrixd",  PyGL_MultMatrixd,  METH_VARARGS, "" },
    { "MultMatrixf",  PyGL_MultMatrixf,  METH_VARARGS, "" },
*/
    { "Flush",        PyGL_Flush,        METH_NOARGS,  "" },
    { "Finish",       PyGL_Finish,       METH_NOARGS,  "" },
// camera functions
    { "Viewport",   PyGL_Viewport,   METH_VARARGS, "" },
    { "Ortho",      PyGL_Ortho,      METH_VARARGS, "" },
    { "Frustum",    PyGL_Frustum,    METH_VARARGS, "" },
    { "DepthRange", PyGL_DepthRange, METH_VARARGS, "" },
// buffers
    { "Clear",          PyGL_Clear,          METH_O,       "" },
    { "ClearColor",     PyGL_ClearColor,     METH_VARARGS, "" },
    { "ClearDepth",     PyGL_ClearDepth,     METH_O,       "" },
// call list functions
    { "GenLists",    PyGL_GenLists,    METH_O,       "" },
    { "NewList",     PyGL_NewList,     METH_VARARGS, "" },
    { "EndList",     PyGL_EndList,     METH_NOARGS,  "" },
    { "CallList",    PyGL_CallList,    METH_O,       "" },
    { "IsList",      PyGL_IsList,      METH_O,       "" },
    { "DeleteLists", PyGL_DeleteLists, METH_VARARGS, "" },
// drawing functions
    { "Begin",       PyGL_Begin,       METH_O,       "" },
    { "End",         PyGL_End,         METH_NOARGS,  "" },
    { "PointSize",   PyGL_PointSize,   METH_O,       "" },
    { "LineWidth",   PyGL_LineWidth,   METH_O,       "" },
// vertex functions
    { "Vertex2f", PyGL_Vertex2f, METH_VARARGS, "" },
    { "Vertex3d", PyGL_Vertex3d, METH_VARARGS, "" },
    { "Vertex3f", PyGL_Vertex3f, METH_VARARGS, "" },
// normals
    { "Normal3f", PyGL_Normal3f, METH_VARARGS, "" },

// ???
    { "PolygonMode", PyGL_PolygonMode, METH_VARARGS, "" },
    { "FrontFace",   PyGL_FrontFace,   METH_O,       "" },
    { "CullFace",    PyGL_CullFace,    METH_O,       "" },
    { "EdgeFlag",    PyGL_EdgeFlag,    METH_O,       "" },
// transforms
    { "Rotated",    PyGL_Rotated,    METH_VARARGS, "" },
    { "Rotatef",    PyGL_Rotatef,    METH_VARARGS, "" },
    { "Translated", PyGL_Translated, METH_VARARGS, "" },
    { "Translatef", PyGL_Translatef, METH_VARARGS, "" },
    { "Scaled",     PyGL_Scaled,     METH_VARARGS, "" },
    { "Scalef",     PyGL_Scalef,     METH_VARARGS, "" },
// shape functions
    { "Rectd", PyGL_Rectd, METH_VARARGS, "" },
    { "Rectf", PyGL_Rectf, METH_VARARGS, "" },
// color functions
    { "Color3d",    PyGL_Color3d,    METH_VARARGS, "" },
    { "Color3f",    PyGL_Color3f,    METH_VARARGS, "" },
    { "Color4f",    PyGL_Color4f,    METH_VARARGS, "" },
    { "ShadeModel", PyGL_ShadeModel, METH_O,       "" },
// lighting functions
    { "Lighti",  PyGL_Lighti,  METH_VARARGS, "" },
    { "Lightf",  PyGL_Lightf,  METH_VARARGS, "" },
    { "Lightiv", PyGL_Lightiv, METH_VARARGS, "" },
    { "Lightfv", PyGL_Lightfv, METH_VARARGS, "" },
// material functions
    { "Materiali",  PyGL_Materiali,  METH_VARARGS, "" },
    { "Materialf",  PyGL_Materialf,  METH_VARARGS, "" },
    { "Materialiv", PyGL_Materialiv, METH_VARARGS, "" },
    { "Materialfv", PyGL_Materialfv, METH_VARARGS, "" },
// texture functions
    { "GenTextures",    PyGL_GenTextures,    METH_O,       "" },
    { "DeleteTextures", PyGL_DeleteTextures, METH_O,       "" },
    { "ISTexture",      PyGL_IsTexture,      METH_O,       "" },
    { "BindTexture",    PyGL_BindTexture,    METH_VARARGS, "" },
    { "TexCoord2i",     PyGL_TexCoord2i,     METH_VARARGS, "" },
    { "TexImage2D",     PyGL_TexImage2D,     METH_VARARGS, "" },
    { "TexSubImage2D",  PyGL_TexSubImage2D,  METH_VARARGS, "" },
    { "TexParameterf",  PyGL_TexParameterf,  METH_VARARGS, "" },
    { "TexParameteri",  PyGL_TexParameteri,  METH_VARARGS, "" },
// raster functions
    { "RasterPos2i", PyGL_RasterPos2i, METH_VARARGS, "" },
    { "WindowPos2i", PyGL_WindowPos2i, METH_VARARGS, "" },
    { "WindowPos3i", PyGL_WindowPos3i, METH_VARARGS, "" },
    { "PixelZoom",   PyGL_PixelZoom,   METH_VARARGS, "" },
    { "PixelStorei", PyGL_PixelStorei, METH_VARARGS, "" },
    { "DrawPixels",  PyGL_DrawPixels,  METH_VARARGS, "" },
    { "CopyPixels",  PyGL_CopyPixels,  METH_VARARGS, "" },
    { "ReadPixels",  PyGL_ReadPixels,  METH_VARARGS, "" },

    { "WindowPos2i", PyGL_WindowPos2i, METH_VARARGS, "" },
// frame buffer object
    { "GenFramebuffers",         PyGL_GenFramebuffers,         METH_O,       "" },
    { "DeleteFramebuffers",      PyGL_DeleteFramebuffers,      METH_O,       "" },
    { "BindFramebuffer",         PyGL_BindFramebuffer,         METH_O,       "" },
    { "FramebufferRenderbuffer", PyGL_FramebufferRenderbuffer, METH_VARARGS, "" },
// render buffer object
    { "GenRenderbuffers",    PyGL_GenRenderbuffers,    METH_O,       "" },
    { "DeleteRenderbuffers", PyGL_DeleteRenderbuffers, METH_O,       "" },
    { "BindRenderbuffer",    PyGL_BindRenderbuffer,    METH_O,       "" },
    { "RenderbufferStorage", PyGL_RenderbufferStorage, METH_VARARGS, "" },
// ???
    { "DepthFunc",      PyGL_DepthFunc,      METH_O,       "" },

    { "DepthMask",      PyGL_DepthMask,      METH_O,       "" },
    { "BindBuffer",     PyGL_BindBuffer,     METH_VARARGS, "" },

// GLU
    { "uOrtho2D",     PyGLu_Ortho2D,     METH_VARARGS, "" },
    { "uPerspective", PyGLu_Perspective, METH_VARARGS, "" },
    { "uLookAt",      PyGLu_LookAt,      METH_VARARGS, "" },
    { NULL }
};

#endif // __PYGL_H__
