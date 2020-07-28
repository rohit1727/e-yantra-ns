/*
*Team Id: eYRC#263
*Author List: Rohit Vasant Patil, Brijesh Kumar Patra 
*Filename: Color Sensor Demo Code.c
*Theme: Nutty Squirrel
*Functions: rgb_port_config, color_sensor_pin_config, color_sensor_pin_interrupt_init, ISR, init_devices, filter_red, filter_green
*			filter_blue, , filter_clear, color_sensor_scaling, red_read, green_read, blue_read, main
*Global Variables: pulse, red, green, blue
*
*/

#define F_CPU 16000000UL//define F_CPU value
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h> //included to support power function

//You can declare here global variable as per your requirement
volatile unsigned long int pulse = 0; // pulse: to store the number of pulses generated by the color sensor.
									  // It is volatile because its value changes very frequently.
									  
volatile unsigned long int red;       // red: variable to store the pulses when red filter is selected
volatile unsigned long int blue;      // blue: variable to store the pulses when blue filter is selected
volatile unsigned long int green;     // green: variable to store the pulses when green filter is selected


/*
*Function Name: rgb_port_config
*Input: void
*Output: void
*Logic: defines output pins for RGB led on PORTH pins PH4, PH5, PH6
*		and makes all pins high to intially turn off the led
*Example Call: rgb_port_config()
*/
void rgb_port_config (void)
{
	/*****************************************
	Define DDR and PORT values for the port on which RGB LED is connected
	******************************************/
	DDRH = DDRH | 0x70; //01110000 setting PH4, PH5, PH6 as output for RGB led
	PORTH = PORTH | 0xFF; // initially all off
}

/*
*Function Name: color_sensor_pin_config
*Input: void
*Output: void
*Logic:	sets PORTD pin 0 i.e. PD0/INT0(external interrupt pin 0) as input for color sensor
*		and PORTB pins PB4, PB5, PB6, PB7 as output pins for S0, S1, S2, S3 for selecting different color filters and output frequency scaling
*Example Call: color_sensor_pin_config()
*/
void color_sensor_pin_config(void)
{
	/*****************************************
	Define DDR and PORT values for the port on which Color sensor is connected
	******************************************/
	DDRD = DDRD & 0x00;
	DDRD = DDRD | 0xFE; //11111110 //Set the direction of PORTD pin 0 as input
	PORTD = PORTD | 0x01; //enable internal pull-up for PORTD 0 pin
	
	DDRB = DDRB | 0xF0;		//set PORTB PB4,PB5,PB6,PB7 as output for S0,S1,S2,S3 of color sensor respectively
	PORTB = PORTB & 0x00;	//initially all low
}

/*
*Function Name: color_sensor_pin_interrupt_init
*Input: void
*Output: void
*Logic: sets external interrupt 0 to trigger with falling edge
		by setting bit 0 of External Interrupt Control Register A(EICRA) as 0 and bit 1 of EICRA as 1  
*Example Call: color_sensor_pin_interrupt_init()
*/
void color_sensor_pin_interrupt_init(void) //Interrupt 0 enable
{
	cli(); //Clears the global interrupt  
	EICRA = EICRA | 0x02; // INT0 is set to trigger with falling edge
	EIMSK = EIMSK | 0x01; // Enable Interrupt INT0 for color sensor
	sei(); // Enables the global interrupt
}

//ISR for color sensor

/*
*Function Name: ISR
*Input: INT0_vect
*Output: void
*Logic: This is the Interrupt Service Routine which increments pulse count by 1 each time it is called 
*Example Call: it is automatically called when an external interrupt is triggered on PD0/INT0
*/
ISR(INT0_vect) // Pass the timer number in place of n in INTn_vect
{
	//increment on receiving pulse from the color sensor
	pulse++;
}

/*
*Function Name: init_devices
*Input: void
*Output: void
*Logic: initialises all devices by calling respective functions
*Example Call: init_devices()
*/
void init_devices(void)
{
	cli(); //Clears the global interrupt
	//Initialize all the ports here
	rgb_port_config(); //RGB led pin configuration
	color_sensor_pin_config(); //color sensor pin configuration
	color_sensor_pin_interrupt_init();
	sei();   // Enables the global interrupt
}

//Filter Selection

/*
*Function Name: filter_red
*Input: void
*Output: void
*Logic: sets PB6 and PB7 to low for selecting red filter
*Example Call:filter_red()
*/
void filter_red(void)    //Used to select red filter
{
	//Filter Select - red filter
	 PORTB = PORTB & 0xBF;//set S2 low
	 PORTB = PORTB & 0x7F;//set S3 low
}

