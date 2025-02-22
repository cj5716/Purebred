CXX     := g++
TARGET  := Purebred
NAME    := Purebred
TMPDIR   = .tmp
SOURCES := $(wildcard src/*.cpp)
OBJECTS := $(patsubst %.cpp,$(TMPDIR)/%.o,$(SOURCES))
DEPENDS := $(patsubst %.cpp,$(TMPDIR)/%.d,$(SOURCES))
DEBUG   := no

WARNINGS := -Wall -Wcast-qual -Wextra -Wshadow -Wdouble-promotion -Wformat=2 -Wnull-dereference -Wlogical-op -Wold-style-cast -Wundef -pedantic
NORMAL   := -O3 -std=c++20 $(WARNINGS) -funroll-loops -flto -fno-exceptions
DEBUGS   := -g3 -fno-omit-frame-pointer -fsanitize=address -fsanitize=leak -fsanitize=undefined
NONDEBUG := -DNDEBUG

SSE41FLAGS   := -DUSE_SSE41 -msse4.1
AVXFLAGS     := -DUSE_AVX -mavx $(SSE41FLAGS)
AVX2FLAGS    := -DUSE_AVX2 -mavx2 $(AVXFLAGS)
AVX512FLAGS  := -DUSE_AVX512 -mavx512f -mavx512bw $(AVX2FLAGS)
VNNI512FLAGS := -DUSE_VNNI -mavx512vnni $(AVX512FLAGS)
ARCHFLAGS    :=

LDFLAGS     :=
SUFFIX      :=
DETECTED_OS :=
MKDIR       :=
NATIVE      :=
BUILT       := NO

ifeq ($(OS), Windows_NT)
	DETECTED_OS := Windows
	SUFFIX := .exe
	MKDIR := mkdir
else
	DETECTED_OS := $(shell uname -s)
	SUFFIX :=
	LDFLAGS += -pthread
	ifeq ($(COMP), MINGW)
		MKDIR := mkdir
	else
		MKDIR := mkdir -p
	endif
endif

ifeq ($(CXX), clang++)
	ifneq ($(DETECTED_OS), Darwin)
		LDFLAGS += -fuse-ld=lld
	endif
endif

CXXFLAGS := $(NORMAL)
ifeq ($(DEBUG), yes)
	CXXFLAGS += $(DEBUGS)
else
	CXXFLAGS += $(NONDEBUG)
endif

PROPERTIES     := $(shell echo | $(CXX) -march=native -E -dM -)
DETECTED_FLAGS :=
ifneq ($(findstring __SSE41__, $(PROPERTIES)),)
	DETECTED_FLAGS := $(SSE41FLAGS)

endif

ifneq ($(findstring __AVX__, $(PROPERTIES)),)
	DETECTED_FLAGS := $(AVXFLAGS)
endif

ifneq ($(findstring __AVX2__, $(PROPERTIES)),)
	DETECTED_FLAGS := $(AVX2FLAGS)
endif

ifneq ($(findstring __AVX512F__, $(PROPERTIES)),)
	ifneq ($(findstring __AVX512BW__, $(PROPERTIES)),)
		DETECTED_FLAGS := $(AVX512FLAGS)
	endif
endif

ifneq ($(findstring __AVX512VNNI__, $(PROPERTIES)),)
	DETECTED_FLAGS := $(VNNI512FLAGS)
endif

ifndef BUILD
	BUILD := native
endif

ifeq ($(BUILD), x86-64)
	NATIVE := -mtune=znver1
	ARCHFLAGS :=
	BUILT := YES
endif

ifeq ($(BUILD), x86-64-sse41)
	NATIVE := -march=nehalem
	ARCHFLAGS := $(SSE41FLAGS)
	BUILT := YES
endif

ifeq ($(BUILD), x86-64-avx)
	NATIVE := -march=sandybridge
	ARCHFLAGS := $(AVXFLAGS)
	BUILT := YES
endif

ifeq ($(BUILD), x86-64-avx2)
	NATIVE := -march=bdver4
	ARCHFLAGS := $(AVX2FLAGS)
	BUILT := YES
endif

ifeq ($(BUILD), x86-64-avx512)
	NATIVE := -march=x86-64-v4
	ARCHFLAGS := $(AVX512FLAGS)
	BUILT := YES
endif

ifeq ($(BUILD), x86-64-vnni512)
	NATIVE := -march=znver4
	ARCHFLAGS := $(VNNI512FLAGS)
	BUILT := YES
endif

ifeq ($(BUILD), native)
	NATIVE := -march=native
	ARCHFLAGS := $(DETECTED_FLAGS)
	BUILT := YES
endif

ifeq ($(BUILT), NO)
    $(error Invalid build type)
endif

EXE := $(NAME)$(SUFFIX)

all: $(TARGET)
clean:
	@rm -rf $(TMPDIR) *.o  $(DEPENDS) *.d

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(ARCHFLAGS) $(NATIVE) -MMD -MP -o $(EXE) $^ $(FLAGS)

$(TMPDIR)/%.o: %.cpp | $(TMPDIR)
	$(CXX) $(CXXFLAGS) $(ARCHFLAGS) $(NATIVE) -MMD -MP -c $< -o $@ $(FLAGS)

$(TMPDIR):
	$(MKDIR) "$(TMPDIR)" "$(TMPDIR)/src"

-include $(DEPENDS)