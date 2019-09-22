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

- The details of the simulation should be printed to a log.txt file in the current directory. This includes the
 disassembled instructions and the initial values of the data memory. At each point an instruction is executed, you
should print the disassembled instruction, the current values of the registers, and the current values of the words in
the data segment ofmemory.
- 

----------

### System Call Codes

| System Call Code | Argument      | Explanation                                                                |
|------------------|---------------|----------------------------------------------------------------------------|
| 1                | $a0 = integer | Print an integer value followed by a newline character to standard output. |
| 5                |               | Read an integer value from standard input and assign the value to $v0.     |
| 10               |               | Exit the simulation.                                                       |

### Exceptions to handle

| Type                        | Explanation                                            |
|-----------------------------|--------------------------------------------------------|
| illegal instruction         | Illegal combination of opcode and funct field values.  |
| illegal instruction address | PC is referencing address outside of the instructions. |
| illegal data address        | Address of load or store from outside of thedata.      |
| divide by zero              | Integer divide by zero.                                |


