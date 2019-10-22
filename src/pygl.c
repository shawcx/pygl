#include "pygl.h"

void add_constants(PyObject *mod);

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
	if(0 == glewInit()) {
		Py_RETURN_TRUE;
	}
	else {
		Py_RETURN_FALSE;
	}
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
	// TODO: XXX: IMPLEMENT
	Py_RETURN_NONE;
}

static PyObject * PyGL_Hint(PyObject *self, PyObject *pyoArgs) {
	GLenum target,mode;

	target = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 0));
	mode   = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 1));

	glHint(target, mode);

	Py_RETURN_NONE;
}

static PyObject * PyGL_BlendFunc(PyObject *self, PyObject *pyoArgs) {
	GLenum src,dst;

	src = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 0));
	dst = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 1));

	glBlendFunc(src, dst);

	Py_RETURN_NONE;
}

static PyObject * PyGL_GetString(PyObject *self, PyObject *pyoName) {
	GLenum name;
	const char *string;

	name = (GLenum)PyLong_AsUnsignedLong(pyoName);
	string = (const char *)glGetString(name);

	if(string) {
		return PyUnicode_FromString(string);
	}
	else {
		Py_RETURN_NONE;
	}
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
	GLint x, y;
	GLsizei w, h;

	x = (GLint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 0));
	y = (GLint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 1));
	w = (GLsizei)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 2));
	h = (GLsizei)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 3));

	glViewport(x, y, w, h);

	Py_RETURN_NONE;
}

static PyObject * PyGL_Ortho(PyObject *self, PyObject *pyoArgs) {
	GLdouble l,r,b,t,n,f;

	l = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 0));
	r = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 1));
	b = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 2));
	t = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 3));
	n = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 4));
	f = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 5));

	glOrtho(l, r, b, t, n, f);

	Py_RETURN_NONE;
}

static PyObject * PyGL_Frustum(PyObject *self, PyObject *pyoArgs) {
	GLdouble l,r,b,t,n,f;

	l = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 0));
	r = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 1));
	b = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 2));
	t = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 3));
	n = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 4));
	f = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 5));

	glFrustum(l, r, b, t, n, f);

	Py_RETURN_NONE;
}

static PyObject * PyGL_DepthRange(PyObject *self, PyObject *pyoArgs) {
	GLclampd n,f;

	n = (GLclampd)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 0));
	f = (GLclampd)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 1));

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
	int ok;

	ok = PyArg_ParseTuple(pyoArgs, "ffff", &r, &g, &b, &a);
	if(!ok)
	{
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
	GLsizei size;
	GLuint range;

	size = (GLsizei)PyLong_AsUnsignedLong(pyoRange);

	range = glGenLists(size);
	return Py_BuildValue("I", range);
}

static PyObject * PyGL_NewList(PyObject *self, PyObject *pyoArgs) {
	GLuint idx;
	GLenum mode;

	idx = (GLuint)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 0));

	if(2 == PyTuple_Size(pyoArgs))
	{
		mode = (GLuint)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 1));
	}
	else
	{
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
	GLuint idx;

	idx = (GLuint)PyLong_AsUnsignedLong(pyoList);

	glCallList(idx);

	Py_RETURN_NONE;
}

static PyObject * PyGL_IsList(PyObject *self, PyObject *pyoList) {
	GLuint idx;
	GLboolean isList;

	idx = (GLuint)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoList, 0));

	isList = glIsList(idx);

	if(isList)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}

static PyObject * PyGL_DeleteLists(PyObject *self, PyObject *pyoArgs) {
	GLuint idx;

	idx = (GLuint)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 0));

	glCallList(idx);

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
	GLfloat x,y;

	x = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 0));
	y = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 1));

	glVertex2f(x, y);

	Py_RETURN_NONE;
}

static PyObject * PyGL_Vertex3d(PyObject *self, PyObject *pyoArgs) {
	GLdouble x,y,z;

	x = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 0));
	y = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 1));
	z = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 2));

	glVertex3d(x, y, z);

	Py_RETURN_NONE;
}

static PyObject * PyGL_Vertex3f(PyObject *self, PyObject *pyoArgs) {
	GLfloat x,y,z;

	x = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 0));
	y = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 1));
	z = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 2));

	glVertex3f(x, y, z);

	Py_RETURN_NONE;
}

// ****************************************************************************
// normal functions
// ****************************************************************************

static PyObject * PyGL_Normal3f(PyObject *self, PyObject *pyoArgs) {
	GLfloat x,y,z;

	x = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 0));
	y = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 1));
	z = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 2));

	glNormal3f(x, y, z);

	Py_RETURN_NONE;
}


// ****************************************************************************

static PyObject * PyGL_PolygonMode(PyObject *self, PyObject *pyoArgs) {
	GLenum face,mode;

	face = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 0));
	mode = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 1));

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
	GLdouble a,x,y,z;

	a = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 0));
	x = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 1));
	y = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 2));
	z = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 3));

	glRotated(a, x, y, z);

	Py_RETURN_NONE;
}

static PyObject * PyGL_Rotatef(PyObject *self, PyObject *pyoArgs) {
	GLfloat a,x,y,z;

	a = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 0));
	x = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 1));
	y = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 2));
	z = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 3));

	glRotatef(a, x, y, z);

	Py_RETURN_NONE;
}

static PyObject * PyGL_Translated(PyObject *self, PyObject *pyoArgs) {
	GLdouble x,y,z;

	x = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 0));
	y = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 1));
	z = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 2));

	glTranslated(x, y, z);

	Py_RETURN_NONE;
}

