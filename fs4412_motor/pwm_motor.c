/*
 * pwm_motor.c : test demo driver
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include "fs4412_pwm.h"

int main()
{
	int dev_fd;
	int i = 0;

	dev_fd = open("/dev/pwm", O_RDWR | O_NONBLOCK);
	if ( dev_fd == -1 ) {
		perror("open");
		exit(1);
	}

	ioctl(dev_fd, PWM_ON);
	printf("###   Open successfully!   ###\n\n");
	ioctl(dev_fd, SET_CNT, &i);
	sleep(1);
	
	while(1)
	{
		printf("---------------------\n\n");
		printf("#   1~7：调整转速   #\n");
		printf("#     8：暂停转动   #\n");
		printf("#     9：退出程序   #\n");
		printf("---------------------\n\n");
		printf("请输入1~9之间的数字：\n");

		scanf("%d", &i);
		if(i < 0 || i > 9)
		{
			printf("请输入1~9之间的数字！\n");
			continue;
		 } 
		if(i == 8)
		{
		    i = 0;
			printf("Pause!!\n");
		}
		if(i == 9)
		{
			i = 0;
			ioctl(dev_fd, SET_CNT, &i);
			break;
		}

		ioctl(dev_fd, SET_CNT, &i);
		printf("rate level:%d\n", i);
		
	}

	return 0;
}
