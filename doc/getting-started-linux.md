# Getting started on Linux
Download the latest release of Daisy from [https://github.com/daisy-model/daisy/releases/latest](https://github.com/daisy-model/daisy/releases/latest).

We provide the following packages
 - `deb`
 - `flatpak`

## Install `deb` package
Assuming you have downloaded `daisy_7.1.3_amd64.deb` to the current working directory
```
# apt install ./daisy_7.1.3._amd64.deb
```
You should now be able to run Daisy
```
$ daisy -v
Daisy crop/soil simulation version 7.1.3. (Dec 11 2025)
Copyright 1996 - 2016 Per Abrahamsen, Søren Hansen and KU.
Storing 'daisy.log' in ...
```

## Install `flatpak` package
Follow instructions from https://flatpak.org/ to setup flatpak.

Assuming you have downloaded `daisy-7.1.3.flatpak` to the current working directory
```
flatpak install --user daisy-7.1.3.flatpak
```
It requires
- network permissions to enable installing additional python packages into the environment using pip
- access to home to read input and write output

You should now be able to run Daisy
```
$ flatpak run dk.ku.daisy --info
Installing lib and sample
Daisy crop/soil simulation version 7.1.3. (Dec 17 2025)
Python 3.13.9
```

You can create a convenience script that calls daisy through flatpak. Assuming `~/.local/bin` is on your path
```
printf "#%s/bin/sh\nflatpak run dk.ku.daisy \$@\n" "!" > ~/.local/bin/daisy && chmod +x ~/.local/bin/daisy
```

You should now be able to run Daisy
```
$ daisy --info
Daisy crop/soil simulation version 7.1.3. (Dec 17 2025)
Python 3.13.9
```

The following assumes you have created this convenience script.

## Managing Daisy's python environment
Daisy can be extended with functions implemented in python.

### deb package
If you installed the `deb` package Daisy will use your environment's python and you should refer to the documentation for your environment for how to manage it.

### flatpak package
If you installed the `flatpak` package, then Daisy comes with a python environment. If you need to run Daisy's python interpreter, for example to debug scripts, you can start it using
```
daisy --python
```

You can install additional packages by passing `--pip` to daisy. To install `numpy`
```
daisy --pip install numpy
```

Anything after `--pip` is passed to `pip`, so to upgrade `numpy`
```
daisy --pip install --upgrade numpy
```

To uninstall `numpy`
```
daisy --pip uninstall numpy
```

If you want to inspect the python environment it *should* be stored under `~/.var/app/dk.ku.daisy/data/python/lib/` but it might change depending on flatpak.


## Setting up an environment for running Daisy
You can run Daisy from the commandline, but we recommend that you use an editor like [VSCode](https://code.visualstudio.com). See [instructions for setting up VSCode](setup-vscode.md).

Once you have set up an editor you can try [running your first daisy program](running-your-first-daisy-program.md).
