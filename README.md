# C Wrapper for Bullet Physics (WIP)

This is a **work-in-progress** C wrapper for the [Bullet Physics](https://github.com/bulletphysics/bullet3) engine.

> This project does **not** include the original Bullet Physics source code. You must manually copy the Bullet source code into `wrapper/bullet3`.

---

## Building

Quick build:

```bash
./build.sh
```

Manual steps:

```bash
# Generate the C API header
python generate_api.py wrapper/bullet_wrapper.cpp cbinding/bullet_api.h

# Configure and build
mkdir build
cd build
cmake ..
cmake --build .
cd ..
```

---

## Including in Your Project

To use the wrapper in your C project:

### Include:

```c
#include "bullet_api.h"
```

### Link with:

```
-lbullet_wrapper -lBulletDynamics -lBulletCollision -lLinearMath -lstdc++
```

> Note: `-lstdc++` is required to link against the C++ standard library.

---

## 📁 Required Files

Make sure the following files are available in your project:

* `cbinding/bullet_api.h`
* `build/libbullet_wrapper.a`
* `build/libBulletCollision.a`
* `build/libBulletDynamics.a`
* `build/libLinearMath.a`