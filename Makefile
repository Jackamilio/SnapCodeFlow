CXX = g++

rwildcard = \
    $(foreach d,$(wildcard $1/*),$(call rwildcard,$d,$2)) \
    $(filter $(subst *,%,$2),$(wildcard $1/$2))

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

LDFLAGS = \
	-lraylib \
	-lopengl32 \
	-lgdi32 \
	-lwinmm \
	-lcomdlg32

SRC = \
	$(call rwildcard,src,*.cpp) \
	external/imgui/imgui.cpp \
	external/imgui/imgui_draw.cpp \
	external/imgui/imgui_tables.cpp \
	external/imgui/imgui_widgets.cpp \
	external/rlImGui/rlImGui.cpp

OBJ = $(SRC:%.cpp=build/obj/%.o)

DEP = $(OBJ:.o=.d)

TARGET = build/SnapCodeFlow.exe


all: $(TARGET)


$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)


build/obj/%.o: %.cpp
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CXX) $(CXXFLAGS) -c $< -o $@


clean:
	@if exist build rmdir /S /Q build


run: $(TARGET)
	$(TARGET)

-include $(DEP)