import os
import csv
import argparse
import re
from spiral import ronin

#https://7esl.com/conjunctions-list/
conjunctions = ["for", "and", "nor", "but", "or", "yet", "so", "although", "after", "before", "because", "how",
                "if", "once", "since", "until", "unless", "when", "as", "that", "though", "till", "while"]

#https://en.wikipedia.org/wiki/List_of_English_determiners
determiners = ["a", "all", "an", "another", "any", "anybody", "anyone", "anything", "anywhere", "both", "certain", "each", "either", "enough", "every", "everybody", 
               "everyone", "everything", "everywhere", "few", "fewer", "fewest", "last", "least", "less", "little", "many", "many", "more", "most", "much", "neither", 
               "next", "no", "nobody", "none", "nothing", "nowhere", "once", "one", "said", "several", "some", "somebody", "something", "somewhere", "sufficient", "that", 
               "the", "these", "this", "those", "three", "thrice", "twice", "two", "us", "various", "we", "what", "whatever", "which", "whichever", "you", "zero"]

def parse_csv_files(directory_path):
    for entry in os.listdir(directory_path):
        file_path = os.path.join(directory_path, entry)

        # Check if the entry is a regular file, if so, parse it as a CSV file
        if os.path.isfile(file_path):
            with open(file_path, 'r') as csv_file:
                reader = csv.reader(csv_file, delimiter=' ')
                num_dets = num_cjs = 0
                with open ('results/conjunctions', 'a') as cjs, open ('results/determiners', 'a') as dts,open ('results/counts', 'a') as cts:
                    print("\n\nCurrently scanning: {file}\n\n".format(file=file_path), file=cts)
                    for row in reader:
                        file_split = row[5].split('/')
                        word_list = ronin.split(row[1])  # Modify this line to process each row as needed
                        if any(re.search(r'\b[a-z_]*test[a-z_]*\b', x.lower()) for x in file_split) or any(re.search(r'\b[a-z_]*test[a-z_]*\b', x.lower()) for x in word_list):
                            continue
                        if any(str1 == str2 for str1 in word_list for str2 in conjunctions):
                            num_dets = num_dets + 1
                            print(' '.join(row), file=cjs)
                        if any(str1 == str2 for str1 in word_list for str2 in determiners):
                            num_cjs = num_cjs + 1
                            print(' '.join(row), file=dts)
                    print("\n\nFinish scanning: {file}\n\n Number of identifiers {num_dets}, {num_cjs}".format(num_dets=num_dets, num_cjs=num_cjs, file=file_path), file=cts)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Parse each file in the specified directory as a CSV file separated by space.")
    parser.add_argument("target_directory", help="The path to the target directory.")
    args = parser.parse_args()

    target_directory = args.target_directory

    parse_csv_files(target_directory)