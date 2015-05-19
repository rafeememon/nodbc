#ifndef CONNECTION_H
#define CONNECTION_H

#include "nan.h"
#include "nanodbc.h"

namespace nodbc {

  class Connection : public node::ObjectWrap {
  public:
    static void Init(v8::Handle<v8::Object> target, const char *name);
    static NAN_METHOD(New);
    static NAN_METHOD(IsConnected);
    static NAN_METHOD(Open);
    static NAN_METHOD(Close);
    static NAN_METHOD(Execute);

  private:
    nanodbc::connection connection;
  };

}

#endif
