import os
import subprocess
import argparse

def run_grabidentifiers(directory_path):
    for entry in os.listdir(directory_path):
        file_path = os.path.join(directory_path, entry)

        # Check if the entry is a regular file, if so, run grabidentifiers
        if os.path.isfile(file_path):
            output_file = file_path + '.csv'
            command_args = ['./grabidentifiers', '-cFUNCTION,PARAMETER,DECLARATION,CLASS,ATTRIBUTE', file_path]

            try:
                with open(output_file, 'w') as output:
                    subprocess.run(command_args, check=True, stdout=output)
                print(f"Successfully processed {file_path}")
            except subprocess.CalledProcessError as e:
                print(f"Error processing {file_path}: {e}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Run the grabidentifiers tool on every file within the specified directory.")
    parser.add_argument("target_directory", help="The path to the target directory.")
    args = parser.parse_args()

    target_directory = args.target_directory

    run_grabidentifiers(target_directory)