#include <string.h>
#include <v8.h>
#include <node.h>
#include <node_version.h>
#include <time.h>
#include <uv.h>

#include "connection_async.h"
#include "connection_util.h"

namespace nodbc {

  OpenWorker::OpenWorker(
    NanCallback *callback,
    nanodbc::connection &connection,
    std::string connectionString)
    : NanAsyncWorker(callback),
    connection(connection),
    connectionString(connectionString) {};

  void OpenWorker::Execute() {
    try {
      connection.connect(connectionString);
    }
    catch (const nanodbc::database_error &err) {
      SetErrorMessage(err.what());
    }
  }

  CloseWorker::CloseWorker(
    NanCallback *callback,
    nanodbc::connection &connection)
    : NanAsyncWorker(callback),
    connection(connection) {};

  void CloseWorker::Execute() {
    connection.disconnect();
  }

  ExecuteWorker::ExecuteWorker(
    NanCallback *callback,
    nanodbc::connection &connection,
    std::string query,
    Parameters parameters)
    : NanAsyncWorker(callback),
    connection(connection),
    query(query),
    parameters(parameters) {};

  void ExecuteWorker::Execute() {
    try {
      nanodbc::result result;

      if (!parameters.empty()) {
        nanodbc::statement statement(connection, query);
        BindParametersToStatement(statement, parameters);
        result = nanodbc::execute(statement);
      }
      else {
        result = nanodbc::execute(connection, query);
      }

      json = GetResultAsJson(result);
    }
    catch (const std::runtime_error &err) {
      SetErrorMessage(err.what());
    }
  }

  void ExecuteWorker::HandleOKCallback() {
    NanScope();

    v8::Local<v8::Value> argv[] = {
      NanNull(),
      NanNew(json.c_str())
    };

    callback->Call(2, argv);
  };

}
