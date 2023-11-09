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

#ifndef RESOURCES_HPP_
#define RESOURCES_HPP_

#include <filesystem>
#include <string_view>
#include <vector>

#ifdef __unix__
#define DB_DEFAULT_PATH "/opt/resources/resources.sqlite"
#elifdef _WIN32
#define DB_DEFAULT_PATH "C:\Program Files\Resources\resources.sqlite"
#endif

class Resources {
  class impl;
  impl *pimpl;

public:
  //! tuple with content, width, height, and components per pixel
  typedef std::tuple<std::vector<unsigned char>, std::size_t, std::size_t,
                     std::size_t>
      texture_t;

  //! Constructor with the default database path.
  Resources(const std::filesystem::path &path = DB_DEFAULT_PATH);
  ~Resources();

  //! Get texture using its unique name. Returns the content filped top side down
  //! and the width, height, and number of componentes like 3 for GL_RGB or 4 to 
  //! GL_RGBA.
  texture_t getTexture(std::string_view name);

  //! Get the shader file by its unique name.
  std::string getShader(std::string_view name);

#ifdef RESOURCES_UTILITY
  //! Store texture file compressed with zlib using the unique name as identifier.
  void storeTexture(std::string_view filename, std::string_view name);
  //! Store a shader file with zlib using the unique name as identifier and a type to 
  //! help to know how kind of shader it was.
  void storeShader(std::string_view filename, std::string_view name, std::string_view type);
#endif
};

#endif