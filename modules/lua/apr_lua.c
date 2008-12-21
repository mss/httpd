#include "apr.h"
#include "apr_tables.h"

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "apr_lua.h"

/**
 * make a userdata out of a C pointer, and vice versa
 * instead of using lightuserdata
 */
#ifndef lua_boxpointer
#define lua_boxpointer(L,u) (*(void **)(lua_newuserdata(L, sizeof(void *))) = (u))
#define lua_unboxpointer(L,i)	(*(void **)(lua_touserdata(L, i)))
#endif


apr_table_t* check_apr_table(lua_State* L, int index) {
    luaL_checkudata(L, index, "Apr.Table");
    apr_table_t* t = (apr_table_t*)lua_unboxpointer(L, index);
    return t;
}


void apl_push_apr_table(lua_State* L, const char *name, apr_table_t *t) {
    lua_boxpointer(L, t);    
    luaL_getmetatable(L, "Apr.Table");
    lua_setmetatable(L, -2);
    lua_setfield(L, -2, name);
}

static int lua_table_set(lua_State* L) {
    apr_table_t *t = check_apr_table(L, 1);
    const char* key = luaL_checkstring(L, 2);
    const char* val = luaL_checkstring(L, 3);

    apr_table_set(t, key, val);
    return 0;
}

static int lua_table_get(lua_State* L) {
    apr_table_t *t = check_apr_table(L, 1);
    const char* key = luaL_checkstring(L, 2);
    const char *val = apr_table_get(t, key);
    lua_pushstring(L, val);
    return 1;
}

static const luaL_reg lua_table_methods[] = {
    {"set", lua_table_set},
    {"get", lua_table_get},
    {0, 0}
};


int apr_lua_init(lua_State *L, apr_pool_t *p) {
    luaL_newmetatable(L, "Apr.Table");
    luaL_openlib(L, "apr_table", lua_table_methods, 0);
    lua_pushstring(L, "__index");
    lua_pushstring(L, "get");
    lua_gettable(L, 2);
    lua_settable(L, 1);

    lua_pushstring(L, "__newindex");
    lua_pushstring(L, "set");
    lua_gettable(L, 2);
    lua_settable(L, 1);
    
    return 0;
}
