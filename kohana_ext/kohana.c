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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "zend.h"
#include "ext/standard/info.h"
#include "ext/standard/php_string.h"
#include "php_kohana.h"

#include <ctype.h>

/* 
 * If you declare any globals in php_kohana.h uncomment this:
 * kohana_globals
 */
ZEND_DECLARE_MODULE_GLOBALS(kohana)

/* True global resources - no need for thread safety here */
static int le_kohana;

/* {{{ kohana_functions[]
 *
 * Every user visible function must have an entry in kohana_functions[].
 */
const zend_function_entry kohana_functions[] = {
    PHP_FE(confirm_kohana_compiled, NULL) /* For testing, remove later. */ {
        NULL, NULL, NULL
    } /* Must be the last line in kohana_functions[] */
};
/* }}} */

/* {{{ kohana_module_entry
 */
zend_module_entry kohana_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    "kohana",
    kohana_functions,
    PHP_MINIT(kohana),
    PHP_MSHUTDOWN(kohana),
    PHP_RINIT(kohana), /* Replace with NULL if there's nothing to do at request start */
    PHP_RSHUTDOWN(kohana), /* Replace with NULL if there's nothing to do at request end */
    PHP_MINFO(kohana),
#if ZEND_MODULE_API_NO >= 20010901
    "0.1", // NO_VERSION_YET
#endif
    /*
        NO_MODULE_GLOBALS, //PHP_MODULE_GLOBALS(kohana), // NO_MODULE_GLOBALS
        NULL,
        NULL,
        NULL,
        STANDARD_MODULE_PROPERTIES_EX
     */
    STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_KOHANA

ZEND_GET_MODULE(kohana)
#endif

/* {{{ PHP_INI
 */
PHP_INI_BEGIN()
STD_PHP_INI_ENTRY("kohana.module_database", "disable", PHP_INI_ALL, OnUpdateString, module_database, zend_kohana_globals, kohana_globals)
STD_PHP_INI_ENTRY("kohana.module_cache", "disable", PHP_INI_ALL, OnUpdateString, module_cache, zend_kohana_globals, kohana_globals)
STD_PHP_INI_ENTRY("kohana.module_orm", "disable", PHP_INI_ALL, OnUpdateString, module_orm, zend_kohana_globals, kohana_globals)
STD_PHP_INI_ENTRY("kohana.module_auth", "disable", PHP_INI_ALL, OnUpdateString, module_auth, zend_kohana_globals, kohana_globals)
PHP_INI_END()
/* }}} */

/* {{{ php_kohana_init_globals
 */
static void php_kohana_init_globals(zend_kohana_globals *kohana_globals) {
    kohana_globals->module_database = NULL;
    kohana_globals->module_cache = NULL;
    kohana_globals->module_orm = NULL;
    kohana_globals->module_auth = NULL;
}
/* }}} */

/* 类申明 */
zend_class_entry *kohana_arr_ce;

/* {{{ 类方法的参数 */

/*
ZEND_BEGIN_ARG_INFO(arg_person_setname, 0)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()
 */

