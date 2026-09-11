#!/usr/bin/env python3
#
# Generate C wrappers and constant registrations for pygl from the Khronos
# OpenGL registry (tools/gl.xml).  The output matches the hand-written house
# style in src/ so the generated files read the same as the manual ones.
#
# Scope: the GL 1.0 - 4.6 compatibility API (the <remove> blocks are ignored, so
# the fixed-function pipeline is kept alongside modern core).  Anything the
# classifier cannot handle mechanically (pixel/vertex data, strings, callbacks)
# is either hand-written in src/modern.c or listed in tools/manifest.json under
# "unimplemented".
#
#   python3 tools/generate.py           regenerate src/generated/*
#   python3 tools/generate.py --report  print the classification only
#

import argparse
import json
import os
import re
import sys
import xml.etree.ElementTree as ET

HERE      = os.path.dirname(os.path.abspath(__file__))
ROOT      = os.path.dirname(HERE)
REGISTRY  = os.path.join(HERE, 'gl.xml')
OUT_DIR   = os.path.join(ROOT, 'src', 'generated')
MANIFEST  = os.path.join(HERE, 'manifest.json')

# The full desktop core timeline. <remove> blocks (the core-profile purge of
# the fixed-function API at 3.1) are ignored, so pygl tracks the compatibility
# profile: everything from glBegin to GL 4.6 in one module. A context that
# actually exposes both only exists on Linux/Windows - macOS caps compat at 2.1.
VERSIONS  = ['1.0', '1.1', '1.2', '1.3', '1.4', '1.5', '2.0', '2.1',
             '3.0', '3.1', '3.2', '3.3',
             '4.0', '4.1', '4.2', '4.3', '4.4', '4.5', '4.6']

# Never wrap: pointer getters (no meaningful Python value) and the ARB/KHR
# callback setters (glDebugMessageCallback itself is hand-written in modern.c).
FORCE_SKIP = {
    'glGetPointerv', 'glGetBufferPointerv', 'glGetVertexAttribPointerv',
    'glGetPointervKHR', 'glGetNamedBufferPointerv',
    'glDebugMessageCallbackARB', 'glDebugMessageCallbackKHR',
}


def hand_written_names():
    '''Python names already provided by a PyMethodDef table in a src/*.c file.

    The hand-written wrappers (manual.c, data.c, shaders.c, modern.c, glu.c)
    are the source of truth for what must not be generated - the generator
    simply skips any command whose stripped name appears in one of their
    method tables.
    '''
    src = os.path.join(ROOT, 'src')
    names = set()
    row = re.compile(r'\{\s*"([A-Za-z_]\w*)"\s*,\s*PyGL')
    for entry in os.listdir(src):
        if not entry.endswith('.c'):
            continue
        with open(os.path.join(src, entry)) as fh:
            names.update(row.findall(fh.read()))
    return names


def hand_written_gated():
    '''Hand-written wrappers whose method-table row sits behind an #ifdef guard.

    modern.c guards every GL 3.x+ entry point on its GLEW macro; a name whose
    row is compiled out is absent from the module, exactly like a gated
    generated wrapper, so it belongs in manifest["gated"] too.
    '''
    src = os.path.join(ROOT, 'src')
    names = set()
    row = re.compile(r'#ifdef\s+gl\w+\s*\n\s*\{\s*"([A-Za-z_]\w*)"\s*,\s*PyGL')
    for entry in os.listdir(src):
        if not entry.endswith('.c'):
            continue
        with open(os.path.join(src, entry)) as fh:
            names.update(row.findall(fh.read()))
    return names


# Getters with no pname to size the result from.
COUNT_HINTS = {
    'glGetClipPlane': 4,
}

# ----------------------------------------------------------------------------
# registry parsing
# ----------------------------------------------------------------------------

def node_type_name(node):
    '''Return (c-type-string, identifier) for a <proto> or <param> node.'''
    parts = []
    if node.text:
        parts.append(node.text)
    ident = None
    for child in node:
        if child.tag == 'name':
            ident = child.text
        elif child.text:
            parts.append(child.text)
        if child.tail:
            parts.append(child.tail)
    ctype = ' '.join(''.join(parts).split())
    return ctype, ident


