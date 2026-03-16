# Getting started on Windows
Download the latest release of Daisy from [https://github.com/daisy-model/daisy/releases/latest](https://github.com/daisy-model/daisy/releases/latest)

We provide both a classical Windows installer and a zip file. The content is the same so pick whichever you prefer.

The installer requires administrator privileges, so pick the zip file if this a problem.
The zip file just needs to be unzipped to whatever location you prefer.

In the following we will refer to `<daisy-dir>`. This is the folder where you install Daisy, which is most likely `C:\Program Files\daisy <version-number>` if you use the installer.

## Test that it works
Open a shell (either `cmd.exe` or `Powershell` should be fine) and execute the following, remember to replace `<daisy-dir>` with the path from above
```
cd <daisy-dir>\bin
.\daisy.cmd --info
```
that should display something similar to
```
.\daisy.cmd --info
1:Daisy crop/soil simulation version 7.1.4. (Mar 16 2026)
Python 3.13.11
Sample dir: <sample-dir>
```
where `<sample-dir>` will be a directory containing sample files illustrating the use of Daisy.

## Managing Daisy's python environment
Daisy comes with a python environment that allows you to extend Daisy with functions implemented in python. If you need to run Daisy's python interpreter, for example to debug scripts, you can start it using
```
.\daisy.cmd --python
```

You can install additional packages into the environment by calling daisy with `--pip`. To install `numpy`
```
.\daisy.cmd --pip install numpy
```

Anything after `--pip` is passed to `pip`, so to upgrade `numpy`
```
.\daisy.cmd --pip install --upgrade numpy
```

To uninstall `numpy`
```
.\daisy.cmd --pip uninstall numpy
```

If you want to inspect the python environment it is stored under `<daisy-dir>\bin\Lib`.

### Repair pip
If `pip` does not work, you can repair it with
```
.\daisy.cmd --repair-pip
```

and
```
.\daisy.cmd --pip install --upgrade pip
```
to upgrade it.


## Setting up an environment for running Daisy
You can run Daisy from the commandline, but we recommend that you use an editor like [VSCode](https://code.visualstudio.com). See [instructions for setting up VSCode](setup-vscode.md).

Once you have set up an editor you can try [running your first daisy program](running-your-first-daisy-program.md).
