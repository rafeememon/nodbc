#ifndef CONNECTION_PARAMS_H
#define CONNECTION_PARAMS_H

#include "nanodbc.h"
#include <v8.h>

namespace nodbc {

  class Parameter {
  public:
    Parameter();
    Parameter(int32_t int32_value);
    Parameter(double number_value);
    Parameter(std::string string_value);

    void Bind(nanodbc::statement &statement, short index);

  private:
    enum Type {
      Null,
      Int32,
      Number,
      String
    };

    Type type;
    int32_t int32_value;
    double number_value;
    std::string string_value;
  };
  
  typedef std::vector<Parameter> Parameters;
  
  Parameters GetParametersFromArray(v8::Local<v8::Array> values);

  void BindParametersToStatement(nanodbc::statement &statement, Parameters &parameters);

}

#endif
