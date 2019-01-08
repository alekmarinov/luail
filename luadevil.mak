# modules/c/gui/il/luail.mak

TARGET=luadevil
VERSION=1.0
OBJS=luail.o luailu.o
# OBJS+=luailut.o

# EXTRA_DEFS=-DILUT_USE_OPENGL=1
EXTRA_DEFS=-DWITH_ILU
#EXTRA_DEFS+=-DWITH_ILUT
EXTRA_LIBS=$(shell pkg-config ILU --libs)
EXTRA_LIBS+=$(shell pkg-config ILUT --libs)

