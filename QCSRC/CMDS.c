// This command will generate the data file containg the list of game commands
// CRTPF FILE(GWZ/GAMELIST) SRCFILE(GWZ/QDDSSRC) SRCMBR(GAMELIST)

// This command will compile the display file for the game menu application
// CRTDSPF FILE(GWZ/CMDDSP) SRCFILE(GWZ/QDDSSRC) SRCMBR(CMDDSP)
//   MAXDEV(128)

// These commands were used to generate the C headers for the previous two files
// GENCSRC OBJ('/QSYS.LIB/GWZ.LIB/CMDLIST.FILE')
//   SRCFILE(GWZ/QCSRC) SRCMBR(CMDLIST) ONEBYTE(*CHAR)
// GENCSRC OBJ('/QSYS.LIB/GWZ.LIB/CMDDSP.FILE')
//   SRCFILE(GWZ/QCSRC) SRCMBR(CMDDSP) ONEBYTE(*CHAR)

// These are the commands needed to compile the game menu application
// CRTCMOD MODULE(GWZ/CMDS) SRCFILE(GWZ/QCSRC) DBGVIEW(*ALL)
// CRTPGM PGM(GWZ/CMDS) MODULE(GWZ/CMDS)

#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <recio.h>
#include <pwd.h>
#include <unistd.h>

#include "cmdlist"
#include "cmddsp"

#define GR_SIZE sizeof(GWZ_CMDLIST_CMDRCD_both_t)

_RFILE   *gp, *dp;

void strlap(char *out, char *in, int len) {
  int i;
  for(i = 0; i < len-1; i++) {
    if(in[i] == 0) break;
    out[i] = in[i];
  }
  for(; i < len-1; i++) {
    out[i] = ' ';
  }
  out[len-1] = 0;
}

int strlens(char *str, int maxlen) {
  int i;
  for(i = 0; i < maxlen; i++) {
    if(str[i] == ' ') return i;
    if(str[0] == 0) return i;
  }
  return maxlen;
}

void msgcpyl(char *out, char *in, int len) {
  int i = 0;
  while((i < len) && (in[i] != 0)) {
    out[i] = in[i];
    i++;
  }
  while(i < len) {
    out[i] = ' ';
    i++;
  }
}

void msgcpyc(char *out, char *in, int len) {
  int i = 0;
  int left = (len - strlen(in))/2;
  memset(out, ' ', left);

  while(((left+i) < len) && (in[i] != 0)) {
    out[left+i] = in[i];
    i++;
  }
  while((left+i) < len) {
    out[left+i] = ' ';
    i++;
  }
}

void msgcpyr(char *out, char *in, int len) {
  int i = 0;
  int left = len - strlen(in);
  memset(out, ' ', left);

  while(((left+i) < len) && (in[i] != 0)) {
    out[left+i] = in[i];
    i++;
  }
  while((left+i) < len) {
    out[left+i] = ' ';
    i++;
  }
}

int count_cmds(void) {
  GWZ_CMDLIST_CMDRCD_both_t gr;
  _RIOFB_T *iofb;
  int i = 0;

  iofb = _Rreadf(gp, &gr, GR_SIZE, __NO_LOCK);
  while(iofb->num_bytes != EOF) {
    i++;
    _Rreadn(gp, &gr, GR_SIZE, __NO_LOCK);
  }

  return i;
}

void read_desc(int entry, char *line) {
  GWZ_CMDLIST_CMDRCD_both_t gr;
  _RIOFB_T *iofb;

  iofb = _Rreadf(gp, &gr, GR_SIZE, __NO_LOCK);
  while(iofb->num_bytes != EOF) {
    if(gr.CMDID == entry) {
      line[0] = (entry > 99) ? (0xF0 + ((entry / 100) % 10)) : ' ';
      line[1] = (entry > 9) ? (0xF0 + ((entry / 10) % 10)) : ' ';
      line[2] = (0xF0 + (entry % 10));
      line[3] = '.';
      line[4] = ' ';
      memcpy(line+5, gr.DESC, 73);
      return;
    }
    _Rreadn(gp, &gr, GR_SIZE, __NO_LOCK);
  }
  memset(line, ' ', 78);
}

