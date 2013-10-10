Live for Speed S2 0.6E DEDICATED HOST
=====================================


Welcome to Live for Speed - www.lfs.net


Important :
-----------
This is a special dedicated host program for Live for Speed S2.

By installing this software you agree to the following :

- Live for Speed is in continual development.  Features may be
added, updated or removed at any time.  Some functional but
incomplete features may be included in order to enhance the
variety and content of the simulator.

- This software comes with no warranties of any kind.

- You install and use this software at your own risk.  The
developers of Live for Speed can not accept any responsibility for
personal injury or damage to your computer system that may arise
during the use of the software.

- The DEMO content is given to all people to use, free of charge.
The LOCKED content is for appropriately licensed users only.  You
must not make any attempt to gain access to the locked content, or
help anyone else to do so, other than by purchasing a license and
using the in-game unlocking screen.

Thank you.


Installation :
--------------
To install the program, unzip the file into a suitable folder on your
hard drive.  You must ensure that the directory structure is preserved
by using the appropriate option in your unzip utility.  You will know
that it has been correctly unzipped if you see a data folder beside
the LFS.exe program and some more folders inside the data folder.


To run LFS S2 DEDICATED HOST :
------------------------------
Use a shortcut, batch file, or any method you like to start LFS.exe
with a command line.  If you do not use a command line, LFS will take
its startup options from the setup.cfg file.


Command line options :
----------------------
A command line or a command file is required for the dedicated host.
The commands are listed in the file Commands.txt in the docs folder.


Status file :
-------------
While your host is running, a file host63392.txt can be found in your
LFS folder, and is updated whenever the info changes :

63392 is the port number of the host.
It contains the following information:

lfs=0.6E             :version
status=ingame        :offline / online / ingame
guests=4             :current number of guests
maxguests=4          :maximum guests allowed
host=Host Name       :game name as listed on master server
pass=Optional Pass   :host password
usemaster=yes        :no / yes / hidden
trackcfg=BL1         :short name for track and config
cars=[18x"0"or"1"]   :cars allowed on host (0=no / 1=yes)
qual=0               :qualifying minutes
laps=5               :number of laps
conn=Host            :player name
conn=Guest 1         :player name
conn=Guest 2         :player name
...


Thanks :
--------
Thanks to all internet hosts for your support.


Copyright :
-----------
(c) 2002-2012 Scawen Roberts - Eric Bailey - Victor van Vlaardingen