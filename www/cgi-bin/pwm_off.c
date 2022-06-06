#include <stdio.h>
#include <sys/io.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include "cgic.h"
#include <linux/input.h>
#include "fs4412_pwm.h"
#include "main.h"
#include "Ex3_API.h"
#include "FS4412_segdis_drv_2.h"

int cgiMain()
{
	int dev_fd;
	dev_fd = open("/dev/pwm",O_RDWR | O_NONBLOCK);

	uint8_t cmd[9];
	int fd_segdis = open(SEGMENT_DISPLAY, O_WRONLY);
	cmd[0]=1;
	cmd[1]=0;
	FS4412_segdis_w_dig(fd_segdis, cmd);

	int i = 0;
	ioctl(dev_fd,PWM_ON);
	ioctl(dev_fd, SET_CNT, &i);

		cgiHeaderContentType("text/html; charset=utf-8");
	    fprintf(cgiOut, "<HTML><HEAD>\n");
	    fprintf(cgiOut,"<script type=\"text/javascript\"> window.onload=function(){	window.location.href=\"../index.html\";}</script>\n");
		
	    fprintf(cgiOut,"<TITLE>FormMessage</TITLE></HEAD>\n");
	    fprintf(cgiOut, "<BODY>");
	  
	    fprintf(cgiOut, "</BODY>\n");
	    fprintf(cgiOut, "</HTML>\n");
		fprintf(cgiOut,"<script type=\"text/javascript\"> alert(\"设备已退出!\");</script>\n");


       remove("flag.txt");

	   FILE *fp;
	   fp = fopen("flag.txt","w");
	   fclose(fp);
	   return 0;

}
