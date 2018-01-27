# Elevator simulator
This is elevator simulator. You can give it commands to move it to the desired floor. 
As a result, the program prints messages about what happens with the elevator.

## Settings
The simulator can accept parameters, like floors count, hight of one floor, elevator speed, time while doors are open.
If some of the parameters are omitted, instead them will use the default settings.
```
  ./elevator_simulator <floors_count> <floor_height> <speed> <doors_open_time>
```
  
## Commands
 - 'move N', 'm N' - Move elevator to floor N, from the elevator.
 - 'call N', 'c N' - Call elevator to floor N, from floor.
 - 'help', 'h'     - Show help message.
 - 'setup', 's'    - Show elevator setup.
 - 'exit', 'e'     - Exit program.
 
## Elevator messages
The simulator reports about passing the floor and about opening and closing doors.
  
## Behavior algorithm
At the program start, the elevator is located on the first floor and wait for commands. If the command comes (*move* or *call*), the elevator starts moving to the requested floor. While the elevator is moving the *call* commands are ignored, but it accepts any *move* commands. When the elevator is located on the floor where it should stop, it stops and opens the doors. After doors closing, the elevator starts moving to the next requested floor in the same direction it has been moving previously. If there is not requested floors in this direction, the elevator starts moving to the floors in opposite direction. If there are no requested floors left, the elevator stops and waiting for the commands.

## How to build
```
  mkdir build
  cd ./build
  cmake ../
  make
```
