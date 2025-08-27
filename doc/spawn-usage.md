# Parallel execution of Daisy

* [`spawn`](#daisy-spawn) is a Daisy program for running multiple Daisy programs in in parallel.
* [`nwaps`](#daisy-nwaps) is a Daisy program for aggregating the output from `spawn`
    
The R package [`daisyrVis`](https://github.com/daisy-model/daisyrVis) is an alternative to `nwaps` for working with output from `spawn`.
    
## Daisy `spawn`
If you want to run two simulations `p1` and `p2` defined with `defprogram`, write:

```{lisp}
  (defprogram main spawn (program p1 p2))
  (run main)
```
This works for any number of programs.

This is intended for situations where you need to run many simulations, such as scenario analysis or Monte-Carlo simulations.

- Each subprogram is run in a separate Daisy process. This means if one program crashes, it won't stop the others.

- Only a limited number of programs will be run in parallel. Once one finishes, the next is started (if any are left).

- Each subprogram is run in a separate directory (folder), created by the spawn program. If the directory already exists, the subprogram will be skipped.

This means that you can start the same spawn program on multiple computers sharing a network drive, and the computers will automatically distribute the work between them (only one computer will successfully create the directory[^1] and run the program, the rest will skip it).

A longer example is found as [test-spawn.dai](../sample/test-spawn.dai) in the [sample](../sample) folder.

Some more parameters you usually won't need:

```{lisp}
(parallel 4)
```
Max number of programs to run in parallel.  By default, this is equivalent to the number of cores in your CPU.

```{lisp}
(exe "C:/daisy/bin/daisy.exe")
```
Daisy executable to spawn for each program.  By default, the same as is running the main program.

```{lisp}
(directory p1 p2)
```
Name of the subfolders to run the programs.  By default, the same as the names of the programs.

```{lisp}
(file "setup-p1.dai" "setup-p2.dai")
```
names of setup files where the programs are found.  By default, it will look in the same setup file as the main program.

```{lisp}
(input_directory "..")
```
Where to look for input files, instead of the current directory ("."). The default value is "..", meaning the parent directory, which is where the main program is found.

[^1]: In theory... some network file systems allows multiple computers to create the same directory at the same time, in that case output will be garbage.


## Daisy `nwaps`
The nwaps program will collect dlf files with the same name in different subdirectories, and generate csv files with the result concatenated, and the subdirectory name prepended each line. Compared to the dlf files, the combined file will have `out_` prepended, no header, and the tags and unit lines combined.  The resulting csv files can easily be imported in e.g. Excel, R, or Python.

The nwaps program is intended to be used with the output of the spawn program.

Apart from the `out_` files, some simple statistics for each dlf files will bu summaries on `sum_` csv files.
