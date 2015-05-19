#include "connection.h"
#include "connection_async.h"

namespace nodbc {

  static v8::Persistent<v8::FunctionTemplate> nodbc_constructor;

  NAN_METHOD(Connection::New) {
    NanScope();

    Connection *obj = new Connection();
    obj->Wrap(args.Holder());

    NanReturnValue(args.Holder());
  }

  NAN_METHOD(Connection::IsConnected) {
    NanScope();

    Connection *self = ObjectWrap::Unwrap<Connection>(args.Holder());

    NanReturnValue(NanNew(self->connection.connected()));
  }

  NAN_METHOD(Connection::Open) {
    NanScope();

    Connection *self = ObjectWrap::Unwrap<Connection>(args.Holder());
    std::string connectionString(*NanAsciiString(args[0].As<v8::String>()));
    NanCallback *callback = new NanCallback(args[1].As<v8::Function>());

    OpenWorker *worker = new OpenWorker(callback, &self->connection, connectionString);
    worker->SaveToPersistent("database", args.Holder());
    NanAsyncQueueWorker(worker);

    NanReturnUndefined();
  }

  NAN_METHOD(Connection::Close) {
    NanScope();

    Connection *self = ObjectWrap::Unwrap<Connection>(args.Holder());
    NanCallback *callback = new NanCallback(args[0].As<v8::Function>());

    CloseWorker *worker = new CloseWorker(callback, &self->connection);
    worker->SaveToPersistent("database", args.Holder());
    NanAsyncQueueWorker(worker);

    NanReturnUndefined();
  }

  NAN_METHOD(Connection::Execute) {
    NanScope();

    Connection *self = ObjectWrap::Unwrap<Connection>(args.Holder());
    std::string query(*NanAsciiString(args[0].As<v8::String>()));
    v8::Local<v8::Array> values = args[1].As<v8::Array>();
    Parameters parameters = GetParametersFromArray(values);
    NanCallback *callback = new NanCallback(args[2].As<v8::Function>());

    ExecuteWorker *worker = new ExecuteWorker(callback, &self->connection, query, parameters);
    worker->SaveToPersistent("database", args.Holder());
    NanAsyncQueueWorker(worker);

    NanReturnUndefined();
  }

  void Connection::Init(v8::Handle<v8::Object> target, const char *name) {
    v8::Local<v8::FunctionTemplate> tpl = NanNew<v8::FunctionTemplate>(Connection::New);
    NanAssignPersistent(nodbc_constructor, tpl);
    tpl->SetClassName(NanNew("Connection"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    NODE_SET_PROTOTYPE_METHOD(tpl, "isConnected", Connection::IsConnected);
    NODE_SET_PROTOTYPE_METHOD(tpl, "open", Connection::Open);
    NODE_SET_PROTOTYPE_METHOD(tpl, "close", Connection::Close);
    NODE_SET_PROTOTYPE_METHOD(tpl, "execute", Connection::Execute);
    target->Set(NanNew(name), NanNew<v8::FunctionTemplate>(nodbc_constructor)->GetFunction());
  }

}
