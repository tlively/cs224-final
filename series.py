import subprocess
import sys

n_dags = 30
machines = [4,8,16]
timeout = 60
timeout_skip = 12

for bound in ["Fujita", "Fernandez"]:
    clean = subprocess.run(["make", "clean"],
                          stdout=subprocess.DEVNULL,
                          stderr=subprocess.DEVNULL)
    if clean.returncode != 0:
        print("make clean failed")
        sys.exit(1)
    args = ["FB=1"] if bound == "Fernandez" else []
    make = subprocess.run(["make"] + args,
                          stdout=subprocess.PIPE,
                          stderr=subprocess.PIPE,
                          encoding="utf-8")
    if make.returncode != 0:
        print("make failed")
        sys.exit(1)
    for size in range(12,26):
        for m in machines:
            n_timeouts = 0
            for dag in range(n_dags):
                path = "series/data{}01/Pat{}.rcp".format(size, dag)
                result = subprocess.run(
                    ["./bbexps", path, str(m), str(timeout)],
                    stdout=subprocess.PIPE,
                    encoding="utf-8")
                print("{}, {}".format(result.stdout[:-1], bound))
                if "-2" in result.stdout:
                    n_timeouts += 1
                else:
                    n_timeouts = 0
                if n_timeouts >= timeout_skip:
                    break
