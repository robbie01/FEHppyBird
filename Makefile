GITBINARY = git

CPPFLAGS = -MMD -MP -Os -DOBJC_OLD_DISPATCH_PROTOTYPES -g

WARNINGS = -Wall

LIB_DIR = simulator_libraries

INC_DIRS = -I$(LIB_DIR) -I.

OBJS = $(LIB_DIR)/FEHLCD.o $(LIB_DIR)/FEHRandom.o $(LIB_DIR)/FEHSD.o $(LIB_DIR)/FEHUtility.o $(LIB_DIR)/tigr.o

ifeq ($(OS),Windows_NT)
	CC=gcc
	LDFLAGS = -lopengl32 -lgdi32
	EXEC = game.exe
	SHELL := CMD
else
	ifeq ($(shell uname), Linux)
		LDFLAGS = -lGLU -lGL -lX11
	else
		LDFLAGS = -framework OpenGL -framework Cocoa
	endif
	EXEC = game.out
endif

SRC_FILES := $(wildcard ./*.cpp)
OBJ_FILES := $(patsubst ./%.cpp,./%.o,$(SRC_FILES))

$(EXEC): $(OBJ_FILES) $(OBJS)
	$(CXX) $(CPPFLAGS) $(WARNINGS) $(INC_DIRS) $(OBJ_FILES) $(OBJS) -o $(EXEC) $(LDFLAGS)

./%.o: ./%.cpp
	$(CXX) $(CPPFLAGS) $(WARNINGS) $(INC_DIRS) -c -o $@ $<

clean:
ifeq ($(OS),Windows_NT)
	del $(LIB_DIR)\*.o
	del $(LIB_DIR)\*.d
	del *.o *.d $(EXEC)
else
	rm -f $(LIB_DIR)/*.o $(LIB_DIR)/*.d
	rm -f *.o *.d $(EXEC)
endif
