#!/usr/bin/env python3

#
# Copyright (C) 1999-2001  Brian Paul   All Rights Reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
# BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
# AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
# $XFree86: xc/programs/glxgears/glxgears.c,v 1.4 2003/10/24 20:38:11 tsi Exp $ */

#
# This is a port of the infamous "gears" demo to straight GLX (i.e. no GLUT)
# Port by Brian Paul  23 March 2001
#
# Command line options:
#    -info      print GL implementation information
#


#
# Port of glxgears to test SDL/GL python bindings
# By Matthew Oertle 2016
#

import sys
import math

import SDL2 as SDL
import GL

window = None

def gear(inner_radius, outer_radius, width, teeth, tooth_depth):
    r0 = inner_radius
    r1 = outer_radius - tooth_depth / 2.0
    r2 = outer_radius + tooth_depth / 2.0

    da = 2.0 * math.pi / teeth / 4.0

    GL.ShadeModel(GL.FLAT)

    GL.Normal3f(0.0, 0.0, 1.0)

    # draw front face
    GL.Begin(GL.QUAD_STRIP)
    for i in range(teeth+1):
        angle = i * 2.0 * math.pi / teeth
        GL.Vertex3f(r0 * math.cos(angle), r0 * math.sin(angle), width * 0.5)
        GL.Vertex3f(r1 * math.cos(angle), r1 * math.sin(angle), width * 0.5)
        if i < teeth:
            GL.Vertex3f(r0 * math.cos(angle), r0 * math.sin(angle), width * 0.5)
            GL.Vertex3f(r1 * math.cos(angle + 3 * da), r1 * math.sin(angle + 3 * da), width * 0.5)
    GL.End()

    # draw front sides of teeth
    GL.Begin(GL.QUADS)
    da = 2.0 * math.pi / teeth / 4.0
    for i in range(teeth):
        angle = i * 2.0 * math.pi / teeth

        GL.Vertex3f(r1 * math.cos(angle), r1 * math.sin(angle), width * 0.5)
        GL.Vertex3f(r2 * math.cos(angle + da), r2 * math.sin(angle + da), width * 0.5)
        GL.Vertex3f(r2 * math.cos(angle + 2 * da), r2 * math.sin(angle + 2 * da), width * 0.5)
        GL.Vertex3f(r1 * math.cos(angle + 3 * da), r1 * math.sin(angle + 3 * da), width * 0.5)
    GL.End()

    GL.Normal3f(0.0, 0.0, -1.0)

    # draw back face
    GL.Begin(GL.QUAD_STRIP)
    for i in range(teeth+1):
        angle = i * 2.0 * math.pi / teeth
        GL.Vertex3f(r1 * math.cos(angle), r1 * math.sin(angle), -width * 0.5)
        GL.Vertex3f(r0 * math.cos(angle), r0 * math.sin(angle), -width * 0.5)
        if i < teeth:
            GL.Vertex3f(r1 * math.cos(angle + 3 * da), r1 * math.sin(angle + 3 * da), -width * 0.5)
            GL.Vertex3f(r0 * math.cos(angle), r0 * math.sin(angle), -width * 0.5)
    GL.End()

    # draw back sides of teeth
    GL.Begin(GL.QUADS)
    da = 2.0 * math.pi / teeth / 4.0
    for i in range(teeth):
        angle = i * 2.0 * math.pi / teeth

        GL.Vertex3f(r1 * math.cos(angle + 3 * da), r1 * math.sin(angle + 3 * da), -width * 0.5)
        GL.Vertex3f(r2 * math.cos(angle + 2 * da), r2 * math.sin(angle + 2 * da), -width * 0.5)
        GL.Vertex3f(r2 * math.cos(angle + da), r2 * math.sin(angle + da), -width * 0.5)
        GL.Vertex3f(r1 * math.cos(angle), r1 * math.sin(angle), -width * 0.5)
    GL.End()

    # draw outward faces of teeth
    GL.Begin(GL.QUAD_STRIP)
    for i in range(teeth):
        angle = i * 2.0 * math.pi / teeth

        GL.Vertex3f(r1 * math.cos(angle), r1 * math.sin(angle), width * 0.5)
        GL.Vertex3f(r1 * math.cos(angle), r1 * math.sin(angle), -width * 0.5)
        u = r2 * math.cos(angle + da) - r1 * math.cos(angle)
        v = r2 * math.sin(angle + da) - r1 * math.sin(angle)
        len = math.sqrt(u * u + v * v)
        u /= len
        v /= len
        GL.Normal3f(v, -u, 0.0)
        GL.Vertex3f(r2 * math.cos(angle + da), r2 * math.sin(angle + da), width * 0.5)
        GL.Vertex3f(r2 * math.cos(angle + da), r2 * math.sin(angle + da), -width * 0.5)
        GL.Normal3f(math.cos(angle), math.sin(angle), 0.0)
        GL.Vertex3f(r2 * math.cos(angle + 2 * da), r2 * math.sin(angle + 2 * da), width * 0.5)
        GL.Vertex3f(r2 * math.cos(angle + 2 * da), r2 * math.sin(angle + 2 * da), -width * 0.5)
        u = r1 * math.cos(angle + 3 * da) - r2 * math.cos(angle + 2 * da)
        v = r1 * math.sin(angle + 3 * da) - r2 * math.sin(angle + 2 * da)
        GL.Normal3f(v, -u, 0.0)
        GL.Vertex3f(r1 * math.cos(angle + 3 * da), r1 * math.sin(angle + 3 * da), width * 0.5)
        GL.Vertex3f(r1 * math.cos(angle + 3 * da), r1 * math.sin(angle + 3 * da), -width * 0.5)
        GL.Normal3f(math.cos(angle), math.sin(angle), 0.0)

    GL.Vertex3f(r1 * math.cos(0), r1 * math.sin(0), width * 0.5)
    GL.Vertex3f(r1 * math.cos(0), r1 * math.sin(0), -width * 0.5)

    GL.End()

    GL.ShadeModel(GL.SMOOTH)

    # draw inside radius cylinder
    GL.Begin(GL.QUAD_STRIP)
    for i in range(teeth+1):
        angle = i * 2.0 * math.pi / teeth
        GL.Normal3f(-math.cos(angle), -math.sin(angle), 0.0)
        GL.Vertex3f(r0 * math.cos(angle), r0 * math.sin(angle), -width * 0.5)
        GL.Vertex3f(r0 * math.cos(angle), r0 * math.sin(angle), width * 0.5)
    GL.End()


