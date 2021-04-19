Static Configuration Generation Flow SJA1105PQRS
================================================

This package contains the python tools to generate
the static configuration sequence in binary loader format
aka HEX file.

Installation
============
In order to run the files a Python 2 installation (>=2.6) or Python 3.x
is required. Python can be obtained from the python.org website.
It is recommended to put the python binary in the Window's PATH environment.

This package does not need to be installed and can be be extracted to a
custom directory. The scripts are executed through the windows command line
(cmd.exe). 

  1. Open cmd.exe
  2. Navigate to the target directory 
     cd C:\your_directory
  3. Execute the script to create the hex file
     python.exe simplePQRS.py
  4. The script will generate two hex files,
     one for simplePQRS_SJA1105QS and another for simplePQRS_SJA1105PR.
     The difference is the device id which is embedded
     in the hex file at address 0x20000. Otherwise, the files
     are identical.

Files
=====

simple.py
  A simple example with reasonable defaults for the SJA1105(T).
  
simpleT.py
  Same as simple.py, but with additional settings for 
  time-triggered ethernet tables only available on SJA1105T:
  TAS schedules and stream-based policing
  
simplePQRS.py
  A simple example with reasonable defaults. Comparable to the 
  examples as found in the SJA1105(T) package.

examplePQRS.py
  A more realistic, sophisticated example with policing and 
  credit based shaper control.

examplePQRS_TSN.py
  Same as examplePQRS.py, but with the additional settings for 
  time-triggered ethernet available on SJA1105QS
  
decode.py
    A diassmbler to peek into hex files and decode them.


Changelog
=========

* Revision 0.1, 2.3.2017
    Initial release
* Revision 0.2 18.4.2017
    Fixed wrong symbol in L2 Forwarding Table from VLAN_BC to BC_DOMAIN.
* Revision 0.3 03.07.2017
    Moved to new branch of generation scripts
    Support for SJA1105/T and SJA1105/P/Q/R/S
    Added examples for Qbv (time aware shaper) and Qci (stream based policing)
* Revision 0.4 16.04.2021
    Update to use Python 3.x