class Param:
    def __init__(self, node):
        self.ctype, self.name = node_type_name(node)
        self.len  = node.get('len')
        self.base = self.ctype.replace('const', '').replace('*', '').strip()
        self.ptr  = self.ctype.count('*')
        self.const = self.ctype.strip().startswith('const')


class Command:
    def __init__(self, node):
        self.ctype, self.name = node_type_name(node.find('proto'))
        self.ret_base = self.ctype.replace('const', '').replace('*', '').strip()
        self.ret_ptr  = self.ctype.count('*')
        self.params   = [Param(p) for p in node.findall('param')]

    @property
    def py_name(self):
        return self.name[2:]        # drop the leading "gl"


# GL 1.0/1.1 is the guaranteed baseline the base gl.h always declares; anything
# newer reaches the build through GLEW and gets an #ifdef guard so the module
# still compiles against an older GLEW that predates it.
BASELINE = ('1.0', '1.1')


def load_registry():
    root = ET.parse(REGISTRY).getroot()

    commands, enums, baseline = set(), [], set()
    seen_enum = set()
    for feature in root.findall('feature'):
        if feature.get('api') != 'gl' or feature.get('number') not in VERSIONS:
            continue
        base = feature.get('number') in BASELINE
        for require in feature.findall('require'):
            for c in require.findall('command'):
                commands.add(c.get('name'))
                if base:
                    baseline.add(c.get('name'))
            for e in require.findall('enum'):
                name = e.get('name')
                if base:
                    baseline.add(name)
                if name not in seen_enum:
                    seen_enum.add(name)
                    enums.append(name)

    by_name = {}
    for c in root.find('commands').findall('command'):
        cmd = Command(c)
        if cmd.name in commands:
            by_name[cmd.name] = cmd

    enum_values = {}
    for block in root.findall('enums'):
        for e in block.findall('enum'):
            enum_values.setdefault(e.get('name'), e.get('value'))

    return by_name, enums, enum_values, baseline


# ----------------------------------------------------------------------------
# type mapping
# ----------------------------------------------------------------------------

# base GL type -> PyArg_ParseTuple format code
FMT = {
    'GLenum': 'I', 'GLbitfield': 'I', 'GLuint': 'I', 'GLhandleARB': 'I',
    'GLint': 'i', 'GLsizei': 'i', 'GLfixed': 'i', 'GLchar': 'i', 'GLbyte': 'i',
    'GLfloat': 'f', 'GLclampf': 'f',
    'GLdouble': 'd', 'GLclampd': 'd',
    'GLboolean': 'b', 'GLubyte': 'b',
    'GLshort': 'h', 'GLushort': 'H',
    'GLintptr': 'n', 'GLsizeiptr': 'n', 'GLintptrARB': 'n', 'GLsizeiptrARB': 'n',
    'GLuint64': 'K', 'GLuint64EXT': 'K', 'GLint64': 'L', 'GLint64EXT': 'L',
}

# base GL type -> (sequence helper, canonical C element type)
SEQ = {
    'GLfloat':  ('pygl_floats',  'GLfloat'),  'GLclampf': ('pygl_floats',  'GLfloat'),
    'GLdouble': ('pygl_doubles', 'GLdouble'), 'GLclampd': ('pygl_doubles', 'GLdouble'),
    'GLint':    ('pygl_ints',    'GLint'),    'GLsizei':  ('pygl_ints',    'GLint'),
    'GLuint':   ('pygl_uints',   'GLuint'),   'GLenum':   ('pygl_uints',   'GLuint'),
    'GLshort':  ('pygl_shorts',  'GLshort'),  'GLushort': ('pygl_ushorts', 'GLushort'),
    'GLbyte':   ('pygl_bytes',   'GLbyte'),   'GLubyte':  ('pygl_ubytes',  'GLubyte'),
    'GLboolean':('pygl_ubytes',  'GLubyte'),
}

# base GL type -> (result-builder, canonical C element type)
BUILD = {
    'GLfloat':  ('pygl_build_floats',  'GLfloat'),  'GLclampf': ('pygl_build_floats',  'GLfloat'),
    'GLdouble': ('pygl_build_doubles', 'GLdouble'), 'GLclampd': ('pygl_build_doubles', 'GLdouble'),
    'GLint':    ('pygl_build_ints',    'GLint'),    'GLsizei':  ('pygl_build_ints',    'GLint'),
    'GLuint':   ('pygl_build_uints',   'GLuint'),   'GLenum':   ('pygl_build_uints',   'GLuint'),
    'GLboolean':('pygl_build_bools',   'GLboolean'),
    'GLint64':  ('pygl_build_int64',   'GLint64'),  'GLuint64': ('pygl_build_uint64',  'GLuint64'),
}

