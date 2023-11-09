#include <span>
#include <sqlite3.h>
#include <stdexcept>

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