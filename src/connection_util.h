#ifndef CONNECTION_UTIL_H
#define CONNECTION_UTIL_H

#include "nanodbc.h"

namespace nodbc {

  std::string GetResultAsJson(nanodbc::result *result);

}

#endif
