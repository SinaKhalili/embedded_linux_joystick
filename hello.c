#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>

#define UP 0
#define DOWN 3
#define OFF "0"
#define ON "1"

#define TRIGGER_FILE_LED_UP "/sys/class/leds/beaglebone:green:usr0/trigger"
#define TRIGGER_FILE_LED_DOWN "/sys/class/leds/beaglebone:green:usr3/trigger"
#define BRIGHTNESS_FILE_LED_UP "/sys/class/leds/beaglebone:green:usr0/brightness"
#define BRIGHTNESS_FILE_LED_DOWN "/sys/class/leds/beaglebone:green:usr3/brightness"

#define EXPORTS_FILE "/sys/class/gpio/export"

#define JSUP_FILE "/sys/class/gpio/gpio26/value"
#define JSRT_FILE "/sys/class/gpio/gpio47/value"
#define JSDN_FILE "/sys/class/gpio/gpio46/value"
#define JSLFT_FILE "/sys/class/gpio/gpio65/value"

#define JSUP "26"
#define JSRT "47"
#define JSDN "46"
#define JSLFT "65"

void writeToFile(FILE *fp, const char* string){
	int charWritten = fprintf(fp, "%s", string );
	if(charWritten <= 0){
		printf("ERROR writing %s to file \n", string);
	}
}
void openAFileWrite(const char* name, const char* string){
	FILE *fp = fopen(name, "w");
	if(fp == NULL){
		printf("Error opening file %s\n", name);
		exit(-1);
	}
	writeToFile(fp, string);
	fclose(fp);
}
FILE* openAFileRead(const char* name){
	FILE *fp = fopen(name, "r");
	if(fp == NULL){
		printf("Error opening file %s\n", name);
		exit(-1);
	}
	return fp;
}
void initializeExports(const char* name){
	FILE *fp = fopen(name, "w");
	if(fp == NULL){
		printf("Error opening file %s\n", name);
		exit(-1);
	}
	writeToFile(fp, JSUP);
	writeToFile(fp, JSDN);
	writeToFile(fp, JSLFT);
	writeToFile(fp, JSRT);
	fclose(fp);
}
int getNewTarget(){
	srand(time(NULL));
	int r = rand();
	int target = r%2 == 0? UP : DOWN;
	return target;
}
void showScore(int score, int numberquestions){
	printf("Your score is now %d / %d \n", score, numberquestions);
}
void initializeLedBrightness(const char* name1, const char* name2){
	openAFileWrite(name1, OFF);
	openAFileWrite(name2, OFF);
}

