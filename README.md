### Concept Overview
![Design Overview](https://cdn.rawgit.com/miniME89/hfsm-exec/master/doc/design.png "Design Overview")

### Status
- [90%] State machine execution
    - DONE: Core state machine components are working
    - DONE: Basic state machines can be build and executed
    - DONE: Proper concept/implementation for transitions using events
    - DONE: Plugin system for different communication middlewares
    - DONE: Use parameter container for getting and setting parameters used by the states
    - DONE: Control dataflow between different states (input and output parameters)

- [10%] Plugins for communication middlewares
    - WORK: Implement some basic plugins for different communication middlewares
    - TODO: HTTP plugin
    - TODO: Webservice plugin
    - TODO: RPC plugin
    - TODO: ROS plugin

- [60%] State machine representation language (SMRL)
    - DONE: Concept for easily implementing decoders for different state machine representations
    - DONE: Basic structure and decoding of XML representation of a state machine
    - TODO: Basic structure and decoding of JSON representation of a state machine
    - TODO: Basic structure and decoding of YAML representation of a state machine
    - TODO: Detailed concept and description of all representation languages

- [90%] parameter container
    - DONE: Get, set and delete arbitrary values
    - DONE: Support basic types: bool, int, float, string, array, object
    - DONE: Default values
    - DONE: read/write parameters from/to XML, JSON and YAML

- [50%] RESTful API
    - DONE: Integration of framework for web support
    - DONE: Realtime interface for monitoring and notification service (long polling)
    - WORK: API for state machine loading, execution and control

- General
    - DONE: Integration of XML, JSON and YAML parsers
    - DONE: Integration of logging library

### Project Structure
- doc/ - Documentation
- examples/ - Examples
- main/ - The source code of the main program
- plugins/ - Implementations of communication and decoder plugins

### API
A RESTful API provides an easy interface for interacting with the executor.

| Status  | Method | Location              | Description                                 |
|---------|--------|-----------------------|---------------------------------------------|
| WORK    | GET    | /log                  | Get log messages. (Server push)             |
| WORK    | GET    | /statemachine         | Get state machine status                    |
| WORK    | PUT    | /statemachine/load    | Load state machine                          |
| WORK    | PUT    | /statemachine/unload  | Unload state machine                        |
| WORK    | PUT    | /statemachine/start   | Start loaded state machine                  |
| WORK    | PUT    | /statemachine/stop    | Stop loaded state machine                   |
| WORK    | PUT    | /statemachine/event   | Post an event to the running state machine  |

### Dependencies
- Qt5
- microhttpd
- pugixml
- jsoncpp
- yaml-cpp

### Compiler
Compiler needs to support C++11.

### Build
Install the required dependencies on your system and execute the following commands in the root directory:

    mkdir build
    cd build
    cmake ..
    make

### License
GNU General Public License
