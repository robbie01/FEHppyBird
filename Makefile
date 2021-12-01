CC = g++
LD = $(CC)
GITBINARY = git
FIRMWAREREPO = simulator_libraries
FEHURL = code.osu.edu

CPPFLAGS = -MMD -MP -Os -DOBJC_OLD_DISPATCH_PROTOTYPES -g

WARNINGS = -Wall

LIB_DIR = simulator_libraries

INC_DIRS = -I$(LIB_DIR) -I.

OBJS = $(LIB_DIR)/FEHLCD.o $(LIB_DIR)/FEHRandom.o $(LIB_DIR)/FEHSD.o $(LIB_DIR)/FEHUtility.o $(LIB_DIR)/tigr.o

ifeq ($(OS),Windows_NT)
	LDFLAGS = -lopengl32 -lgdi32
	EXEC = game.exe
	SHELL := CMD
else
	LDFLAGS = -framework OpenGL -framework Cocoa
	EXEC = game.out
endif

SRC_FILES := $(wildcard ./*.cpp)
OBJ_FILES := $(patsubst ./%.cpp,./%.o,$(SRC_FILES))

all: pre-build $(EXEC)

pre-build:
ifeq ($(OS),Windows_NT)	
# check for internet connection
	@ping -n 1 -w 1000 $(FEHURL) > NUL & \
	if errorlevel 1 \
	( \
		( echo "Warning: No internet connection!" ) \
	) \
	else \
	( \
		( \
			cd $(LIB_DIR) && \
			$(GITBINARY) stash && \
			$(GITBINARY) pull && \
			cd .. \
		) \
	) 
else
# Mac/Linux
	@ping -c 1 -W 1000 $(FEHURL) > /dev/null ; \
	if [ "$$?" -ne 0 ]; then \
		echo "Warning: No internet connection!"; \
	else \
		cd $(LIB_DIR) ; \
		$(GITBINARY) stash ; \
		$(GITBINARY) pull ; \
		cd .. ; \
	fi \

endif

$(EXEC): $(OBJ_FILES) $(OBJS)
	$(CC) $(CPPFLAGS) $(WARNINGS) $(INC_DIRS) $(OBJ_FILES) $(OBJS) -o $(EXEC) $(LDFLAGS)

./%.o: ./%.cpp
	$(CC) $(CPPFLAGS) $(WARNINGS) $(INC_DIRS) -c -o $@ $<

clean:
ifeq ($(OS),Windows_NT)
	del $(LIB_DIR)\*.o
	del $(LIB_DIR)\*.d
	del *.o *.d $(EXEC)
else
	rm $(LIB_DIR)/*.o $(LIB_DIR)/*.d
	rm *.o *.d $(EXEC)
endif