ZEND_BEGIN_ARG_INFO(args_for_kohana_arr_is_assoc, 0)
ZEND_ARG_ARRAY_INFO(0, array, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(args_for_kohana_arr_is_array, 0)
ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(args_for_kohana_arr_path, 0, 0, 2)
ZEND_ARG_ARRAY_INFO(0, array, 0)
ZEND_ARG_INFO(0, path)
ZEND_ARG_INFO(0, default)
ZEND_ARG_INFO(0, delimiter)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(args_for_kohana_arr_set_path, 0, 0, 3)
ZEND_ARG_ARRAY_INFO(1, array, 0)
ZEND_ARG_INFO(0, path)
ZEND_ARG_INFO(0, value)
ZEND_ARG_INFO(0, delimiter)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(args_for_kohana_arr_range, 0, 0, 0)
ZEND_ARG_INFO(0, step)
ZEND_ARG_INFO(0, max)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(args_for_kohana_arr_get, 0, 0, 2)
ZEND_ARG_ARRAY_INFO(1, array, 0)
ZEND_ARG_INFO(0, key)
ZEND_ARG_INFO(0, default)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(args_for_kohana_arr_extract, 0, 0, 2)
ZEND_ARG_ARRAY_INFO(1, array, 0)
ZEND_ARG_ARRAY_INFO(0, key, 0)
ZEND_ARG_INFO(0, default)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(args_for_kohana_arr_pluck, 0, 0, 2)
ZEND_ARG_ARRAY_INFO(1, array, 0)
ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(args_for_kohana_arr_unshift, 0, 0, 2)
ZEND_ARG_ARRAY_INFO(1, array, 0)
ZEND_ARG_INFO(0, key)
ZEND_ARG_INFO(0, val)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(args_for_kohana_arr_map, 0, 0, 2)
ZEND_ARG_ARRAY_INFO(1, array, 0)
ZEND_ARG_INFO(0, func)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(args_for_kohana_arr_overwrite, 0, 0, 2)
ZEND_ARG_ARRAY_INFO(1, arr1, 0)
ZEND_ARG_ARRAY_INFO(1, arr2, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ kohana_arr_functions
 */
const zend_function_entry kohana_arr_functions[] = {
    PHP_ME(Kohana_Arr, is_assoc, args_for_kohana_arr_is_assoc, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Kohana_Arr, is_array, args_for_kohana_arr_is_array, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Kohana_Arr, path, args_for_kohana_arr_path, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Kohana_Arr, set_path, args_for_kohana_arr_set_path, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Kohana_Arr, range, args_for_kohana_arr_range, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Kohana_Arr, get, args_for_kohana_arr_get, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Kohana_Arr, extract, args_for_kohana_arr_extract, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Kohana_Arr, pluck, args_for_kohana_arr_pluck, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Kohana_Arr, unshift, args_for_kohana_arr_unshift, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Kohana_Arr, map, args_for_kohana_arr_map, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Kohana_Arr, merge, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Kohana_Arr, overwrite, args_for_kohana_arr_overwrite, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Kohana_Arr, callback, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Kohana_Arr, flatten, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC) {
        NULL, NULL, NULL
    }
};
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(kohana) {
    REGISTER_INI_ENTRIES();

    zend_class_entry kohana_arr;

    INIT_CLASS_ENTRY(kohana_arr, "Kohana_Arr", kohana_arr_functions);
    kohana_arr_ce = zend_register_internal_class_ex(&kohana_arr, NULL, NULL TSRMLS_CC);

    /* 声明常量 */
    zend_declare_class_constant_stringl(kohana_arr_ce, ZEND_STRL("delimiter"), ZEND_STRL(KOHANA_ARR_DELIMITER) TSRMLS_CC);

    /* 声明私有成员变量  */
    zend_declare_property_null(kohana_arr_ce, ZEND_STRL("_name"), ZEND_ACC_PRIVATE TSRMLS_CC);
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(kohana) {
    UNREGISTER_INI_ENTRIES();
    return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(kohana) {
    return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */

/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(kohana) {
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(kohana) {
    php_info_print_table_start();
    php_info_print_table_header(2, "kohana support", "enabled");
    php_info_print_table_row(2, "author", "dreamszhu");
    php_info_print_table_row(2, "version", "0,1");
    php_info_print_table_end();

    DISPLAY_INI_ENTRIES();
}
/* }}} */


/* Remove the following function when you have succesfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */

/* {{{ proto string confirm_kohana_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_kohana_compiled) {
    char *arg = NULL;
    int arg_len, len;
    char *strg;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
        return;
    }

    len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "kohana", arg);
    RETURN_STRINGL(strg, len, 0);
}
/* }}} */

/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
 */

static int kohana_arr_is_assoc(zval *array) {

    zval **entry;
    char *string_key; /* String key */
    uint string_key_len;
    ulong num_key; /* Numeric key */
    HashPosition pos;

    zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(array), &pos);
    while (zend_hash_get_current_data_ex(Z_ARRVAL_P(array), (void **) &entry, &pos) == SUCCESS) {

        if (zend_hash_get_current_key_ex(Z_ARRVAL_P(array), &string_key, &string_key_len, &num_key, 1, &pos) != HASH_KEY_IS_LONG) {
            return SUCCESS;
        }
        //php_printf("pos:%ld, num_key:%ld<br/>", pos->h, num_key);
        if (num_key != pos->h) {
            return SUCCESS;
        }
        zend_hash_move_forward_ex(Z_ARRVAL_P(array), &pos);
    }

    return FAILURE;
}

static int kohana_arr_is_array(zval *value) {
    if (Z_TYPE_P(value) == IS_ARRAY) {
        return SUCCESS;
    }

    if (Z_TYPE_P(value) == IS_OBJECT) {
        zend_class_entry **ce;
        if (zend_lookup_class_ex("Traversable", 11, 0, &ce TSRMLS_CC) == FAILURE) {
            return FAILURE;
        }

        if (instanceof_function(Z_OBJCE_P(value), *ce TSRMLS_CC)) {
            return SUCCESS;
        }
    }
    return FAILURE;
}

/* {{{ static Kohana_Arr Kohana_Arr::is_assoc([array array])
 */
PHP_METHOD(Kohana_Arr, is_assoc) {
    zval *array = NULL;

    if (ZEND_NUM_ARGS() != 1) {
        WRONG_PARAM_COUNT;
    }

    if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &array)) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Argument #%d should be an array", 1);
        RETURN_FALSE;
    }

    if (kohana_arr_is_assoc(array) == SUCCESS) {
        RETURN_TRUE;
    } else {
        RETURN_FALSE;
    }

    //*return_value = *array;
    RETURN_ZVAL(array, 1, 0);
}
/* }}} */

/* {{{ static Kohana_Arr Kohana_Arr::is_array([mixed value])
 */
PHP_METHOD(Kohana_Arr, is_array) {
    zval *value = NULL;

    if (ZEND_NUM_ARGS() != 1) {
        WRONG_PARAM_COUNT;
    }

    if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &value)) {
        RETURN_FALSE;
    }

    if (SUCCESS != kohana_arr_is_array(value)) {
        RETURN_FALSE;
    }
    RETURN_TRUE;
}

