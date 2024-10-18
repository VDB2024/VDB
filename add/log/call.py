import subprocess

def compile_and_run_c_program(source_file, executable, args):

    compile_command = ['gcc', source_file, '-o', executable, '-Iadd/log/include', '-Ladd/log/lib',  '-lblst', '-lcrypto']
    compile_result = subprocess.run(compile_command, capture_output=True, text=True)
    if compile_result.returncode != 0:
        # print("Compilation failed:")
        print(compile_result.stderr)
        return 
    
    print("Compilation succeeded.")

    run_command = ['./' + executable] + args
    run_result = subprocess.run(run_command, capture_output=True, text=True)


def compare_files(file1, file2):
    try:
        with open(file1, 'r') as f1, open(file2, 'r') as f2:
            for line1, line2 in zip(f1, f2):
                if line1 != line2:
                    return False

            if f1.readline() or f2.readline():
                return False
    except FileNotFoundError:
        print("One of the files does not exist.")
        return False
    except Exception as e:
        print(f"An error occurred: {e}")
        return False
    return True

if __name__ == "__main__":
    source_file = 'add/log/process_logs.c'
    executable = 'add/log/process_logs'
    compile_and_run_c_program(source_file, executable, [])
    source_file = 'add/log/server1.c'
    executable = 'add/log/server1'
    arg = ["add/log/initial_log", "add/log/initial_commitment"]
    compile_and_run_c_program(source_file, executable, arg)
    arg = ["add/log/final_log", "add/log/final_commitment"]
    compile_and_run_c_program(source_file, executable, arg)

    source_file = 'add/log/client.c'
    executable = 'add/log/client'
    arg = ["3", "add/log/initial_commitment", "add/log/initial_commitment_combine"]
    compile_and_run_c_program(source_file, executable, arg)
    arg = ["3", "add/log/final_commitment", "add/log/final_commitment_combine"]
    compile_and_run_c_program(source_file, executable, arg)

    source_file = 'add/log/server2.c'
    executable = 'add/log/server2'
    arg = ["3", "add/log/initial_log", "add/log/initial_commitment_combine_verify"]
    compile_and_run_c_program(source_file, executable, arg)
    arg = ["3", "add/log/final_log", "add/log/final_commitment_combine_verify"]
    compile_and_run_c_program(source_file, executable, arg)

    file_path1 = './add/log/initial_commitment_combine'
    file_path2 = './add/log/initial_commitment_combine_verify'

    if compare_files(file_path1, file_path2):
        print("commitment confirm")
    else:
        print("Files do not match.")


    file_path1 = './add/log/final_commitment_combine'
    file_path2 = './add/log/final_commitment_combine_verify'

    if compare_files(file_path1, file_path2):
        print("commitment confirm")
    else:
        print("Files do not match.")