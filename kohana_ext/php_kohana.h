/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2010 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: dreamszhu                                                    |
  | Email: dreamsxin@qq.com                                              |
  +----------------------------------------------------------------------+
*/

/* $Id: header 297205 2010-03-30 21:09:07Z johannes $ */

#ifndef PHP_KOHANA_H
#define PHP_KOHANA_H

extern zend_module_entry kohana_module_entry;
#define phpext_kohana_ptr &kohana_module_entry

#ifdef PHP_WIN32
#	define PHP_KOHANA_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_KOHANA_API __attribute__ ((visibility("default")))
#else
#	define PHP_KOHANA_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(kohana);
PHP_MSHUTDOWN_FUNCTION(kohana);
PHP_RINIT_FUNCTION(kohana);
PHP_RSHUTDOWN_FUNCTION(kohana);
PHP_MINFO_FUNCTION(kohana);

PHP_FUNCTION(confirm_kohana_compiled);	/* For testing, remove later. */

PHP_METHOD(Kohana_Arr, is_assoc);
PHP_METHOD(Kohana_Arr, is_array);
PHP_METHOD(Kohana_Arr, path);
PHP_METHOD(Kohana_Arr, set_path);
PHP_METHOD(Kohana_Arr, range);
PHP_METHOD(Kohana_Arr, get);
PHP_METHOD(Kohana_Arr, extract);
PHP_METHOD(Kohana_Arr, pluck);
PHP_METHOD(Kohana_Arr, unshift);
PHP_METHOD(Kohana_Arr, map);
PHP_METHOD(Kohana_Arr, merge);
PHP_METHOD(Kohana_Arr, overwrite);
PHP_METHOD(Kohana_Arr, callback);
PHP_METHOD(Kohana_Arr, flatten);

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:
 * 定义 zend_kohana_globals
*/
ZEND_BEGIN_MODULE_GLOBALS(kohana)
	//long  global_value;
	//char *global_string;
        char *module_database;
        char *module_cache;
        char *module_orm;
        char *module_auth;
ZEND_END_MODULE_GLOBALS(kohana)


/* In every utility function you add that needs to use variables 
   in php_kohana_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as KOHANA_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define KOHANA_G(v) TSRMG(kohana_globals_id, zend_kohana_globals *, v)
#else
#define KOHANA_G(v) (kohana_globals.v)
#endif

/* 自定义 */
#define KOHANA_ARR_DELIMITER "."

#endif	/* PHP_KOHANA_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