/* }}} */

static int ctype_digit(zval *c) {
    zval tmp;
    if (Z_TYPE_P(c) == IS_LONG) {
        if (Z_LVAL_P(c) <= 255 && Z_LVAL_P(c) >= 0) {
            if (!isdigit(Z_LVAL_P(c))) {
                return FAILURE;
            }
            return SUCCESS;
        } else if (Z_LVAL_P(c) >= -128 && Z_LVAL_P(c) < 0) {
            if (!isdigit(Z_LVAL_P(c) + 256)) {
                return FAILURE;
            }
            return SUCCESS;
        }
        tmp = *c;
        zval_copy_ctor(&tmp);
        convert_to_string(&tmp);
    } else {
        tmp = *c;
    }
    if (Z_TYPE(tmp) == IS_STRING) {
        char *p = Z_STRVAL(tmp), *e = Z_STRVAL(tmp) + Z_STRLEN(tmp);
        if (e == p) {
            if (Z_TYPE_P(c) == IS_LONG) zval_dtor(&tmp);
            return FAILURE;
        }
        while (p < e) {
            if (!isdigit((int) *(unsigned char *) (p++))) {
                if (Z_TYPE_P(c) == IS_LONG) zval_dtor(&tmp);
                return FAILURE;
            }
        }
    } else {
        return FAILURE;
    }

    return SUCCESS;
}

