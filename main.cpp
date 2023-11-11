/*MIT License

Copyright (c) 2023 Jordan Gaspar Alves Silva

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

/*
Author: Jordan Gaspar Alves Silva;
Year: 2023;
E-mail: jordangaspar@gmail.com
*/

#ifdef RESOURCES_UTILITY

#include <Resources.hpp>
#include <iostream>

int main(int argc, char *argv[]) {
  try {
    if (argc == 1) {
      std::cout << "Usage: " << std::endl;
      std::cout
          << "\t./Resources store texture '../file_name.png' 'texture_name' "
          << "// to store texture with texture_name\n"
          << std::endl;

      std::cout << "\t./Resources store shader '../file_name.glsl' "
                   "'shader_name' 'shader_type' "
                << "// to store shader with shader_name of shader_type"
                << std::endl;
    } else if (std::string(argv[1]) == "store") {
      Resources db;
      if (std::string(argv[2]) == "texture") {
        db.storeTexture(argv[3], argv[4]);
      } else if (std::string(argv[2]) == "shader") {
        db.storeShader(argv[3], argv[4], argv[5]);
      }
    }
  } catch (const std::exception &except) {
    std::cerr << "\nBad usage! Reason: " << except.what() << std::endl;
  }
}

#endif