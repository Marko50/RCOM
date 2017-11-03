/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include "functions.h"

#define BAUDRATE B115200
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE;


int main(int argc, char** argv)
{
    int fd;
    //int c, res;
    struct termios oldtio,newtio;

    //int i, sum = 0, speed = 0;

    if ( (argc < 3) ||
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) &&
  	      (strcmp("/dev/ttyS1", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1 pinguim.gif\n");
      exit(1);
    }

    char * filename = argv[2];

  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */


    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd <0) {perror(argv[1]); exit(-1); }

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;   /* blocking read until 1 unsigned char received */



  	/*
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
    leitura do(s) pr�ximo(s) caracter(es)
  	*/



  tcflush(fd, TCIOFLUSH);

  if( tcsetattr(fd,TCSANOW,&newtio) == -1) {
    perror("tcsetattr");
    exit(-1);
  }

  printf("New termios structure set\n");

  int cmp = llopen(fd, 0);
  if(cmp == -1)
    return -1;

	int test = applicationLayer(0,fd,filename);
	if(test == -1){
		printf("applicationLayer FAILED OMG RKO OUT OF NOWHERE. WRITENONCANONICAL.C\n");
	}
  sleep(3);

  if(tcsetattr(fd,TCSANOW,&oldtio) == -1) {
    perror("tcsetattr");
    exit(-1);
  }
  int ret = llclose(fd);
  return ret;
}
