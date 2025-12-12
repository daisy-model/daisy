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

Assuming you have downloaded `daisy.flatpak` to the current working directory
```
flatpak install --user daisy.flatpak
```
It requires
- network permissions to enable installing additional python packages into the environment using pip
- access to home to read input and write output

You should now be able to run Daisy
```
$ flatpak run dk.ku.daisy -v
Daisy crop/soil simulation version 7.1.3. (Dec 12 2025)
Copyright 1996 - 2016 Per Abrahamsen, Søren Hansen and KU.
Storing 'daisy.log' in ...
```

You can create a convenience script that calls daisy through flatpak. Assuming `~/.local/bin` is on your path
```
printf "#%s/bin/sh\nflatpak run dk.ku.daisy \$@\n" "!" > ~/.local/bin/daisy && chmod +x ~/.local/bin/daisy
```

You should now be able to run Daisy
```
$ daisy -v
Daisy crop/soil simulation version 7.1.3. (Dec 12 2025)
Copyright 1996 - 2016 Per Abrahamsen, Søren Hansen and KU.
Storing 'daisy.log' in ...
```

The following assumes you have created this convenience script.


### Managing python in flatpak environment
The flatpak package comes with python. You can check the version of Daisy and python with
```
$ daisy --version
Daisy crop/soil simulation version 7.1.3. (Dec 12 2025)
Python 3.13.9
```

You can install additional packages with pip. To install `numpy`
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


## Running your first Daisy program
Several sample programs are included in the Daisy distribution. These are located in the folder `sample` under `<daisy-dir>`.

1. Create a new folder named `daisy` on your Desktop
2. Copy the file `test.dai` from the `sample` folder to the newly created `daisy` folder
3. Open the `test.dai` file from the `daisy` folder in your editor
4. Run the `test.dai` file. This should produce the following files in the `daisy` folder
   - `checkpoint-1987-8-7+6.dai`.
      the state of the simulation. Can be used to hot start the simulation.
   - `daisy.log`
      Log of the simulation that was just run
   - `field_nitrogen.dlf`
   - `field_water.dlf`
   - `harvest.dlf`
   - `sbarley.dlf`
   - `soil_nitrogen.dlf`
   - `soil_water.dlf`

The `.dlf` files are the output of the simulation. These are tab separated files with a custom header that can be read into spreadsheet applications or with you favorite programming language. We recommend using RStudio with the [daisyrVis](https://github.com/daisy-model/daisyrVis) package or Python with the [daispy-vis](https://github.com/daisy-model/daisypy-vis) package.
