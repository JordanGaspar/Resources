# Resources library for OpenGL software

## **It wasn't tested!!!** ##

This library stores textures and shaders with zlib in a Sqlite3 database.

It's a utility tool too. (Look CMakeLists.txt to see how to turn it off - *it will disable the store functions too!!!*)

*After compiling it you have the option to install it using cmake --install build_folder.*

This library has [ZLIB](https://www.zlib.net/), [ZLibCPP](https://github.com/JordanGaspar/ZLibCPP), [sqlite3](https://www.sqlite.org/), and [stb_image.h](https://github.com/nothings/stb) dependencies. See the respective licenses.

**This library supposes that you are using VCPKG to install dependencies.**

# Usage example

```
#include <Resources.hpp>

int main(){
    try {

        Resources db;

        db.storeTexture("../file_name.png", "texture_unique_identifier");

        auto [content, width, height, components] = db.getTexture("texture_unique_identifier");

    } catch (const std::exception& ec){

        std::cerr << ec.what() << std::endl;

    }

    return 0;
}
```