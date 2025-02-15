# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -std=c++17 -fexceptions -g

# commands
SRCS = $(if $(filter $(OS),Windows_NT),src\windows.cpp,src/linux.cpp)
TARGET = $(if $(filter $(OS),Windows_NT),build\shush.exe,build/shush)

.PHONY: adjl adjm clean build

# Build
build:
	$(if $(filter $(OS),Windows_NT),if not exist build mkdir build,if [ ! -d build ]; then mkdir build; fi)
	$(CXX) $(CXXFLAGS) -c include/*.cpp
	$(CXX) $(CXXFLAGS) -c $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) *.o
	$(if $(filter $(OS),Windows_NT),move *.o build\,mv *.o build/)

# Clean
clean:
	$(if $(filter $(OS),Windows_NT),del build\*.o,rm -f build/*.o)
	$(if $(filter $(OS),Windows_NT),del $(TARGET),rm -f $(TARGET))

# Install
install:
	$(if $(filter $(OS),Windows_NT),copy $(TARGET) C:\Windows\System32\,sudo cp $(TARGET) /usr/bin/)