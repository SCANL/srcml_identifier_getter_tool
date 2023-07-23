import os
import random
import argparse
from spiral import ronin

def read_file_into_list(file_path):
    result_list = []

    with open(file_path, 'r') as file:
        for line in file:
            line_values = line.strip().split(',')
            result_list.append(line_values)

    return result_list

def group_by_attribute(data_list):
    attribute_groups = {}
    for line_values in data_list:
        attribute = line_values[2] + '-' + line_values[3]  # Assuming the third column is always the 'ATTRIBUTE' column
        attribute_groups.setdefault(attribute, []).append(line_values)
    return attribute_groups

def take_balanced_sample(attribute_groups, sample_size):
    balanced_sample = []
    for attribute, group_data in attribute_groups.items():
        if len(group_data) > sample_size:
            balanced_sample.extend(random.sample(group_data, sample_size))
        else:
            balanced_sample.extend(group_data)
    return balanced_sample

def read_files_from_directory(directory_path):
    data_list = []
    for filename in os.listdir(directory_path):
        file_path = os.path.join(directory_path, filename)
        data_list.extend(read_file_into_list(file_path))
    return data_list

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Read and process files from a directory.")
    parser.add_argument("directory_path", help="Path to the directory containing the data files.")
    args = parser.parse_args()

    data_list = read_files_from_directory(args.directory_path)

    attribute_groups = group_by_attribute(data_list)
    sample_size = 30  # Set the desired sample size for each attribute group

    balanced_sample = take_balanced_sample(attribute_groups, sample_size)

    # Print the balanced sample to check the result
    for line_values in balanced_sample:
        print(','.join(line_values))
