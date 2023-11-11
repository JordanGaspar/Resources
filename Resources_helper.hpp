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

#include <span>
#include <sqlite3.h>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <cstddef>
#include <zlib.h>

//! Open database helper function.
inline void open_helper(sqlite3*& db, const std::string& query){
  char *errmsg;

  if (sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errmsg) !=
      SQLITE_OK) {
    std::string error_message(errmsg);
    sqlite3_free(errmsg);
    throw std::runtime_error(error_message);
  }
}

//! Prepare statements helper function
inline void prepare_helper(sqlite3 *&db, const std::string &query,
                    sqlite3_stmt *&stmt) {
  if (sqlite3_prepare_v2(db, query.c_str(), query.size(), &stmt, nullptr) !=
      SQLITE_OK) {
    throw std::runtime_error(sqlite3_errmsg(db));
  }
}

inline void bind_text_helper(sqlite3 *&db, sqlite3_stmt *&stmt,
                             std::string_view data, int position) {
  if (sqlite3_bind_text(stmt, position, data.data(), data.size(),
                        SQLITE_STATIC) != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errmsg(db));
  }
}

inline void bind_int_helper(sqlite3 *&db, sqlite3_stmt *&stmt, int data,
                            int position) {
  if (sqlite3_bind_int(stmt, position, data) != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errmsg(db));
  }
}

inline void bind_blob64_helper(sqlite3 *&db, sqlite3_stmt *&stmt,
                               std::string_view data, int position) {
  if (sqlite3_bind_blob64(stmt, position, data.data(), data.size(),
                          SQLITE_STATIC) != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errmsg(db));
  }
}

inline void bind_blob64_helper(sqlite3 *&db, sqlite3_stmt *&stmt,
                               std::span<unsigned char> data, int position) {
  if (sqlite3_bind_blob64(stmt, position, data.data(), data.size_bytes(),
                          SQLITE_STATIC) != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errmsg(db));
  }
}