static PyObject * PyGL_Translatef(PyObject *self, PyObject *pyoArgs) {
	GLfloat x,y,z;

	x = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 0));
	y = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 1));
	z = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 2));

	glTranslatef(x, y, z);

	Py_RETURN_NONE;
}

static PyObject * PyGL_Scaled(PyObject *self, PyObject *pyoArgs) {
	GLdouble x,y,z;

	x = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 0));
	y = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 1));
	z = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 2));

	glScaled(x, y, z);

	Py_RETURN_NONE;
}

static PyObject * PyGL_Scalef(PyObject *self, PyObject *pyoArgs) {
	GLfloat x,y,z;

	x = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 0));
	y = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 1));
	z = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 2));

	glScalef(x, y, z);

	Py_RETURN_NONE;
}

// ****************************************************************************
// shape functions
// ****************************************************************************

static PyObject * PyGL_Rectd(PyObject *self, PyObject *pyoArgs) {
	GLdouble x1,y1,x2,y2;

	x1 = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 0));
	y1 = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 1));
	x2 = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 2));
	y2 = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 2));

	glRectd(x1, y1, x2, y2);

	Py_RETURN_NONE;
}

static PyObject * PyGL_Rectf(PyObject *self, PyObject *pyoArgs) {
	GLfloat x1,y1,x2,y2;

	x1 = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 0));
	y1 = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 1));
	x2 = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 2));
	y2 = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 2));

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
	GLdouble r,g,b;

	r = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoColors, 0));
	g = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoColors, 1));
	b = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoColors, 2));

	glColor3d(r, g, b);

	Py_RETURN_NONE;
}

static PyObject * PyGL_Color3f(PyObject * self, PyObject * pyoColors) {
	GLfloat r,g,b;

	r = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoColors, 0));
	g = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoColors, 1));
	b = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoColors, 2));

	glColor3f(r, g, b);

	Py_RETURN_NONE;
}

static PyObject * PyGL_Color4f(PyObject * self, PyObject * pyoColors) {
	GLfloat r,g,b,a;

	r = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoColors, 0));
	g = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoColors, 1));
	b = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoColors, 2));
	a = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoColors, 3));

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
	Py_RETURN_NONE;
}

static PyObject * PyGL_Lightf(PyObject *self, PyObject *pyoArgs) {
	Py_RETURN_NONE;
}

static PyObject * PyGL_Lightiv(PyObject *self, PyObject *pyoArgs) {
	PyObject *pyoList;
	GLenum light;
	GLenum name;
	GLint args[4];
	Py_ssize_t size;
	int idx;

	light = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 0));
	name  = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 1));
	pyoList = PyTuple_GetItem(pyoArgs, 2);

	size = PyList_Size(pyoList);
	if(4 < size)
	{
		return NULL;
	}

	for(idx = 0; idx < size; ++idx)
	{
		args[idx] = (GLint)PyLong_AsLong(PyList_GetItem(pyoList, idx));
	}

	glLightiv(light, name, args);

	Py_RETURN_NONE;
}

static PyObject * PyGL_Lightfv(PyObject *self, PyObject *pyoArgs) {
	PyObject *pyoList;
	GLenum light;
	GLenum name;
	GLfloat args[4];
	Py_ssize_t size;
	int idx;

	light = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 0));
	name  = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 1));
	pyoList = PyTuple_GetItem(pyoArgs, 2);

	size = PyList_Size(pyoList);
	if(4 < size)
	{
		return NULL;
	}

	for(idx = 0; idx < size; ++idx)
	{
		args[idx] = (GLfloat)PyFloat_AsDouble(PyList_GetItem(pyoList, idx));
	}

	glLightfv(light, name, args);

	Py_RETURN_NONE;
}

// ****************************************************************************
// material functions
// ****************************************************************************

static PyObject * PyGL_Materiali(PyObject *self, PyObject *pyoArgs) {
	Py_RETURN_NONE;
}

static PyObject * PyGL_Materialf(PyObject *self, PyObject *pyoArgs) {
	Py_RETURN_NONE;
}

static PyObject * PyGL_Materialiv(PyObject *self, PyObject *pyoArgs) {
	PyObject *pyoList;
	GLenum face;
	GLenum name;
	GLint args[4];
	Py_ssize_t size;
	int idx;

	face = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 0));
	name = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 1));
	pyoList = PyTuple_GetItem(pyoArgs, 2);

	size = PyList_Size(pyoList);
	if(4 < size)
	{
		return NULL;
	}

	for(idx = 0; idx < size; ++idx)
	{
		args[idx] = (GLint)PyLong_AsLong(PyList_GetItem(pyoList, idx));
	}

	glMaterialiv(face, name, args);

	Py_RETURN_NONE;
}

static PyObject * PyGL_Materialfv(PyObject *self, PyObject *pyoArgs) {
	PyObject *pyoList;
	GLenum face;
	GLenum name;
	GLfloat args[4];
	Py_ssize_t size;
	int idx;

	face = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 0));
	name = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 1));
	pyoList = PyTuple_GetItem(pyoArgs, 2);

	size = PyList_Size(pyoList);
	if(4 < size)
	{
		return NULL;
	}

	for(idx = 0; idx < size; ++idx)
	{
		args[idx] = (GLfloat)PyFloat_AsDouble(PyList_GetItem(pyoList, idx));
	}

	glMaterialfv(face, name, args);

	Py_RETURN_NONE;
}

// ****************************************************************************
// texture functions
// ****************************************************************************

