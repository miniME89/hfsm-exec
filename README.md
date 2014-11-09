### Status
- [50%] State machine execution
    - DONE: Core state machine components are working
    - DONE: Basic state machines can be build and executed
    - TODO: Proper concept/implementation for transitions using some external event system (StringEvent is only for testing and temporary)
    - TODO: Implement some "communication states" which will communicate over different middlewares (e.g. web service, RPC, ROS, ...) to execute different applications
    - TODO: Plugin system for communication states
    - TODO: Use parameter server for getting and setting parameters used by the states
    - TODO: Dataflow between different states (input and output parameters)

- [0%] State machine markup language
    - TODO: Basic structure for representing a state machine as XML or JSON
    - TODO: Parser

- [80%] Parameter server
    - DONE: get, set and delete parameters on a centralized structure
    - DONE: Support basic types: bool, int, float, string, array, object
    - TODO: option for merging parameter objects (?)

- [20%] RESTful API
    - DONE: Integration of framework (CppCMS or other?) for web support
    - DONE: Basic test with framework and working example for GET/PUT/DELETE using the parameter server
    - TODO: API for state machine loading, execution and control
    - TODO: Realtime interface for monitoring and notification service (?)

### Dependencies
- QT
- CppCMS

### License
TODO
