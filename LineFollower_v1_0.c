/*Follower Program				       */
/*  Version number:        Ver.1.00                                    */
/*  Date:                  06.10.2018                                  */
/*  Author:                Fatih CEKEN						 	       */
/***********************************************************************/

/*======================================*/
/* Include                              */
/*======================================*/
#include "iodefine.h"
#include "mylib/timers.h"
#include "mylib/misc.h"
#include "funcs.h"

#define  SPEED 	95
int CURVE_SPEED = SPEED * 0.50;//0.70
int PERP_SPEED = SPEED * 0.30;
int CROSSLINE_SPEED = SPEED * 0.18;
DIR fPERP = NoDir;
DIR fCrossDir = NoDir;
unsigned short fSlow = 0;
unsigned short fLeftCross = 0;
int CURRENT_SPEED = SPEED;
unsigned char sensor_port_data = 0;
unsigned char s1 = 0, s2 = 0, s3 = 0;
unsigned short CrossCount = 0;
int timer = 0;

unsigned long sum=0, count=0;
TIMER_TIMER PERP_Timer;
/*======================================*/
/* Prototype declarations               */
/*======================================*/

/*======================================*/
/* Main program                         */
/*======================================*/

unsigned int Sensor_Read(void)
{
	unsigned int i = 0;
	unsigned int  location = 0;
	static unsigned int  location_last = DESIRED_LOCATION;
	unsigned char led_data = 0;

	sum=0;
	count=0;
	sensor_port_data = 0;
//	sensor_port_data = PORTB.PORT.BYTE;	// Sol 8 sensör
	sensor_port_data = PORT4.PORT.BYTE;
	for (i = 0; i < 8; i++) {
		if(!GET_BIT(sensor_port_data, 7-i) == TRUE){	// i = 0 --> en sol sensor, i = 7 --> en sağ orta sensor.
			sum += ((i+1)*10);
			count++;
		}
	}
/*
	sensor_port_data = PORT4.PORT.BYTE;	// Sağ 8 sensör
	for (i = 0; i < 8; i++) {
		if(GET_BIT(sensor_port_data, 7-i) == TRUE){	// i = 0 --> en sağ sensor, i = 7 --> en sol orta sensor.
			sum += ((i+9)*10);
			count++;
		}
		if ( i > 3 && i < 8) {
			if(GET_BIT(sensor_port_data, i) == TRUE) {
				switch (i) {
					case 4: PORTA.DR.BIT.B0 = 0; break;
					case 5: PORTA.DR.BIT.B1 = 0; break;
					case 6: PORTA.DR.BIT.B2 = 0; break;
					case 7: PORTA.DR.BIT.B3 = 0; break;
					default:
						break;
				}
			}else {
				switch (i) {
					case 4: PORTA.DR.BIT.B0 = 1; break;
					case 5: PORTA.DR.BIT.B1 = 1; break;
					case 6: PORTA.DR.BIT.B2 = 1; break;
					case 7: PORTA.DR.BIT.B3 = 1; break;
					default:
						break;
				}
			}
		}
	}*/

	if (count) {
		location = sum/count;
	}else {
	//	location = 0;
	}
  // if ( location == 0 ) { location=location_last; }
  if ((!GET_BIT(sensor_port_data, 7) && !GET_BIT(sensor_port_data, 6) && !GET_BIT(sensor_port_data, 0)) ||
	   (!GET_BIT(sensor_port_data, 0) && !GET_BIT(sensor_port_data, 1) && !GET_BIT(sensor_port_data, 7)) ||
	   (!GET_BIT(sensor_port_data, 0) && !GET_BIT(sensor_port_data, 1) && !GET_BIT(sensor_port_data, 6) && !GET_BIT(sensor_port_data, 7)) ||
	   (!GET_BIT(sensor_port_data, 7) && !GET_BIT(sensor_port_data, 6)) ||
	   (!GET_BIT(sensor_port_data, 0) && !GET_BIT(sensor_port_data, 1)) ||
	   (!GET_BIT(sensor_port_data, 7) && !GET_BIT(sensor_port_data, 0))/* ||
	   (!GET_BIT(sensor_port_data, 7)) ||
	   (!GET_BIT(sensor_port_data, 0))*/) {
	   location=location_last;
   }


   location_last = location;
   return location;
}

