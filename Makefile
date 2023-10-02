CXX := g++

BINDIR := bin
OBJDIR := obj
# Too lazy adding another directory to .gitignore, lol
DEPDIR := $(OBJDIR)

SRC := $(wildcard *.cpp patches/*.cpp patches/*/*.cpp patches/*/*/*.cpp)
HEADERS := $(wildcard *.h patches/*.h patches/*/*.h patches/*/*/*.h doukutsu/*.h)
DEF := exports_mingw.def

DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d
CXXFLAGS := -std=c++20 -Wall -pedantic-errors -m32 -iquote "$(CURDIR)"
LDFLAGS := -luuid -lole32 -lwinmm -lshlwapi -static -shared -Wl,--subsystem,windows

BIN := $(BINDIR)/dinput.dll
OBJ := $(patsubst %.cpp,$(OBJDIR)/%.o,$(SRC))
DEP := $(patsubst %.cpp,$(DEPDIR)/%.d,$(SRC))

OBJ_SUBDIRS := $(patsubst %/,%,$(sort $(dir $(OBJ))))

# Need to compile with mingw32-x64-i686-gcc to not have errors when linking
ifneq ($(MSYSTEM_CARCH), i686)
$(error Please compile in the MSYS2 MinGW 32-bit environment!)
endif

all: release
release: OFLAGS += -O2 -s
debug: OFLAGS += -g
debug: CXXFLAGS += -D_DEBUG

release debug: $(BIN)

clean:
	rm -r $(BINDIR) $(OBJDIR)

.SECONDEXPANSION:
$(OBJDIR)/%.o: %.cpp $(DEPDIR)/%.d | $$(@D)
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) $(OFLAGS) -c $< -o $@

$(BIN): $(OBJ) $(DEF) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(OFLAGS) $^ $(LDFLAGS) -o $(BIN)

$(BINDIR) $(OBJ_SUBDIRS):
	@mkdir -p $@

$(DEP):
include $(wildcard $(DEP))

.PHONY: all release debug clean
