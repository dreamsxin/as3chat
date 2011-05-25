dnl $Id$
dnl config.m4 for extension kohana

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(kohana, for kohana support,
dnl Make sure that the comment is aligned:
dnl [  --with-kohana             Include kohana support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(kohana, whether to enable kohana support,
Make sure that the comment is aligned:
[  --enable-kohana           Enable kohana support])

if test "$PHP_KOHANA" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-kohana -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/kohana.h"  # you most likely want to change this
  dnl if test -r $PHP_KOHANA/$SEARCH_FOR; then # path given as parameter
  dnl   KOHANA_DIR=$PHP_KOHANA
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for kohana files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       KOHANA_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$KOHANA_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the kohana distribution])
  dnl fi

  dnl # --with-kohana -> add include path
  dnl PHP_ADD_INCLUDE($KOHANA_DIR/include)

  dnl # --with-kohana -> check for lib and symbol presence
  dnl LIBNAME=kohana # you may want to change this
  dnl LIBSYMBOL=kohana # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $KOHANA_DIR/lib, KOHANA_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_KOHANALIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong kohana lib version or lib not found])
  dnl ],[
  dnl   -L$KOHANA_DIR/lib -lm
  dnl ])
  dnl
  dnl PHP_SUBST(KOHANA_SHARED_LIBADD)

  PHP_NEW_EXTENSION(kohana, kohana.c, $ext_shared)
fi