void run_cmd(char *select) {
  char line[78];
  char text[3];
  int entry;

  GWZ_CMDLIST_CMDRCD_both_t gr;
  _RIOFB_T *iofb;

  if(select[0] == ' ') {
    text[0] = select[1];
    text[1] = 0;
  } else if(select[1] == ' ') {
    text[0] = select[0];
    text[1] = 0;
  } else {
    text[0] = select[0];
    text[1] = select[1];
    text[2] = 0;
  }
  entry = strtoul(text, NULL, 10);

  iofb = _Rreadf(gp, &gr, GR_SIZE, __NO_LOCK);
  while(iofb->num_bytes != EOF) {
    if(gr.CMDID == entry) {
      memcpy(line, gr.CMD, sizeof(line)-1);
      line[sizeof(line)-1] = 0;
      system(line);
      return;
    }
    _Rreadn(gp, &gr, GR_SIZE, __NO_LOCK);
  }
}

void update_list(int PAGE, int PAGES, GWZ_CMDDSP_CMDLIST_o_t *or) {
  int i;
  int FIRST = (PAGE * 14) + 1;
  for(i = 0; i < 14; i++) {
    read_desc(FIRST + i, or->SCRN[i]);
  }
  PAGE++;
  memcpy(&or->TITLE[67], "Page", 4);
  or->TITLE[72] = 0xF0 + ((PAGE / 10) % 10);
  or->TITLE[73] = 0xF0 + (PAGE % 10);
  or->TITLE[74] = '/';
  or->TITLE[75] = 0xF0 + ((PAGES / 10) % 10);
  or->TITLE[76] = 0xF0 + (PAGES % 10);
}

int main(int argc, char *argv[]) {
  GWZ_CMDDSP_CMDLIST_o_t or;
  GWZ_CMDDSP_CMDLIST_i_t ir;
  int PAGE, PAGEMAX, CMDMAX;

  // Open display
  if((dp = _Ropen("GWZ/CMDDSP", "ar+")) == NULL) {
    printf("Opening display failed\n");
    exit(1);
  }

  if(argc > 1) {
    // Open commands list
    if((gp = _Ropen(argv[1], "rr")) == NULL) {
      printf("Opening command list failed\n");
      exit(1);
    }
  } else {
    // Open commands list
    if((gp = _Ropen("GWZ/CMDLIST", "rr")) == NULL) {
      printf("Opening command list failed\n");
      exit(1);
    }
  }

  PAGE = 0;
  CMDMAX = count_cmds();
  PAGEMAX = (CMDMAX / 14) + 1;

  // UI Loop
  if(argc > 1) {
    msgcpyr(or.KEYS, "F12=Go Back", sizeof(or.KEYS));
  } else {
    msgcpyr(or.KEYS, "F12=Sign Off", sizeof(or.KEYS));
  }

  if(argc > 2) {
    msgcpyc(or.TITLE, argv[2], sizeof(or.TITLE));
  } else {
    msgcpyc(or.TITLE, "Commands List", sizeof(or.TITLE));
  }

  memset(or.SEP1, '_', sizeof(or.SEP1));
  memset(or.SEP2, '_', sizeof(or.SEP2));
  memset(or.SCRN, ' ', sizeof(or.SCRN));
  update_list(PAGE, PAGEMAX, &or);

  for(;;) {
    // Send display data record and wait for input
    _Rformat(dp, "CMDLIST");
    _Rwrite(dp, &or, sizeof(or));
    ir.IN97 = '0';
    ir.IN98 = '0';
    ir.IN99 = '0';
    _Rreads(dp, &ir, sizeof(ir), __DFT);
    if(ir.IN97=='1') {
      //printf("User pressed previous screen key\n");
      if(PAGE > 0) {
        PAGE--;
        update_list(PAGE, PAGEMAX, &or);
      }
      continue;
    }
    if(ir.IN98=='1') {
      //printf("User pressed next screen key\n");
      if(PAGE < PAGEMAX-1) {
        PAGE++;
        update_list(PAGE, PAGEMAX, &or);
      }
      continue;
    }
    if(ir.IN99=='1') {
      //printf("User pressed an exit key\n");
      break;
    }

    if(ir.INPUT[0] != ' ' || ir.INPUT[1] != ' ') {
      run_cmd(ir.INPUT);
    }
  }

  // Close command list
  _Rclose(gp);

  // Close display
  _Rclose(dp);

  if(argc > 1) {
    return 0;
  } else {
    system("SIGNOFF");
  }

  return 0;
}
