BUILD_DIR := ./build
OBJ_DIR:= $(BUILD_DIR)/objects
INSTALLATION_DIR := /usr/local/bin

cppFiles = $(wildcard include/*.cpp)
objects := $(cppFiles:.cpp=.o)

objects := $(addprefix $(OBJ_DIR)/,$(notdir $(objects)))

main: $(objects) $(OBJ_DIR)/main.o
	$(CXX) -Wno-invalid-offsetof $^ -o $(BUILD_DIR)/WayVes `pkg-config --cflags --libs gtk4-layer-shell-0 libpipewire-0.3 epoxy gtk4` -lm -lcyaml

$(objects): $(OBJ_DIR)/%.o: include/%.cpp
	$(CXX) -Wno-invalid-offsetof -c $^ -o $@ `pkg-config --cflags --libs gtk4-layer-shell-0 libpipewire-0.3 epoxy gtk4` -lm -lcyaml 

$(OBJ_DIR)/main.o: main.cpp
	$(CXX) -Wno-invalid-offsetof -c main.cpp -o $(OBJ_DIR)/main.o `pkg-config --cflags --libs gtk4-layer-shell-0 libpipewire-0.3 epoxy gtk4` -lm -lcyaml 

clean:
	rm -f $(OBJ_DIR)/*.o $(BUILD_DIR)/WayVes

install: main
	install -m 755 $(BUILD_DIR)/WayVes $(INSTALLATION_DIR)/WayVes