RET_BUILD = {
    'GLuint': '"I"', 'GLenum': '"I"', 'GLbitfield': '"I"',
    'GLint': '"i"', 'GLsizei': '"i"',
    'GLfloat': '"d"', 'GLdouble': '"d"',
    'GLboolean': '"i"', 'GLhandleARB': '"I"',
    'GLint64': '"L"', 'GLuint64': '"K"',
}

NUMERIC_SEQ = set(SEQ)


def small_literal(text):
    return bool(text) and re.fullmatch(r'\d+', text) is not None and int(text) <= 16


def vector_len_ok(param):
    if param.len is None:
        return True
    if small_literal(param.len):
        return True
    if param.len == 'COMPSIZE(pname)':
        return param.base not in ('GLbyte', 'GLubyte')
    return False


# ----------------------------------------------------------------------------
# classification
# ----------------------------------------------------------------------------

def classify(cmd, hand=frozenset()):
    if cmd.py_name in hand:
        return 'manual', None
    if cmd.name in FORCE_SKIP:
        return 'skip', None

    p = cmd.params

    # opaque handle / callback types the generator has no marshalling for
    OPAQUE = ('GLsync', 'GLDEBUGPROC', 'GLDEBUGPROCARB', 'GLDEBUGPROCKHR',
              'GLDEBUGPROCAMD', 'GLVULKANPROCNV', 'GLeglImageOES',
              'GLeglClientBufferEXT', 'struct')
    if cmd.ret_base in OPAQUE or any(x.base in OPAQUE for x in p):
        return 'skip', 'opaque'

    # double indirection / opaque / non-scalar pointer -> not for these phases
    hard = (any(x.ptr >= 2 for x in p)
            or any(x.base in ('void', 'GLvoid') and x.ptr for x in p)
            or any(x.ptr and x.base not in FMT for x in p))

    # -- phase 5: object creation / deletion / state queries ------------------
    if not hard:
        # glGen*/glCreate*( [target,] n, GLuint *out )  ->  list
        if ((cmd.name.startswith('glGen') or cmd.name.startswith('glCreate'))
                and 2 <= len(p) <= 3 and p[-1].ptr == 1 and not p[-1].const
                and p[-1].base in BUILD and p[-2].base == 'GLsizei'
                and all(x.ptr == 0 for x in p[:-1])):
            return 'gen', None
        if (cmd.name.startswith('glDelete') and len(p) == 2
                and p[0].ptr == 0 and p[1].ptr == 1 and p[1].const
                and p[1].base in SEQ):
            return 'delete', None
        if (cmd.name.startswith('glGet') and cmd.ret_ptr == 0 and cmd.ret_base == 'void'
                and p and p[-1].ptr == 1 and not p[-1].const and p[-1].base in BUILD
                and all(x.ptr == 0 for x in p[:-1])):
            return 'getter', None

    # -- phase 3: pure scalar ----------------------------------------------------
    if (all(x.ptr == 0 for x in p) and cmd.ret_ptr == 0
            and (cmd.ret_base in ('void', 'GLvoid') or cmd.ret_base in FMT)):
        return 'scalar', None

    # -- phase 4: trailing numeric vector --------------------------------------
    if (cmd.ret_ptr == 0 and cmd.ret_base in ('void', 'GLvoid') and p
            and p[-1].ptr == 1 and p[-1].const and p[-1].base in NUMERIC_SEQ
            and all(x.ptr == 0 for x in p[:-1])):
        if any(x.name in ('count', 'n', 'bufSize', 'size', 'stride', 'mapsize')
               for x in p[:-1]):
            return 'skip', 'has-count'
        if vector_len_ok(p[-1]):
            return 'vector', None
        return 'skip', 'data-array'

    return 'skip', 'unclassified'


# ----------------------------------------------------------------------------
# code emission
# ----------------------------------------------------------------------------

