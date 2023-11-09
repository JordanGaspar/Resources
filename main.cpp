#ifdef RESOURCES_UTILITY

#include <Resources.hpp>
#include <iostream>

int main(int argc, char *argv[]) {
  try {
    if (argc == 1) {
      std::cout << "Usage: " << std::endl;
      std::cout
          << "\t./Resources store texture '../file_name.png' 'texture_name' "
          << "// to store texture with texture_name\n" << std::endl;

      std::cout
          << "\t./Resources store shader '../file_name.glsl' 'shader_name' 'shader_type' "
          << "// to store shader with shader_name of shader_type" << std::endl;
    }
    else if (std::string(argv[1]) == "store") {
      Resources db;
      if (std::string(argv[2]) == "texture") {
        db.storeTexture(argv[3], argv[4]);
      } else if (std::string(argv[2]) == "shader"){
        db.storeShader(argv[3], argv[4], argv[5]);
      }
    }
  } catch (const std::exception &except) {
    std::cerr << "\nBad usage! Reason: " << except.what() << std::endl;
  }
}

#endif