static PyObject * PyGL_GenTextures(PyObject *self, PyObject *pyoNum) {
	PyObject *pyoList;
	GLuint textures[512];
	int nSize;
	int idx;

	nSize = PyLong_AsLong(pyoNum);
	if(0 >= nSize || 512 < nSize)
	{
		return NULL;
	}

	glGenTextures(nSize, textures);

	pyoList = PyList_New(nSize);
	for(idx = 0; idx < nSize; ++idx)
	{
		PyList_SetItem(pyoList, idx, PyLong_FromLong(textures[idx]));
	}

	return pyoList;
}

static PyObject * PyGL_DeleteTextures(PyObject *self, PyObject *pyoList) {
	GLuint textures[512];
	Py_ssize_t size;
	int idx;

	size = PyList_Size(pyoList);
	if(512 < size)
	{
		return NULL;
	}

	for(idx = 0; idx < size; ++idx)
	{
		textures[idx] = PyLong_AsLong(PyList_GetItem(pyoList, idx));
	}

	glDeleteTextures((GLsizei)size, textures);

	Py_RETURN_NONE;
}

static PyObject * PyGL_IsTexture(PyObject *self, PyObject *pyoTexture) {
	GLuint idx;
	GLboolean isTexture;

	idx = (GLuint)PyLong_AsUnsignedLong(pyoTexture);

	isTexture = glIsTexture(idx);

	if(isTexture)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}

static PyObject * PyGL_BindTexture(PyObject *self, PyObject *pyoArgs) {
	GLenum target;
	GLuint texture;

	target  = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 0));
	texture = (GLuint)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 1));

	glBindTexture(target, texture);

	Py_RETURN_NONE;
}

static PyObject * PyGL_TexCoord2i(PyObject *self, PyObject *pyoArgs) {
	GLint s,t;

	s = (GLenum)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 0));
	t = (GLuint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 1));

	glTexCoord2i(s, t);

	Py_RETURN_NONE;
}

static PyObject * PyGL_TexImage2D(PyObject *self, PyObject *pyoArgs) {
	int target, level, intfmt, w, h, border, fmt, type, size;
	unsigned char *pixels;

	int ok;

	ok = PyArg_ParseTuple(pyoArgs, "iiiiiiiis#", &target, &level, &intfmt, &w, &h, &border, &fmt, &type, &pixels, &size);
	if(!ok)
	{
		return NULL;
	}

	glTexImage2D(target, level, intfmt, w, h, border, fmt, type, pixels);

	Py_RETURN_NONE;
}

static PyObject * PyGL_TexSubImage2D(PyObject *self, PyObject *pyoArgs) {
	GLenum target;
	GLint level;
	GLint x, y;
	GLsizei w, h;
	GLenum fmt, type;

	unsigned char *pixels;

	target = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 0));
	fmt    = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 6));
	type   = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 7));
	level = (GLint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 1));
	x = (GLint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 2));
	y = (GLint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 3));
	w = (GLsizei)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 4));
	h = (GLsizei)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 5));

	pixels = (unsigned char *)PyBytes_AsString(PyTuple_GetItem(pyoArgs, 8));

	glTexSubImage2D(target, level, x, y, w, h, fmt, type, pixels);

	Py_RETURN_NONE;
}

static PyObject * PyGL_TexParameterf(PyObject *self, PyObject *pyoArgs) {
	int target, name;
	float param;
	int ok;

	ok = PyArg_ParseTuple(pyoArgs, "iif", &target, &name, &param);
	if(!ok)
	{
		return NULL;
	}

	glTexParameterf(target, name, param);

	Py_RETURN_NONE;
}

static PyObject * PyGL_TexParameteri(PyObject *self, PyObject *pyoArgs) {
	int target, name, param;
	int ok;

	ok = PyArg_ParseTuple(pyoArgs, "iii", &target, &name, &param);
	if(!ok)
	{
		return NULL;
	}

	glTexParameteri(target, name, param);

	Py_RETURN_NONE;
}

// ****************************************************************************
// raster functions
// ****************************************************************************

static PyObject * PyGL_RasterPos2i(PyObject *self, PyObject *pyoArgs) {
	GLint x,y;

	x = (GLint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 0));
	y = (GLint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 1));

	glRasterPos2i(x, y);

	Py_RETURN_NONE;
}

static PyObject * PyGL_WindowPos2i(PyObject *self, PyObject *pyoArgs) {
	GLint x,y;

	x = (GLint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 0));
	y = (GLint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 1));

	glWindowPos2i(x, y);

	Py_RETURN_NONE;
}

static PyObject * PyGL_WindowPos3i(PyObject *self, PyObject *pyoArgs) {
	GLint x,y,z;

	x = (GLint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 0));
	y = (GLint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 1));
	z = (GLint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 2));

	glWindowPos3i(x, y, z);

	Py_RETURN_NONE;
}

static PyObject * PyGL_PixelZoom(PyObject *self, PyObject *pyoArgs) {
	GLfloat x,y;

	x = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 0));
	y = (GLfloat)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 1));

	glPixelZoom(x, y);

	Py_RETURN_NONE;
}

static PyObject * PyGL_PixelStorei(PyObject *self, PyObject *pyoArgs) {
	GLenum name;
	GLint param;

	name = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 0));
	param = (GLint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 1));

	glPixelStorei(name, param);

	Py_RETURN_NONE;
}

static PyObject * PyGL_DrawPixels(PyObject *self, PyObject *pyoArgs) {
	GLsizei w,h;
	GLenum fmt,type;
	char *pixels;

	w = (GLsizei)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 0));
	h = (GLsizei)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 1));
	fmt  = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 2));
	type = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 3));
	pixels = PyBytes_AS_STRING(PyTuple_GetItem(pyoArgs, 4));

	glDrawPixels(w, h, fmt, type, pixels);

	Py_RETURN_NONE;
}