# new window size or exposure
def reshape(width, height):
    h = float(height) / float(width)
    GL.Viewport(0, 0, width, height)
    GL.MatrixMode(GL.PROJECTION)
    GL.LoadIdentity()
    GL.Frustum(-1.0, 1.0, -h, h, 5.0, 60.0)
    GL.MatrixMode(GL.MODELVIEW)
    GL.LoadIdentity()
    GL.Translatef(0.0, 0.0, -40.0)


def main():
    Frames = 0
    T0 = 0
    gear1 = None
    gear2 = None
    gear3 = None
    view_rotx = 20.0
    view_roty = 30.0
    view_rotz = 0.0
    angle = 0.0

    SDL.Init(SDL.INIT_VIDEO)

    window = SDL.Window("SDLgears", (300,300), flags=SDL.WINDOW_OPENGL | SDL.WINDOW_RESIZABLE)
    if not window:
        print("Couldn't set 300x300 GL video mode: %s\n", SDL.GetError())
        SDL.Quit()
        sys.exit(2)

    window.GL_CreateContext()

    w,h = window.GetWindowSize()

    pos = [5.0, 5.0, 10.0, 0.0]
    red = [0.8, 0.1, 0.0, 1.0]
    green = [0.0, 0.8, 0.2, 1.0]
    blue = [0.2, 0.2, 1.0, 1.0]

    GL.Lightfv(GL.LIGHT0, GL.POSITION, pos)
    GL.Enable(GL.CULL_FACE)
    GL.Enable(GL.LIGHTING)
    GL.Enable(GL.LIGHT0)
    GL.Enable(GL.DEPTH_TEST)

    # make the gears
    gear1 = GL.GenLists(1)
    GL.NewList(gear1, GL.COMPILE)
    GL.Materialfv(GL.FRONT, GL.AMBIENT_AND_DIFFUSE, red)
    gear(1.0, 4.0, 1.0, 20, 0.7)
    GL.EndList()

    gear2 = GL.GenLists(1)
    GL.NewList(gear2, GL.COMPILE)
    GL.Materialfv(GL.FRONT, GL.AMBIENT_AND_DIFFUSE, green)
    gear(0.5, 2.0, 2.0, 10, 0.7)
    GL.EndList()

    gear3 = GL.GenLists(1)
    GL.NewList(gear3, GL.COMPILE)
    GL.Materialfv(GL.FRONT, GL.AMBIENT_AND_DIFFUSE, blue)
    gear(1.3, 2.0, 0.5, 10, 0.7)
    GL.EndList()

    GL.Enable(GL.NORMALIZE)

    if len(sys.argv) > 1 and sys.argv[1] == "-info":
        print("GL_RENDERER   =", GL.GetString(GL.RENDERER))
        print("GL_VERSION    =", GL.GetString(GL.VERSION))
        print("GL_VENDOR     =", GL.GetString(GL.VENDOR))
        print("GL_EXTENSIONS =", GL.GetString(GL.EXTENSIONS))

    reshape(w,h)

    done = False
    while not done:
        angle += 2.0

        while True:
            event = SDL.PollEvent()
            if not event:
                break

            if event[0] == SDL.QUIT:
                done = True
                break

            #if event[0] == SDL.VIDEORESIZE:
            #    window = SDL.SetVideoMode(event[1], event[2], 16, SDL.OPENGL | SDL.RESIZABLE)
            #    if not window:
            #        print('Bad resize event!')
            #        sys.exit(-1)
            #    reshape(window.w, window.h)

        keys = SDL.GetKeyboardState()

        if keys[SDL.K_ESCAPE]:
            done = True

        if keys[SDL.K_SPACE]:
            spin = not spin

