import pexpect
import sys


def get_resident_memory_by_process_id(pid):
    with open(f"/proc/{pid}/status") as f:
        rss = f.read().split('VmRSS:')[1].split('\n')[0][:-3]
    return int(rss.strip())


# Spawn a child process with the enclave
analyzer = pexpect.spawn(" ".join(sys.argv[1:]), encoding='utf-8', timeout=None)

# Wait until the enclave is ready to be measured
analyzer.expect("Insertion done")

# Retrieve and display the resident memory of the enclave's process
print(get_resident_memory_by_process_id(analyzer.pid))

# Send a dummy input to continue the child process
analyzer.sendline("0")

# Wait until the enclave ends
analyzer.wait()
