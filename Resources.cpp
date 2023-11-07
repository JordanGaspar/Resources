#include <Resources.hpp>
#include <iostream>
#include <sqlite3.h>
#include <stdexcept>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <stb_image.h>

struct Resources::impl {
  std::string database_path;
  const char *getDataBasePath();
  void setDataBasePath(const std::filesystem::path &path);
  void open();
  void prepare();
  sqlite3 *db;
  #ifdef RESOURCES_UTILITY
  sqlite3_stmt *insert_texture_stmt;
  #endif
  sqlite3_stmt *select_texture_by_name_stmt;
};

//! Get database_path as const char*
const char *Resources::impl::getDataBasePath() { return database_path.c_str(); }

//! Create database directory if not exists and set database_path;
void Resources::impl::setDataBasePath(const std::filesystem::path &path) {

  if (!std::filesystem::exists(path.parent_path())) {
    std::filesystem::create_directory(path.parent_path());
  }

  database_path = path.string();
}

//! Open database connection on path folder.
void Resources::impl::open() {

  if (sqlite3_open(getDataBasePath(), &db) != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errmsg(db));
  }

  #ifdef RESOURCES_UTILITY
  std::string texture_table =
      "CREATE TABLE IF NOT EXISTS TEXTURES "
      "(ID INTEGER NOT NULL PRIMARY KEY, NAME TEXT UNIQUE NOT NULL, WIDTH "
      "INTEGER NOT NULL, "
      "HEIGHT INTEGER NOT NULL, COMPONENTS INTEGER NOT NULL, CONTENT BLOB NOT "
      "NULL);";

  char *errmsg;

  if (sqlite3_exec(db, texture_table.c_str(), nullptr, nullptr, &errmsg) !=
      SQLITE_OK) {
    std::string error_message(errmsg);
    sqlite3_free(errmsg);
    throw std::runtime_error(error_message);
  }
  #endif
}

//! Prepare statements.
void Resources::impl::prepare() {

  #ifdef RESOURCES_UTILITY
  std::string insert_query =
      "INSERT INTO TEXTURES (NAME, WIDTH, HEIGHT, COMPONENTS, CONTENT)"
      "VALUES (?, ?, ?, ?, ?);";
  #endif

  std::string select_texture_by_name_query =
      "SELECT WIDTH, HEIGHT, COMPONENTS, CONTENT FROM TEXTURES WHERE NAME = ?;";

  #ifdef RESOURCES_UTILITY
  if (sqlite3_prepare_v2(db, insert_query.c_str(), insert_query.size(),
                         &insert_texture_stmt, nullptr) != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errmsg(db));
  }
  #endif

  if (sqlite3_prepare_v2(db, select_texture_by_name_query.c_str(),
                         select_texture_by_name_query.size(),
                         &select_texture_by_name_stmt, nullptr) != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errmsg(db));
  }
}

Resources::Resources(const std::filesystem::path &path) : pimpl{new impl} {
  pimpl->setDataBasePath(path);
  pimpl->open();
  pimpl->prepare();
}

Resources::~Resources() { delete pimpl; }

#ifdef RESOURCES_UTILITY
//! store new texture on the database;
void Resources::storeTexture(std::string_view filename, std::string_view name) {
  stbi_set_flip_vertically_on_load(true);

  int width, height, comp;
  auto file = stbi_load(filename.data(), &width, &height, &comp, 0);

  if (file == nullptr) {
    throw std::runtime_error(stbi_failure_reason());
  }

  unsigned int file_size = height * width * comp * sizeof(stbi_uc);

  std::stringstream compressed;
  boost::iostreams::filtering_ostream out;
  out.push(boost::iostreams::zlib_compressor());
  out.push(compressed);
  out.write(reinterpret_cast<char *>(file), file_size);
  boost::iostreams::close(out);

  stbi_image_free(file);

  if (sqlite3_bind_text(pimpl->insert_texture_stmt, 1, name.data(), name.size(),
                        SQLITE_STATIC) != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errmsg(pimpl->db));
  }

  if (sqlite3_bind_int(pimpl->insert_texture_stmt, 2, width) != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errmsg(pimpl->db));
  }

  if (sqlite3_bind_int(pimpl->insert_texture_stmt, 3, height) != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errmsg(pimpl->db));
  }

  if (sqlite3_bind_int(pimpl->insert_texture_stmt, 4, comp) != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errmsg(pimpl->db));
  }

  if (sqlite3_bind_blob64(pimpl->insert_texture_stmt, 5,
                          compressed.str().data(), compressed.str().size(),
                          SQLITE_STATIC) != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errmsg(pimpl->db));
  }

  if (sqlite3_step(pimpl->insert_texture_stmt) != SQLITE_DONE) {
    throw std::runtime_error(sqlite3_errmsg(pimpl->db));
  }

  if (sqlite3_clear_bindings(pimpl->insert_texture_stmt) != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errmsg(pimpl->db));
  }
}
#endif

Resources::texture_t Resources::getTexture(std::string_view name) {
  if (sqlite3_bind_text(pimpl->select_texture_by_name_stmt, 1, name.data(),
                        name.size(), SQLITE_STATIC) != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errmsg(pimpl->db));
  }

  int stage = 0;

  texture_t texture;

  auto &[content, width, height, components] = texture;

  while ((stage = sqlite3_step(pimpl->select_texture_by_name_stmt)) !=
         SQLITE_DONE) {
    if (stage == SQLITE_BUSY) {
      continue;
    } else if (stage == SQLITE_ROW) {
      width = sqlite3_column_int(pimpl->select_texture_by_name_stmt, 0);
      height = sqlite3_column_int(pimpl->select_texture_by_name_stmt, 1);
      components = sqlite3_column_int(pimpl->select_texture_by_name_stmt, 2);
      int size = sqlite3_column_bytes(pimpl->select_texture_by_name_stmt, 3);
      const char *buffer = reinterpret_cast<const char *>(
          sqlite3_column_blob(pimpl->select_texture_by_name_stmt, 3));

      std::stringstream strm;

      boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
      in.push(boost::iostreams::zlib_decompressor());
      in.push(boost::iostreams::array_source(buffer, size));

      boost::iostreams::copy(in, strm);

      auto begin = strm.tellg();
      strm.seekg(0, std::ios::end);
      auto strm_size = (strm.tellg() - begin);
      strm.seekg(0, std::ios::beg);

      content.resize(strm_size);

      strm.read(reinterpret_cast<char *>(content.data()),
                std::streamsize(content.size()));
    } else {
      throw std::runtime_error(sqlite3_errmsg(pimpl->db));
    }
  }

  if (sqlite3_clear_bindings(pimpl->select_texture_by_name_stmt) != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errmsg(pimpl->db));
  }

  return texture;
}