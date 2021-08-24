# projekt: flp20-log, rubikova kocka
# autor: Matus Liscinsky
# login: xlisci02
# mail: xlisci02@stud.fit.vutbr.cz
# datum: 26.4.2020

import subprocess
import os

BINARY = "./flp20-log"
CORRECT_EC = 0
TEST_DIRS = ["1tah", "2tahy", "3tahy", "4tahy", "5tahov", "6tahov", "7tahov"]


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


def test(print_option=False):
    for dir in TEST_DIRS:
        print("---", dir,"---")
        for file in sorted(os.listdir("test/"+dir)):
            abs_path_file = os.path.abspath("test/"+dir+"/"+file)
            file_handler = open(abs_path_file)
            ec, output, time_output = execute_bin(["time", "-p", BINARY], stdin=file_handler)
            if ec == CORRECT_EC and \
                "555\n555\n555\n111 222 333 444\n111 222 333 444\n111 222 333 444\n666\n666\n666\n" \
                in output :
                print("[OK]", file, "time:", time_output.split()[1],"s")
            else:
                print("[FAIL]", output, file)
        print("---", "-"*len(dir),"---")


if __name__ == "__main__":
    test()
