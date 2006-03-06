dnl
dnl  Copyright (C) 2005, 2006 Free Software Foundation, Inc.
dnl
dnl  This program is free software; you can redistribute it and/or modify
dnl  it under the terms of the GNU General Public License as published by
dnl  the Free Software Foundation; either version 2 of the License, or
dnl  (at your option) any later version.
dnl
dnl  This program is distributed in the hope that it will be useful,
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl  GNU General Public License for more details.
dnl
dnl  You should have received a copy of the GNU General Public License
dnl  along with this program; if not, write to the Free Software
dnl  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

AC_DEFUN([GNASH_PATH_PTHREADS],
[
  AC_ARG_ENABLE(pthreads, [  --enable-pthreads       Enable support for playing pthreadss],
  [case "${enableval}" in
    yes) pthreads=yes ;;
    no)  pthreads=no ;;
    *)   AC_MSG_ERROR([bad value ${enableval} for enable-pthreads option]) ;;
  esac], pthreads=yes)

  if test x"$pthreads" = x"yes"; then
    dnl Look for the header
  AC_ARG_WITH(pthreads_incl, [  --with-pthread-incl    directory where Pthread header is], with_pthread_incl=${withval})
    AC_CACHE_VAL(ac_cv_path_pthread_incl,[
    if test x"${with_pthread_incl}" != x ; then
      if test -f ${with_pthread_incl}/pthread.h ; then
	ac_cv_path_pthread_incl=`(cd ${with_pthread_incl}; pwd)`
      else
	AC_MSG_ERROR([${with_pthread_incl} directory doesn't contain pthread.h])
      fi
    fi
    ])

    dnl If the path hasn't been specified, go look for it.
    if test x"${ac_cv_path_pthread_incl}" = x; then
      AC_CHECK_HEADERS(pthread.h, [AC_DEFINE([HAVE_PTHREAD_H], [], [Has POSIX Thread header])], [
      if test x"${ac_cv_path_pthread_incl}" = x; then
        AC_MSG_CHECKING([for libpthread header])
        incllist="${prefix}/include /sw/include  /usr/pkg/pthreads/include /usr/local/include /home/latest/include /opt/include /usr/include .. ../.."
        for i in $incllist; do
	  if test -f $i/pthreads/pthread.h -o -f $i/pthread.h; then
            ac_cv_path_pthread_incl="$i"
	  fi
        done
      fi], [INCLUDES = -I/usr/pkg/pthreads/include -I/usr/include])
    else
      AC_MSG_RESULT(${ac_cv_path_pthread_incl})
      if test x"${ac_cv_path_pthread_incl}" != x"/usr/include"; then
	ac_cv_path_pthread_incl="${ac_cv_path_pthread_incl}"
       else
	ac_cv_path_pthread_incl=""
      fi
    fi

    if test x"${ac_cv_path_pthread_incl}" != x ; then
      AC_MSG_RESULT(yes)
      AC_DEFINE([HAVE_PTHREAD_H], [], [Has POSIX Thread header])
      if test x"$i" != x"/usr/include"; then
        PTHREAD_CFLAGS="-I${ac_cv_path_pthread_incl}"
      else
        PTHREAD_CFLAGS=""
      fi
    fi

      dnl Look for the library
    AC_ARG_WITH(pthread_lib, [  --with-pthread-lib     directory where pthreads library is], with_pthread_lib=${withval})
      AC_CACHE_VAL(ac_cv_path_pthread_lib,[
      if test x"${with_pthread_lib}" != x ; then
        if test -f ${with_pthread_lib}/libpthread.a -o -f ${with_pthread_lib}/libpthread.so; then
	  ac_cv_path_pthread_lib=-L`(cd ${with_pthread_lib}; pwd)`
        else
	  AC_MSG_ERROR([${with_pthread_lib} directory doesn't contain Pthread library.])
        fi
      fi
      ])

      dnl If the header doesn't exist, there is no point looking for the library.
      if test x"${ac_cv_path_pthread_lib}" = x; then
        AC_CHECK_LIB(pthread, pthread_kill, [ac_cv_path_pthread_lib="-lpthread"],[
          AC_MSG_CHECKING([for libpthreads library])
          libslist="${prefix}/lib64 ${prefix}/lib /usr/lib64 /usr/lib /usr/pkg/pthreads/lib /sw/lib /usr/local/lib /home/latest/lib /opt/lib /usr/pkg/lib .. ../.."
          for i in $libslist; do
	    if test -f $i/libpthread.a -o -f $i/libpthread.so; then
	      if test x"$i" != x"/usr/lib"; then
	        ac_cv_path_pthread_lib="-L$i -lpthread"
                AC_MSG_RESULT(${ac_cv_path_pthread_lib})
	        break
              else
	        ac_cv_path_pthread_lib=""
                AC_MSG_RESULT(yes)
	        break
	      fi
	    fi
          done
	])
      else
        if test -f ${ac_cv_path_pthread_lib}/libpthread.a -o -f ${ac_cv_path_pthread_lib}/libpthread.so; then

          if test x"${ac_cv_path_pthread_lib}" != x"/usr/lib"; then
	    ac_cv_path_pthread_lib="-L${ac_cv_path_pthread_lib} -lpthread"
           else
	    ac_cv_path_pthread_lib=""
          fi
        fi
      fi

      if test x"${ac_cv_path_pthread_lib}" != x ; then
        PTHREAD_LIBS="${ac_cv_path_pthread_lib} -lpthread"
      else
        PTHREAD_LIBS="-lpthread"
      fi
    fi

  if test x"${ac_cv_path_pthread_lib}" != x ; then
      PTHREAD_LIBS="${ac_cv_path_pthread_lib}"
  fi

  AM_CONDITIONAL(PTHREAD, [test x$pthread = xyes])

  if test x$pthreads = xyes; then
    AC_DEFINE([HAVE_PTHREADS], [], [Use POSIX Threads])
  fi

  AC_SUBST(PTHREAD_CFLAGS)
  AC_SUBST(PTHREAD_LIBS)
])
