============================================================
   	OpenCarrom (C) By Mrinal Dhar 
============================================================

A multi-threaded, multiplayer Carrom based on OpenGL.




How to compile?
-> run 'make' in the directory with the files main.cpp and Makefile. This generates an executable with the name 'carrom_md'

How to run? 
-> first, compile the code using the above instruction. then, run './carrom_md' in that directory. 

How to play? 
-> the instructions and controls to play appear before every game starts, and you can make those instructions appear by pressing the RETURN key in the middle of a game.



-----------------------------------------------------------
		:: ** Important Note ** ::
-----------------------------------------------------------

This game has been tested successfully on:
	1. Mac OS X 10.10 with XCode 6
	2. Ubuntu 14.04

The makefile that is being shipped with this game contains all 
the necessary flags for it to function properly. 

It uses multi threading, so the pthread library has been used. 

It uses timing functions, so chronos library has been used. 
(For which an experimental gcc flag has been set as seen in the makefile.)