void Check_CrossLine(void)
{
	sensor_port_data = PORT4.PORT.BYTE;
	if ((!GET_BIT(sensor_port_data, 7)) &&
		(!GET_BIT(sensor_port_data, 6)) &&
		/*(!GET_BIT(sensor_port_data, 5)) &&*/
		/*(GET_BIT(sensor_port_data, 2)) &&*/
		(GET_BIT(sensor_port_data, 1)) &&
		(GET_BIT(sensor_port_data, 0))) {
		fCrossDir = Left;
	}


	if ((!GET_BIT(sensor_port_data, 0)) &&
		(!GET_BIT(sensor_port_data, 1)) &&
		/*(!GET_BIT(sensor_port_data, 2)) &&*/
		/*(GET_BIT(sensor_port_data, 5)) &&*/
		(GET_BIT(sensor_port_data, 6)) &&
		(GET_BIT(sensor_port_data, 7))) {
		fCrossDir = Right;
	}
	if (fSlow == 0) {
		Sensor_Read();
			if (fCrossDir == Left) {
				Set_Main_Motor(-55, -55); //-55 -55
				Set_Servo_Angle(0);
				delayMs(110);
				while(GET_BIT(sensor_port_data, 5) && GET_BIT(sensor_port_data, 6)){
					sensor_port_data = PORT4.PORT.BYTE;
					Set_Main_Motor(10, 40);//10 40
					Set_Servo_Angle(-27);
				}
					fCrossDir = NoDir;
					CrossCount++;
			}else if (fCrossDir == Right) {
				Set_Main_Motor(-55, -55); //-55 -55
				Set_Servo_Angle(0);
				delayMs(110);//150
				while(GET_BIT(sensor_port_data, 1) && GET_BIT(sensor_port_data, 2)){
					Sensor_Read();
					Set_Main_Motor(40, 10);//40 10
					Set_Servo_Angle(27);
				}
					fCrossDir = NoDir;
					CrossCount++;
			}

	}

/*
	if (CrossCount == 1) {
		CURRENT_SPEED  = 50;
	}else {
		if (CURRENT_SPEED == 50) {
			CURRENT_SPEED = SPEED;
		}
	}*/
}

void Check_oval(void){


	sensor_port_data = PORT4.PORT.BYTE;// DEGİSTİREBİLİRİZ

	if( ((GET_BIT(sensor_port_data, 7))||(GET_BIT(sensor_port_data, 6))||(GET_BIT(sensor_port_data, 5)))&&

		((GET_BIT(sensor_port_data, 4))&&(GET_BIT(sensor_port_data, 3)))&&

		((!GET_BIT(sensor_port_data, 7))&&(!GET_BIT(sensor_port_data, 7))&&(!GET_BIT(sensor_port_data, 7))) && (timer == 1)){

		Set_Servo_Angle(-27);
		CURRENT_SPEED = CURVE_SPEED;
		Set_Main_Motor(10, 40);
		TIMER_Start(&PERP_Timer, 250);

		}
	else if(((GET_BIT(sensor_port_data, 7))||(GET_BIT(sensor_port_data, 6))||(GET_BIT(sensor_port_data, 5)))&&

		((GET_BIT(sensor_port_data, 4))&&(GET_BIT(sensor_port_data, 3)))&&

		((!GET_BIT(sensor_port_data, 7))&&(!GET_BIT(sensor_port_data, 7))&&(!GET_BIT(sensor_port_data, 7)))){
			timer++;
			Set_Servo_Angle(0);
			CURRENT_SPEED = SPEED;
			Set_Main_Motor(CURRENT_SPEED, CURRENT_SPEED);
			TIMER_Start(&PERP_Timer, 250);




		}

}

void Check_EightBits(void)
{
	sensor_port_data = PORT4.PORT.BYTE;// DEGİSTİREBİLİRİZ

	if (    (GET_BIT(sensor_port_data, 7)) &&
			(GET_BIT(sensor_port_data, 6)) &&
			(GET_BIT(sensor_port_data, 5)) &&
			(GET_BIT(sensor_port_data, 2)) &&
			(GET_BIT(sensor_port_data, 1)) &&
			(GET_BIT(sensor_port_data, 0)) &&
			(GET_BIT(sensor_port_data, 3)) &&
			(GET_BIT(sensor_port_data, 4))
			) {

			Set_Servo_Angle(0);
			CURRENT_SPEED = SPEED;
			Set_Main_Motor(CURRENT_SPEED, CURRENT_SPEED);
			TIMER_Start(&PERP_Timer, 500);

	}

}
void Check_WhiteLine(void)
{
	sensor_port_data = PORT4.PORT.BYTE;


			Set_Servo_Angle(0);
			CURRENT_SPEED = SPEED;
			Set_Main_Motor(CURRENT_SPEED, CURRENT_SPEED);
			TIMER_Start(&PERP_Timer, 250);// Delay değişimi yapılacak
			if((GET_BIT(sensor_port_data, 7)) && (GET_BIT(sensor_port_data, 6)) && (GET_BIT(sensor_port_data, 5)) &&
				(GET_BIT(sensor_port_data, 2)) && (GET_BIT(sensor_port_data, 1)) &&(GET_BIT(sensor_port_data, 0)))
				{
				Set_Servo_Angle(0);
				CURRENT_SPEED = SPEED;
				Set_Main_Motor(CURRENT_SPEED, CURRENT_SPEED);
				//TIMER_Start(&PERP_Timer, 250);// Delay değişimi yapılacak
				}
			else if((GET_BIT(sensor_port_data, 3)) && (GET_BIT(sensor_port_data, 2))
					&& (GET_BIT(sensor_port_data, 1)) && (GET_BIT(sensor_port_data, 0)))
			{
				sensor_port_data = PORT4.PORT.BYTE;
				Set_Main_Motor(10, 40);//10 40
				Set_Servo_Angle(-60);
			}
			else if((GET_BIT(sensor_port_data,4)) && (GET_BIT(sensor_port_data,5)) &&
					(GET_BIT(sensor_port_data,6)) && (GET_BIT(sensor_port_data,7)))
			{
				Sensor_Read();
				Set_Main_Motor(40, 10);//40 10
				Set_Servo_Angle(60);
			}
			Set_Servo_Angle(0);
			CURRENT_SPEED = SPEED;
			Set_Main_Motor(CURRENT_SPEED, CURRENT_SPEED);
			TIMER_Start(&PERP_Timer, 250);// Delay değişimi yapılacak
}

