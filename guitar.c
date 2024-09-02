#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

struct simple_char {
	char name[10];
	float hz;
	uint8_t key;	//high 4 is pitch, low 4 bit is key
};

struct simple_char Pitch_Name[] = {
	{ "C0", 16.35, 0x01},
	{ "D0", 18.35, 0x02},
	{ "E0", 20.60, 0x03},
	{ "F0", 21.83, 0x04},
	{ "G0", 24.50, 0x05},
	{ "A0", 27.50, 0x06},
	{ "B0", 30.87, 0x07},
	{ "C1", 32.70, 0x11},
	{ "D1", 36.71, 0x12},
	{ "E1", 41.20, 0x13},
	{ "F1", 43.65, 0x14},
	{ "G1", 49.00, 0x15},
	{ "A1", 55.00, 0x16},
	{ "B1", 61.74, 0x17},
	{ "C2", 65.41, 0x21},
	{ "D2", 73.42, 0x22},
	{ "E2", 82.41, 0x23},
	{ "F2", 87.31, 0x24},
	{ "G2", 98.00, 0x25},
	{ "A2", 110.00, 0x26},
	{ "B2", 123.47, 0x27},
	{ "C3", 130.81, 0x31},
	{ "D3", 146.83, 0x32},
	{ "E3", 164.81, 0x33},
	{ "F3", 174.61, 0x34},
	{ "G3", 196.00, 0x35},
	{ "A3", 220.00, 0x36},
	{ "B3", 246.94, 0x37},
	{ "C4", 261.63, 0x41},
	{ "D4", 293.66, 0x42},
	{ "E4", 329.63, 0x43},
	{ "F4", 349.23, 0x44},
	{ "G4", 392.00, 0x45},
	{ "A4", 440.00, 0x46},
	{ "B4", 493.88, 0x47},
	{ "C5", 523.25, 0x51},
	{ "D5", 587.33, 0x52},
	{ "E5", 659.26, 0x53},
	{ "F5", 698.46, 0x54},
	{ "G5", 783.99, 0x55},
	{ "A5", 880.00, 0x56},
	{ "B5", 987.77, 0x57},
	{ "C6", 1046.50, 0x61},
	{ "D6", 1174.66, 0x62},
	{ "E6", 1318.51, 0x63},
	{ "F6", 1396.91, 0x64},
	{ "G6", 1567.98, 0x65},
	{ "A6", 1760.00, 0x66},
	{ "B6", 1975.53, 0x67},
	{ "C7", 2093.00, 0x71},
	{ "D7", 2349.32, 0x72},
	{ "E7", 2637.02, 0x73},
	{ "F7", 2793.83, 0x74},
	{ "G7", 3135.96, 0x75},
	{ "A7", 3520.00, 0x76},
	{ "B7", 3951.07, 0x77},
};

void play_sound(float hz, float time)
{
	char command[256];

	sprintf(command, "sox -n output.wav synth %f sine %f", time, hz);
	printf("%s\n", command);
	system(command);

	sprintf(command, "aplay output.wav");
	printf("%s\n", command);
	system(command);
}

#if 0
//duration: 时值
enum {
	

};
#endif
struct simple_input {
	char symbol;
	int duration;
	int pitch;//音高
};

enum key_signature {
	KEY_SIGNATURE_C,
	KEY_SIGNATURE_D,
	KEY_SIGNATURE_E,
	KEY_SIGNATURE_F,
	KEY_SIGNATURE_G,
	KEY_SIGNATURE_A,
	KEY_SIGNATURE_B,
};

#define KEY_SIGNATURE  KEY_SIGNATURE_A  //: 调号
/* 2: 符号
 * 16:duration: 时值
 * C4音高。
*/
static const struct simple_input input[] = {
	{ '2', 16, 4},
	{ '3', 8, 4},
	{ '.', 8, 0},//副点 ,时值是上一个音符时值的一半， 8* 2=16
	{ '5', 16, 4},
	{ '3', 16, 4},
	{ '2', 16, 4},
	{ '1', 16, 4},
	{ '2', 16, 4},
	{ '3', 8, 4},
	{ '.', 8, 0},//副点
	{ '3', 8, 4},
	{ '2', 16, 4},
	{ '1', 16, 4},
};

static uint8_t get_roll_call_by_simple(char symbol, int pitch)
{
	uint8_t key;

	switch (symbol) {
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
		key = atoi(&symbol);
		key |= (0x10 * pitch) & 0xF0;

		key += KEY_SIGNATURE;

		if ((key & 0xF) > 7) {
			key += 0x10;
			key -= 7;
		}
		printf("%s: key 0x%02x\n",  __func__, key);
		break;
	default:
		printf("Unsupport key %c\n", symbol);
		return -1;
	}

	

	return key;
}

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))

int main() {
	int i, j;

	// 低音5（E4）的频率大约是330 Hz
	// 持续时间为3秒
//	float hz = 330;
	float time;

	uint8_t key;
	key = get_roll_call_by_simple('1', 4);

	


#if 1
	for (i = 0; i < ARRAY_SIZE(input); i++) {
		printf("symbol %c\n", input[i].symbol);
		key = get_roll_call_by_simple(input[i].symbol, input[i].pitch);
		if (key < 0)
			continue;
		time = 4.0 / input[i].duration;
#if 1
	for (j = 0; j < ARRAY_SIZE(Pitch_Name); j++) {
		if (key == Pitch_Name[j].key) {
			printf("%s: ", Pitch_Name[j].name);
			play_sound(Pitch_Name[j].hz, time);
//			printf("0x%x\n", Pitch_Name[j].key);
		}
	}
#endif
//		time = 1 * input[i].duration;
//		play_sound(Pitch_Name[i].hz, time);
//		printf("0x%x\n", Pitch_Name[i].key);
	}
#endif
	

	return 0;
}
