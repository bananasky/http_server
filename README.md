# HTTP Server

**Authored by**: Melvin Loh 
for *Computer Networks Assignment 1*

A HTTP-like server that provides key-value store services, written in C++.

The server consists of two components. 
1. The first is a custom simplified
protocol from HTTP 1.1. 
2. The key-value
store supports CRUD operations on key-value
pairs. The counter represents the remaining retrieval
times of a temporary record in the key-value store. Counter will be decreased by 1 for each
retrieval of the record in key-value store. 

A record in key-value store without a counter
can be retrieved/updated as many times as possible. In contrast, a temporary record is readonly
and will be deleted after the corresponding counter reaches 0.

See `(assgn_1.pdf)` more detailed specification.

## Setup

### Prerequisites

Ensure `cmake` with minimum `v3.22`, and a compiler with `C++20` support.

**Envrionment:** Ubuntu 22.04.2 LTS

### Automated Build and Test Scripts

To use the grading script, run the following command in root directory.

```sh
bash test/WebServer.sh
```

By default, the script runs through all test cases. Run a certain
test case by specifying the case number in the command:

```sh
bash test/WebServer.sh 3
```

By default, the script will create a `build` directory, run `cmake` to 
configure and build the project and execute the testing script with the test cases.

### Building and running

If you are not using the automated test scripts, in the project root, run:

```sh
mkdir build && cd build && cmake .. && make
```

This builds the binary `WebServer` which you can run with `./WebServer <port>` in the `build/` folder. 

**Note:** You may change the port on which the server is run: `4567` by default in `src/WebServer.h`.

### Unit Testing

After building using the `cmake` and `make` command above,
to run custom tests using `catch2` test framework, `cd` into the `build/` 
directory and run `ctest`.

Following is the sample output from unit tests.

```sh
Test project /http_server/build
    Start 1: encodeOutput with empty data
1/9 Test #1: encodeOutput with empty data ...................................................   Passed    0.18 sec
    Start 2: Response::encodeOutput with non-empty data
2/9 Test #2: Response::encodeOutput with non-empty data .....................................   Passed    0.17 sec
    Start 3: Response::encodeOutput with 404 status
3/9 Test #3: Response::encodeOutput with 404 status .........................................   Passed    0.18 sec
    Start 4: Request processes GET /key/{key} request correctly
4/9 Test #4: Request processes GET /key/{key} request correctly .............................   Passed    0.19 sec
    Start 5: Request processes POST /key/{key} request correctly
5/9 Test #5: Request processes POST /key/{key} request correctly ............................   Passed    0.19 sec
    Start 6: Request rejects duplicate POST /key/{key} with counter > 0 requests with 405
6/9 Test #6: Request rejects duplicate POST /key/{key} with counter > 0 requests with 405 ...   Passed    0.21 sec
    Start 7: Request processes DELETE /key/{key} request correctly
7/9 Test #7: Request processes DELETE /key/{key} request correctly ..........................   Passed    0.21 sec
    Start 8: Request handles GET /counter/{key} correctly
8/9 Test #8: Request handles GET /counter/{key} correctly ...................................   Passed    0.21 sec
    Start 9: Request handles GET /counter/{key} without counter correctly
9/9 Test #9: Request handles GET /counter/{key} without counter correctly ...................   Passed    0.24 sec

100% tests passed, 0 tests failed out of 9

Total Test time (real) =   2.05 sec
```

