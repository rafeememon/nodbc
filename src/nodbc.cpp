#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

#include <sql.h>
#include <sqlext.h>

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
    static NAN_METHOD(Execute);
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

class ExecuteWorker : public NanAsyncWorker {
public:
    ExecuteWorker(NanCallback *callback, nanodbc::connection *connection, string query)
        : NanAsyncWorker(callback), connection(connection), query(query) {}

    void Execute() {
        try {
            nanodbc::result result = nanodbc::execute(*connection, query);

            picojson::array rows;
            const short columns = result.columns();

            while (result.next()) {
                picojson::object row;
                for (short col = 0; col < columns; col++) {
                    row[result.column_name(col)] = GetJsonValue(&result, col);
                }
                rows.push_back(picojson::value(row));
            }

            json = picojson::value(rows).serialize();
        }
        catch (const nanodbc::database_error &err) {
            SetErrorMessage(err.what());
        }
    }

    void HandleOKCallback() {
        NanScope();

        v8::Local<v8::Value> argv[] = {
            NanNull(),
            NanNew(json.c_str())
        };

        callback->Call(2, argv);
    };

private:
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
            return picojson::value(result->get<string>(col));
        }
    }

    nanodbc::connection *connection;
    string query;
    string json;
};

NAN_METHOD(NodbcConnection::Execute) {
    NanScope();

    NodbcConnection *self = ObjectWrap::Unwrap<NodbcConnection>(args.Holder());
    string query(*NanAsciiString(args[0].As<v8::String>()));
    NanCallback *callback = new NanCallback(args[1].As<v8::Function>());

    ExecuteWorker *worker = new ExecuteWorker(callback, &self->connection, query);
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
    NODE_SET_PROTOTYPE_METHOD(tpl, "execute", NodbcConnection::Execute);
}

void Init(v8::Handle<v8::Object> target) {
    NodbcConnection::Init();
    target->Set(NanNew("NodbcConnection"),
        NanNew<v8::FunctionTemplate>(nodbc_constructor)->GetFunction());
}

NODE_MODULE(nodbc, Init)