static void kohana_arr_path(zval *array, zval *path, zval *delim, zval *defaultvalue, zval *return_value) {

    zval *keys;
    zval **findvalue;
    long limit = LONG_MAX;


    switch (Z_TYPE_P(path)) {
        case IS_ARRAY:
            keys = path;
            break;
        case IS_STRING:

            if (zend_hash_find(Z_ARRVAL_P(array), Z_STRVAL_P(path), Z_STRLEN_P(path), (void **) findvalue) == SUCCESS) {
                RETURN_ZVAL(*findvalue, 1, 0);
                return;
            }

            MAKE_STD_ZVAL(keys);
            array_init(keys);

            php_explode(delim, path, keys, limit);
            break;
        case IS_LONG:
            if (zend_hash_index_find(Z_ARRVAL_P(array), Z_LVAL_P(path), (void **) &findvalue) == SUCCESS) {
                RETURN_ZVAL(*findvalue, 1, 0);
                return;
            } else {
                RETURN_ZVAL(defaultvalue, 1, 0);
                return;
            }
            break;
        case IS_NULL:
            if (zend_hash_find(Z_ARRVAL_P(array), "", 1, (void **) &findvalue) == SUCCESS) {
                RETURN_ZVAL(*findvalue, 1, 0);
                return;
            } else {
                RETURN_ZVAL(defaultvalue, 1, 0);
                return;
            }
            break;
        default:
            php_error_docref(NULL TSRMLS_CC, E_WARNING, "The second argument should be either a string or an integer");
            return;
    }

    zval **key;
    HashPosition pos;

    zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(keys), &pos);
    while (zend_hash_get_current_data_ex(Z_ARRVAL_P(keys), (void **) &key, &pos) == SUCCESS) {

        zval *tmp;
        MAKE_STD_ZVAL(tmp);
        *tmp = **key;
        zval_copy_ctor(tmp);
        char *p = Z_STRVAL_P(tmp);
        zend_hash_index_del(Z_ARRVAL_P(keys), pos->h);
        zend_hash_move_forward_ex(Z_ARRVAL_P(keys), &pos);

        if (ctype_digit(tmp) == SUCCESS) {
            ulong idx = atol(Z_STRVAL_P(tmp));
            if (zend_hash_index_find(Z_ARRVAL_P(array), idx, (void **) &findvalue) == SUCCESS) {
                if (zend_hash_get_current_data_ex(Z_ARRVAL_P(array), (void **) &key, &pos) == SUCCESS) {
                    if (SUCCESS == kohana_arr_is_array(*findvalue)) {
                        array = *findvalue;
                        continue;
                    }
                } else {
                    RETURN_ZVAL(*findvalue, 1, 0);
                    return;
                }
            }
        } else {
            if (zend_hash_find(Z_ARRVAL_P(array), Z_STRVAL_P(tmp), Z_STRLEN_P(tmp) + 1, (void **) &findvalue) == SUCCESS) {
                if (zend_hash_get_current_data_ex(Z_ARRVAL_P(array), (void **) &key, &pos) == SUCCESS) {
                    if (SUCCESS == kohana_arr_is_array(*findvalue)) {
                        array = *findvalue;
                        continue;
                    }
                } else {
                    RETURN_ZVAL(*findvalue, 1, 0);
                    return;
                }
            } else if (*p == '*') {

                zval **item;
                HashPosition pos2;
                zval *dotdelim;

                Z_STRLEN_P(dotdelim) = sizeof (KOHANA_ARR_DELIMITER) - 1;
                Z_STRVAL_P(dotdelim) = estrdup(KOHANA_ARR_DELIMITER);

                zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(array), &pos2);
                array_init(return_value);
                while (zend_hash_get_current_data_ex(Z_ARRVAL_P(array), (void **) &item, &pos2) == SUCCESS) {
                    php_implode(dotdelim, keys, path TSRMLS_DC);
                    zval *value;
                    kohana_arr_path(*item, path, delim, defaultvalue, value);
                    add_next_index_zval(return_value, value);
                }
            }
            break;
        }
    }

    RETURN_ZVAL(defaultvalue, 1, 0);
}

