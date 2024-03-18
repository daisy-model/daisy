'''Program for running daisy tests'''
import argparse
import os
import shutil
import subprocess
import sys
import tempfile
import warnings

from compare_dlf_files import compare_dlf_files

def main():
    # pylint: disable=missing-function-docstring
    parser = argparse.ArgumentParser(
        description='Run daisy programs and compare generated output against a reference'
    )
    parser.add_argument('daisy_binary', type=str, help='Name of or path to the daisy binary')
    parser.add_argument('program', type=str, help='Path to the .dai file to run')
    parser.add_argument('reference_dir', type=str, help='''Path to directory containing reference
    output files. All files in the directory will be compared against the generated log files''')
    parser.add_argument('out_dir', type=str, help='Output directory for errors')
    parser.add_argument('--no-warnings', action='store_true',
                        help='If set do not emit warnings from SML comparisons')
    parser.add_argument('--sml-warn-level', type=float, default=0.001,
                        help='Emit a warning if difference larger than `sml_warn_level` * sml')
    parser.add_argument('--default-float-epsilon', type=float, default=1e-8, help='''Pass numeric
    comparison if absolute difference is less than this value. Only used if no SML is defined''')
    args = parser.parse_args()

    if args.no_warnings:
        warnings.showwarning = lambda message, *args: message
    else:
        warnings.showwarning = lambda message, *args: print('WARNING:', message, file=sys.stderr)

    with tempfile.TemporaryDirectory() as tmpdir:
        daisy_args = [args.daisy_binary, args.program, '-d', tmpdir, '-q']
        print(' '.join(daisy_args))
        result = subprocess.run(daisy_args, check=False)
        if result.returncode != 0:
            print('ERROR: Daisy execution failed', file=sys.stderr)
            return 1

        errors, mismatch = [], []
        for entry in os.scandir(args.reference_dir):
            if entry.is_file():
                new_file_path = os.path.join(tmpdir, entry.name)
                if not os.path.exists(new_file_path):
                    print(f'{new_file_path} does not exist')
                    errors.append(entry.name)
                else:
                    match = True
                    error_file_path = os.path.join(args.out_dir, entry.name)
                    file_type = os.path.splitext(entry.name)[-1]
                    if file_type == '.dlf':
                        match = compare_dlf_files(entry.path, new_file_path,
                                                  precision=args.default_float_epsilon,
                                                  sml_warn_level=args.sml_warn_level)
                    else:
                        warnings.warn(f'Skipping file type {file_type}')
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

if __name__ == '__main__':
    sys.exit(main())
