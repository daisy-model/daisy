'''Program for running daisy tests'''
import argparse
import filecmp
import os
import shutil
import subprocess
import sys
import tempfile
from itertools import zip_longest


def main():
    # pylint: disable=missing-function-docstring
    parser = argparse.ArgumentParser(
        description='Run daisy programs and compare generated output against a reference'
    )
    parser.add_argument('daisy_binary', type=str, help='Name of or path to the daisy binary')
    parser.add_argument('program', type=str, help='Path to the .dai file to run')
    parser.add_argument('reference_dir', type=str, help='Path to directory containing reference ou'\
                        'tput files. All files in the directory will be compared agains the genera'\
                        'ted log files')
    parser.add_argument('out_dir', type=str, help='Output directory for errors')
    args = parser.parse_args()

    os.makedirs(args.out_dir, exist_ok=True)
        
    with tempfile.TemporaryDirectory() as tmpdir:
        daisy_args = [args.daisy_binary, args.program, '-d', tmpdir, '-q']
        print(' '.join(daisy_args))
        result = subprocess.run(daisy_args, check=False)
        if result.returncode != 0:
            return 1

        errors, mismatch = [], []
        for entry in os.scandir(args.reference_dir):
            if entry.is_file():
                new_file_path = os.path.join(tmpdir, entry.name)
                if not os.path.exists(new_file_path):
                    errors.append(entry.name)
                else:
                    match = False
                    error_file_path = os.path.join(args.out_dir, entry.name)
                    if entry.name == 'daisy.log':
                        match = compare_log_files(entry.path, new_file_path)
                    elif entry.name[-4:] == '.dlf':
                        match = compare_dlf_files(entry.path, new_file_path)
                    elif entry.name[:10] == 'checkpoint':
                        match = compare_checkpoints(entry.path, new_file_path)
                    else:
                        errors.append(entry.name)                        
                    if not match:
                        mismatch.append(entry.name)
                        shutil.copy(new_file_path, error_file_path)

    if len(errors) > 0:
        print('Error comparing\n\t', '\n\t'.join(errors))
        return 2

    if len(mismatch) > 0:
        print('Mismatches\n\t', '\n\t'.join(mismatch))
        return 4

    return 0



def keep_line(ignore_tokens):
    def func(s):
        s = s.strip()
        for tok in ignore_tokens:
            if s.startswith(tok):
                return False
        return True
    return func

def compare_checkpoints(path1, path2):
    keep = keep_line([';;'])
    with open(path1) as f1, open(path2) as f2:
        lines1 = filter(keep, f1)
        lines2 = filter(keep, f2)
        for l1, l2 in zip_longest(lines1, lines2):
            if l1 != l2:
                print(l1, l2)
                return False
        return True

def compare_dlf_files(path1, path2):
    keep = keep_line([';;', 'RUN:', 'SIMFILE:'])
    with open(path1) as f1, open(path2) as f2:
        lines1 = filter(keep, f1)
        lines2 = filter(keep, f2)    
        for l1, l2 in zip_longest(lines1, lines2):
            if l1 != l2:
                print(l1, l2)
                return False
        return True

def compare_log_files(path1, path2):
    '''Compare two daisy log files

    Everything until the first "* column" line is ignored.
    Lines starting with 'Time' or 'Program' are ignored

    It is assumed that the files exists

    path1, path2 : str
      Paths to files to compare

    Returns
    -------
    True if files match otherwise False
    
    '''
    start_tokens = [
        '* column',
        '* rootdens'
    ]
    with open(path1, encoding='utf8') as f1, open(path2, encoding='utf8') as f2:
        lines1 = list(f1)
        lines2 = list(f2)

    done = False
    for i in range(len(lines1)): #pylint: disable=consider-using-enumerate
        for token in start_tokens:
            if lines1[i].startswith(token):
                lines1 = lines1[i:]
                done = True
                break
        if done:
            break
        
    done = False
    for i in range(len(lines2)): #pylint: disable=consider-using-enumerate
        for token in start_tokens:
            if lines2[i].startswith(token):
                lines2 = lines2[i:]
                done = True
                break
        if done:
            break

    if len(lines1) != len(lines2):
        print(len(lines1), len(lines2))        
        return False

    for l1,l2 in zip(lines1, lines2):
        if l1 != l2:
            if not ((l1.startswith('Time') and l2.startswith('Time')) or
                    (l1.startswith('Program') and l2.startswith('Program'))):
                print('Lines do not match:', l1, l2)
                return False
    return True

if __name__ == '__main__':
    sys.exit(main())
