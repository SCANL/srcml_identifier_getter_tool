import os
import subprocess
import argparse

def run_srcml_command(directory_path):
    for entry in os.listdir(directory_path):
        file_path = os.path.join(directory_path, entry)

        # Check if the entry is a directory, if so, run the srcml command
        if os.path.isdir(file_path):
            output_file = file_path + '.xml'
            srcml_args = ['srcml', '--position', file_path, '-o', output_file]

            try:
                print("Running srcml on: {system}".format(system=file_path))
                subprocess.run(srcml_args, check=True)
                print(f"Successfully processed {file_path}")
            except subprocess.CalledProcessError as e:
                print(f"Error processing {file_path}: {e}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Run the srcml command on directories within the specified target directory.")
    parser.add_argument("target_directory", help="The path to the target directory.")
    args = parser.parse_args()

    target_directory = args.target_directory

    run_srcml_command(target_directory)