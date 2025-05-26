# define the Cpp compiler to use
CXX = g++

# define any compile-time flags
CXXFLAGS := -std=c++17 -Wall -Wextra -g

# define library paths in addition to /usr/lib
LFLAGS =

# define output directory
OUTPUT := output

# define source directory
SRC := src

# define include directory
INCLUDE := include

# define lib directory
LIB := lib
LIBRARIES := -lassimp -lglad -lglfw3dll -lfreeglut -lopengl32 -lglu32

MAIN := main
SOURCEDIRS := $(SRC)
INCLUDEDIRS := $(INCLUDE)
LIBDIRS := $(LIB)
FIXPATH = $1
RM = rm -f
MD := mkdir -p

# define any directories containing header files other than /usr/include
INCLUDES := $(patsubst %,-I%, $(INCLUDEDIRS:%/=%)) -I$(INCLUDE)/imgui -I$(INCLUDE)/imgui/backends -I$(INCLUDE)/assimp

# define the C libs
LIBS := $(patsubst %,-L%, $(LIBDIRS:%/=%))

# define main source file
MAIN_SRC := $(SRC)/main.cpp
MAIN_OBJ := $(MAIN_SRC:.cpp=.o)

# define other source files
OTHER_SRCS := $(filter-out $(MAIN_SRC), $(wildcard $(patsubst %,%/*.cpp, $(SOURCEDIRS))) $(wildcard $(INCLUDE)/imgui/*.cpp))
OTHER_OBJS := $(OTHER_SRCS:.cpp=.o)

OUTPUTMAIN := $(call FIXPATH,$(OUTPUT)/$(MAIN))

all: $(OUTPUT) $(MAIN)
	@echo Executing 'all' complete!

$(OUTPUT):
	$(MD) $(OUTPUT)

$(MAIN): $(MAIN_OBJ) $(OTHER_OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(OUTPUTMAIN) $^ $(LFLAGS) $(LIBS) $(LIBRARIES)

# Rule to compile main.cpp
$(MAIN_OBJ): $(MAIN_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Rule to compile other source files
.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: clean run depend
clean:
	$(RM) $(OUTPUTMAIN)
	$(RM) $(MAIN_OBJ)
	@echo Cleanup complete!

run: all
	./$(OUTPUTMAIN)
	@echo Executing 'run: all' complete!