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
# Port of glxgears to test SDL2 / GL python bindings
# By Matthew Shaw 2019
#

import collections
import math
import sys
import threading
import time

import SDL2
import pygl as GL

class Gears:
    def __init__(self, index=0):
        self.index        = index
        self.title        = f'Gears {index+1}'
        self.frames       = 0
        self.gear1        = None
        self.gear2        = None
        self.gear3        = None
        self.view_rotx    = 20.0
        self.view_roty    = 30.0
        self.view_rotz    = 0.0
        self.angle        = 0.0
        self.isFullscreen = False

        x,y,w,h = SDL2.GetDisplayBounds(self.index)
        self.window = SDL2.Window(
            self.title,
            size     = (w-200,h-200),
            position = (x+100,y+100),
            flags    = SDL2.WINDOW_OPENGL | SDL2.WINDOW_RESIZABLE
            )

        if not self.window:
            print(f'Could not set {w}x{h} GL video mode: {SDL2.GetError()}')
            SDL2.Quit()
            sys.exit(2)

        self.init()
        self.reshape(w, h)
        self.time0 = 0

    def toggleFullscreen(self):
        if self.isFullscreen:
            w,h = self.oldSize
            x,y = self.oldPosition
            self.window.SetWindowFullscreen(0)
            self.window.SetWindowSize((w,h))
            self.window.SetWindowPosition((x,y))
            self.isFullscreen = False
        else:
            self.oldSize     = self.window.GetWindowSize()
            self.oldPosition = self.window.GetWindowPosition()
            x,y,w,h = SDL2.GetDisplayBounds(self.index)
            self.window.SetWindowSize((w,h))
            self.window.SetWindowFullscreen(SDL2.WINDOW_FULLSCREEN_DESKTOP)
            self.isFullscreen = True

    def init(self):
        pos   = [5.0, 5.0, 10.0, 0.0]
        red   = [0.8, 0.1, 0.0, 1.0]
        green = [0.0, 0.8, 0.2, 1.0]
        blue  = [0.2, 0.2, 1.0, 1.0]

        self.window.GL_CreateContext()
        self.window.GL_MakeCurrent()

        GL.Lightfv(GL.LIGHT0, GL.POSITION, pos)
        GL.Enable(GL.CULL_FACE)
        GL.Enable(GL.LIGHTING)
        GL.Enable(GL.LIGHT0)
        GL.Enable(GL.DEPTH_TEST)

        # make the gears
        self.gear1 = GL.GenLists(1)
        GL.NewList(self.gear1, GL.COMPILE)
        GL.Materialfv(GL.FRONT, GL.AMBIENT_AND_DIFFUSE, red)
        gear(1.0, 4.0, 1.0, 20, 0.7)
        GL.EndList()

        self.gear2 = GL.GenLists(1)
        GL.NewList(self.gear2, GL.COMPILE)
        GL.Materialfv(GL.FRONT, GL.AMBIENT_AND_DIFFUSE, green)
        gear(0.5, 2.0, 2.0, 10, 0.7)
        GL.EndList()

        self.gear3 = GL.GenLists(1)
        GL.NewList(self.gear3, GL.COMPILE)
        GL.Materialfv(GL.FRONT, GL.AMBIENT_AND_DIFFUSE, blue)
        gear(1.3, 2.0, 0.5, 10, 0.7)
        GL.EndList()

        GL.Enable(GL.NORMALIZE)

    # new window size or exposure
    def reshape(self, width, height):
        h = float(height) / float(width)

        self.window.GL_MakeCurrent()
        GL.Viewport(0, 0, width, height)
        GL.MatrixMode(GL.PROJECTION)
        GL.LoadIdentity()
        GL.Frustum(-1.0, 1.0, -h, h, 5.0, 60.0)
        GL.MatrixMode(GL.MODELVIEW)
        GL.LoadIdentity()
        GL.Translatef(0.0, 0.0, -40.0)

    def draw(self):
        self.window.GL_MakeCurrent()

        GL.Clear(GL.COLOR_BUFFER_BIT | GL.DEPTH_BUFFER_BIT)

        GL.PushMatrix()
        GL.Rotatef(self.view_rotx, 1.0, 0.0, 0.0)
        GL.Rotatef(self.view_roty, 0.0, 1.0, 0.0)
        GL.Rotatef(self.view_rotz, 0.0, 0.0, 1.0)

        GL.PushMatrix()
        GL.Translatef(-3.0, -2.0, 0.0)
        GL.Rotatef(self.angle, 0.0, 0.0, 1.0)
        GL.CallList(self.gear1)
        GL.PopMatrix()

        GL.PushMatrix()
        GL.Translatef(3.1, -2.0, 0.0)
        GL.Rotatef(-2.0 * self.angle - 9.0, 0.0, 0.0, 1.0)
        GL.CallList( 2)
        GL.PopMatrix()

        GL.PushMatrix()
        GL.Translatef(-3.1, 4.2, 0.0)
        GL.Rotatef(-2.0 * self.angle - 25.0, 0.0, 0.0, 1.0)
        GL.CallList(self.gear3)
        GL.PopMatrix()

        GL.PopMatrix()

        self.window.GL_SwapWindow()

        self.frames += 1

        t = SDL2.GetTicks()
        if (t - self.time0) >= 5000:
            seconds = (t - self.time0) / 1000.0
            fps = self.frames / seconds
            print(f'{self.title}: {self.frames} frames in {seconds:.2f} seconds = {fps:.2f} FPS')
            self.time0  = t
            self.frames = 0


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


