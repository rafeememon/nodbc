#include "nan.h"
#include "nanodbc.h"
#include "picojson.h"

using std::string;

static v8::Persistent<v8::FunctionTemplate> nodbc_constructor;

class NodbcConnection : node::ObjectWrap {
public:
    static void Init();
    static NAN_METHOD(New);
    static NAN_METHOD(IsConnected);
    static NAN_METHOD(Open);
    static NAN_METHOD(Close);
private:
    nanodbc::connection connection;
};

NAN_METHOD(NodbcConnection::New) {
    NanScope();

    NodbcConnection *obj = new NodbcConnection();
    obj->Wrap(args.Holder());

    NanReturnValue(args.Holder());
}

NAN_METHOD(NodbcConnection::IsConnected) {
    NanScope();

    NodbcConnection *self = ObjectWrap::Unwrap<NodbcConnection>(args.Holder());

    NanReturnValue(NanNew(self->connection.connected()));
}

class OpenWorker : public NanAsyncWorker {
public:
    OpenWorker(NanCallback *callback, nanodbc::connection *connection, string connectionString)
        : NanAsyncWorker(callback), connection(connection), connectionString(connectionString) {}

    void Execute() {
        try {
            connection->connect(connectionString);
        }
        catch (const nanodbc::database_error &err) {
            SetErrorMessage(err.what());
        }
    }

private:
    nanodbc::connection *connection;
    string connectionString;
};

NAN_METHOD(NodbcConnection::Open) {
    NanScope();

    NodbcConnection *self = ObjectWrap::Unwrap<NodbcConnection>(args.Holder());
    string connectionString(*NanAsciiString(args[0].As<v8::String>()));
    NanCallback *callback = new NanCallback(args[1].As<v8::Function>());

    OpenWorker *worker = new OpenWorker(callback, &self->connection, connectionString);
    worker->SaveToPersistent("database", args.Holder());
    NanAsyncQueueWorker(worker);

    NanReturnUndefined();
}

class CloseWorker : public NanAsyncWorker {
public:
    CloseWorker(NanCallback *callback, nanodbc::connection *connection)
        : NanAsyncWorker(callback), connection(connection) {}

    void Execute() {
        connection->disconnect();
    }

private:
    nanodbc::connection *connection;
};

NAN_METHOD(NodbcConnection::Close) {
    NanScope();

    NodbcConnection *self = ObjectWrap::Unwrap<NodbcConnection>(args.Holder());
    NanCallback *callback = new NanCallback(args[0].As<v8::Function>());

    CloseWorker *worker = new CloseWorker(callback, &self->connection);
    worker->SaveToPersistent("database", args.Holder());
    NanAsyncQueueWorker(worker);

    NanReturnUndefined();
}

void NodbcConnection::Init() {
    v8::Local<v8::FunctionTemplate> tpl = NanNew<v8::FunctionTemplate>(NodbcConnection::New);
    NanAssignPersistent(nodbc_constructor, tpl);
    tpl->SetClassName(NanNew("NodbcConnection"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    NODE_SET_PROTOTYPE_METHOD(tpl, "isConnected", NodbcConnection::IsConnected);
    NODE_SET_PROTOTYPE_METHOD(tpl, "open", NodbcConnection::Open);
    NODE_SET_PROTOTYPE_METHOD(tpl, "close", NodbcConnection::Close);
}

void Init(v8::Handle<v8::Object> target) {
    NodbcConnection::Init();
    target->Set(NanNew("NodbcConnection"),
        NanNew<v8::FunctionTemplate>(nodbc_constructor)->GetFunction());
}

NODE_MODULE(nodbc, Init)
