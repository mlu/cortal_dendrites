echo *** Executing .gdbint to set up the environment for debugging gdb:\n

# This connects to OpenOcd at localhost:3333
target remote localhost:3333

# omap3_dbginit must be run in OpenOCD after every reset
monitor omap3_dbginit

# Stop core
monitor halt

echo *** Environment ready, now load and start executeable:\n

# Load the program executable called "LEDblink"
load LEDblink

# Load the symbols for the program.
symbol-file LEDblink

# Set a breakpoint at main().
b main

# Run to the breakpoint.
cont

