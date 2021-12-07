CFLAGS = -MMD -MP -Os -DOBJC_OLD_DISPATCH_PROTOTYPES -g
CXXFLAGS = -std=c++11 $(CFLAGS)
WARNINGS = -Wall -Wextra

LIB_DIR = simulator_libraries

CPPFLAGS = -I$(LIB_DIR) -I.

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
OBJS += $(patsubst ./%.cpp,./%.o,$(SRC_FILES))

$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) $(WARNINGS) $(INC_DIRS) $^ -o $@ $(LDFLAGS)

.PHONY: clean

clean:
ifeq ($(OS),Windows_NT)
	del $(LIB_DIR)\*.o
	del $(LIB_DIR)\*.d
	del *.o *.d $(EXEC)
else
	rm -f $(LIB_DIR)/*.o $(LIB_DIR)/*.d
	rm -f *.o *.d $(EXEC)
endif