static PyObject * PyGL_CopyPixels(PyObject *self, PyObject *pyoArgs) {
	GLint x,y;
	GLsizei w,h;
	GLenum type;

	x = (GLint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 0));
	y = (GLint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 1));
	w = (GLsizei)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 2));
	h = (GLsizei)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 3));
	type = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 4));

	glCopyPixels(x, y, w, h, type);

	Py_RETURN_NONE;
}

static PyObject * PyGL_ReadPixels(PyObject *self, PyObject *pyoArgs) {
	PyObject *pyoPixels;
	char *pixels;
	size_t len;

	GLint x,y;
	GLsizei w,h;
	GLenum fmt,type;

	x = (GLint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 0));
	y = (GLint)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 1));
	w = (GLsizei)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 2));
	h = (GLsizei)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 3));
	fmt  = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 4));
	type = (GLenum)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 5));

	if(GL_RGBA == fmt)
		len = w * h * 4;
	else
		len = w * h * 3;

	pyoPixels = PyBytes_FromStringAndSize(NULL, len);
	pixels = PyBytes_AS_STRING(pyoPixels);

	glReadPixels(x, y, w, h, fmt, type, pixels);

	return pyoPixels;
}

// ****************************************************************************
// frame buffer objects
// ****************************************************************************

static PyObject * PyGL_GenFramebuffers(PyObject *self, PyObject *pyoNum) {
	PyObject *pyoList;
	GLuint fbos[64];
	int nSize;
	int idx;

	nSize = PyLong_AsLong(pyoNum);
	if(0 >= nSize || 64 < nSize) {
		return NULL;
	}

	glGenFramebuffers(nSize, fbos);

	pyoList = PyList_New(nSize);
	for(idx = 0; idx < nSize; ++idx) {
		PyList_SetItem(pyoList, idx, PyLong_FromLong(fbos[idx]));
	}

	return pyoList;
}

static PyObject * PyGL_DeleteFramebuffers(PyObject *self, PyObject *pyoList) {
	GLuint fbos[64];
	Py_ssize_t size;
	int idx;

	size = PyList_Size(pyoList);
	if(64 < size)
	{
		return NULL;
	}

	for(idx = 0; idx < size; ++idx)
	{
		fbos[idx] = PyLong_AsLong(PyList_GetItem(pyoList, idx));
	}

	glDeleteFramebuffers((GLsizei)size, fbos);

	Py_RETURN_NONE;
}

static PyObject * PyGL_BindFramebuffer(PyObject *self, PyObject *pyoNum) {
	GLuint fbo;

	fbo = (GLuint)PyLong_AsUnsignedLong(pyoNum);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	Py_RETURN_NONE;
}

static PyObject * PyGL_FramebufferRenderbuffer(PyObject *self, PyObject *pyoArgs) {
	GLenum attachment;
	GLuint rbo;

	attachment = (GLenum)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 0));
	rbo = (GLuint)PyLong_AsUnsignedLong(PyTuple_GetItem(pyoArgs, 1));

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, rbo);

	Py_RETURN_NONE;
}

// ****************************************************************************
// render buffer objects
// ****************************************************************************

static PyObject * PyGL_GenRenderbuffers(PyObject *self, PyObject *pyoNum) {
	PyObject *pyRenderBuffers;
	GLuint rbos[64];
	int nSize;
	int idx;

	nSize = PyLong_AsLong(pyoNum);
	if(0 >= nSize || 64 < nSize) {
		return NULL;
	}

	glGenRenderbuffers(nSize, rbos);

	pyRenderBuffers = PyList_New(nSize);
	for(idx = 0; idx < nSize; ++idx) {
		PyList_SetItem(pyRenderBuffers, idx, PyLong_FromLong(rbos[idx]));
	}

	return pyRenderBuffers;
}

static PyObject * PyGL_DeleteRenderbuffers(PyObject *self, PyObject *pyoList) {
	GLuint rbos[64];
	Py_ssize_t size;
	int idx;

	size = PyList_Size(pyoList);
	if(64 < size) {
		return NULL;
	}

	for(idx = 0; idx < size; ++idx) {
		rbos[idx] = PyLong_AsLong(PyList_GetItem(pyoList, idx));
	}

	glDeleteRenderbuffers((GLsizei)size, rbos);

	Py_RETURN_NONE;
}

static PyObject * PyGL_BindRenderbuffer(PyObject *self, PyObject *pyoNum) {
	GLuint rbo;

	rbo = (GLuint)PyLong_AsUnsignedLong(pyoNum);

	glBindRenderbuffer(GL_RENDERBUFFER, rbo);

	Py_RETURN_NONE;
}

static PyObject * PyGL_RenderbufferStorage(PyObject *self, PyObject *pyoArgs) {
	GLenum fmt;
	GLsizei w,h;

	fmt = (GLenum)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 0));
	w = (GLsizei)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 1));
	h = (GLsizei)PyLong_AsLong(PyTuple_GetItem(pyoArgs, 2));

	glRenderbufferStorage(GL_RENDERBUFFER, fmt, w, h);

	Py_RETURN_NONE;
}


// ****************************************************************************

static PyObject * PyGL_DepthFunc(PyObject *self, PyObject *pyoFunc) {
	glDepthFunc(PyLong_AsLong(pyoFunc));

	Py_RETURN_NONE;
}

// ****************************************************************************
// GLu functions
// ****************************************************************************

static PyObject *PyGLu_Ortho2D(PyObject *self, PyObject *pyoArgs) {
	GLdouble l,r,b,t;

	l = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 0));
	r = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 1));
	b = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 2));
	t = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 3));

	gluOrtho2D(l, r, b, t);

	Py_RETURN_NONE;
}

