#include "connection.h"

void Init(v8::Handle<v8::Object> target) {
  nodbc::Connection::Init(target, "Connection");
}

NODE_MODULE(nodbc, Init)