PHP_METHOD(Kohana_Arr, path) {
    zval *array = NULL;
    zval *path = NULL;
    zval *defaultvalue = NULL;
    zval *delim = NULL;
    if (ZEND_NUM_ARGS() < 2 || ZEND_NUM_ARGS() > 4) {
        WRONG_PARAM_COUNT;
    }
    if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz|zz", &array, &path, &defaultvalue, &delim)) {
        RETURN_ZVAL(defaultvalue, 1, 0);
    }

    if (SUCCESS != kohana_arr_is_array(array)) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Argument #%d should be an array", 1);
        RETURN_ZVAL(defaultvalue, 1, 0);
    }
    if (defaultvalue == NULL) {
        MAKE_STD_ZVAL(defaultvalue);
        ZVAL_NULL(defaultvalue);
    }

    if (delim == NULL) {
        MAKE_STD_ZVAL(delim);
        ZVAL_NULL(delim);
    }
    if (Z_TYPE_P(delim) == IS_NULL) {
        Z_STRLEN_P(delim) = sizeof (KOHANA_ARR_DELIMITER) - 1;
        Z_STRVAL_P(delim) = estrdup(KOHANA_ARR_DELIMITER);
    }
    kohana_arr_path(array, path, delim, defaultvalue, return_value TSRMLS_DC);
}

static void kohana_arr_set_path(zval *array, zval *path, zval *value, zval *delim) {

    zval *keys;
    long limit = LONG_MAX;

    MAKE_STD_ZVAL(keys);
    array_init(keys);

    php_explode(delim, path, keys, limit);

    zval **key;
    HashPosition pos;

    zval *currentItem;
    zval *tempArr = array;
    //MAKE_STD_ZVAL(currentItem);

    ulong count = zend_hash_num_elements(Z_ARRVAL_P(keys));

    zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(keys), &pos);
    while (zend_hash_get_current_data_ex(Z_ARRVAL_P(keys), (void **) &key, &pos) == SUCCESS) {
        zval *tmp;
        MAKE_STD_ZVAL(tmp);
        *tmp = **key;
        zval_copy_ctor(tmp);

        zval *emptyarr;
        zval **findvalue;
        MAKE_STD_ZVAL(emptyarr);
        array_init(emptyarr);

        convert_to_string(tmp);

        if (ctype_digit(tmp) == SUCCESS) {
            ulong idx = atol(Z_STRVAL_P(tmp));
            if (zend_hash_index_find(Z_ARRVAL_P(tempArr), idx, (void **) &findvalue) == FAILURE) {
                add_index_zval(array, idx, emptyarr);
                currentItem = emptyarr;
            } else {
                currentItem = *findvalue;
            }
        } else {
            //php_printf("%s, %d.<br/>", Z_STRVAL_P(tmp), __LINE__);
            if (zend_hash_find(Z_ARRVAL_P(tempArr), Z_STRVAL_P(tmp), Z_STRLEN_P(tmp) + 1, (void **) &findvalue) == FAILURE) {
                add_assoc_zval_ex(array, Z_STRVAL_P(tmp), Z_STRLEN_P(tmp) + 1, emptyarr); //add_assoc_zval
                currentItem = emptyarr;
            } else {
                currentItem = *findvalue;
            }
        }
        tempArr = currentItem;
        if (count - 1 == pos->h) {
            *currentItem = *value;
            zval_copy_ctor(currentItem);
        }

        zend_hash_move_forward_ex(Z_ARRVAL_P(keys), &pos);
    }
}

PHP_METHOD(Kohana_Arr, set_path) {
    zval *array = NULL;
    zval *path = NULL;
    zval *value = NULL;
    zval *delim = NULL;
    if (ZEND_NUM_ARGS() < 3 || ZEND_NUM_ARGS() > 4) {
        WRONG_PARAM_COUNT;
    }

    if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzz|z", &array, &path, &value, &delim)) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Not enough arguments");
        RETURN_FALSE;
    }

    if (SUCCESS != kohana_arr_is_array(array)) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Argument #%d should be an array", 1);
        RETURN_FALSE;
    }

    if (delim == NULL) {
        MAKE_STD_ZVAL(delim);
        ZVAL_NULL(delim);
    }
    if (Z_TYPE_P(delim) == IS_NULL) {
        Z_STRLEN_P(delim) = sizeof (KOHANA_ARR_DELIMITER) - 1;
        Z_STRVAL_P(delim) = estrdup(KOHANA_ARR_DELIMITER);
    }
    kohana_arr_set_path(array, path, value, delim);
    RETURN_TRUE;
}

