CC=gcc
CXX=g++

WARNING_FLAGS=-Wall -Wextra -Werror-implicit-function-declaration -Wshadow -Wstrict-prototypes -pedantic
CFLAGS=-g -O2 -std=c99 $(WARNING_FLAGS) -fno-strict-aliasing
UNAME=$(shell uname -s)

ifeq ($(UNAME), Darwin)
BULLET=../bullet64MAC
LDFLAGS=-framework GLUT -framework OpenGL -framework GLEW -framework Cocoa
else
#BULLET=../bullet32
BULLET=../bullet64
LDFLAGS=-g -lGL -lglut -lGLU -lGLEW -L${BULLET}/lib -lm
endif

CXXFLAGS=-g -O2 -W -Wall -I${BULLET}/include
LIBS=-lBulletDynamics -lBulletCollision -lLinearMath

.c.o:
	$(CC) -c $(CFLAGS) $<
.cpp.o:
	$(CXX) -c $(CXXFLAGS) $<

all: singlecurve multicurve
singlecurve: singlecurve.o bezier.o
	$(CC) -o singlecurve singlecurve.o bezier.o $(LDFLAGS)
multicurve: multicurve.o bezier.o v3math.o plymodel.o rply.o physics.o
	$(CXX) -o multicurve multicurve.o bezier.o v3math.o plymodel.o rply.o physics.o $(LDFLAGS) $(LIBS)

clean:
	rm -f *.o
	rm -f singlecurve
	rm -f multicurve

bezier.o        : bezier.h bezier.c
singlecurve.o   : bezier.h singlecurve.c
multicurve.o    : bezier.h multicurve.c plymodel.h physics.h
v3math.o        : v3math.h v3math.c
plymodel.o      : plymodel.c plymodel.h
rply.o          : rply.c rply.h
physics.o       : physics.h
