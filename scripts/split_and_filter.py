import os
import csv
import argparse
from spiral import ronin

conjunctions = ["for", "and", "nor", "but", "or", "yet", "so", "although", "after", "before", "because", "how",
                "if", "once", "since", "until", "unless", "when"]

def parse_csv_files(directory_path):
    for entry in os.listdir(directory_path):
        file_path = os.path.join(directory_path, entry)

        # Check if the entry is a regular file, if so, parse it as a CSV file
        if os.path.isfile(file_path):
            with open(file_path, 'r') as csv_file:
                reader = csv.reader(csv_file, delimiter=' ')
                for row in reader:
                    word_list = ronin.split(row[1])  # Modify this line to process each row as needed
                    if any(str1 == str2 for str1 in word_list for str2 in conjunctions):
                        print(' '.join(row))

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Parse each file in the specified directory as a CSV file separated by space.")
    parser.add_argument("target_directory", help="The path to the target directory.")
    args = parser.parse_args()

    target_directory = args.target_directory

    parse_csv_files(target_directory)