PHP_METHOD(Kohana_Arr, range) {
    int step = 0;
    int max = 0;
    if (ZEND_NUM_ARGS() > 2) {
        WRONG_PARAM_COUNT;
    }

    if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|ll", &step, &max)) {
        RETURN_FALSE;
    }

    zval *array = NULL;
    MAKE_STD_ZVAL(array);
    array_init(array);
    if (step < 0) {
        RETURN_ZVAL(array, 1, 0);
    }

    int i;
    for (i = step; i <= max; i += step) {
        add_index_long(array, i, i);
    }

    RETURN_ZVAL(array, 1, 0);
}

PHP_METHOD(Kohana_Arr, get) {
    zval *array = NULL;
    zval *key = NULL;
    zval *defualt = NULL;
    if (ZEND_NUM_ARGS() < 2 || ZEND_NUM_ARGS() > 3) {
        WRONG_PARAM_COUNT;
    }

    if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz|z", &array, &key, &defualt)) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Not enough arguments");
        RETURN_FALSE;
    }

    if (SUCCESS != kohana_arr_is_array(array)) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Argument #%d should be an array", 1);
        RETURN_FALSE;
    }

    if (defualt == NULL) {
        MAKE_STD_ZVAL(defualt);
        ZVAL_NULL(defualt);
    }

    convert_to_string(key);

    ulong idx = 0;
    zval **findvalue;
    if (ctype_digit(key) == SUCCESS) {
        idx = atol(Z_STRVAL_P(key));
        if (zend_hash_index_find(Z_ARRVAL_P(array), idx, (void **) &findvalue) == SUCCESS) {
            RETURN_ZVAL(*findvalue, 1, 0);
        }
    } else {
        if (zend_hash_find(Z_ARRVAL_P(array), Z_STRVAL_P(key), Z_STRLEN_P(key) + 1, (void **) &findvalue) == SUCCESS) {
            RETURN_ZVAL(*findvalue, 1, 0);
        }
    }
    RETURN_ZVAL(defualt, 1, 0);
}

PHP_METHOD(Kohana_Arr, extract) {
    zval *array = NULL;
    zval *keys = NULL;
    zval *defualt = NULL;
    if (ZEND_NUM_ARGS() < 2 || ZEND_NUM_ARGS() > 3) {
        WRONG_PARAM_COUNT;
    }

    if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz|z", &array, &keys, &defualt)) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Not enough arguments");
        RETURN_FALSE;
    }

    if (SUCCESS != kohana_arr_is_array(array)) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Argument #%d should be an array", 1);
        RETURN_FALSE;
    }

    if (Z_TYPE_P(keys) != IS_ARRAY) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Argument #%d should be an array", 2);
        RETURN_FALSE;
    }

    if (defualt == NULL) {
        MAKE_STD_ZVAL(defualt);
        ZVAL_NULL(defualt);
    }

    zval *found = NULL;
    MAKE_STD_ZVAL(found);
    array_init(found);

    zval **key;
    HashPosition pos;

    zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(keys), &pos);
    while (zend_hash_get_current_data_ex(Z_ARRVAL_P(keys), (void **) &key, &pos) == SUCCESS) {
        convert_to_string(*key);

        zval **findvalue;
        if (ctype_digit(*key) == SUCCESS) {
            ulong idx = atol(Z_STRVAL_P(*key));
            if (zend_hash_index_find(Z_ARRVAL_P(array), idx, (void **) &findvalue) == SUCCESS) {
                add_index_zval(found, idx, *findvalue);
            } else {
                add_index_zval(found, idx, defualt);
            }
        } else {
            if (zend_hash_find(Z_ARRVAL_P(array), Z_STRVAL_P(*key), Z_STRLEN_P(*key) + 1, (void **) &findvalue) == SUCCESS) {

                //add_assoc_zval(found, key, *findvalue);
                add_assoc_zval_ex(found, Z_STRVAL_P(*key), Z_STRLEN_P(*key) + 1, *findvalue);
            } else {
                //add_assoc_zval(found, key, defualt);
                add_assoc_zval_ex(found, Z_STRVAL_P(*key), Z_STRLEN_P(*key) + 1, defualt);
            }
        }

        zend_hash_move_forward_ex(Z_ARRVAL_P(keys), &pos);

    }
    RETURN_ZVAL(found, 1, 0);
}

