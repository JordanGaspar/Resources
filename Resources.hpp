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