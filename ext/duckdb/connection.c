#include "ruby-duckdb.h"

static VALUE cDuckDBConnection;

static void deallocate(void *ctx)
{
    rubyDuckDBConnection *p = (rubyDuckDBConnection *)ctx;

    duckdb_disconnect(&(p->con));
    xfree(p);
}

static VALUE allocate(VALUE klass)
{
    rubyDuckDBConnection *ctx = xcalloc((size_t)1, sizeof(rubyDuckDBConnection));
    return Data_Wrap_Struct(klass, NULL, deallocate, ctx);
}

VALUE create_connection(VALUE oDuckDBDatabase) {
    rubyDuckDB *ctxdb;
    rubyDuckDBConnection *ctxcon;
    VALUE obj;

    Data_Get_Struct(oDuckDBDatabase, rubyDuckDB, ctxdb);

    obj = allocate(cDuckDBConnection);
    Data_Get_Struct(obj, rubyDuckDBConnection, ctxcon);

    if (duckdb_connect(ctxdb->db, &(ctxcon->con)) == DuckDBError) {
        rb_raise(rb_eRuntimeError, "connection error");
    }

    // rb_ivar_set(obj, rb_intern("database"), oDuckDBDatabase);
    return obj;
}

static VALUE duckdb_connection_query(VALUE self, VALUE str) {

    rubyDuckDBConnection *ctx;
    rubyDuckDBResult *ctxr;

    VALUE result = create_result();

    Data_Get_Struct(self, rubyDuckDBConnection, ctx);
    Data_Get_Struct(result, rubyDuckDBResult, ctxr);

    if (duckdb_query(ctx->con, StringValueCStr(str), &(ctxr->result)) == DuckDBError) {
        rb_raise(rb_eRuntimeError, "%s", ctxr->result.error_message);
    }
    return result;
}

void init_duckdb_connection(void)
{
    cDuckDBConnection = rb_define_class_under(mDuckDB, "Connection", rb_cObject);
    rb_define_alloc_func(cDuckDBConnection, allocate);

    rb_define_method(cDuckDBConnection, "query", duckdb_connection_query, 1)
}
