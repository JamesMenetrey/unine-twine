import time
import pexpect
import sys

# Start the timer for measuring process time
t_start = time.perf_counter()

# Spawn a child process with the enclave
analyzer = pexpect.spawn(" ".join(sys.argv[1:]), encoding='utf-8', timeout=None)

# Wait until the enclave is ready to be measured
analyzer.expect("Initialization done")

# End the timer for measuring process time
t_end = time.perf_counter()

# Send a dummy input to continue the child process
analyzer.sendline("0")

# Print the time to start the process in ms
print(int((t_end - t_start) * 1_000))

# Wait until the enclave ends
analyzer.wait()
