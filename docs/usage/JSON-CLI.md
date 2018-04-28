# JSON-controlled CLI

**Fnsim** CLI supports JSON-controlled interactive simulations, with all communication happening via standard input/output.

Supported actions include running and stepping through program execution, inspecting processor state, setting breakpoints and watches, and much more.

It is started in this way:
```
./fnsim-cli PROC_ARCH PFILE_PATH -j
```
where `PROC_ARCH` is a processor architecture on which the simulation will be run (thus far only FRISC is supported), and `PFILE_PATH` is a filesystem path to the machine-code file (in .p format) containing instructions of the program whose execution should be simulated.

Once the simulator is started, it can be given commands by sending it JSON-formatted messages of the following format:
```js
{ "command": "COMMAND_NAME", "args": [ /* array of argument values */ ] }
```

Supported commands are:
- `help`            -- Show this help message
- `run [ADDR]`      -- Run simulation from the start (or from a given ADDR)
- `start [ADDR]`    -- Init simulation to the start (or to a given ADDR)
- `cont`            -- Continue paused simulation
- `step`            -- Execute one simulation step (ie. instruction)
- `step_n N`        -- Execute N (or less) simulation steps
- `until ADDR`      -- Run simulation until reaching ADDR
- `reset`           -- Reset simulator - clear registers and memory contents
- `quit`            -- Quit interactive simulation
- `break (list | add ADDR | del ADDR | clear)`
                    -- List breakpoint | Add/remove breakpoint at ADDR | Clear all breakpoints
- `watch (list | add ADDR | del ADDR | clear)`
                    -- List watches | Add/remove watch at ADDR | Clear all watches
- `info`            -- Display processor state (ie. value of it's registers)
