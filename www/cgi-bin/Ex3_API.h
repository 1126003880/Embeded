
#ifndef _EX3_API_H
#define _EX3_API_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <pthread.h>
#include <signal.h>

/* path of devices */
#define LED ("/dev/FS4412_LED")
#define SEGMENT_DISPLAY ("/dev/FS4412_segdis")

#define LED_MAGIC 'L'
#define LED_STATE	_IOR(LED_MAGIC, 0, uint8_t)
#define LED_SWITCH  _IOW(LED_MAGIC, 1, uint8_t)
#define LED_ROTATE	_IOW(LED_MAGIC, 2, size_t)

/**********************************************************************
 * @Function:	FS4412_LED_state
 * @Description:	get LED state
 * @Param[in]:	(int)fd:
 * 					file descriptor of LED device,
 * 				(uint8_t)led_sts[4]:
 * 					led_sts[n] will be set 1 if led n is on,
 * 					otherwise led_sts[n] will be set 0
 * @Return:	other than -1 upon successful completion
 **********************************************************************/
#define FS4412_LED_state(fd, led_sts) 	ioctl(fd, LED_STATE, led_sts)

 /**********************************************************************
  * @Function:	FS4412_LED_switch
  * @Description:	switch on/off LED
  * @Param[in]:	(int)fd:
  * 					file descriptor of LED device,
  * 				(uint_8)led:
  * 					LED number
  * @Return:	other than -1 upon successful completion
  **********************************************************************/
#define FS4412_LED_switch(fd, led) 		ioctl(fd, LED_SWITCH, led)

  /**********************************************************************
   * @Function:	FS4412_LED_rotate
   * @Description:	move LED
   * @Param[in]:	(int)fd:
   * 					file descriptor of LED device,
   * 				(size_t)rt_args[2]:
   *					rt_args[0]:	0: stop rotating
   *								1: move anticlockwise
   *								2: move clockwise
   *								3: rotate anticlockwise
   *								4: rotate clockwise
   *					rt_args[1]: rotation delay (ms)
   * @Return:	other than -1 upon successful completion
   **********************************************************************/
#define FS4412_LED_rotate(fd, rt_args) 	ioctl(fd, LED_ROTATE, rt_args)



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
