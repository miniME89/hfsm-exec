### Concept Overview
![Design Overview](https://cdn.rawgit.com/miniME89/hfsm-exec/master/doc/design.png "Design Overview")

### Status
- [80%] State machine execution
    - DONE: Core state machine components are working
    - DONE: Basic state machines can be build and executed
    - DONE: Proper concept/implementation for transitions using events
    - DONE: Plugin system for different communication middlewares
    - WORK: Implement some basic plugins for different communication middlewares (e.g. web service, RPC, ROS, ...), which will be used for invoking applications
    - TODO: Use value container for getting and setting parameters used by the states
    - TODO: Control dataflow between different states (input and output parameters)

- [60%] State machine representation language (SMRL)
    - DONE: Concept for easily implementing decoders for different state machine representations
    - DONE: Basic structure and decoding of XML representation of a state machine
    - TODO: Basic structure and decoding of JSON representation of a state machine
    - TODO: Basic structure and decoding of YAML representation of a state machine
    - TODO: Detailed concept and description of all representation languages

- [90%] value container
    - DONE: Get, set and delete arbitrary values
    - DONE: Support basic types: bool, int, float, string, array, object
    - DONE: Default values
    - DONE: read/write the value from/to XML, JSON and YAML

- [20%] RESTful API
    - DONE: Integration of framework (CppCMS or other?) for web support
    - TODO: API for state machine loading, execution and control
    - TODO: Realtime interface for monitoring and notification service (?)

- General
    - DONE: Integration of XML, JSON and YAML parsers
    - DONE: Integration of logging library

### Project Structure
- doc/ - Documentation
- main/ - The source code of the main program
- plugins/ - Implementations of communication plugins

### API
A RESTful API using HTTP provides an interface for interacting with the executor.

| Status  | Method | Location              | Description                                 |
|---------|--------|-----------------------|---------------------------------------------|
| TODO    | GET    | /statemachine         | Get state machine status                    |
| TODO    | PUT    | /statemachine/load    | Load state machine                          |
| TODO    | PUT    | /statemachine/unload  | Unload state machine                        |
| TODO    | PUT    | /statemachine/start   | Start loaded state machine                  |
| TODO    | PUT    | /statemachine/stop    | Stop loaded state machine                   |
| DONE    | PUT    | /statemachine/event   | Post an event to the running state machine  |
| ...     |        |                       |                                             |

### Dependencies
- Qt5
- CppCMS
- pugixml
- jsoncpp
- yaml-cpp

### Build
Install the required dependencies on your system and execute the following commands in the root directory:

    mkdir build
    cd build
    cmake ..
    make

### License
GNU General Public License