#        if keys[SDL.K_UP]:
#            view_rotx += 5.0
#
#        if keys[SDL.K_DOWN]:
#            view_rotx -= 5.0
#
#        if keys[SDL.K_LEFT]:
#            view_roty += 5.0
#
#        if keys[SDL.K_RIGHT]:
#            view_roty -= 5.0
#
#        if keys[SDL.K_z]:
#            if SDL.GetModState() & SDL.KMOD_SHIFT:
#                view_rotz -= 5.0
#            else:
#                view_rotz += 5.0
#
        GL.Clear(GL.COLOR_BUFFER_BIT | GL.DEPTH_BUFFER_BIT)

        GL.PushMatrix()
        GL.Rotatef(view_rotx, 1.0, 0.0, 0.0)
        GL.Rotatef(view_roty, 0.0, 1.0, 0.0)
        GL.Rotatef(view_rotz, 0.0, 0.0, 1.0)

        GL.PushMatrix()
        GL.Translatef(-3.0, -2.0, 0.0)
        GL.Rotatef(angle, 0.0, 0.0, 1.0)
        GL.CallList(gear1)
        GL.PopMatrix()

        GL.PushMatrix()
        GL.Translatef(3.1, -2.0, 0.0)
        GL.Rotatef(-2.0 * angle - 9.0, 0.0, 0.0, 1.0)
        GL.CallList(gear2)
        GL.PopMatrix()

        GL.PushMatrix()
        GL.Translatef(-3.1, 4.2, 0.0)
        GL.Rotatef(-2.0 * angle - 25.0, 0.0, 0.0, 1.0)
        GL.CallList(gear3)
        GL.PopMatrix()

        GL.PopMatrix()

        window.GL_SwapWindow()

        Frames += 1

        t = SDL.GetTicks()
        if (t - T0) >= 5000:
            seconds = (t - T0) / 1000.0
            fps = Frames / seconds
            print("%d frames in %g seconds = %g FPS" % (Frames,seconds,fps))
            T0 = t
            Frames = 0

    del window

    SDL.Quit()

if '__main__' == __name__:
    main()

