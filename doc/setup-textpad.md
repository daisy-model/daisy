# Setup TextPad
Introduction

Although you can run Daisy from the command line or through the primitive GUI interface, by far the most convenient environment is a good text editor. One such editor is TextPad. By following the instructions on this page, you will gain:

* Parenthesis pair matching.
* Highlighting of models, fixed components and parentheses in input files.
* Run Daisy within TextPad.

## Instructions

The configuration process can be divided into three tasks, first downloading and installing the files, then adding support for Daisy setup files, and finally for running the daisy command line executable (daisy.exe) from within TextPad.
### Download and install

1. [Fetch TextPad](http://www.textpad.com/) by following the link.
2. Install TextPad on you computer, and remember the installation directory.
3. [Fetch and unpack the TextPad Daisy support files](https://daisy.ku.dk/download/windows/textpad/textpad-daisy-v2.0.zip), again follow the link.
4. Copy the unpacked files (Dai.syn, Daisy.tcl, Daisy2.tcl, and Daisy3.tcl) to the "Samples" directory, below the TextPad install directory.

### Create a Daisy document class

1. Start TextPad.
2. Open the "Configure" menu.
3. Select "New Document Class..."
4. Specify Daisy as the class name, and press "Next".
5. Specify *.dai as the class members, and press "Next".
6. Check the "Enable syntax highlighting" check mark box.
7. Select "Dai.syn" from the "Syntax definition file:" drop down menu.
8. Press "Next" and "Apply".
9. Exit TextPad to make the changes take effect.

If you open a Daisy setup file with a file name suffix of ".dai" with TextPad, you should now see pretty colors.

### Add Daisy to the "Tools" menu

1. Start TextPad.
2. Open the "Configure" menu.
3. Select "Preferences..."
4. Click on "Tools" (the name, not the small box with a beside it).
5. Click on "Add" and choose "Program..." from the drop down menu.
6. Find and select the "daisy.exe" file. It is normally located in the "bin" subfolder of the folder where you installed Daisy.
6. Press "Apply".
7. Click the box next to "Tools" to view the available tools.
8. Select "Daisy".
9. Copy the string "start /b /wait daisy.exe $File" into the text field names "Parameters"
10. Copy the exact string ([^:\\]+):(\d+):(\d+) to text field named "Regular expression to match output", replacing the old content. Use cut and paste to get it right.
11. Choose "1" from the "File" drop down, choose "2" from the line drop down, and choose "3" from the "Column" drop down menu.
12. Press Apply.
13. Exit TextPad to make the changes take effect.

If you open a Daisy setup file, you should now be able to run the simulation by opening the "Tools" menu, the "External tools" submenu, and then selecting "Daisy". The output from the simulation will be placed in another window. If an error message contain a file name and line number, you will (sometimes) be able to go directly to the specified location by double clicking on the error message.
