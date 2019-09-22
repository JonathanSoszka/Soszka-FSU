# CDA 3101 Project 2: MIPPS Simulator

### Requirements
- simulator should read a machine code file that was output from the previous assignment.
- The machine code file is specified as the first argument
on the command line. For instance, if your solution’s executable is named sim.exe and the machine code file
to be simulated is named sum.obj, then you should use the following command to simulate the execution.
% sim.exe sum.obj
- After reading the instructions and the data, the simulator should start the simulation from the first instruction in the
machine code file and should continue the simulation until it encounters a syscall thatexits.
- required to simulate all the types of instructions specified in the previous assignment.

----------

### system call codes

| System Call Code | Argument      | Explanation                                                                |
|------------------|---------------|----------------------------------------------------------------------------|
| 1                | $a0 = integer | Print an integer value followed by a newline character to standard output. |
| 5                |               | Read an integer value from standard input and assign the value to $v0.     |
| 10               |               | Exit the simulation.                                                       |

