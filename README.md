# atomspace-rpc

This project uses [grpc](https://grpc.io) for executing pattern matching searches on a remote [AtomSpace](https://github.com/opencog). 

It also allows users to create multiple atomspaces and access them separately. Users can use a JSON config file to specify the id and the directory containing the atomese code to load. An example config looks like the following:

```json
 [
   {
     "id": "asp1",
     "pathDir": "/path/to/atomese/code/"
   },
   {
     "id": "asp2",
     "pathDir": "/path/to/another/atomese/code"
   }   
 ]
```

You can use the above JSON to start the server:

```bash
$ atom_server --config /path/to/setting.json
```

By default the server runs at `localhost:50051` . But you can that using the `--host` and `--port` arguments.

### Building and Installation.

#### 1. Requirements:

This project depends on the following libraries:
 - [AtomSpace](https://github.com/opencog/atomspace)
 - [Boost](https://www.boost.org/)
   * On Debian you can use  `apt-get install libboost-dev`
 - [Protobuff](https://github.com/protocolbuffers/protobuf)
 - [gRPC C++](https://github.com/grpc/grpc/tree/master/src/cpp)
    * See also here - https://grpc.io/docs/languages/cpp/quickstart/
    * Note: While building gRPC from the source, make sure to have the following cmake options. For more info check out this [issue comment](https://github.com/grpc/grpc/issues/13841#issuecomment-355510534) from the library maintainers.
        

        cmake -DgRPC_INSTALL=ON -DgRPC_BUILD_TESTS=OFF -DgRPC_PROTOBUF_PROVIDER=package -DgRPC_ZLIB_PROVIDER=package -DgRPC_CARES_PROVIDER=package -DgRPC_SSL_PROVIDER=package -DCMAKE_BUILD_TYPE=Release ..
 - [nlohmann_json](https://github.com/nlohmann/json)
    * Since nlohmann json is a header only library just copy [json.hpp](https://github.com/nlohmann/json/releases/download/v3.9.1/json.hpp) to `/usr/local/include/nlohmann` directory. Create the nlohmann sub directory if it doesn't exist. Run the following command:
    
        ```
      $ mkdir -p /usr/local/include/nlohmann && \
        
        wget -O /usr/local/include/nlohmann/json.hpp https://github.com/nlohmann/json/releases/download/v3.9.1/json.hpp
      ```
 - [agi-bio](https://github.com/opencog/agi-bio) - this is OPTIONAL and will be required if you are working with
       bio related atomspace
 
Guile is also must be installed to use the scheme bindings
      
#### 2. Building

```bash
$ cd atomspace_server
$ mkdir build && cd build
$ cmake ..
$ make 
$ make install
```

### Usage

Once you have started the `atom_server`, run Guile REPL and execute the following commands:

A. To execute a pattern matching query

```scheme
scheme@(guile-user)> (use-modules (opencog grpc))
scheme@(guile-user)> (exec-pattern "prod-atom" (Get  (Member (Gene "IGF1") (Variable "$pway"))))
```

In the above example, `prod-atom` is the id of the atomspace you want the pattern to be searched in.

B. Check if a node exists
```scheme
scheme@(guile-user)> (check-node "prod-atom" 'Concept "A")
```

In the above example, we are checking if `(Concept A)` exists in remote atomspace `prod-atom`

C. Find Nodes with similar name
```scheme
scheme@(guile-user)> (find-similar-node "prod-atom" 'Concept "ca")
```

The above query will return nodes that have `ca` in their name, such as `(Concept "car")` , `(Concept "cat")`..etc
. The result is limited to return at most 10 atoms.
