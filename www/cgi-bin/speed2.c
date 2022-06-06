#include <stdio.h>
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
	FILE *r = fopen("flag.txt","r");
	int size = 0;
	fseek(r,0,SEEK_END);
	size = ftell(r);
	fclose(r);
	if(size != 0){

		int dev_fd;
		dev_fd = open("/dev/pwm",O_RDWR | O_NONBLOCK);
		int i = 2;
		ioctl(dev_fd,PWM_ON);
		//scanf("%d",&i);
		ioctl(dev_fd, SET_CNT, &i);

		uint8_t cmd[9];
	int fd_segdis = open(SEGMENT_DISPLAY, O_WRONLY);
	cmd[0]=1;
	cmd[1]=2;
	FS4412_segdis_w_dig(fd_segdis, cmd);

		cgiHeaderContentType("text/html; charset=utf-8");
		fprintf(cgiOut, "<HTML><HEAD>\n");
		fprintf(cgiOut,"<script type=\"text/javascript\"> window.onload=function(){	window.location.href=\"../index.html\";}</script>\n");
		
		fprintf(cgiOut,"<TITLE>FormMessage</TITLE></HEAD>\n");
		fprintf(cgiOut, "<BODY>");
	  
		fprintf(cgiOut, "</BODY>\n");
		fprintf(cgiOut, "</HTML>\n");
	}
	else{
		cgiHeaderContentType("text/html; charset=utf-8");
	    fprintf(cgiOut, "<HTML><HEAD>\n");
	    fprintf(cgiOut,"<script type=\"text/javascript\"> window.onload=function(){	window.location.href=\"../index.html\";}</script>\n");
		
	    fprintf(cgiOut,"<TITLE>FormMessage</TITLE></HEAD>\n");
	    fprintf(cgiOut, "<BODY>");
	  
	    fprintf(cgiOut, "</BODY>\n");
	    fprintf(cgiOut, "</HTML>\n");
		fprintf(cgiOut,"<script type=\"text/javascript\"> alert(\"请先打开设备!\");</script>\n");
	}
	return 0;
}
