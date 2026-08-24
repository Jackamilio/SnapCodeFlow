CXX = g++

rwildcard = \
    $(foreach d,$(wildcard $1/*),$(call rwildcard,$d,$2)) \
    $(filter $(subst *,%,$2),$(wildcard $1/$2))

# --------------------------------------------------
# Détection de l'OS
# --------------------------------------------------

ifeq ($(OS),Windows_NT)
    PLATFORM = WINDOWS
else
    PLATFORM = LINUX
endif

# --------------------------------------------------
# Flags communs
# --------------------------------------------------

CXXFLAGS = \
	-std=c++20 \
	-Wall \
	-Wextra \
	-g \
	-MMD \
	-MP \
	-Iinclude \
	-Iexternal/imgui \
	-Iexternal/rlImGui

SRC = \
	$(call rwildcard,src,*.cpp) \
	external/imgui/imgui.cpp \
	external/imgui/imgui_draw.cpp \
	external/imgui/imgui_tables.cpp \
	external/imgui/imgui_widgets.cpp \
	external/imgui/imgui_demo.cpp \
	external/rlImGui/rlImGui.cpp

OBJ = $(SRC:%.cpp=build/obj/%.o)
DEP = $(OBJ:.o=.d)

# --------------------------------------------------
# Windows
# --------------------------------------------------

ifeq ($(PLATFORM),WINDOWS)

    LUAJIT_INCLUDE = C:/msys64/ucrt64/include/luajit-2.1

    CXXFLAGS += -I$(LUAJIT_INCLUDE)

    LDLIBS = \
        -lraylib \
        -lopengl32 \
        -lgdi32 \
        -lwinmm \
        -lcomdlg32 \
        -lluajit-5.1

    TARGET = build/SnapCodeFlow.exe

    MKDIR = @if not exist "$(dir $@)" mkdir "$(dir $@)"
    RUNCMD = $(TARGET)
    CLEAN = @if exist build rmdir /S /Q build

# --------------------------------------------------
# Linux
# --------------------------------------------------

else

    CXXFLAGS += $(shell pkg-config --cflags luajit)

    LDLIBS = \
        -lraylib \
        $(shell pkg-config --libs luajit)

    TARGET = build/SnapCodeFlow

    MKDIR = @mkdir -p "$(dir $@)"
    RUNCMD = ./$(TARGET)
    CLEAN = @rm -rf build

endif

# --------------------------------------------------
# Cibles
# --------------------------------------------------

all: $(TARGET)


$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDLIBS)


build/obj/%.o: %.cpp
	$(MKDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@


clean:
	$(CLEAN)


run: $(TARGET)
	$(RUNCMD)

-include $(DEP)