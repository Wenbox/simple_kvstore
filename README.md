# simple_kvstore

This is a simple in-memory ordered key-value store server with the following APIs:
- `get(string key)`: retrive the value of a given key;
- `put(string key, string value)`: map a value to a given key;
- `delete(string key)`: delete a specific key, if it exists;
- `scan(string lower_key, string upper_key)`: return a collection of all key-value pairs, whose keys are between `lower_key` and `upper_key`(inclusive).

The keys are fixed **8 Bytes**, and the values are fixed **256 Bytes**.

## How to build 
Requirements:
- CMake version >= 3.7

1. Checkout repository and submodule (asio): 
`git clone --recurse-submodules https://github.com/Wenbox/simple_kvstore.git`<br>
(earlier version of Git maybe use `--recursive` flag instead of `--recurse-submodules`.

2. Using CMake to build:  
`mkdir build && cd build`  
`cmake ..`   
`make`   

3. Start the server:  
`./bin/server`

4. Start the client:  
`./bin/client`
