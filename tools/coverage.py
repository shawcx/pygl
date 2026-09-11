#!/usr/bin/env python3
#
# Report how much of the OpenGL 1.0 - 4.6 compatibility API pygl exposes.
# Compares the module's callables against the registry feature set the
# generator targets (<remove> blocks ignored, so the fixed-function pipeline
# counts too).
#
#   python3 tools/coverage.py            summary + missing list
#   python3 tools/coverage.py --enums    include constant coverage
#

import argparse
import os
import sys
import xml.etree.ElementTree as ET

HERE     = os.path.dirname(os.path.abspath(__file__))
ROOT     = os.path.dirname(HERE)
REGISTRY = os.path.join(HERE, 'gl.xml')
VERSIONS = ['1.0', '1.1', '1.2', '1.3', '1.4', '1.5', '2.0', '2.1',
            '3.0', '3.1', '3.2', '3.3',
            '4.0', '4.1', '4.2', '4.3', '4.4', '4.5', '4.6']

# Deliberately not wrapped - calls with no practical use from Python, no modern
# use at all, or a deep-cut variant of something already covered:
WONTFIX = {
    # evaluators, pixel-transfer maps, selection / feedback, texture residency
    'glMap1d', 'glMap1f', 'glMap2d', 'glMap2f',
    'glPixelMapfv', 'glPixelMapuiv', 'glPixelMapusv',
    'glGetPixelMapusv', 'glGetnPixelMapusv',
    'glSelectBuffer', 'glFeedbackBuffer',
    'glAreTexturesResident', 'glPrioritizeTextures',
    # pointer getters
    'glGetPointerv', 'glGetBufferPointerv', 'glGetVertexAttribPointerv',
    'glGetNamedBufferPointerv', 'glGetObjectPtrLabel', 'glObjectPtrLabel',
    # imaging-subset robustness getters (the base calls are wrapped)
    'glGetnColorTable', 'glGetnConvolutionFilter', 'glGetnHistogram',
    'glGetnMinmax', 'glGetnSeparableFilter', 'glGetnCompressedTexImage',
    'glGetnPolygonStipple', 'glGetnMapdv', 'glGetnMapfv', 'glGetnMapiv',
    'glGetnPixelMapfv', 'glGetnPixelMapuiv', 'glGetnUniformdv',
    # multi-draw indirect + base-vertex deep cuts (MultiDrawArrays/Elements done)
    'glMultiDrawArraysIndirect', 'glMultiDrawElementsIndirect',
    'glMultiDrawArraysIndirectCount', 'glMultiDrawElementsIndirectCount',
    'glMultiDrawElementsBaseVertex',
    # program-interface and subroutine introspection
    'glGetProgramResourceIndex', 'glGetProgramResourceName',
    'glGetProgramResourceiv', 'glGetProgramResourceLocation',
    'glGetProgramResourceLocationIndex', 'glGetProgramPipelineInfoLog',
    'glGetActiveSubroutineName', 'glGetActiveSubroutineUniformName',
    'glGetActiveSubroutineUniformiv', 'glGetSubroutineIndex',
    'glGetSubroutineUniformLocation', 'glGetUniformSubroutineuiv',
    'glUniformSubroutinesuiv',
    # SPIR-V / binary shader path
    'glShaderBinary', 'glSpecializeShader',
    # multi-bind with parallel offset/size/stride arrays
    'glBindBuffersRange', 'glBindVertexBuffers', 'glVertexArrayVertexBuffers',
    'glVertexArrayVertexBuffer', 'glBindImageTexture',
    # ProgramUniform double + non-square matrix variants (fv/iv/uiv + square done)
    'glProgramUniform1dv', 'glProgramUniform2dv', 'glProgramUniform3dv',
    'glProgramUniform4dv', 'glProgramUniformMatrix2x3fv', 'glProgramUniformMatrix3x2fv',
    'glProgramUniformMatrix2x4fv', 'glProgramUniformMatrix4x2fv',
    'glProgramUniformMatrix3x4fv', 'glProgramUniformMatrix4x3fv',
    'glProgramUniformMatrix2dv', 'glProgramUniformMatrix3dv', 'glProgramUniformMatrix4dv',
    'glProgramUniformMatrix2x3dv', 'glProgramUniformMatrix3x2dv',
    'glProgramUniformMatrix2x4dv', 'glProgramUniformMatrix4x2dv',
    'glProgramUniformMatrix3x4dv', 'glProgramUniformMatrix4x3dv',
    'glUniformMatrix2x3dv', 'glUniformMatrix3x2dv', 'glUniformMatrix2x4dv',
    'glUniformMatrix4x2dv', 'glUniformMatrix3x4dv', 'glUniformMatrix4x3dv',
    # named-object variants of already-wrapped calls with awkward array args
    'glInvalidateNamedFramebufferData', 'glInvalidateNamedFramebufferSubData',
    'glClearNamedFramebufferfi', 'glNamedFramebufferDrawBuffer',
    'glGetCompressedTextureSubImage', 'glGetTextureSubImage',
    # debug callback (a Python trampoline is installed via DebugMessageCallback,
    # but the introspection getter has no value from Python)
    'glDebugMessageCallback',
    'glGetStringi',
}


def registry_commands():
    root = ET.parse(REGISTRY).getroot()
    names = set()
    for feature in root.findall('feature'):
        if feature.get('api') == 'gl' and feature.get('number') in VERSIONS:
            for require in feature.findall('require'):
                for c in require.findall('command'):
                    names.add(c.get('name'))
    return names


def registry_enums():
    root = ET.parse(REGISTRY).getroot()
    names = set()
    for feature in root.findall('feature'):
        if feature.get('api') == 'gl' and feature.get('number') in VERSIONS:
            for require in feature.findall('require'):
                for e in require.findall('enum'):
                    names.add(e.get('name'))
    return names


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--enums', action='store_true')
    ap.add_argument('--strict', action='store_true',
                    help='exit non-zero if a non-WONTFIX command is missing')
    args = ap.parse_args()

    sys.path.insert(0, ROOT)
    import pygl

    have = {n for n in dir(pygl) if callable(getattr(pygl, n))}

    commands = registry_commands()
    covered, missing = set(), set()
    for name in commands:
        py = name[2:]
        (covered if py in have else missing).add(name)

    gaps = sorted(missing - WONTFIX)
    total = len(commands)
    print('commands : %d / %d  (%.1f%%)  +%d GLU/GLEW extras'
          % (len(covered), total, 100.0 * len(covered) / total,
             len(have) - len(covered)))
    print('wontfix  : %d' % len(missing & WONTFIX))
    if gaps:
        print('missing  : %d' % len(gaps))
        for name in gaps:
            print('   ', name)
    else:
        print('missing  : 0 (every remaining gap is on the wontfix list)')

    if args.enums:
        enums = registry_enums()
        ehave = {n for n in dir(pygl) if isinstance(getattr(pygl, n), int)}
        ecov = sum(1 for n in enums
                   if (('_' + n[3:]) if n[3:4].isdigit() else n[3:]) in ehave)
        print('enums    : %d / %d  (%.1f%%)' % (ecov, len(enums),
                                                100.0 * ecov / len(enums)))

    if args.strict and gaps:
        sys.exit(1)


if __name__ == '__main__':
    main()
