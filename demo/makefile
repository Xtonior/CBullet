CC = gcc
CFLAGS = -I../cbinding
LDFLAGS = -L../build -lbullet_wrapper -lBulletDynamics -lBulletCollision -lLinearMath -lstdc++ -lGL -lglfw -lGLEW -lcglm -lm

BUILD_DIR = build
OBJ_DIR = obj
SRC_DIR = .

TARGET = $(BUILD_DIR)/main
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

$(TARGET): $(OBJ) | $(BUILD_DIR)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	@mkdir -p $@

clean:
	rm -rf $(BUILD_DIR) $(OBJ_DIR)
