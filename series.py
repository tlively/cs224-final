import subprocess

n_dags = 30
machines = [4,8,16]
timeout = 60

for size in range(12,26):
    for m in machines:
        for dag in range(n_dags):
            if m == 4 and size > 23:
                continue
            path = "series/data{}01/Pat{}.rcp".format(size, dag)
            subprocess.run(["./bbexps", path, str(m), str(timeout)]),
