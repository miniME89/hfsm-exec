### Concept Overview
![Design Overview](https://cdn.rawgit.com/miniME89/hfsm-exec/master/doc/design.png "Design Overview")

### Description
...

### Status
The software is considered feature complete. All primary features are implemented.

### Project Structure
- doc/ - Documentation
- examples/ - Examples
- main/ - The source code of the main program
- plugins/ - Implementations of some core plugins

### API
A RESTful API provides an easy interface for interacting with the executor.

| Status  | Method | Location              | Description                                 |
|---------|--------|-----------------------|---------------------------------------------|
| WORK    | GET    | /log                  | Get log messages (server push)              |
| WORK    | GET    | /statemachine/state   | Get state/transition changes (server push)  |
| WORK    | POST   | /statemachine/load    | Load state machine                          |
| WORK    | POST   | /statemachine/unload  | Unload state machine                        |
| WORK    | POST   | /statemachine/start   | Start loaded state machine                  |
| WORK    | POST   | /statemachine/stop    | Stop loaded state machine                   |
| WORK    | POST   | /statemachine/event   | Post an event to the running state machine  |

### Dependencies
- Qt5 (Modules: core, network, script)
- microhttpd
- pugixml
- jsoncpp
- yaml-cpp

Note: All dependencies, except for QT5, can be installed during compilation using the *ext_install* target.

### Compiler
Compiler needs to support C++11.

Following compilers where tested:
 - gcc 4.8.2 (Linux)

### Build
Execute the following commands in the root directory:

    mkdir build
    cd build
    cmake ..
    sudo make ext_install
    make

This will firstly download, build and install the necessary project dependencies (except for QT5). The main program and all plugins will be build to the *bin/* directory.

### Usage
bin/hfsm-exec -h

    Options:
    -h, --help                    Displays this help.
    -v, --version                 Displays version information.
    -l, --logger <logger>         Enable only the specified loggers. Possible
                                  Loggers are: api, application, builder,
                                  value, plugin, statemachine. [Default: all]
    -f, --logger-file <filename>  Set the filename (including the path) for the
                                  log file.
    -d, --plugin-dir <directory>  Set the path to the directories where the
                                  plugins will be loaded from. [Default:
                                  ./plugins/]
    -a, --api                     Enable the REST API. This will startup the
                                  internal HTTP server.
    -p, --api-port <port>         Set port of the HTTP server for the REST API.
                                  [Default: 8080]
    -i, --import <filename>       Import a state machine.
    -o, --export <filename>       Export the imported state machine.
    -e, --encoding <encoding>     Encoding of the imported/exported state
                                  machine.

### License
GNU General Public License
