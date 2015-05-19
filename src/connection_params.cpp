#include "connection_params.h"
#include "nan.h"

namespace nodbc {

  Parameter::Parameter()
    : type(Null) {};

  Parameter::Parameter(int32_t int32_value)
    : type(Int32), int32_value(int32_value) {};

  Parameter::Parameter(double number_value)
    : type(Number), number_value(number_value) {};

  Parameter::Parameter(std::string string_value)
    : type(String), string_value(string_value) {};

  void Parameter::Bind(nanodbc::statement &statement, short index) {
    switch (type) {
    case Null:
      statement.bind_null(index);
      break;
    case Int32:
      statement.bind(index, &int32_value);
      break;
    case Number:
      statement.bind(index, &number_value);
      break;
    case String:
      statement.bind(index, string_value.c_str());
      break;
    }
  }

  Parameter GetParameterFromValue(v8::Local<v8::Value> value) {
    if (value->IsInt32()) {
      return Parameter(value->Int32Value());
    }
    else if (value->IsNumber()) {
      return Parameter(value->NumberValue());
    }
    else if (value->IsString()) {
      std::string string_value(*NanUtf8String(value));
      return Parameter(string_value);
    }
    else {
      return Parameter();
    }
  }

  Parameters GetParametersFromArray(v8::Local<v8::Array> values) {
    Parameters parameters;
    const short count = values->Length();
    for (short index = 0; index < count; index++) {
      parameters.push_back(GetParameterFromValue(values->Get(index)));
    }
    return parameters;
  }

  void BindParametersToStatement(nanodbc::statement &statement, Parameters &parameters) {
    Parameters::iterator it;
    short index = 0;
    for (it = parameters.begin(); it < parameters.end(); it++, index++) {
      it->Bind(statement, index);
    }
  }

}
