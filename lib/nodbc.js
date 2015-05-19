var nodbc = require('bindings')('nodbc');

function Connection() {
  this.connection = new nodbc.Connection();
}

Connection.prototype.open = function(connectionString, callback) {
  this.connection.open(connectionString, callback);
}

Connection.prototype.close = function(callback) {
  this.connection.close(callback);
}

Connection.prototype.isConnected = function() {
  return this.connection.isConnected();
}

Connection.prototype.execute = function(query, values, callback) {
  if (!callback) {
    callback = values;
    values = [];
  }

  this.connection.execute(query, values, function(err, json) {
    if (err) {
      callback(err);
    } else {
      callback(null, JSON.parse(json));
    }
  });
}

Connection.open = function(connectionString, callback) {
  var database = new Connection();
  database.open(connectionString, function(err) {
    if (err) {
      callback(err);
    } else {
      callback(null, database);
    }
  });
}

module.exports = Connection;
