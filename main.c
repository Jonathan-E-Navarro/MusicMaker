/*
 * Project3.c
 *
 * Created: 2/14/2018 12:55:04 PM
 * Author : Jonathan
 */ 

#include <avr/io.h>
#include "avr.c"
#include "lcd.c"
#include <stdio.h>
#include <math.h>


//holds frequencies for two octaves starting from A4
float note_array[] ={440,466.16,493.88,523.25,554.37,587.33,622.25,659.25,698.46,739.99,783.99,830.61,880.00,932.33,987.77,1046.50,1108.73,
1174.66,1244.51,1318.51,1396.91,1479.98,1567.98,1661.22,1760.00,10}; 

double dur_array[] = {1,0.5,0.25,0.125,0.0625};

//note places in frequency array 
#define A4 0
#define AS4 1
#define B4 2
#define C5 3
#define CS5 4
#define D5 5
#define DS5 6
#define E5 7
#define F5 8
#define FS5 9
#define G5 10
#define GS5 11
#define A5 12
#define AS5 13
#define B5 14
#define C6 15
#define CS6 16
#define D6 17
#define DS6 18
#define E6 19
#define F6 20
#define FS6 21
#define G6 22
#define GS6 23
#define A6 24
#define Rest 25

//note durations
#define W 0
#define H 1
#define Q 2
#define E 3
#define S 4

//Song: Iron Man by Black Sabbath 
//B--D--|DEE-|GF#GF#GF#DDEE

struct note 
{
    unsigned char frequency;
	unsigned char duration;
};

struct note Song [16] = {{B4,W},{D5,W},{D5,H},{E5,H},{E5,W},{Rest,S},{G5,Q},{FS5,Q},{G5,Q},{FS5,Q},{G5,Q},{FS5,Q},{D5,H},{D5,H},{E5,H},{E5,W}};
struct note Song2 [16] = {{C5,H},{G5,H},{F5,Q},{E5,Q},{D5,Q},{C6,H},{G5,H},{F5,Q},{E5,Q},{D5,Q},{C6,H},{G5,H},{F5,Q},{E5,Q},{F5,Q},{D5,Q}};

char temp[40];
void play_note(double freq, double dur){
	float period = (1/freq);
	int TH = period/2*20000; 
	int K = dur*freq;
	for(int i =0;i < K;i++){
		 SET_BIT(PORTB,PINB3);
		 wait_avr2(TH);
		 CLR_BIT(PORTB,PINB3);
		 wait_avr2(TH);
	}
}

void play_music(struct note song[],int N){
	for (int i =0; i < N;i++)
	{
		play_note(note_array[song[i].frequency],dur_array[song[i].duration]);
	}
}


int is_pressed(int r, int c)
{
	// DDRC = 0x00; //configuration of C input

	for(int i = 0; i <8; i++){
		CLR_BIT(DDRC,i);
	}

	//sets the column to be pulled as output
	SET_BIT(DDRC,r);

	//set z
	CLR_BIT(PORTC,PINC0);
	CLR_BIT(PORTC,PINC1);
	CLR_BIT(PORTC,PINC2);
	CLR_BIT(PORTC,PINC3);
	//set last four pins to z
	CLR_BIT(PORTC,PINC4);
	CLR_BIT(PORTC,PINC5);
	CLR_BIT(PORTC,PINC6);
	CLR_BIT(PORTC,PINC7);

	SET_BIT(PORTC,c+4);

	wait_avr(1);
	//check if button is pressed
	if(!GET_BIT(PINC, c+4)){
		return 1;                    //button is pressed
	}

	return 0;
}
int get_key(void){
	for(int r = 0; r < 4; r++)
	{
		for(int c = 0; c < 4; c++)
		{
			if(is_pressed(r,c))
			{
				return r*4+c+1;
			}
		}
	}
	return 0;
}
void poll_button(int *pause,int *play1, int *play2){
	int key = get_key();
	char buf[50];
	if(key == 1)
	{
		//pos_lcd(0,5);
		//put_lcd('0'+key);
		wait_avr(500);
		if(*pause == 1){
		    *pause = 0;
			clr_lcd();
			pos_lcd(0,0);
			puts_lcd2("playing!");
			pos_lcd(1,0);
			puts_lcd2("Song: Iron Man");
			*play1 = 1;
			*play2 = 0;
		}
	    else if(*pause == 0){
		    *pause = 1;
			clr_lcd();
			pos_lcd(0,0);
			puts_lcd2("Paused");
		}	
	}
	if(key == 2)
	{
		//pos_lcd(0,5);
		//put_lcd('0'+key);
		wait_avr(500);
		if(*pause == 1){
			*pause = 0;
			clr_lcd();
			pos_lcd(0,0);
			puts_lcd2("playing!");
			pos_lcd(1,0);
			puts_lcd2("Song: Star Wars");
			*play1 = 0;
			*play2 = 1;
		}
		else if(*pause == 0){
			*pause = 1;
			clr_lcd();
			pos_lcd(0,0);
			puts_lcd2("Paused");
		}
	}
}

int main(void)
{
	//set port A to be output
	DDRA = 0x01;   
	DDRB = 0x08;

	//initialize our LCD
	ini_lcd();
	//character buffer for writing to our LCD
	char buf[50];

	//place holder format
	puts_lcd2("Press to play:");

	int pause = 1;
	int play1 = 0;
	int play2 = 0;
	while(1){
	while(pause){
		poll_button(&pause,&play1,&play2);
	}

    while(!pause) 
    {
	 if(play1){
	     poll_button(&pause,&play1,&play2);
	     play_music(Song,16);
	     poll_button(&pause,&play1,&play2);
	 }	
	 else if(play2){
		 poll_button(&pause,&play1,&play2);
		 play_music(Song2,16);
		 poll_button(&pause,&play1,&play2);
	 }
	 


    }
	}
}

