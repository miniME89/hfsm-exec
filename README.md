### Concept Overview
![Design Overview](https://cdn.rawgit.com/miniME89/hfsm-exec/master/doc/design.png "Design Overview")

### Status
- [60%] State machine execution
    - DONE: Core state machine components are working
    - DONE: Basic state machines can be build and executed
    - WORK: Plugin system for different communication middlewares
    - WORK: Implement some basic plugins for different communication middlewares (e.g. web service, RPC, ROS, ...), which will be used for invoking applications
    - TODO: Proper concept/implementation for transitions using some external event system (StringEvent is only for testing and temporary)
    - TODO: Use value container for getting and setting parameters used by the states
    - TODO: Dataflow between different states (input and output parameters)

- [30%] State machine representation language (SMRL)
    - DONE: Concept for easily implementing decoders for different state machine representations
    - WORK: Basic structure and decoding of XML representation of a state machine
    - TODO: Basic structure and decoding of JSON representation of a state machine
    - TODO: Basic structure and decoding of YAML representation of a state machine
    - TODO: Detailed concept and description of all representation languages

- [90%] value container
    - DONE: Get, set and delete arbitrary values
    - DONE: Support basic types: bool, int, float, string, array, object
    - DONE: Default values
    - TODO: read/write the value from/to XML, JSON and YAML
    - TODO: Option for merging value objects (?)

- [20%] RESTful API
    - DONE: Integration of framework (CppCMS or other?) for web support
    - DONE: Basic test with framework and working example for GET/PUT/DELETE using the parameter server
    - TODO: API for state machine loading, execution and control
    - TODO: Realtime interface for monitoring and notification service (?)

- General
    - DONE: Integration of XML, JSON and YAML parsers

### Project Structure
- doc/ - Documentation
- ext/ - External dependencies source code which will be build along with the main program
- main/ - The source code of the main program
- plugins/ - Implementations of communication plugins

### API
A RESTful API using HTTP provides an interface for interacting with the executor.

| Status  | Method | Location              | Description                                 |
|---------|--------|-----------------------|---------------------------------------------|
| TESTING | GET    | /parameters/{path}    | Get parameters from the parameter server    |
| TESTING | PUT    | /parameters/{path}    | Set parameters on the parameter server      |
| TESTING | DELETE | /parameters/{path}    | Delete parameters from the parameter server |
| TODO    | GET    | /statemachine         | Get state machine status                    |
| TODO    | PUT    | /statemachine/load    | Load state machine                          |
| TODO    | PUT    | /statemachine/unload  | Unload state machine                        |
| TODO    | PUT    | /statemachine/start   | Start loaded state machine                  |
| TODO    | PUT    | /statemachine/stop    | Stop loaded state machine                   |
| DONE    | PUT    | /statemachine/event   | Post an event to the running state machine  |
| ...     |        |                       |                                             |

### Build
Execute the following command in the root directory:
<pre>
qmake && make
</pre>

### Dependencies
- Qt
- CppCMS

### License
GNU General Public License
