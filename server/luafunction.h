/* 
 * File:   luafunction.h
 * Author: Administrator
 *
 * Created on 2009��11��12��, ����9:49
 */

#ifndef _LUAFUNCTION_H
#define	_LUAFUNCTION_H

#ifdef	__cplusplus
extern "C" {
#endif

    //lua
    #include "lua/lua.h"
    #include "lua/lualib.h"
    #include "lua/lauxlib.h"
    
    lua_State *L;//Lua解释器指针
    void lua_init();
    void lua_load_config();
    char *lua_load_key(const char *account);
    void lua_load_room();

#ifdef	__cplusplus
}
#endif

#endif	/* _LUAFUNCTION_H */

