echo Setting up the environment for debugging gdb.\n

# This connects to OpenOcd at localhost:3333
target remote localhost:3333

# Increase the packet size to improve download speed.
set remote memory-write-packet-size 1024
set remote memory-write-packet-size fixed

#omap3_dbginit must be run in OpenOCD after every reset
monitor omap3_dbginit

# Load the program executable called "LEDblink"
load LEDblink

# Load the symbols for the program.
symbol-file LEDblink

# Set a breakpoint at main().
#b main

# Run to the breakpoint.


