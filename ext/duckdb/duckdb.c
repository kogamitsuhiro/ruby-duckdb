#include "ruby-duckdb.h"

VALUE mDuckDB;

void
Init_duckdb(void) {
    mDuckDB = rb_define_module("DuckDB");

    init_duckdb_database();
    init_duckdb_connection();
    init_duckdb_result();
}
