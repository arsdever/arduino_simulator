#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(RECEIVER_LIB)
#  define RECEIVER_EXPORT Q_DECL_EXPORT
# else
#  define RECEIVER_EXPORT Q_DECL_IMPORT
# endif
#else
# define RECEIVER_EXPORT
#endif
