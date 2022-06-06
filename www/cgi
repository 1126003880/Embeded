#ifndef _MAIN_H
#define _MAIN_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <pthread.h>
#include <signal.h>
#include <sys/stat.h>


#define PWM_ON 	_IO('K', 0)
#define PWM_OFF _IO('K', 1)
#define SET_PRE _IOW('K', 2, int)
#define SET_CNT _IOW('K', 3, int)

   /*************************** NOTICE ***********************************
	* 		there should be an interval of more than 2 ms
	* 		between segment display control functions calls
	*********************************************************************/

	/**********************************************************************
	 * @Function:	FS4412_segdis_clr
	 * @Description:	clear segment displays
	 * @Param[in]:	(int)fd:
	 * 					file descriptor of segment displays device,
	 * 				(uint8_t)cmd[2]:
	 *					cmd[0]: must be 0
	 *					cmd[1]:	digit postion(1-8), if 0 clear all
	 * @Return:	other than -1 upon successful completion
	 **********************************************************************/
#define FS4412_segdis_clr(fd, cmd)		write(fd, cmd, 2)

	 /**********************************************************************
	  * @Function:	FS4412_segdis_w_dig
	  * @Description:	display 1 digit
	  * @Param[in]:	(int)fd_LED:
	  * 					file descriptor of segment displays device,
	  * 				(uint8_t)dig[2]:
	  *					dig[0]: digit position(1-8)
	  *					dig[1]: (char)digit to be displayed
	  * @Return:	other than -1 upon successful completion
	  **********************************************************************/
#define FS4412_segdis_w_dig(fd, dig) 	write(fd, dig, 2)

	  /**********************************************************************
	   * @Function:	FS4412_segdis_w_num
	   * @Description:	display 8 number
	   * @Param[in]:	(int)fd:
	   * 					file descriptor of segment displays device,
	   * 				(uint8_t)num[9]:
	   *					num[0]: must be 9
	   *					num[1-8]: (char)numbers to be displayed
	   * @Return:	other than -1 upon successful completion
	   **********************************************************************/
#define FS4412_segdis_w_num(fd, num) 	write(fd, num, 9)

#endif