def main():
    SDL2.Init(SDL2.INIT_VIDEO)
    numOfDisplays = SDL2.GetNumVideoDisplays()

    windows = [Gears(i) for i in range(numOfDisplays)]

    if len(sys.argv) > 1 and sys.argv[1] == "-info":
        print(f'VENDOR   = {GL.GetString(GL.VENDOR)}')
        print(f'RENDERER = {GL.GetString(GL.RENDERER)}')
        print(f'VERSION  = {GL.GetString(GL.VERSION)}')
        print()

        extensions = GL.GetString(GL.EXTENSIONS).split()
        extensions.sort()

        info = collections.defaultdict(list)
        for e in extensions:
            e = e.split('_',2)[1:]
            info[e[0]].append(e[1])

        for vendor,extensions in info.items():
            print(vendor)
            print('=' * len(vendor))
            for extension in extensions:
                print(f'GL_{vendor}_{extension}')
            print()

    try:
        prevTick = 0
        done = False
        while not done:
            keys = SDL2.GetKeyState()
            if keys[SDL2.SCANCODE_ESCAPE]:
                done = True
            if keys[SDL2.SCANCODE_UP]:
                for w in windows:
                    w.view_rotx += 5.0
            if keys[SDL2.SCANCODE_DOWN]:
                for w in windows:
                    w.view_rotx -= 5.0
            if keys[SDL2.SCANCODE_LEFT]:
                for w in windows:
                    w.view_roty += 5.0
            if keys[SDL2.SCANCODE_RIGHT]:
                for w in windows:
                    w.view_roty -= 5.0
            if keys[SDL2.SCANCODE_Z]:
                if SDL2.GetModState() & SDL2.KMOD_SHIFT:
                    for gear in windows:
                        gear.view_rotz -= 5.0
                else:
                    for w in windows:
                        w.view_rotz += 5.0

            delta = SDL2.GetTicks() - prevTick
            prevTick += delta
            for w in windows:
                w.draw()
                w.angle += delta / 10

            while True:
                event = SDL2.PollEvent()
                if not event:
                    break
                event,data = event

                if event == SDL2.QUIT:
                    done = True
                    break
                elif event == SDL2.WINDOWEVENT:
                    if data[0] == SDL2.WINDOWEVENT_SIZE_CHANGED:
                        for w in windows:
                            if w.window.GetWindowID() == data[3]:
                                w.reshape(data[1],data[2])
                                break
                    elif data[0] == SDL2.WINDOWEVENT_CLOSE:
                        for w in windows:
                            if w.window.GetWindowID() == data[3]:
                                del w.window
                                windows.remove(w)
                elif event == SDL2.KEYDOWN:
                    if data[1] == SDL2.SCANCODE_F:
                        for w in windows:
                            w.toggleFullscreen()

        SDL2.Quit()

    except KeyboardInterrupt:
        pass

if '__main__' == __name__:
    main()
