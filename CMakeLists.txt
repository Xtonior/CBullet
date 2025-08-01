cmake_minimum_required(VERSION 3.10)
project(BulletCWrapper C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

# Укажи путь к Bullet (заголовки + библиотеки)
set(BULLET_SRC_DIR ${CMAKE_SOURCE_DIR}/wrapper/bullet3/src)
set(BULLET_LIB_DIR ${CMAKE_SOURCE_DIR}/wrapper/bullet3/build/src)

include_directories(${BULLET_SRC_DIR})
link_directories(
    ${BULLET_LIB_DIR}/BulletDynamics
    ${BULLET_LIB_DIR}/BulletCollision
    ${BULLET_LIB_DIR}/LinearMath
)

# Обёртка — компилируется как C++
add_library(bullet_wrapper STATIC wrapper/bullet_wrapper.cpp)

add_executable(test wrapper/main.c)

target_include_directories(bullet_wrapper PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/cbinding
)

# Линкуем обёртку и библиотеки Bullet
target_link_libraries(test bullet_wrapper
    BulletDynamics BulletCollision LinearMath
)

# Путь, куда скопировать всё необходимое
set(BUILD_OUTPUT_DIR ${CMAKE_BINARY_DIR})

# Скопируй .a библиотеки Bullet
file(COPY
    ${BULLET_LIB_DIR}/BulletDynamics/libBulletDynamics.a
    ${BULLET_LIB_DIR}/BulletCollision/libBulletCollision.a
    ${BULLET_LIB_DIR}/LinearMath/libLinearMath.a
    DESTINATION ${BUILD_OUTPUT_DIR}
)

# Скопируй обёртку
add_custom_command(TARGET bullet_wrapper POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:bullet_wrapper>
        ${BUILD_OUTPUT_DIR}/libbullet_wrapper.a
)
