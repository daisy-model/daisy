# Getting started on MacOS
Download the latest release of Daisy from [https://github.com/daisy-model/daisy/releases/latest](https://github.com/daisy-model/daisy/releases/latest). Note that there are two versions, the standard version that includes python support and a version without python support. Pick the latter if you do not want python support.

Unzip the archive you just downloaded and place it somewhere you can remember. In the following we will refer to this location as `<daisy-dir>`.

## Test that it works
Open a terminal and execute the following, remember to replace `<daisy-dir>` with the path from above
```
cd <daisy-dir>/Daisy
bin/daisy --version
```
that should display something similar to

```
Daisy % bin/daisy --info
Daisy crop/soil simulation version 7.1.3. (Dec 15 2025)
Python 3.13.11
Sample dir: <sample-dir>
```
where `<sample-dir>` will be a directory containing sample files illustrating the use of Daisy.

## Setting up an environment for running Daisy
You can run Daisy from the commandline, but we recommend that you use an editor like [VSCode](https://code.visualstudio.com). See [instructions for setting up VSCode](setup-vscode.md).

Once you have set up an editor you can try [running your first daisy program](running-your-first-daisy-program.md).
