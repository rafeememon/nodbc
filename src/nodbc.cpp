#include <string.h>
#include <v8.h>
#include <node.h>
#include <node_version.h>
#include <time.h>
#include <uv.h>

#include "connection.h"

void Init(v8::Handle<v8::Object> target) {
  nodbc::Connection::Init(target, "Connection");
}

NODE_MODULE(nodbc, Init)
