Replace GWZ in the commands listed below with the name of the library
you use for this application.


This command will generate the data file containg the list of game commands

CRTPF FILE(GWZ/CMDLIST) SRCFILE(GWZ/QDDSSRC) SRCMBR(CMDLIST)


This command will compile the display file for the game menu application

CRTDSPF FILE(GWZ/CMDDSP) SRCFILE(GWZ/QDDSSRC) SRCMBR(CMDDSP)
  MAXDEV(128)


These commands were used to generate the C headers for the previous two files

GENCSRC OBJ('/QSYS.LIB/GWZ.LIB/CMDLIST.FILE')
  SRCFILE(GWZ/QCSRC) SRCMBR(CMDLIST) ONEBYTE(*CHAR)

GENCSRC OBJ('/QSYS.LIB/GWZ.LIB/CMDDSP.FILE')
  SRCFILE(GWZ/QCSRC) SRCMBR(CMDDSP) ONEBYTE(*CHAR)


These are the commands needed to compile the game menu application

CRTCMOD MODULE(GWZ/CMDS) SRCFILE(GWZ/QCSRC) DBGVIEW(*ALL)

CRTPGM PGM(GWZ/CMDS) MODULE(GWZ/CMDS)


If you start the application with no parameters, it will default to loading
the CMDLIST physical file, and signoff on exit.

CALL GWZ/CMDS


If you wish to avoid this behavior or call a submenu, you must provide
the name of the physical file and menu heading on the commandline

CALL GWZ/CMDS PARM('GWZ/ACTNGAMES' 'Action Games') 
