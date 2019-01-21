#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(ARDUINO_UNO_LIB)
#  define ARDUINO_UNO_EXPORT Q_DECL_EXPORT
# else
#  define ARDUINO_UNO_EXPORT Q_DECL_IMPORT
# endif
#else
# define ARDUINO_UNO_EXPORT
#endif
