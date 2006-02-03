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

AC_DEFUN([AM_PATH_OPENGL],
[
  AC_ARG_ENABLE(opengl, [  --disable-opengl        Disable support for OpenGL],
  [case "${enableval}" in
    yes) opengl=no  ;;
    no)  opengl=yes ;;
    *)   AC_MSG_ERROR([bad value ${enableval} for disable-opengl option]) ;;
  esac], opengl=yes)

  if test x"$opengl" = x"yes"; then
    dnl Look for the headers.
    AC_ARG_WITH(opengl_includes, [  --with-opengl-includes  directory where OpenGL headers are], with_opengl_includes=${withval})
    AC_CACHE_VAL(ac_cv_path_opengl_includes,[
    if test x"${with_opengl_includes}" != x ; then
      if test -f ${with_opengl_includes}/GL/gl.h ; then
        ac_cv_path_opengl_includes=`(cd ${with_opengl_includes}; pwd)`
      else
        AC_MSG_ERROR([${with_opengl_includes} directory doesn't contain GL.h])
      fi
    fi
    ])

    dnl If the include path hasn't been specified, go look for it.
    if test x"${ac_cv_path_opengl_includes}" = x; then
      AC_CHECK_HEADERS(GL/gl.h, [ac_cv_path_opengl_includes=""],[
      if test x"${ac_cv_path_opengl_includes}" = x; then
        AC_MSG_CHECKING([for OpenGL headers])
        incllist="/usr/include /usr/local/include /opt/include /usr/X11R6/include /usr/pkg/include .. ../.."

        for i in $incllist; do
          if test -f $i/GL/gl.h; then
            if test x"$i" != x"/usr/include"; then
              ac_cv_path_opengl_includes="-I$i"
              break
            else
              ac_cv_path_opengl_includes=""
              break
            fi
          fi
        done
      fi])
    else
      AC_MSG_RESULT(-I${ac_cv_path_opengl_includes})
      if test x"${ac_cv_path_opengl_includes}" != x"/usr/include"; then
        ac_cv_path_opengl_includes="-I${ac_cv_path_opengl_includes}"
       else
        ac_cv_path_opengl_includes=""
      fi
    fi

    if test x"${ac_cv_path_opengl_includes}" != x ; then
      OPENGL_CFLAGS="${ac_cv_path_opengl_includes}"
      AC_MSG_RESULT(${ac_cv_path_opengl_includes})
    else
      OPENGL_CFLAGS=""
    fi

    dnl Look for the libraries.
    AC_ARG_WITH(opengl_lib, [  --with-opengl-libraries directory where OpenGL libraries are], with_opengl_libraries=${withval})
    AC_CACHE_VAL(ac_cv_path_opengl_libraries,[
    if test x"${with_opengl_libraries}" != x ; then
      if test -f ${with_opengl_libraries}/libGL.a -o -f ${with_opengl_libraries}/libGL.so; then
        ac_cv_path_opengl_libraries=`(cd ${with_opengl_libraries}; pwd)`
      else
        AC_MSG_ERROR([${with_opengl_libraries} directory doesn't contain libGL.])
      fi
    fi
    ])

    dnl If the header doesn't exist, there is no point looking for the library.
    if test x"${ac_cv_path_opengl_libraries}" = x; then
      AC_CHECK_LIB(GL, glBegin, [ac_cv_path_opengl_libraries="-lGL -lGLU"],[
        AC_MSG_CHECKING([for libGL library])
        libslist="/usr/X11R6/lib /usr/lib /usr/local/lib /opt/lib /usr/pkg/lib .. ../.."
        for i in $libslist; do
          if test -f $i/libGL.a -o -f $i/libGL.so; then
            if test x"$i" != x"/usr/lib"; then
              ac_cv_path_opengl_libraries="-L$i"
              AC_MSG_RESULT(${ac_cv_path_opengl_libraries})
              break
            else
              ac_cv_path_opengl_libraries=""
              AC_MSG_RESULT(yes)
              break
            fi
          fi
        done])
    else
      if test -f ${ac_cv_path_opengl_libraries}/libGL.a -o -f ${ac_cv_path_opengl_libraries}/libGL.so; then
        if test x"${ac_cv_path_opengl_libraries}" != x"/usr/lib"; then
          ac_cv_path_opengl_libraries="-L${ac_cv_path_opengl_libraries}"
         else
          ac_cv_path_opengl_libraries=""
        fi
      fi
    fi

    if test x"${ac_cv_path_opengl_libraries}" != x ; then
      OPENGL_LIBS="${ac_cv_path_opengl_libraries}"
    else
      OPENGL_LIBS=""
    fi
  fi

  if test x"${ac_cv_path_opengl_libraries}" != x ; then
      OPENGL_LIBS="${ac_cv_path_opengl_libraries}"
  fi

  AM_CONDITIONAL(opengl, [test x$opengl = xyes])

  AC_SUBST(OPENGL_CFLAGS)
  AC_SUBST(OPENGL_LIBS)
])
