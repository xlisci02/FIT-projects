# projekt: RSA
# autor: Matus Liscinsky
# login: xlisci02
# mail: xlisci02@stud.fit.vutbr.cz
# datum: 2.5.2020

import os
import random
import subprocess

BINARY = "./kry"
INCORRECT_EC = 1
LENGTH_LIMIT_MIN = 7
LENGTH_LIMIT_MAX = 100
ITERATIONS = 10
MESSAGE_MODULUS_DIFFERENCE = 4

def execute_bin(command, timeout=None, stdin=None):
    command = list(filter(None, command))
    try:
        process = subprocess.Popen(
            command, stdin=stdin, stdout=subprocess.PIPE, stderr=subprocess.PIPE
        )

        stdout, stderr = process.communicate(timeout=timeout)
        exit_code = process.wait()

    except subprocess.TimeoutExpired:
        process.terminate()
        raise
    return exit_code, stdout.decode('utf-8'), stderr.decode('utf-8')


def test_correct(modulus_length):

    while(True):
        ec, output, err_ouput = execute_bin([BINARY,"-g", str(modulus_length)])
        if len(err_ouput):
            continue
        p,q,n,e,d = output.split()
        break

    message=str(hex(random.getrandbits(int(modulus_length-MESSAGE_MODULUS_DIFFERENCE))))

    ec, output, _ = execute_bin([BINARY,"-e", e,n,message])
    if ec == INCORRECT_EC: return ["-e", e,n,message],0
    c = output.split()[0]

    ec, output, _ = execute_bin([BINARY,"-d", d,n,c])
    if ec == INCORRECT_EC: return ["-d", d,n,c],0
    m = output.split()[0]
    if m != message:
        print("\noriginal message:",message)
        print("\ndecrypted message:",m)
        return ["-d", d,n,c],0
    ec, output, time_output = execute_bin(["time", "-p", BINARY,"-b", e,n,c])
    if ec == INCORRECT_EC: return ["-b", e,n,c],0
    p1,q1,m1 = output.split()


    if int(p1,16)*int(q1,16) != int(p,16)*int(q,16) or m!=m1:
        print("p",p)
        print("n",n)
        print("q",q)
        print("e",e)
        print("d",d)
        print("c",c)
        print("m",m)
        print("p1",p1)
        print("q1",q1)
        print("m1",m1)
        return ["-b", e,n,c],0
    return 0, float(time_output.split()[1])


if __name__ == "__main__":

    for i in range(LENGTH_LIMIT_MIN,LENGTH_LIMIT_MAX+1):
        times = []
        print("Testing modulus length:", i, end="")
        for j in range(0,ITERATIONS):
            result = test_correct(i)
            if not result[0]:
                times.append(result[1])
                print(".", end="")
            else: print("[ERR]", result)
        print("[OK]",
        "min_time:", "%.2f" % min(times), "s "
        "max_time:", "%.2f" % max(times), "s "
        "avg_time:", "%.3f" % (sum(times)/len(times)), "s")
