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

#include "Resources_helper.hpp"
#include <Resources.hpp>
#include <fstream>
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
  sqlite3_stmt *insert_shader_stmt;
  sqlite3_stmt *insert_shader_type_stmt;
  sqlite3_stmt *select_shader_type_stmt;
#endif
  sqlite3_stmt *select_texture_by_name_stmt;
  sqlite3_stmt *select_shader_by_name_stmt;
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

  std::string shader_type_table =
      "CREATE TABLE IF NOT EXISTS SHADER_TYPE "
      "(ID INTEGER NOT NULL PRIMARY KEY, TYPE TEXT UNIQUE NOT NULL);";

  std::string shader_table =
      "CREATE TABLE IF NOT EXISTS SHADERS "
      "(ID INTEGER NOT NULL PRIMARY KEY, NAME TEXT UNIQUE NOT NULL, "
      "TYPE INTEGER NOT NULL, "
      "CONTENT BLOB NOT NULL,"
      "FOREIGN KEY (TYPE) REFERENCES SHADER_TYPE(ID));";

  open_helper(db, texture_table);
  open_helper(db, shader_type_table);
  open_helper(db, shader_table);
#endif
}

//! Prepare statements.
void Resources::impl::prepare() {

#ifdef RESOURCES_UTILITY
  std::string insert_texture_query =
      "INSERT INTO TEXTURES (NAME, WIDTH, HEIGHT, COMPONENTS, CONTENT)"
      "VALUES (?, ?, ?, ?, ?);";

  std::string insert_shader_type_query =
      "INSERT INTO SHADER_TYPE (TYPE) VALUES (?) RETURNING ID;";

  std::string insert_shader_query =
      "INSERT INTO SHADERS (NAME, TYPE, CONTENT) VALUES (?, ?, ?);";

  std::string select_shader_type_query =
      "SELECT ID FROM SHADER_TYPE WHERE TYPE = ?;";

#endif

  std::string select_texture_by_name_query =
      "SELECT WIDTH, HEIGHT, COMPONENTS, CONTENT FROM TEXTURES WHERE NAME = ?;";

  std::string select_shader_by_name_query =
      "SELECT CONTENT FROM SHADERS WHERE NAME = ?;";

#ifdef RESOURCES_UTILITY
  prepare_helper(db, insert_texture_query, insert_texture_stmt);
  prepare_helper(db, insert_shader_type_query, insert_shader_type_stmt);
  prepare_helper(db, insert_shader_query, insert_shader_stmt);
  prepare_helper(db, select_shader_type_query, select_shader_type_stmt);
#endif

  prepare_helper(db, select_texture_by_name_query, select_texture_by_name_stmt);
  prepare_helper(db, select_shader_by_name_query, select_shader_by_name_stmt);
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

  bind_text_helper(pimpl->db, pimpl->insert_texture_stmt, name, 1);

  bind_int_helper(pimpl->db, pimpl->insert_texture_stmt, 2, width);

  bind_int_helper(pimpl->db, pimpl->insert_texture_stmt, 3, height);

  bind_int_helper(pimpl->db, pimpl->insert_texture_stmt, 4, comp);

  bind_blob64_helper(pimpl->db, pimpl->insert_texture_stmt, compressed.str(),
                     5);

  if (sqlite3_step(pimpl->insert_texture_stmt) != SQLITE_DONE) {
    throw std::runtime_error(sqlite3_errmsg(pimpl->db));
  }

  if (sqlite3_clear_bindings(pimpl->insert_texture_stmt) != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errmsg(pimpl->db));
  }
}

bool shaderTypeDialog(std::string_view type) {
  std::string response("z");

  while (response != "y") {
    response.clear();
    std::cout << "\nThe type \'" << type << "\' not exists!" << std::endl;
    std::cout << "You want to add it? \'y\' to yes; \'n\' to no, and \'a\' to "
                 "yes to all: ";
    std::cin >> response;

    if (response == "a") {
      return true;
    }

    if (response == "n") {
      std::cout << "So bye!" << std::endl;
      std::exit(EXIT_SUCCESS);
    }
  }

  return false;
}