PHP_METHOD(Kohana_Arr, pluck) {
    zval *array = NULL;
    zval *key = NULL;
    if (ZEND_NUM_ARGS() != 2) {
        WRONG_PARAM_COUNT;
    }

    if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &array, &key)) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Not enough arguments");
        RETURN_FALSE;
    }

    if (SUCCESS != kohana_arr_is_array(array)) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Argument #%d should be an array", 1);
        RETURN_FALSE;
    }

    if (Z_TYPE_P(key) != IS_STRING) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Argument #%d should be an string", 2);
        RETURN_FALSE;
    }

    zval *found = NULL;
    MAKE_STD_ZVAL(found);
    array_init(found);

    zval **row;
    HashPosition pos;

    zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(array), &pos);
    while (zend_hash_get_current_data_ex(Z_ARRVAL_P(array), (void **) &row, &pos) == SUCCESS) {

        if (Z_TYPE_P(*row) != IS_ARRAY) {
            continue;
        }
        zval **findvalue;
        if (ctype_digit(key) == SUCCESS) {
            ulong idx = atol(Z_STRVAL_P(key));
            if (zend_hash_index_find(Z_ARRVAL_P(*row), idx, (void **) &findvalue) == SUCCESS) {
                add_next_index_zval(found, *findvalue);
            }
        } else {
            if (zend_hash_find(Z_ARRVAL_P(*row), Z_STRVAL_P(key), Z_STRLEN_P(key) + 1, (void **) &findvalue) == SUCCESS) {

                add_next_index_zval(found, *findvalue);
            }
        }

        zend_hash_move_forward_ex(Z_ARRVAL_P(array), &pos);

    }
    RETURN_ZVAL(found, 1, 0);
}

static void kohana_arr_reverse(zval *input, zend_bool preserve_keys) {

    zval *tmp;
    zval **entry;
    char *string_key;
    uint string_key_len;
    ulong num_key;
    HashPosition pos;
    /* Initialize return array */

    MAKE_STD_ZVAL(tmp);
    array_init_size(tmp, zend_hash_num_elements(Z_ARRVAL_P(input)));

    zend_hash_internal_pointer_end_ex(Z_ARRVAL_P(input), &pos);
    while (zend_hash_get_current_data_ex(Z_ARRVAL_P(input), (void **) &entry, &pos) == SUCCESS) {
        zval_add_ref(entry);

        switch (zend_hash_get_current_key_ex(Z_ARRVAL_P(input), &string_key, &string_key_len, &num_key, 0, &pos)) {
            case HASH_KEY_IS_STRING:
                zend_hash_update(Z_ARRVAL_P(tmp), string_key, string_key_len, entry, sizeof (zval *), NULL);
                break;

            case HASH_KEY_IS_LONG:
                if (preserve_keys) {
                    zend_hash_index_update(Z_ARRVAL_P(tmp), num_key, entry, sizeof (zval *), NULL);
                } else {
                    zend_hash_next_index_insert(Z_ARRVAL_P(tmp), entry, sizeof (zval *), NULL);
                }
                break;
        }

        zend_hash_move_backwards_ex(Z_ARRVAL_P(input), &pos);
    }
    *input = *tmp;
}

