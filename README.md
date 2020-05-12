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
4. Start the client for testing:  
`./bin/client`

## Tests and benchmark   
Both the client and server are running on the same machine with the following specifications:  
* OS: Ubuntu 16.04, 64 bit  
* Memory: 8 GB
* CPU: Intel Core i5 @ 3.1 GHz * 4   
The latency between a request and reply is messured as the metrics.  

### Scenario 1: single benign client   
You can find this test case in **tests/scenario.cpp**. A single threaded client performs the following operations consequentially:
1. GET 10000 keys from an empty store, should receive NOT_FOUND;   
2. PUT 10000 keys;   
3. GET these 10000 keys;   
4. SCAN these 10000 keys;   
5. SCAN half of the range of keys (5000);   
6. SCAN an empty range of keys, should receive an empty pair **although the replied status is OK**;   
7. DELETE half of the keys (5000);   
8. GET the original 10000 keys, the deleted ones should return NOT_FOUND;   
9. SCAN the whole range after DELETE, only 5000 key-value pairs should be returned;    
The results are printed:
```
test GET non-existed keys
	get 10000 non-existing keys in 0.606577 seconds
test PUT
	put 10000 keys in 0.647572 seconds
test GET
	get 10000 keys in 0.670173 seconds
test SCAN
	scan 10000 keys in 0.048216 seconds
	scan 5001 keys in 0.024846 seconds
	scan returns empty in the given range
test DELETE
	delete 5000 keys in 0.317169 seconds
test GET after DELETE
	get 10000 keys in 0.653219 seconds
test SCAN after DELETE
	scan 5000 keys in 0.022083 seconds
Passed all tests!
```