def c_type(base):
    return base


def docstring(cmd, names):
    return '"%s(%s)"' % (cmd.py_name, ', '.join(names))


def emit_scalar(cmd):
    names   = [x.name for x in cmd.params]
    returns = cmd.ret_base not in ('void', 'GLvoid')
    body    = []
    if not cmd.params:
        flag = 'METH_NOARGS'
        body.append('static PyObject * PyGL_%s(PyObject *self, PyObject *pyo) {' % cmd.py_name)
        call = 'gl%s()' % cmd.py_name
    else:
        flag = 'METH_VARARGS'
        body.append('static PyObject * PyGL_%s(PyObject *self, PyObject *pyoArgs) {' % cmd.py_name)
        width = max(len(c_type(x.base)) for x in cmd.params)
        for x in cmd.params:
            body.append('    %-*s %s;' % (width, c_type(x.base), x.name))
        fmt = ''.join(FMT[x.base] for x in cmd.params)
        args = ', '.join('&%s' % x.name for x in cmd.params)
        body.append('    if(!PyArg_ParseTuple(pyoArgs, "%s", %s)) {' % (fmt, args))
        body.append('        return NULL;')
        body.append('    }')
        call = 'gl%s(%s)' % (cmd.py_name, ', '.join(names))

    if not returns:
        body.append('    %s;' % call)
        body.append('    Py_RETURN_NONE;')
    elif cmd.ret_base == 'GLboolean':
        body.append('    GLboolean result = %s;' % call)
        body.append('    if(result) { Py_RETURN_TRUE; } else { Py_RETURN_FALSE; }')
    else:
        body.append('    %s result = %s;' % (cmd.ret_base, call))
        body.append('    return Py_BuildValue(%s, result);' % RET_BUILD[cmd.ret_base])
    body.append('}')
    return '\n'.join(body), flag, docstring(cmd, names)


def emit_vector(cmd):
    lead   = cmd.params[:-1]
    tail   = cmd.params[-1]
    helper, elem = SEQ[tail.base]
    names  = [x.name for x in lead] + [tail.name]
    out    = []
    if lead:
        flag = 'METH_VARARGS'
        out.append('static PyObject * PyGL_%s(PyObject *self, PyObject *pyoArgs) {' % cmd.py_name)
        width = max([len(c_type(x.base)) for x in lead] + [len('PyObject *')])
        for x in lead:
            out.append('    %-*s %s;' % (width, c_type(x.base), x.name))
        out.append('    %-*s *_seq;' % (width - 1, 'PyObject'))
        fmt  = ''.join(FMT[x.base] for x in lead) + 'O'
        args = ', '.join('&%s' % x.name for x in lead) + ', &_seq'
        out.append('    if(!PyArg_ParseTuple(pyoArgs, "%s", %s)) {' % (fmt, args))
        out.append('        return NULL;')
        out.append('    }')
    else:
        flag = 'METH_O'
        out.append('static PyObject * PyGL_%s(PyObject *self, PyObject *_seq) {' % cmd.py_name)
    out.append('    %s _out[PYGL_MAX_VEC];' % elem)
    out.append('    Py_ssize_t _n = %s(_seq, _out, PYGL_MAX_VEC);' % helper)
    out.append('    if(_n < 0) {')
    out.append('        return NULL;')
    out.append('    }')
    out.append('    (void)_n;')
    out.append('    gl%s(%s);' % (cmd.py_name, ', '.join(x.name for x in lead) + (', ' if lead else '') + '_out'))
    out.append('    Py_RETURN_NONE;')
    out.append('}')
    return '\n'.join(out), flag, docstring(cmd, names)


