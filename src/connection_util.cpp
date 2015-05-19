#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <sql.h>
#include <sqlext.h>

#include "connection_util.h"
#include "picojson.h"

namespace nodbc {

  picojson::value GetJsonValue(nanodbc::result *result, short col) {
    if (result->is_null(col)) {
      return picojson::value();
    }

    switch (result->column_datatype(col)) {
    case SQL_NUMERIC:
    case SQL_DECIMAL:
    case SQL_INTEGER:
    case SQL_SMALLINT:
    case SQL_TINYINT:
    case SQL_BIGINT:
    case SQL_FLOAT:
    case SQL_REAL:
    case SQL_DOUBLE:
      return picojson::value(result->get<double>(col));
    default:
      return picojson::value(result->get<std::string>(col));
    }
  }

  picojson::value GetJsonObject(nanodbc::result *result) {
    picojson::object object;
    const short columns = result->columns();
    for (short col = 0; col < columns; col++) {
      object[result->column_name(col)] = GetJsonValue(result, col);
    }
    return picojson::value(object);
  }

  picojson::value GetJsonObjects(nanodbc::result *result) {
    picojson::array rows;
    while (result->next()) {
      rows.push_back(GetJsonObject(result));
    }
    return picojson::value(rows);
  }

  std::string GetResultAsJson(nanodbc::result *result) {
    if (result->columns() == 0) {
      return picojson::value().serialize();
    }
    return GetJsonObjects(result).serialize();
  }

}