PHP_METHOD(Kohana_Arr, unshift) {
    zval *input;
    zval *key;
    zval *val;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "azz", &input, &key, &val) == FAILURE) {
        return;
    }
    kohana_arr_reverse(input, 1);
    add_assoc_zval_ex(input, Z_STRVAL_P(key), Z_STRLEN_P(key) + 1, val);
    kohana_arr_reverse(input, 1);
    //HashPosition pos;
    //zend_hash_get_current_key_type_ex(Z_ARRVAL_P(input), &pos);

    //zend_hash_quick_add(Z_ARRVAL_P(input), Z_STRVAL_P(key), Z_STRLEN_P(key)+1, 1, &val, sizeof(zval*), NULL);


    RETURN_ZVAL(input, 1, 0);
}

static void kohana_arr_map(zval *input, zval*func) {

    zval **entry;
    HashPosition pos;

    zval * argvs[1];
    MAKE_STD_ZVAL(argvs[0]);
    zend_hash_internal_pointer_end_ex(Z_ARRVAL_P(input), &pos);
    while (zend_hash_get_current_data_ex(Z_ARRVAL_P(input), (void **) &entry, &pos) == SUCCESS) {
        zval_add_ref(entry);
        ZVAL_ZVAL(argvs[0], *entry, 1, 0);

        if (Z_TYPE_P(*entry) == IS_ARRAY) {
            kohana_arr_map(*entry, func);
        } else {
            call_user_function(EG(function_table), NULL, func, *entry, 1, argvs);
        }

        zend_hash_move_backwards_ex(Z_ARRVAL_P(input), &pos);
    }
}

PHP_METHOD(Kohana_Arr, map) {
    zval *array;
    zval *func;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "az", &array, &func) == FAILURE) {
        return;
    }
    kohana_arr_map(array, func);
    RETURN_ZVAL(array, 1, 0);
}

static int kohana_arr_in_array(zval *value, zval *array, zend_bool strict) {
    zval **entry, /* pointer to array entry */
            res; /* comparison result */
    HashPosition pos; /* hash iterator */
    int (*is_equal_func)(zval *, zval *, zval * TSRMLS_DC) = is_equal_function;

    if (strict) {
        is_equal_func = is_identical_function;
    }

    zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(array), &pos);
    while (zend_hash_get_current_data_ex(Z_ARRVAL_P(array), (void **) &entry, &pos) == SUCCESS) {
        is_equal_func(&res, value, *entry TSRMLS_CC);
        if (Z_LVAL(res)) {
            return SUCCESS;
        }
        zend_hash_move_forward_ex(Z_ARRVAL_P(array), &pos);
    }

    return FAILURE;
}

static void kohana_arr_merge(zval *input, zval*func) {

    zval **entry;
    HashPosition pos;

    zval * argvs[1];
    MAKE_STD_ZVAL(argvs[0]);
    zend_hash_internal_pointer_end_ex(Z_ARRVAL_P(input), &pos);
    while (zend_hash_get_current_data_ex(Z_ARRVAL_P(input), (void **) &entry, &pos) == SUCCESS) {
        zval_add_ref(entry);
        ZVAL_ZVAL(argvs[0], *entry, 1, 0);

        if (Z_TYPE_P(*entry) == IS_ARRAY) {
            kohana_arr_map(*entry, func);
        } else {
            call_user_function(EG(function_table), NULL, func, *entry, 1, argvs);
        }

        zend_hash_move_backwards_ex(Z_ARRVAL_P(input), &pos);
    }
}

PHP_METHOD(Kohana_Arr, merge) {
    php_printf("merge called.<br/>");
}

PHP_METHOD(Kohana_Arr, overwrite) {
    php_printf("overwrite called.<br/>");
}

PHP_METHOD(Kohana_Arr, callback) {
    php_printf("callback called.<br/>");
}

PHP_METHOD(Kohana_Arr, flatten) {
    php_printf("flatten called.<br/>");
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
