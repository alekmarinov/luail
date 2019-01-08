LuaDevIL v1.1
Binding to the Devil [2] library for the Lua [1] language

LuaDevIL is Lua 5.x compatible module providing imaging capabilities
based on the DevIL library. The module interface follows strictly the
DevIL architecture and is very easy to use if the programmer has
already experience with DevIL.

-- Install instructions:
There is no build scripts intentionally. You should create one
yourself depending on your building environment. It is most easy
if you have a unix shell, lua and devil libraries installed and
PKG_CONFIG_PATH properly set

$gcc -shared -o luadevil.so `pkg-config --cflags lua IL ILU ILUT` \
 `pkg-config --libs lua IL ILU ILUT` luail.c  luailu.c  luailut.c 

Then copy the result luadevil.so to where your LUA_CPATH points.

-- Test:

example.lua/example1.lua is a quick and durty example.

$lua example.lua/example1.lua <image.ext>

The result should be 3 new files image_wave.ext, image_scale.ext and
image_buzz.ext which represents the original image with applied some
transofrmations and filtering.

example.lua/example2.lua demonstrate LuaDevIL usage in combination
with LuaGL. Unfortunately it will not work with the original LuaGL
binding [3] as it doesn't support GLUT (at least at the time of
writing this)

-- Thanks:
to Lua community for all nice stuff around and to you for your
interest to LuaDevIL.

-- References:
1. Lua   - http://www.lua.org
2. DevIL - http://openil.sourceforge.net
3. LuaGL - http://luagl.luaforge.net

-- PS:
Don't hesitate to contact me for bugs and requests.
Author:       Alexander Marinov (alekmarinov@gmail.com)
