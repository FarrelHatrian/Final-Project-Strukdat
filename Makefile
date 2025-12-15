# Nama executable
TARGET = bola

# Compiler
CXX = g++

# Flags compile
CXXFLAGS = -std=c++17 -Wall -O2

# Semua file .cpp di folder saat ini
SRC = $(wildcard *.cpp*)

# Library SFML
LIBS = -lsfml-graphics -lsfml-window -lsfml-system

# Default target: build
all: $(TARGET)

# Compile program
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) $(LIBS) -o $(TARGET)

# Jalankan program
run: $(TARGET)
	@echo "Running $(TARGET)..."
	./$(TARGET)

# Bersihkan file executable
clean:
	rm -f $(TARGET)
