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
  Resources(const std::filesystem::path &path = DB_DEFAULT_PATH);
  ~Resources();
  texture_t getTexture(std::string_view name);

#ifdef RESOURCES_UTILITY
  void storeTexture(std::string_view filename, std::string_view name);
#endif
};

#endif