static PyObject *PyGLu_Perspective(PyObject *self, PyObject *pyoArgs) {
	GLdouble v,a,n,f;

	v = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 0));
	a = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 1));
	n = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 2));
	f = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 3));

	gluPerspective(v, a, n, f);

	Py_RETURN_NONE;
}

static PyObject *PyGLu_LookAt(PyObject *self, PyObject *pyoArgs) {
	GLdouble ex,ey,ez;
	GLdouble cx,cy,cz;
	GLdouble ux,uy,uz;

	ex = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 0));
	ey = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 1));
	ez = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 2));
	cx = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 3));
	cy = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 4));
	cz = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 5));
	ux = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 6));
	uy = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 7));
	uz = (GLdouble)PyFloat_AsDouble(PyTuple_GetItem(pyoArgs, 8));

	gluLookAt(ex, ey, ez, cx, cy, cz, ux, uy, uz);

	Py_RETURN_NONE;
}

void add_constants(PyObject *mod) {
// basics
	PyModule_AddIntConstant( mod, "TRUE",  GL_TRUE  );
	PyModule_AddIntConstant( mod, "FALSE", GL_FALSE );

// types
	PyModule_AddIntConstant( mod, "BYTE",           GL_BYTE           );
	PyModule_AddIntConstant( mod, "UNSIGNED_BYTE",  GL_UNSIGNED_BYTE  );
	PyModule_AddIntConstant( mod, "SHORT",          GL_SHORT          );
	PyModule_AddIntConstant( mod, "UNSIGNED_SHORT", GL_UNSIGNED_SHORT );
	PyModule_AddIntConstant( mod, "INT",            GL_INT            );
	PyModule_AddIntConstant( mod, "UNSIGNED_INT",   GL_UNSIGNED_INT   );
	PyModule_AddIntConstant( mod, "FLOAT",          GL_FLOAT          );
	PyModule_AddIntConstant( mod, "DOUBLE",         GL_DOUBLE         );

	PyModule_AddIntConstant( mod, "BITMAP",         GL_BITMAP         );

// logic operators
	PyModule_AddIntConstant( mod, "EQUAL",    GL_EQUAL    );
	PyModule_AddIntConstant( mod, "NOTEQUAL", GL_NOTEQUAL );
	PyModule_AddIntConstant( mod, "GREATER",  GL_GREATER  );
	PyModule_AddIntConstant( mod, "GEQUAL",   GL_GEQUAL   );
	PyModule_AddIntConstant( mod, "LESS",     GL_LESS     );
	PyModule_AddIntConstant( mod, "LEQUAL",   GL_LEQUAL   );
	PyModule_AddIntConstant( mod, "ALWAYS",   GL_ALWAYS   );
	PyModule_AddIntConstant( mod, "NEVER",    GL_NEVER    );

// driver info
	PyModule_AddIntConstant( mod, "VENDOR",     GL_VENDOR     );
	PyModule_AddIntConstant( mod, "RENDERER",   GL_RENDERER   );
	PyModule_AddIntConstant( mod, "VERSION",    GL_VERSION    );
	PyModule_AddIntConstant( mod, "EXTENSIONS", GL_EXTENSIONS );

// faces
	PyModule_AddIntConstant( mod, "FRONT",          GL_FRONT          );
	PyModule_AddIntConstant( mod, "BACK",           GL_BACK           );
	PyModule_AddIntConstant( mod, "FRONT_AND_BACK", GL_FRONT_AND_BACK );

	PyModule_AddIntConstant( mod, "CW",  GL_CW  );
	PyModule_AddIntConstant( mod, "CCW", GL_CCW );

// shapes
	PyModule_AddIntConstant( mod, "POINTS",         GL_POINTS         );
	PyModule_AddIntConstant( mod, "LINES",          GL_LINES          );
	PyModule_AddIntConstant( mod, "LINE_STRIP",     GL_LINE_STRIP     );
	PyModule_AddIntConstant( mod, "LINE_LOOP",      GL_LINE_LOOP      );
	PyModule_AddIntConstant( mod, "TRIANGLES",      GL_TRIANGLES      );
	PyModule_AddIntConstant( mod, "TRIANGLE_STRIP", GL_TRIANGLE_STRIP );
	PyModule_AddIntConstant( mod, "TRIANGLE_FAN",   GL_TRIANGLE_FAN   );
	PyModule_AddIntConstant( mod, "QUADS",          GL_QUADS          );
	PyModule_AddIntConstant( mod, "QUAD_STRIP",     GL_QUAD_STRIP     );
	PyModule_AddIntConstant( mod, "POLYGON",        GL_POLYGON        );

// call lists
	PyModule_AddIntConstant( mod, "COMPILE",             GL_COMPILE             );
	PyModule_AddIntConstant( mod, "COMPILE_AND_EXECUTE", GL_COMPILE_AND_EXECUTE );

// lighting
	PyModule_AddIntConstant( mod, "LIGHTING",   GL_LIGHTING   );
	PyModule_AddIntConstant( mod, "MAX_LIGHTS", GL_MAX_LIGHTS );
	PyModule_AddIntConstant( mod, "LIGHT0",     GL_LIGHT0     );
	PyModule_AddIntConstant( mod, "LIGHT1",     GL_LIGHT1     );
	PyModule_AddIntConstant( mod, "LIGHT2",     GL_LIGHT2     );
	PyModule_AddIntConstant( mod, "LIGHT3",     GL_LIGHT3     );
	PyModule_AddIntConstant( mod, "LIGHT4",     GL_LIGHT4     );
	PyModule_AddIntConstant( mod, "LIGHT5",     GL_LIGHT5     );
	PyModule_AddIntConstant( mod, "LIGHT6",     GL_LIGHT6     );
	PyModule_AddIntConstant( mod, "LIGHT7",     GL_LIGHT7     );

	PyModule_AddIntConstant( mod, "AMBIENT",    GL_AMBIENT    );
	PyModule_AddIntConstant( mod, "DIFFUSE",    GL_DIFFUSE    );
	PyModule_AddIntConstant( mod, "AMBIENT_AND_DIFFUSE", GL_AMBIENT_AND_DIFFUSE );
	PyModule_AddIntConstant( mod, "SPECULAR",   GL_SPECULAR   );
	PyModule_AddIntConstant( mod, "POSITION",   GL_POSITION   );
	PyModule_AddIntConstant( mod, "SHININESS",  GL_SHININESS  );
	PyModule_AddIntConstant( mod, "EMISSION",   GL_EMISSION   );

// pixel formats
	PyModule_AddIntConstant( mod, "R3_G3_B2", GL_R3_G3_B2 );
	PyModule_AddIntConstant( mod, "RGB",      GL_RGB      );
	PyModule_AddIntConstant( mod, "RGB4",     GL_RGB4     );
	PyModule_AddIntConstant( mod, "RGB5",     GL_RGB5     );
	PyModule_AddIntConstant( mod, "RGB8",     GL_RGB8     );
	PyModule_AddIntConstant( mod, "RGB10",    GL_RGB10    );
	PyModule_AddIntConstant( mod, "RGB12",    GL_RGB12    );
	PyModule_AddIntConstant( mod, "RGB16",    GL_RGB16    );
	PyModule_AddIntConstant( mod, "RGBA",     GL_RGBA     );
	PyModule_AddIntConstant( mod, "RGBA2",    GL_RGBA2    );
	PyModule_AddIntConstant( mod, "RGBA4",    GL_RGBA4    );
	PyModule_AddIntConstant( mod, "RGB5_A1",  GL_RGB5_A1  );
	PyModule_AddIntConstant( mod, "RGBA8",    GL_RGBA8    );
	PyModule_AddIntConstant( mod, "RGB10_A2", GL_RGB10_A2 );
	PyModule_AddIntConstant( mod, "RGBA12",   GL_RGBA12   );
	PyModule_AddIntConstant( mod, "RGBA16",   GL_RGBA16   );
#ifdef GL_BGR
	PyModule_AddIntConstant( mod, "BGR",      GL_BGR      );
#endif
#ifdef GL_BGRA
	PyModule_AddIntConstant( mod, "BGRA",     GL_BGRA     );
#endif

// buffer bits
	PyModule_AddIntConstant( mod, "COLOR_BUFFER_BIT",   GL_COLOR_BUFFER_BIT   );
	PyModule_AddIntConstant( mod, "DEPTH_BUFFER_BIT",   GL_DEPTH_BUFFER_BIT   );
	PyModule_AddIntConstant( mod, "STENCIL_BUFFER_BIT", GL_STENCIL_BUFFER_BIT );
	PyModule_AddIntConstant( mod, "ACCUM_BUFFER_BIT",   GL_ACCUM_BUFFER_BIT   );

	PyModule_AddIntConstant( mod, "ALPHA_TEST",                  GL_ALPHA_TEST                  );
	PyModule_AddIntConstant( mod, "DEPTH_TEST",                  GL_DEPTH_TEST                  );
	PyModule_AddIntConstant( mod, "SCISSOR_TEST",                GL_SCISSOR_TEST                );
	PyModule_AddIntConstant( mod, "STENCIL_TEST",                GL_STENCIL_TEST                );

	PyModule_AddIntConstant( mod, "ALPHA",                       GL_ALPHA                       );
	PyModule_AddIntConstant( mod, "ALPHA4",                      GL_ALPHA4                      );
	PyModule_AddIntConstant( mod, "ALPHA8",                      GL_ALPHA8                      );
	PyModule_AddIntConstant( mod, "ALPHA12",                     GL_ALPHA12                     );
	PyModule_AddIntConstant( mod, "ALPHA16",                     GL_ALPHA16                     );
	PyModule_AddIntConstant( mod, "AUTO_NORMAL",                 GL_AUTO_NORMAL                 );
	PyModule_AddIntConstant( mod, "BLEND",                       GL_BLEND                       );
	PyModule_AddIntConstant( mod, "CLIP_PLANE0",                 GL_CLIP_PLANE0                 );
	PyModule_AddIntConstant( mod, "CLIP_PLANE1",                 GL_CLIP_PLANE1                 );
	PyModule_AddIntConstant( mod, "CLIP_PLANE2",                 GL_CLIP_PLANE2                 );
	PyModule_AddIntConstant( mod, "CLIP_PLANE3",                 GL_CLIP_PLANE3                 );
	PyModule_AddIntConstant( mod, "CLIP_PLANE4",                 GL_CLIP_PLANE4                 );
	PyModule_AddIntConstant( mod, "CLIP_PLANE5",                 GL_CLIP_PLANE5                 );
	PyModule_AddIntConstant( mod, "COLOR_LOGIC_OP",              GL_COLOR_LOGIC_OP              );
	PyModule_AddIntConstant( mod, "COLOR_MATERIAL",              GL_COLOR_MATERIAL              );

	PyModule_AddIntConstant( mod, "DITHER",                      GL_DITHER                      );
	PyModule_AddIntConstant( mod, "DONT_CARE",                   GL_DONT_CARE                   );
	PyModule_AddIntConstant( mod, "FASTEST",                     GL_FASTEST                     );
	PyModule_AddIntConstant( mod, "FLAT",                        GL_FLAT                        );
	PyModule_AddIntConstant( mod, "FOG",                         GL_FOG                         );
	PyModule_AddIntConstant( mod, "FOG_HINT",                    GL_FOG_HINT                    );
	PyModule_AddIntConstant( mod, "INDEX_LOGIC_OP",              GL_INDEX_LOGIC_OP              );
	PyModule_AddIntConstant( mod, "LINEAR",                      GL_LINEAR                      );
	PyModule_AddIntConstant( mod, "LINE_SMOOTH",                 GL_LINE_SMOOTH                 );
	PyModule_AddIntConstant( mod, "LINE_SMOOTH_HINT",            GL_LINE_SMOOTH_HINT            );
	PyModule_AddIntConstant( mod, "LINE_STIPPLE",                GL_LINE_STIPPLE                );
	PyModule_AddIntConstant( mod, "MAP1_COLOR_4",                GL_MAP1_COLOR_4                );
	PyModule_AddIntConstant( mod, "MAP1_INDEX",                  GL_MAP1_INDEX                  );
	PyModule_AddIntConstant( mod, "MAP1_NORMAL",                 GL_MAP1_NORMAL                 );
	PyModule_AddIntConstant( mod, "MAP1_TEXTURE_COORD_1",        GL_MAP1_TEXTURE_COORD_1        );
	PyModule_AddIntConstant( mod, "MAP1_TEXTURE_COORD_2",        GL_MAP1_TEXTURE_COORD_2        );
	PyModule_AddIntConstant( mod, "MAP1_TEXTURE_COORD_3",        GL_MAP1_TEXTURE_COORD_3        );
	PyModule_AddIntConstant( mod, "MAP1_TEXTURE_COORD_4",        GL_MAP1_TEXTURE_COORD_4        );
	PyModule_AddIntConstant( mod, "MAP1_VERTEX_3",               GL_MAP1_VERTEX_3               );
	PyModule_AddIntConstant( mod, "MAP1_VERTEX_4",               GL_MAP1_VERTEX_4               );
	PyModule_AddIntConstant( mod, "MAP2_COLOR_4",                GL_MAP2_COLOR_4                );
	PyModule_AddIntConstant( mod, "MAP2_INDEX",                  GL_MAP2_INDEX                  );
	PyModule_AddIntConstant( mod, "MAP2_NORMAL",                 GL_MAP2_NORMAL                 );
	PyModule_AddIntConstant( mod, "MAP2_TEXTURE_COORD_1",        GL_MAP2_TEXTURE_COORD_1        );
	PyModule_AddIntConstant( mod, "MAP2_TEXTURE_COORD_2",        GL_MAP2_TEXTURE_COORD_2        );
	PyModule_AddIntConstant( mod, "MAP2_TEXTURE_COORD_3",        GL_MAP2_TEXTURE_COORD_3        );
	PyModule_AddIntConstant( mod, "MAP2_TEXTURE_COORD_4",        GL_MAP2_TEXTURE_COORD_4        );
	PyModule_AddIntConstant( mod, "MAP2_VERTEX_3",               GL_MAP2_VERTEX_3               );
	PyModule_AddIntConstant( mod, "MAP2_VERTEX_4",               GL_MAP2_VERTEX_4               );
	PyModule_AddIntConstant( mod, "MODELVIEW",                   GL_MODELVIEW                   );
	PyModule_AddIntConstant( mod, "NICEST",                      GL_NICEST                      );
	PyModule_AddIntConstant( mod, "NORMALIZE",                   GL_NORMALIZE                   );
	PyModule_AddIntConstant( mod, "PERSPECTIVE_CORRECTION_HINT", GL_PERSPECTIVE_CORRECTION_HINT );
	PyModule_AddIntConstant( mod, "POINT_SMOOTH",                GL_POINT_SMOOTH                );
	PyModule_AddIntConstant( mod, "POINT_SMOOTH_HINT",           GL_POINT_SMOOTH_HINT           );
	PyModule_AddIntConstant( mod, "POLYGON_OFFSET_FILL",         GL_POLYGON_OFFSET_FILL         );
	PyModule_AddIntConstant( mod, "POLYGON_OFFSET_LINE",         GL_POLYGON_OFFSET_LINE         );
	PyModule_AddIntConstant( mod, "POLYGON_OFFSET_POINT",        GL_POLYGON_OFFSET_POINT        );
	PyModule_AddIntConstant( mod, "POLYGON_SMOOTH",              GL_POLYGON_SMOOTH              );
	PyModule_AddIntConstant( mod, "POLYGON_SMOOTH_HINT",         GL_POLYGON_SMOOTH_HINT         );
	PyModule_AddIntConstant( mod, "POLYGON_STIPPLE",             GL_POLYGON_STIPPLE             );
	PyModule_AddIntConstant( mod, "PROJECTION",                  GL_PROJECTION                  );
	PyModule_AddIntConstant( mod, "PROXY_TEXTURE_2D",            GL_PROXY_TEXTURE_2D            );

	PyModule_AddIntConstant( mod, "SMOOTH",                      GL_SMOOTH                      );

	PyModule_AddIntConstant( mod, "TEXTURE",                     GL_TEXTURE                     );
	PyModule_AddIntConstant( mod, "TEXTURE_1D",                  GL_TEXTURE_1D                  );
	PyModule_AddIntConstant( mod, "TEXTURE_2D",                  GL_TEXTURE_2D                  );
#ifdef GL_TEXTURE_3D
	PyModule_AddIntConstant( mod, "TEXTURE_3D",                  GL_TEXTURE_3D                  );
#endif
	PyModule_AddIntConstant( mod, "TEXTURE_GEN_Q",               GL_TEXTURE_GEN_Q               );
	PyModule_AddIntConstant( mod, "TEXTURE_GEN_R",               GL_TEXTURE_GEN_R               );
	PyModule_AddIntConstant( mod, "TEXTURE_GEN_S",               GL_TEXTURE_GEN_S               );
	PyModule_AddIntConstant( mod, "TEXTURE_GEN_T",               GL_TEXTURE_GEN_T               );
	PyModule_AddIntConstant( mod, "TEXTURE_MIN_FILTER",          GL_TEXTURE_MIN_FILTER          );
	PyModule_AddIntConstant( mod, "TEXTURE_MAG_FILTER",          GL_TEXTURE_MAG_FILTER          );

	PyModule_AddIntConstant( mod, "LUMINANCE",                   GL_LUMINANCE                   );
	PyModule_AddIntConstant( mod, "LUMINANCE4",                  GL_LUMINANCE4                  );
	PyModule_AddIntConstant( mod, "LUMINANCE8",                  GL_LUMINANCE8                  );
	PyModule_AddIntConstant( mod, "LUMINANCE12",                 GL_LUMINANCE12                 );
	PyModule_AddIntConstant( mod, "LUMINANCE16",                 GL_LUMINANCE16                 );
	PyModule_AddIntConstant( mod, "LUMINANCE_ALPHA",             GL_LUMINANCE_ALPHA             );
	PyModule_AddIntConstant( mod, "LUMINANCE4_ALPHA4",           GL_LUMINANCE4_ALPHA4           );
	PyModule_AddIntConstant( mod, "LUMINANCE6_ALPHA2",           GL_LUMINANCE6_ALPHA2           );
	PyModule_AddIntConstant( mod, "LUMINANCE8_ALPHA8",           GL_LUMINANCE8_ALPHA8           );
	PyModule_AddIntConstant( mod, "LUMINANCE12_ALPHA4",          GL_LUMINANCE12_ALPHA4          );
	PyModule_AddIntConstant( mod, "LUMINANCE12_ALPHA12",         GL_LUMINANCE12_ALPHA12         );
	PyModule_AddIntConstant( mod, "LUMINANCE16_ALPHA16",         GL_LUMINANCE16_ALPHA16         );
	PyModule_AddIntConstant( mod, "INTENSITY",                   GL_INTENSITY                   );
	PyModule_AddIntConstant( mod, "INTENSITY4",                  GL_INTENSITY4                  );
	PyModule_AddIntConstant( mod, "INTENSITY8",                  GL_INTENSITY8                  );
	PyModule_AddIntConstant( mod, "INTENSITY12",                 GL_INTENSITY12                 );
	PyModule_AddIntConstant( mod, "INTENSITY16",                 GL_INTENSITY16                 );

	PyModule_AddIntConstant( mod, "COLOR_INDEX",                 GL_COLOR_INDEX                 );
	PyModule_AddIntConstant( mod, "RED",                         GL_RED                         );
	PyModule_AddIntConstant( mod, "GREEN",                       GL_GREEN                       );
	PyModule_AddIntConstant( mod, "BLUE",                        GL_BLUE                        );

	PyModule_AddIntConstant( mod, "CULL_FACE",                   GL_CULL_FACE                   );

	PyModule_AddIntConstant( mod, "POINT",                    GL_POINT                    );
	PyModule_AddIntConstant( mod, "LINE",                     GL_LINE                     );
	PyModule_AddIntConstant( mod, "FILL",                     GL_FILL                     );

	PyModule_AddIntConstant( mod, "UNPACK_ALIGNMENT",         GL_UNPACK_ALIGNMENT         );

	PyModule_AddIntConstant( mod, "ZERO",                     GL_ZERO                     );
	PyModule_AddIntConstant( mod, "ONE",                      GL_ONE                      );
	PyModule_AddIntConstant( mod, "SRC_COLOR",                GL_SRC_COLOR                );
	PyModule_AddIntConstant( mod, "ONE_MINUS_SRC_COLOR",      GL_ONE_MINUS_SRC_COLOR      );
	PyModule_AddIntConstant( mod, "DST_COLOR",                GL_DST_COLOR                );
	PyModule_AddIntConstant( mod, "ONE_MINUS_DST_COLOR",      GL_ONE_MINUS_DST_COLOR      );
	PyModule_AddIntConstant( mod, "SRC_ALPHA",                GL_SRC_ALPHA                );
	PyModule_AddIntConstant( mod, "ONE_MINUS_SRC_ALPHA",      GL_ONE_MINUS_SRC_ALPHA      );
	PyModule_AddIntConstant( mod, "DST_ALPHA",                GL_DST_ALPHA                );
	PyModule_AddIntConstant( mod, "ONE_MINUS_DST_ALPHA",      GL_ONE_MINUS_DST_ALPHA      );
	PyModule_AddIntConstant( mod, "SRC_ALPHA_SATURATE",       GL_SRC_ALPHA_SATURATE       );

	PyModule_AddIntConstant( mod, "COLOR_ATTACHMENT0",        GL_COLOR_ATTACHMENT0        );
	PyModule_AddIntConstant( mod, "DEPTH_ATTACHMENT",         GL_DEPTH_ATTACHMENT         );
	PyModule_AddIntConstant( mod, "STENCIL_ATTACHMENT",       GL_STENCIL_ATTACHMENT       );
	PyModule_AddIntConstant( mod, "DEPTH_STENCIL_ATTACHMENT", GL_DEPTH_STENCIL_ATTACHMENT );

}
