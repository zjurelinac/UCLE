# JSON-controlled CLI

**Fnsim** CLI supports JSON-controlled interactive simulations, with all communication happening via standard input/output.

Supported actions include running and stepping through program execution, inspecting processor state, setting breakpoints and watches, and much more.

It is started in this way:
```
./fnsim-cli PROC_ARCH PFILE_PATH -j
```
where `PROC_ARCH` is a processor architecture on which the simulation will be run (thus far only FRISC is supported), and `PFILE_PATH` is a filesystem path to the machine-code file (in .p format) containing instructions of the program whose execution should be simulated.

Once the simulator is started, it can be given commands by sending it JSON-formatted messages of the following format:
```json
{ "command": "COMMAND_NAME", "args": [ /* array of argument values */ ] }
```