/*
*Function Name: filter_green
*Input: void
*Output: void
*Logic: sets PB6 and PB7 to high for selecting green filter
*Example Call: filter_green()
*/
void filter_green(void)	//Used to select green filter
{
	//Filter Select - green filter
	PORTB = PORTB | 0x40;//set S2 High
	PORTB = PORTB | 0x80;//set S3 High
}

/*
*Function Name: filter_blue
*Input: void
*Output: void
*Logic: sets PB6 to low and PB7 to high for selecting blue filter
*Example Call: filter_blue()
*/
void filter_blue(void)	//Used to select blue filter
{
	//Filter Select - blue filter
	PORTB = PORTB & 0xBF;//set S2 low
	PORTB = PORTB | 0x80;//set S3 High
}

/*
*Function Name: filter_clear
*Input: void
*Output: void
*Logic: sets PB6 to high and PB7 to low for selecting clear filter
*Example Call: filter_clear()
*/
void filter_clear(void)	//select no filter
{
	//Filter Select - no filter
	PORTB = PORTB | 0X40;//set S2 High
	PORTB = PORTB & 0x7F;//set S3 Low
}

//Color Sensing Scaling

/*
*Function Name: color_sensor_scaling
*Input: void
*Output: void
*Logic: sets PB4 and PB5 to high for output scaling 20%
*Example Call: color_sensor_scaling()
*/
void color_sensor_scaling()		//This function is used to select the scaled down version of the original frequency of the output generated by the color sensor, generally 20% scaling is preferable, though you can change the values as per your application by referring datasheet
{
	//Output Scaling 20% from datasheet
	
	PORTB = PORTB | 0x10;//set S0 high
	PORTB = PORTB | 0x20;//set S1 high
}

/*
*Function Name: red_read
*Input: void
*Output: void
*Logic: selects red filter using filter_red then captures pulses for 100ms and stores pulse count in variable red
*Example Call: red_read()
*/
void red_read(void)
{
	//Red
	filter_red(); //select red filter
	pulse = 0;//reset the count to 0
	_delay_ms(100); //capture the pulses for 100 ms or 0.1 second
	red = pulse; //store the count in variable called red
}

/*
*Function Name: green_read
*Input: void
*Output: void
*Logic: selects green filter using filter_green then captures pulses for 100ms and stores pulse count in variable green
*Example Call: green_read()
*/
void green_read(void) 
{
	//Green
	filter_green(); //select green filter
	pulse = 0; //reset the count to 0
	_delay_ms(100); //capture the pulses for 100 ms or 0.1 second
	green = pulse; //store the count in variable called green
}

/*
*Function Name: blue_read
*Input: void
*Output: void
*Logic: selects blue filter using filter_blue then captures pulses for 100ms and stores pulse count in variable blue
*Example Call: blue_read()
*/
void blue_read(void)
{
	//Blue
	filter_blue(); //select blue filter
	pulse=0; //reset the count to 0
	_delay_ms(100); //capture the pulses for 100 ms or 0.1 second
	blue = pulse;  //store the count in variable called blue
}

int main(void)
{
    init_devices();
	color_sensor_scaling();
	
	int black_color_threshold = 1500;
	
	while(1)
    {
      //Write your code here
	  red_read();	//read pulses after selecting red filter
	  green_read();	//read pulses after selecting green filter
	  blue_read();	//read pulses after selecting blue filter
	  
	  
	  //if all three pulse counts are less than the threshold value then color is black
	  if(red < black_color_threshold && green < black_color_threshold && blue < black_color_threshold)
	  {
		//black
		PORTH |= 0xFF; //first making all bits high to turn off led
		PORTH &= 0x8F; //glow white color on led by making PH4, PH5 and PH6 low
	  }
	  else
	  {
		  //if red pulse count is greater than green and blue
		  if(red > green && red > blue)
		  {
			//red-PH4;
			PORTH |= 0xFF;	//first making all bits high to turn off led
			PORTH &= 0xEF;	//making PH4 low for red color diode
		  }
		  //if green pulse count is greater than red and blue
		  else if(green > blue && green > red)
		  {
			//green-PH5;
			PORTH |= 0xFF;	//first making all bits high to turn off led
			PORTH &= 0xDF;	//making PH5 low for green color diode
		  }
		  //if blue pulse count is greater than green and red
		  else if(blue > red && blue > green)
		  {
			//blue-PH6;
			PORTH |= 0xFF;	//first making all bits high to turn off led
			PORTH &= 0xBF;	//making PH4 low for blue color diode
		  }
	  }
	}	  
    
}