void main(void)
{

	int 	perp_counter = 0;
	volatile unsigned int 	sensor_data = 0;
	volatile int 	L_PWM = 0, R_PWM = 0, Angle = 0, Angle_Perp = 0;
	// Servo PID
	float const PID_Coefs_SERVO[3] = {1.85, 0, 10};//8,0.5,0
	float       p_SERVO=0, i_SERVO=0, d_SERVO=0, t_SERVO=0;
	// Differential PID
	float const PID_Coefs_DIFF[3] = {3.60, 0, 3};//17,0.5,0
	float       p_DIFF=0, i_DIFF=0, d_DIFF=0, t_DIFF=0;
	// PID Error variables
	volatile long	    error=0, error_lst=0, error_dif=0;

	System_Init();
	Set_Main_Motor(CURRENT_SPEED, CURRENT_SPEED);
	Set_Servo_Angle(0);
	/*while(1){
		Set_Servo_Angle(0);
		Set_Main_Motor(0, 0);
	}*/
	/*delayMs(1000);
	while (1) {
		perp_counter += 10;
		Set_Servo_Angle(perp_counter);
		delayMs(100);
	}*/
	while(1)
	{

		sensor_data = Sensor_Read();
		Check_EightBits();
		Check_oval();
		Check_CrossLine();
		if ((GET_BIT(sensor_port_data, 7))  &&
		    (GET_BIT(sensor_port_data, 6))  &&
	     	(GET_BIT(sensor_port_data, 5))  &&
		    (!GET_BIT(sensor_port_data, 2)) &&
		    (!GET_BIT(sensor_port_data, 1)) &&
		    (!GET_BIT(sensor_port_data, 0))
		)
		{
			Check_WhiteLine();
		}


		sensor_port_data = PORT4.PORT.BYTE;
		if (fPERP == NoDir) {
			//=============Virajlarda yavaşlama=============//
			if (!GET_BIT(sensor_port_data, 6) || !GET_BIT(sensor_port_data, 5) ||
				!GET_BIT(sensor_port_data, 2) || !GET_BIT(sensor_port_data, 1) ) {
				if (CURRENT_SPEED != PERP_SPEED) {
					CURRENT_SPEED = CURVE_SPEED;
				}

			}else {
				if (CURRENT_SPEED != PERP_SPEED) {
					CURRENT_SPEED = SPEED;
				}
			}
			//=============Virajlarda yavaşlama=============//
			error = sensor_data - DESIRED_LOCATION;
			error_dif = error - error_lst;
			error_lst = error;

			p_SERVO = PID_Coefs_SERVO[P] * error;
			i_SERVO = i_SERVO+(PID_Coefs_SERVO[I] * error_dif);
			d_SERVO = PID_Coefs_SERVO[D] * error_dif;
			t_SERVO = p_SERVO+i_SERVO+d_SERVO;


			Angle = (int)t_SERVO;
			if(Angle < -90)	Angle = -90;
			if(Angle > 90)	Angle = 90;
			if(error == 0)	i_SERVO = 0;
			Set_Servo_Angle(Angle);



			p_DIFF = PID_Coefs_DIFF[P] * error;
			i_DIFF = i_DIFF+(PID_Coefs_DIFF[I] * error_dif);
			d_DIFF = PID_Coefs_DIFF[D] * error_dif;

			t_DIFF = p_DIFF+i_DIFF+d_DIFF;

			if ((CURRENT_SPEED + (int)t_DIFF) > 100 ) {
				i_DIFF = 0;
			}

			if ((CURRENT_SPEED + (int)t_DIFF) < -100 ) {
				i_DIFF = 0;
			}

			L_PWM = CURRENT_SPEED + (int)t_DIFF;
			R_PWM = CURRENT_SPEED - (int)t_DIFF;

			if(L_PWM < -100)	L_PWM = -100;
			if(L_PWM > 100)		L_PWM = 100;

			if(R_PWM < -100)	R_PWM = -100;
			if(R_PWM > 100)		R_PWM = 100;

			if (error == 0) {
				Set_Main_Motor(CURRENT_SPEED, CURRENT_SPEED);
				i_DIFF = 0;
			}else{
				Set_Main_Motor(L_PWM, R_PWM);
			}
		}
	}
}