def emit_gen(cmd):
    py   = cmd.py_name
    lead = cmd.params[:-2]                       # a leading target, or nothing
    if lead:
        names = [x.name for x in lead] + ['n']
        out = ['static PyObject * PyGL_%s(PyObject *self, PyObject *pyoArgs) {' % py]
        for x in lead:
            out.append('    %s %s;' % (x.base, x.name))
        out.append('    Py_ssize_t n;')
        fmt = ''.join(FMT[x.base] for x in lead) + 'n'
        refs = ', '.join('&%s' % x.name for x in lead) + ', &n'
        out.append('    if(!PyArg_ParseTuple(pyoArgs, "%s", %s)) {' % (fmt, refs))
        out.append('        return NULL;')
        out.append('    }')
        flag = 'METH_VARARGS'
        call = 'gl%s(%s, (GLsizei)n, names)' % (py, ', '.join(x.name for x in lead))
    else:
        names = ['n']
        out = ['static PyObject * PyGL_%s(PyObject *self, PyObject *pyoArg) {' % py,
               '    Py_ssize_t n = PyLong_AsSsize_t(pyoArg);',
               '    if(PyErr_Occurred()) {',
               '        return NULL;',
               '    }']
        flag = 'METH_O'
        call = 'gl%s((GLsizei)n, names)' % py
    out += [
        '    if(n < 0 || n > PYGL_MAX_NAMES) {',
        '        PyErr_SetString(PyExc_ValueError, "invalid count");',
        '        return NULL;',
        '    }',
        '    GLuint names[PYGL_MAX_NAMES];',
        '    %s;' % call,
        '    return pygl_list_uints(names, n);',
        '}',
    ]
    return '\n'.join(out), flag, docstring(cmd, names)


def emit_delete(cmd):
    py = cmd.py_name
    out = [
        'static PyObject * PyGL_%s(PyObject *self, PyObject *pyoArg) {' % py,
        '    GLuint names[PYGL_MAX_NAMES];',
        '    Py_ssize_t n = pygl_uints(pyoArg, names, PYGL_MAX_NAMES);',
        '    if(n < 0) {',
        '        return NULL;',
        '    }',
        '    gl%s((GLsizei)n, names);' % py,
        '    Py_RETURN_NONE;',
        '}',
    ]
    return '\n'.join(out), 'METH_O', docstring(cmd, [cmd.params[-1].name])


def emit_getter(cmd):
    lead   = cmd.params[:-1]
    tail   = cmd.params[-1]
    build, elem = BUILD[tail.base]
    hint   = COUNT_HINTS.get(cmd.name, 1)
    names  = [x.name for x in lead] + ['count=%d' % hint]
    out    = ['static PyObject * PyGL_%s(PyObject *self, PyObject *pyoArgs) {' % cmd.py_name]
    width  = max([len(c_type(x.base)) for x in lead] + [len('Py_ssize_t')])
    for x in lead:
        out.append('    %-*s %s;' % (width, c_type(x.base), x.name))
    out.append('    %-*s _count = %d;' % (width, 'Py_ssize_t', hint))
    fmt  = ''.join(FMT[x.base] for x in lead) + '|n'
    args = ', '.join('&%s' % x.name for x in lead) + (', ' if lead else '') + '&_count'
    out.append('    if(!PyArg_ParseTuple(pyoArgs, "%s", %s)) {' % (fmt, args))
    out.append('        return NULL;')
    out.append('    }')
    out.append('    if(_count < 1 || _count > PYGL_MAX_QUERY) {')
    out.append('        PyErr_SetString(PyExc_ValueError, "invalid count");')
    out.append('        return NULL;')
    out.append('    }')
    out.append('    %s _out[PYGL_MAX_QUERY];' % elem)
    out.append('    gl%s(%s);' % (cmd.py_name, ', '.join(x.name for x in lead) + (', ' if lead else '') + '_out'))
    out.append('    return %s(_out, _count);' % build)
    out.append('}')
    return '\n'.join(out), 'METH_VARARGS', docstring(cmd, names)


EMIT = {
    'scalar': emit_scalar, 'vector': emit_vector,
    'gen': emit_gen, 'delete': emit_delete, 'getter': emit_getter,
}

BANNER = {
    'scalar': 'scalar functions', 'vector': 'vector (array) functions',
    'gen': 'object generation', 'delete': 'object deletion', 'getter': 'state queries',
}

HEADER = '''//
// Generated by tools/generate.py from the Khronos OpenGL registry.
// Do not edit by hand - edit the generator or add a manual override.
//

'''


