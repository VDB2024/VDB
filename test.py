import subprocess

def run_sqlite_query():
    sqlite_command = "./sqlite3 ./database/TPC-H_60k.db"
    query = """
    SELECT sum(l_extendedprice * l_discount) as revenue
    FROM lineitem
    WHERE l_shipdate >= date('1994-01-01')
      AND l_shipdate < date('1995-01-01')
      AND l_discount BETWEEN .05 AND .07
      AND l_quantity < 24;
    """

    command = f"echo \"{query}\" | {sqlite_command}"

    try:
        result = subprocess.run(command, shell=True, text=True, capture_output=True, check=True)
        print("Query Output:", result.stdout)
    except subprocess.CalledProcessError as e:
        print("An error occurred while running the SQLite query:", e)

def run_test_script():
    try:
        subprocess.run(["./test.sh"], check=True)
        print("test.sh executed successfully.")
    except subprocess.CalledProcessError as e:
        print("Error running test.sh:", e)

def run_additional_python_script():
    try:
        subprocess.run(["python3", "./add/log/call.py"], check=True)
        print("call.py executed successfully.")
    except subprocess.CalledProcessError as e:
        print("Error running call.py:", e)

if __name__ == "__main__":
    run_sqlite_query()
    run_test_script()
    run_additional_python_script()