#ifndef CONNECTION_ASYNC_H
#define CONNECTION_ASYNC_H

#include "connection_params.h"
#include "nan.h"
#include "nanodbc.h"

namespace nodbc {

  class OpenWorker : public NanAsyncWorker {
  public:
    OpenWorker(
      NanCallback *callback,
      nanodbc::connection &connection,
      std::string connectionString);

    virtual void Execute();

  private:
    nanodbc::connection connection;
    std::string connectionString;
  };

  class CloseWorker : public NanAsyncWorker {
  public:
    CloseWorker(
      NanCallback *callback,
      nanodbc::connection &connection);

    virtual void Execute();

  private:
    nanodbc::connection connection;
  };

  class ExecuteWorker : public NanAsyncWorker {
  public:
    ExecuteWorker(
      NanCallback *callback,
      nanodbc::connection &connection,
      std::string query,
      Parameters parameters);

    virtual void Execute();

    virtual void HandleOKCallback();

  private:
    nanodbc::connection connection;
    std::string query;
    Parameters parameters;
    std::string json;
  };

}

#endif
