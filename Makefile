CXX = g++
CXXFLAGS = -std=c++17 -I../SFML/include

LDFLAGS = -L../SFML/lib
LIBS = -lsfml-graphics -lsfml-window -lsfml-system

SRC = Escalonador.cpp Sistema.cpp Processo.cpp
OBJ = $(SRC:.cpp=.o)

TARGET = escalonador

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) $(LDFLAGS) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)