int joyStickReleaseBusyWait(){
	FILE *joyUp = openAFileRead(JSUP_FILE);
	FILE *joyDn = openAFileRead(JSDN_FILE);
	FILE *joyLf = openAFileRead(JSLFT_FILE);
	FILE *joyRt = openAFileRead(JSRT_FILE);
	const int max_length = 1024;
	char buff1[max_length];
	char buff2[max_length];
	char buff3[max_length];
	char buff4[max_length];
	int up = atoi(fgets(buff1, max_length, joyUp));
	int dn = atoi(fgets(buff2, max_length, joyDn));
	int lf = atoi(fgets(buff3, max_length, joyLf));
	int rt = atoi(fgets(buff4, max_length, joyRt));
	fclose(joyUp);
	fclose(joyDn);
	fclose(joyLf);
	fclose(joyRt);
	if(up == 1	&&
		 dn == 1	&&
		 lf == 1	&&
		 rt == 1	){
			 return 0;
		 }
	return 1;
}
int joyStickBusyWait(int *userInput){
	FILE *joyUp = openAFileRead(JSUP_FILE);
	FILE *joyDn = openAFileRead(JSDN_FILE);
	FILE *joyLf = openAFileRead(JSLFT_FILE);
	FILE *joyRt = openAFileRead(JSRT_FILE);
	const int max_length = 1024;
	char buff1[max_length];
	char buff2[max_length];
	char buff3[max_length];
	char buff4[max_length];
	int up = atoi(fgets(buff1, max_length, joyUp));
	int dn = atoi(fgets(buff2, max_length, joyDn));
	int lf = atoi(fgets(buff3, max_length, joyLf));
	int rt = atoi(fgets(buff4, max_length, joyRt));
	fclose(joyUp);
	fclose(joyDn);
	fclose(joyLf);
	fclose(joyRt);
	if(up != 1){
		*userInput = UP;
		return 0;
	}
	if(dn != 1){
		*userInput = DOWN;
		return 0;
	}
	if(lf != 1){
		*userInput = -1;
		return 0;
	}
	if(rt != 1){
		*userInput = -1;
		return 0;
	}
	return 1;
}
int main(){

	printf("Hello embedded world from Sina Khalili!\n");
	printf(" 'Greatest innovation since playstation' - IGN\n");
	printf(" 'iNCREDIBLE' - Bill Gates\n");
	printf(" 'So good it's almost like they had instructions' - God\n");
	printf(" RULES: Press the joystick in the direction of the LED\n");
	printf(" UP for led 0 and DOWN for led 3. Left/Right to exit!\n");


	//Initialze score, question count, and user option
	int score = 0;
	int numberquestions = 0;
	int userInput = -1;
	//Initialze the pointers to the LED
	openAFileWrite(TRIGGER_FILE_LED_UP , "none");
	openAFileWrite(TRIGGER_FILE_LED_DOWN,"none");
	// initializeLedTriggers(pLedTriggerUp, pLedTriggerDown);
	//Make their triggers none
	//Grab their brightness controllers
	openAFileWrite(BRIGHTNESS_FILE_LED_UP, OFF);
	openAFileWrite(BRIGHTNESS_FILE_LED_DOWN, OFF);
	// initializeLedBrightness(pLedBrightnessUp, pLedBrightnessDown);
	// writeToFile(pLedBrightnessUp, ON);
	// writeToFile(pLedBrightnessDown, OFF);
	//Initialze leds
	//Export the pins
	initializeExports(EXPORTS_FILE);
	//Get new target
	int target = getNewTarget();
	const char* targetFileInit = target == 0? BRIGHTNESS_FILE_LED_UP : BRIGHTNESS_FILE_LED_DOWN;
	openAFileWrite(targetFileInit, ON);

	while(1){
		showScore(score, numberquestions);
		target = getNewTarget();
		initializeLedBrightness(BRIGHTNESS_FILE_LED_UP, BRIGHTNESS_FILE_LED_DOWN);
		const char* targetFile = target == 0? BRIGHTNESS_FILE_LED_UP : BRIGHTNESS_FILE_LED_DOWN;
		openAFileWrite(targetFile, ON);

		while(joyStickBusyWait(&userInput))
			;
		if(userInput == target){
			printf("Correct! Are you a genius?!? \n");
			openAFileWrite(BRIGHTNESS_FILE_LED_UP, ON);
			openAFileWrite(BRIGHTNESS_FILE_LED_DOWN, ON);
			sleep(0.1);
			initializeLedBrightness(BRIGHTNESS_FILE_LED_UP, BRIGHTNESS_FILE_LED_DOWN);
			score++;
		} else if(userInput != -1){
			printf("Incorrect, you absolute buffoon! \n");
			for(int i = 0; i < 5; i++){
				openAFileWrite(BRIGHTNESS_FILE_LED_UP, ON);
				openAFileWrite(BRIGHTNESS_FILE_LED_DOWN, ON);
				sleep(0.1);
				initializeLedBrightness(BRIGHTNESS_FILE_LED_UP, BRIGHTNESS_FILE_LED_DOWN);
				sleep(0.1);
			}
		}
		while(joyStickReleaseBusyWait())
			;



		if(userInput != UP && userInput != DOWN){
			break;
		}
		numberquestions++;
	}
	printf("You final score %d / %d \n",score, numberquestions);
	printf("Thank you for playing! \n");
	initializeLedBrightness(BRIGHTNESS_FILE_LED_UP, BRIGHTNESS_FILE_LED_DOWN);
}














//hi
