# CNAM_SEC102
Assignments for SEC102 module
The following repository contains the code required for the CNAM SEC102 module.
To build :
% build.bat 
If you are using a different MS Visual Studio version to the default value (Visual Studio 15 2017) then 
simply indicate the version on the command line:
% build.bat "Visual Studio 14 2015"

This repository contains the following projects:
- dirlist : list all directories and files in a given directory
- proclist : list all running processes and give information about loaded modules etc.
- rot13 : perform rot13 encoding and decode the Windows registry
- httpclient : simple HTTP client performs a GET on the host/URL given, uses winsock library
- wininetclient : simple HTTP client performs a GET on the host/URL given, uses winint library
