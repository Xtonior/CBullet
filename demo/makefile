CC = gcc
CFLAGS = -I../cbinding
LDFLAGS = -L../build -lbullet_wrapper -lBulletDynamics -lBulletCollision -lLinearMath -lstdc++ -lGL -lglfw -lGLEW -lcglm -lm

BUILD_DIR = build
OBJ_DIR = obj

TARGET = $(BUILD_DIR)/main
OBJ = $(OBJ_DIR)/main.o

$(TARGET): $(OBJ)
	@mkdir -p $(BUILD_DIR)
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJ): main.c ../cbinding/bullet_api.h
	@mkdir -p $(OBJ_DIR)
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -rf $(BUILD_DIR) $(OBJ_DIR)
