# simple_kvstore

This is a simple in-memory ordered key-value store server with the following features:
- [x] `get(string key)`: retrieve the value of a given key;
- [x] `put(string key, string value)`: map a value to a given key;
- [x] `delete(string key)`: delete a specific key, if it exists;
- [x] `scan(string lower_key, string upper_key)`: return a collection of all key-value pairs, whose keys are between `lower_key` and `upper_key`(inclusive).   
The keys are fixed **8 Bytes**, and the values are fixed **256 Bytes**.
- [x] The return from SCAN can have a very big size. I tested with 10.000 returned key-value pairs, which is over 2 MB. 
- [x] data persistence. It is optional, but if you choose it, **be careful that the backup file will grow to unlimited size!**. And checksums is not yet implemented to check the integrity. A corrupted backup file hinder the server from starting up.  
- [x] Integration tests. Because most functions are web IO functions, it is relatively hard to test, so I decide to directly
perform integration tests.


## How to build 
Requirements:
- CMake version >= 3.7

1. Checkout repository and submodule (asio):  
`git clone --recurse-submodules https://github.com/Wenbox/simple_kvstore.git`<br>
(earlier version of Git maybe use `--recursive` flag instead of `--recurse-submodules`.  
Go into the downloaded folder.
2. Using CMake to build:  
`mkdir build && cd build`  
`cmake ..`   
`make`   
**Make sure that all the following steps are perform in `$(project_root)/build` folder, as this affects the relative paths.**    
3. Configure the key-value store in **config.txt**, including: port number, whether to enable persistency (yes/no), and the backup file path (relative to `build`) folder for persistence.     
4. Start the server:  
`./bin/server`   
5. Start the client for testing:  
`./bin/client`

## Overview  
### Server
In the backend is the `std::map<string, string>`. I use the standard library as much as possible, and avoid raw pointers and arrays, to keep the code modern styled.
I used ASIO library to implement the socket and IO. It can used single threaded but work asynchronously.
Compared to boost::asio, this is a header-only version, so you don't need to compile it. This keeps the dependency to the minimum, and easier to cross platform. I tested the code on both Linux and MacOS.   
The main logic is inside the **request_session** class. After a client connects with a session, the request_session will 
read the request, visit the map to get result, and write the results back.
The read must be asynchronous, otherwise the service might be blocked (see the 3rd testcase below).
The write is synchronous.   
### Data persistence  
Redis has two approaches to do this: append only file and periodic snapshot. The former provides strong consistency but for 
every PUT/DELETE there must be disk write. Snapshot has better performance, but some data can get lost between two snapshots.
I decide to choose the append only file solution, but I did not implement the periodic clean up. Which means, the 
backup file will increase unlimited.   
### Client
The client reads and writes to socket both synchronously. The response message to the SCAN request can have a big size, so a loop is necessary 
until the last byte arrives. 


## Tests and benchmark   
Both the client and server are running on the same machine with the following specifications:  
* OS: Ubuntu 16.04, 64 bit  
* Memory: 8 GB
* CPU: Intel Core i5 @ 3.1 GHz * 4   
The latency between a request and reply is measured as the metrics.  

### Scenario 1: single benign client   
You can find this test case in **tests/scenario1.cpp**. A single threaded client performs the following operations consequentially:
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
### Scenario 2: multiple benign clients  
Five client threads are running in parallel, as in **tests/scenario2.cpp**. The same OPs are performed as in the last testcase.     
### Scenario 3: multiple benign clients and faulty clients
In **tests/scenario3.cpp**, besides 5 benign clients doing the same OPs as before, there are 5 other faulty clients:   
* 2 faulty ones send wrong messages, either wrong request tag, or wrong key-value size.
* 3 faulty ones connect to server and hold without sending anything.    

## What can be further optimized
* B+ tree might be better for disk storage because of the data persistence issue.   
* Sharding: as the number of stored keys grow, the search complexity O(log n) is a big overhead. It would be better to shard the key space.  
* Cache: just another optimization to the above mentioned issue.   
* Periodically clean up the append-only file used for persistence.   
* Checksums and authentication for data integrity and security.   
* Multi-threading: although this will cause race condition, but some non-critical part can benefit from it without locks.
For example combined with sharding so that every thread take responsibility of one partition. Also some dedicated threads
dealing with IO is a good choice.