void Resources::storeShader(std::string_view filename, std::string_view name,
                            std::string_view type) {

  bind_text_helper(pimpl->db, pimpl->select_shader_type_stmt, type, 1);

  int stage = 0;
  int type_id = -1;
  while ((stage = sqlite3_step(pimpl->select_shader_type_stmt)) !=
         SQLITE_DONE) {
    switch (stage) {
    case SQLITE_BUSY:
      break;
    case SQLITE_ROW:
      type_id = sqlite3_column_int(pimpl->select_shader_type_stmt, 0);
      break;
    default:
      throw std::runtime_error("Something wrong when selecting TYPE");
    }
  }

  if (sqlite3_clear_bindings(pimpl->select_shader_type_stmt) != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errmsg(pimpl->db));
  }

  static bool yes_to_all = false;

  if (type_id == -1) {

    if (!yes_to_all) {
      yes_to_all = shaderTypeDialog(type);
    }

    bind_text_helper(pimpl->db, pimpl->insert_shader_type_stmt, type, 1);

    int stage;
    while ((stage = sqlite3_step(pimpl->insert_shader_type_stmt)) !=
           SQLITE_DONE) {
      switch (stage) {
      case SQLITE_BUSY:
        continue;
      case SQLITE_ROW:
        type_id = sqlite3_column_int(pimpl->insert_shader_type_stmt, 0);
        break;
      default:
        throw std::runtime_error(sqlite3_errmsg(pimpl->db));
      }
    }

    if (type_id == -1) {
      throw std::runtime_error(
          "Something goes wrong with type inserting! Type: \'" +
          std::string(type) + "\'");
    }

    if (sqlite3_clear_bindings(pimpl->insert_shader_type_stmt) != SQLITE_OK) {
      throw std::runtime_error(sqlite3_errmsg(pimpl->db));
    }
  }

  std::ifstream file((std::filesystem::path(filename)));

  if (!file.is_open()) {
    throw std::runtime_error("Can't open the file \'" + std::string(filename) +
                             "\'");
  }

  std::stringstream strm;
  boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
  in.push(boost::iostreams::zlib_compressor());
  in.push(file);
  boost::iostreams::copy(in, strm);

  file.close();

  strm.seekg(std::ios::end);
  auto strm_size = strm.tellg();
  strm.seekg(std::ios::beg);

  std::vector<char> file_data(strm_size);

  strm.read(file_data.data(), std::streamsize(strm_size));

  bind_text_helper(pimpl->db, pimpl->insert_shader_stmt, name, 1);
  bind_int_helper(pimpl->db, pimpl->insert_shader_stmt, type_id, 2);
  bind_blob64_helper(pimpl->db, pimpl->insert_shader_stmt, file_data.data(), 3);

  int status;
  while ((status = sqlite3_step(pimpl->insert_shader_stmt)) != SQLITE_DONE) {
    switch (status) {
    case SQLITE_BUSY:
      continue;
    default:
      throw std::runtime_error(sqlite3_errmsg(pimpl->db));
    }
  }

  if (sqlite3_clear_bindings(pimpl->insert_shader_stmt)) {
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

std::string Resources::getShader(std::string_view name){
  std::string str;
  bind_text_helper(pimpl->db, pimpl->select_shader_by_name_stmt, name, 1);

  int status;
  while ((status =
              sqlite3_step(pimpl->select_shader_by_name_stmt) != SQLITE_DONE)) {
    switch (status) {
    case SQLITE_BUSY:
      continue;
    case SQLITE_ROW: {
      auto data = sqlite3_column_blob(pimpl->select_shader_by_name_stmt, 0);
      auto size = sqlite3_column_bytes(pimpl->select_shader_by_name_stmt, 0);

      boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
      in.push(boost::iostreams::zlib_decompressor());
      in.push(boost::iostreams::array_source(reinterpret_cast<const char*>(data), size));

      std::stringstream strm;
      boost::iostreams::copy(in, strm);

      str = strm.str();
    } break;
    default:
      throw std::runtime_error(sqlite3_errmsg(pimpl->db));
    }
  }

  if (sqlite3_clear_bindings(pimpl->select_shader_by_name_stmt)){
    throw std::runtime_error(sqlite3_errmsg(pimpl->db));
  }

  return str;
}