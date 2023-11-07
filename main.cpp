#ifdef RESOURCES_UTILITY

#include <Resources.hpp>
#include <iostream>

int main(int argc, char *argv[]) {
  try {
    if (argc == 1) {
      std::cout << "Usage: " << std::endl;
      std::cout
          << "\t./Resources store texture '../file_name.png' 'texture_name' "
          << "// to store texture with texture_name" << std::endl;
    }

    else if (std::string(argv[1]) == "store") {
      if (std::string(argv[2]) == "texture") {
        Resources db;
        db.storeTexture(argv[3], argv[4]);
      }
    }
  } catch (const std::exception &except) {
    std::cerr << "\nBad usage! Reason: " << except.what() << std::endl;
  }
}

#endif