def generate():
    commands, enums, enum_values, baseline = load_registry()

    buckets = {k: [] for k in EMIT}
    manifest = {'generated': {}, 'manual': [], 'unimplemented': {}, 'gated': []}
    hand = hand_written_names()

    for name in sorted(commands):
        cmd = commands[name]
        kind, note = classify(cmd, hand)
        if kind in EMIT:
            buckets[kind].append(cmd)
            manifest['generated'].setdefault(kind, []).append(cmd.py_name)
        elif kind == 'manual':
            manifest['manual'].append(cmd.py_name)
        else:
            manifest['unimplemented'].setdefault(note or 'skip', []).append(name)

    os.makedirs(OUT_DIR, exist_ok=True)

    # A wrapper for anything past GL 1.1 is compiled only when GLEW declares it
    # (GLEW #defines every such entry point as a macro), so an older GLEW just
    # yields a smaller module instead of a broken build.
    def guard(name):
        return None if name in baseline else 'gl' + name[2:]

    rows = []
    parts = [HEADER, '#include "pygl.h"\n\n']
    for kind in ('scalar', 'vector', 'gen', 'delete', 'getter'):
        parts.append('// %s\n// %s\n\n' % ('*' * 74, BANNER[kind]))
        for cmd in buckets[kind]:
            code, flag, doc = EMIT[kind](cmd)
            g = guard(cmd.name)
            if g:
                parts.append('#ifdef %s\n%s\n#endif\n\n' % (g, code))
                manifest['gated'].append(cmd.py_name)
            else:
                parts.append(code + '\n\n')
            rows.append((cmd.py_name, flag, doc, g))

    parts.append('// %s\n// method table\n// %s\n\n' % ('*' * 74, '*' * 74))
    parts.append('static PyMethodDef PyGL_generated_methods[] = {\n')
    nw = max(len(n) for n, _, _, _ in rows) + 2
    fw = max(len(f) for _, f, _, _ in rows)
    for n, f, doc, g in rows:
        row = ('    { %-*s PyGL_%-*s %-*s %s },\n'
               % (nw, '"%s",' % n, nw, '%s,' % n, fw + 1, '%s,' % f, doc))
        parts.append('#ifdef %s\n%s#endif\n' % (g, row) if g else row)
    parts.append('    { NULL }\n};\n')

    with open(os.path.join(OUT_DIR, 'wrappers.c'), 'w') as fh:
        fh.write(''.join(parts))

    # -- constants ----------------------------------------------------------
    # every row guarded: GL 1.1 names are always defined so the guard is inert,
    # newer ones simply drop out on an older GLEW.
    cw = max(len(n) for n in enums) + 1
    clines = [HEADER, '#include "pygl.h"\n\n', 'void gl_add_constants(PyObject *mod) {\n']
    for name in enums:
        py = name[3:] if name.startswith('GL_') else name
        if py[:1].isdigit():
            py = '_' + py
        clines.append('#ifdef %s\n    PyModule_AddIntConstant( mod, %-*s (long)%s );\n#endif\n'
                      % (name, cw + 2, '"%s",' % py, name))
    clines.append('}\n')
    with open(os.path.join(OUT_DIR, 'constants.c'), 'w') as fh:
        fh.write(''.join(clines))

    manifest['gated'] = sorted(set(manifest['gated']) | hand_written_gated())
    with open(MANIFEST, 'w') as fh:
        json.dump(manifest, fh, indent=2, sort_keys=True)

    total = sum(len(v) for v in buckets.values())
    print('wrappers : %d (%s)' % (total, ', '.join('%s %d' % (k, len(v)) for k, v in buckets.items())))
    print('constants: %d' % len(enums))
    print('manual   : %d' % len(manifest['manual']))
    un = manifest['unimplemented']
    print('unimpl.  : %d (%s)' % (sum(len(v) for v in un.values()),
                                  ', '.join('%s %d' % (k, len(v)) for k, v in un.items())))


def report():
    commands, enums, _, _ = load_registry()
    hand = hand_written_names()
    rows = {}
    for name in sorted(commands):
        kind, note = classify(commands[name], hand)
        rows.setdefault(kind if kind in EMIT or kind == 'manual' else note, []).append(name)
    for k, v in sorted(rows.items(), key=lambda kv: -len(kv[1])):
        print('%-16s %4d   %s' % (k, len(v), ', '.join(v[:8])))


if __name__ == '__main__':
    ap = argparse.ArgumentParser()
    ap.add_argument('--report', action='store_true')
    args = ap.parse_args()
    if args.report:
        report()
    else:
        generate()
