var nodbc = require('bindings')('nodbc');

function Database() {
  this.connection = new nodbc.NodbcConnection();
}

Database.prototype.open = function(connectionString, callback) {
  this.connection.open(connectionString, callback);
}

Database.prototype.close = function(callback) {
  this.connection.close(callback);
}

Database.prototype.isConnected = function() {
  return this.connection.isConnected();
}

Database.prototype.execute = function(query, callback) {
  this.connection.execute(query, function(err, json) {
    if (err) {
      callback(err);
    } else {
      callback(null, JSON.parse(json));
    }
  });
}

Database.open = function(connectionString, callback) {
  var database = new Database();
  database.open(connectionString, function(err) {
    if (err) {
      callback(err);
    } else {
      callback(null, database);
    }
  });
}

module.exports = Database;
