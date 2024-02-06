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
                    print(f'{new_file_path} does not exist')
                    errors.append(entry.name)
                else:
                    match = False
                    error_file_path = os.path.join(args.out_dir, entry.name)
                    if entry.name[-4:] == '.log':
                        match = compare_log_files(entry.path, new_file_path)
                    elif entry.name[-4:] == '.dlf':
                        match = compare_dlf_files(entry.path, new_file_path)
                    elif entry.name[-4:] == '.dai':
                        match = compare_checkpoints(entry.path, new_file_path)
                    else:
                        errors.append(entry.name)
                    if not match:
                        mismatch.append(entry.name)
                        os.makedirs(args.out_dir, exist_ok=True)
                        shutil.copy(new_file_path, error_file_path)

    if len(errors) > 0:
        print('== Errors ==', *errors, sep='\n')
        return 2

    if len(mismatch) > 0:
        print('== Mismatches ==', *mismatch, sep='\n')
        return 4

    return 0


def keep_line(ignore_tokens, strip_tokens=None):
    def func(s):
        s = s.strip(strip_tokens)
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
                print('== Checkpoint ==')
                print(l1.strip())
                print(l2.strip())
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
    ignore_tokens = ['*', 'Changing', 'Command', 'In', 'Looking', 'Opening', 'Program', 'Reseting',
                     'Storing', 'Time', 'Trying', 'Using', 'Daisy']
    keep = keep_line(ignore_tokens)
    with open(path1) as f1, open(path2) as f2:
        lines1 = filter(keep, f1)
        lines2 = filter(keep, f2)    
        for l1, l2 in zip_longest(lines1, lines2):
            if l1 != l2:
                print(l1, l2)
                return False
        return True

if __name__ == '__main__':
    sys.exit(main())
