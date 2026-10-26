BUILD_DIR := ./build
OBJ_DIR:= $(BUILD_DIR)/objects
INSTALLATION_DIR := /usr/local/bin

cppFiles = $(wildcard include/*.cpp)
objects := $(cppFiles:.cpp=.o)

objects := $(addprefix $(OBJ_DIR)/,$(notdir $(objects)))

main: $(objects) $(OBJ_DIR)/main.o
	$(CXX) -g -Wno-invalid-offsetof $^ -o $(BUILD_DIR)/WayVes `pkg-config --cflags --libs  dbus-1 gtk4-layer-shell-0 libpipewire-0.3  gbm epoxy gtk4` -lm -lcyaml -lEGL -lGL

$(objects): $(OBJ_DIR)/%.o: include/%.cpp
	$(CXX) -g -Wno-invalid-offsetof -c $^ -o $@ `pkg-config --cflags --libs   dbus-1 gtk4-layer-shell-0 libpipewire-0.3 gbm epoxy gtk4` -lm -lcyaml -lEGL -lGL

$(OBJ_DIR)/main.o: main.cpp
	$(CXX) -g -Wno-invalid-offsetof -c main.cpp -o $(OBJ_DIR)/main.o `pkg-config --cflags --libs   dbus-1 gtk4-layer-shell-0 libpipewire-0.3 gbm epoxy gtk4` -lm -lcyaml -lEGL -lGL

clean:
	rm -f $(OBJ_DIR)/*.o $(BUILD_DIR)/WayVes

install: main
	install -m 755 $(BUILD_DIR)/WayVes $(INSTALLATION_DIR)/WayVes
