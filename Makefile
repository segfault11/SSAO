CC = /usr/bin/g++
FLAGS = -Wall
LIBS = -lSDL2 -lGLEW 
FRAMEWORKS = -framework OpenGL
TARGET = SSAO 
OBJ = OpenGL.o SSAORenderer.o Model.o Vector3f.o Camera.o Timer.o Random.o

$(TARGET): main.cpp $(OBJ)
	$(CC) -o $(TARGET) main.cpp $(OBJ) $(FLAGS) $(LIBS) $(FRAMEWORKS) 

%.o: %.cpp
	$(CC) -c $<
