# projekt: plg-2-nka
# autor: Matus Liscinsky
# login: xlisci02
# mail: xlisci02@stud.fit.vutbr.cz
# datum: 2.4.2020


import subprocess
import os

BINARY = "./plg-2-nka"
INCORRECT_TESTS_DIR = "test/incorrect/"
INCORRECT_EC = 1
CORRECT_TESTS_DIR = "test/correct/"
CORRECT_EC = 0

incorrect_test_cases = []
for file in os.listdir(INCORRECT_TESTS_DIR):
    incorrect_test_cases.append(
        {"file": INCORRECT_TESTS_DIR + file, "option": "-i",
         "output_sub": "error", "exit_code": INCORRECT_EC}
    )


correct_test_cases = []
for file in os.listdir(CORRECT_TESTS_DIR):
    if ".in" in file:
        for option in ["-i", "-1", "-2"]:
                output_file_name = CORRECT_TESTS_DIR + file.split('.')[0] + option + ".out"
                with open(output_file_name, "r") as fp:
                    content = fp.read()
                correct_test_cases.append(
                    {"file": CORRECT_TESTS_DIR + file, "option": option,
                    "output_sub": content, "exit_code": CORRECT_EC}
                )


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
    if exit_code == 0:
        return exit_code, stdout.decode('utf-8')
    else:
        return exit_code, stderr.decode('utf-8')


def test(test_cases, print_option=False):
    for test in test_cases:
        if ".in" in test["file"]:
            ec, output = execute_bin([BINARY, test["option"], test["file"]])
            if ec == test["exit_code"] and test["output_sub"] in output:
                print("[OK]", test["option"] if (print_option) else "",  test["file"])
            else:
                print("[FAIL]", test["file"], "\noutput:\n" + output + "\n" + test["output_sub"])


if __name__ == "__main__":
    test(incorrect_test_cases)
    test(correct_test_cases, print_option=True)
