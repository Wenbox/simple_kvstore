# simple_kvstore

This is a simple in-memory ordered key-value store server with the following APIs:
- `get(string key)`: retrive the value of a given key;
- `put(string key, string value)`: map a value to a given key;
- `delete(string key)`: delete a specific key, if it exists;
- `scan(string lower_key, string upper_key)`: return a collection of all key-value pairs, whose keys are between `lower_key` and `upper_key`(inclusive).

The keys are fixed **8 Bytes**, and the values are fixed **256 Bytes**.
