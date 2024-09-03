#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

//#define KEY_SIGNATURE  KEY_SIGNATURE_C_SHARP  //: 调号
#define KEY_SIGNATURE  KEY_SIGNATURE_F 		//: 调号
//#define KEY_SIGNATURE  KEY_SIGNATURE_B_FLAT 	//: 调号

#define TEMPO 120	//节拍

#define DEAFULT_OCTAVE	4

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))

enum LEVEL {
	LEVEL_1,
	LEVEL_2,
	LEVEL_3,
	LEVEL_4,
	LEVEL_5,
	LEVEL_6,
	LEVEL_7,
};

struct level_step {
	int filed;
	int step;
};

static struct level_step step [] = {
	{LEVEL_1, 1},
	{LEVEL_2, 3},
	{LEVEL_3, 5},
	{LEVEL_4, 6},
	{LEVEL_5, 8},
	{LEVEL_6, 10},
	{LEVEL_7, 12},
};

struct key_step {
	int filed;
	char name[10];
	int level;
	int interval;
};

enum KEY_SIG {
	KEY_SIGNATURE_C_FLAT,
	KEY_SIGNATURE_C,
	KEY_SIGNATURE_C_SHARP,
	KEY_SIGNATURE_D_FLAT,
	KEY_SIGNATURE_D,
	KEY_SIGNATURE_D_SHARP,
	KEY_SIGNATURE_E_FLAT,
	KEY_SIGNATURE_E,
	KEY_SIGNATURE_F_FLAT,
	KEY_SIGNATURE_E_SHARP,
	KEY_SIGNATURE_F,
	KEY_SIGNATURE_F_SHARP,
	KEY_SIGNATURE_G_FLAT,
	KEY_SIGNATURE_G,
	KEY_SIGNATURE_G_SHARP,
	KEY_SIGNATURE_A_FLAT,
	KEY_SIGNATURE_A,
	KEY_SIGNATURE_A_SHARP,
	KEY_SIGNATURE_B_FLAT,
	KEY_SIGNATURE_B,
	KEY_SIGNATURE_B_SHARP,
};

static struct key_step key_symbol [] = {
	{ KEY_SIGNATURE_C_FLAT, "Cb" , LEVEL_1, -10},
	{ KEY_SIGNATURE_C, "C", LEVEL_1, -9 },
	{ KEY_SIGNATURE_C_SHARP, "C#" , LEVEL_1, -8},
	{ KEY_SIGNATURE_D_FLAT, "Db" , LEVEL_2, -8},
	{ KEY_SIGNATURE_D, "D" , LEVEL_2, -7},
	{ KEY_SIGNATURE_D_SHARP, "D#", LEVEL_2, -6 },
	{ KEY_SIGNATURE_E_FLAT, "Eb", LEVEL_3, -6 },
	{ KEY_SIGNATURE_E, "E", LEVEL_3, -5 },
	{ KEY_SIGNATURE_F_FLAT, "Fb", LEVEL_4, -5 },
	{ KEY_SIGNATURE_E_SHARP, "E#", LEVEL_3, -4 },
	{ KEY_SIGNATURE_F, "F", LEVEL_4, -4 },
	{ KEY_SIGNATURE_F_SHARP, "F#", LEVEL_4, -3 },
	{ KEY_SIGNATURE_G_FLAT, "Gb", LEVEL_5 , -3},
	{ KEY_SIGNATURE_G, "G", LEVEL_5 , -2},
	{ KEY_SIGNATURE_G_SHARP, "G#", LEVEL_5 , -1},
	{ KEY_SIGNATURE_A_FLAT, "Ab", LEVEL_6, -1 },
	{ KEY_SIGNATURE_A, "A", LEVEL_6, 0 },
	{ KEY_SIGNATURE_A_SHARP, "A#", LEVEL_6, 1 },
	{ KEY_SIGNATURE_B_FLAT, "Bb", LEVEL_7, 1 },
	{ KEY_SIGNATURE_B, "B", LEVEL_7, 2 },
	{ KEY_SIGNATURE_B_SHARP, "B#", LEVEL_7, 3 },
};

struct simple_char {
	char name[10];
	float hz;
	uint8_t key;	//high 4 is pitch, low 4 bit is key
};

void play_sound(float hz, float time)
{
	char command[256];

	sprintf(command, "sox -n output.wav synth %f sine %f", time, hz);
	printf("%s\n", command);
	system(command);

	sprintf(command, "aplay output.wav > /dev/null");
//	printf("%s\n", command);
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

/* 2: 符号
 * 16:duration: 时值
 * C4音高。
*/
#if 0
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
#else
static const struct simple_input input[] = {
	{ '3', 4, 4},
	{ '3', 8, 4},
	{ '2', 8, 4},//副点 ,时值是上一个音符时值的一半， 8* 2=16
	{ '3', 4, 4},
	{ '-', 0, 0},

	{ '3', 8, 4},
	{ '3', 16, 4},
	{ '5', 16, 4},
	{ '3', 8, 4},
	{ '2', 8, 4},
	{ '3', 4, 4},
	{ '-', 4, 0},

	{ '1', 4, 4},
	{ '1', 8, 4},
	{ '2', 8, 4},
	{ '3', 8, 4},
	{ '5', 16, 4},
	{ '3', 16, 4},
	{ '3', 4, 4},

	{ '2', 4, 4},
	{ '2', 8, 4},
	{ '1', 8, 4},
	{ '2', 4, 4},
	{ '-', 4, 0},
//	{ '.', 0, 0},//副点
	{ '3', 4, 4},
	{ '.', 0, 0},
	{ '5', 16, 4},
	{ '3', 16, 4},
	{ '6', 8, 0},
	{ '5', 4, 4},
	{ '.', 0, 0},

	{ '6', 8, 4},
	{ '5', 8, 0},
	{ '5', 8, 4},
	{ '3', 8, 4},
	{ '5', 4, 0},
	{ '.', 0, 0},
	{ '5', 8, 0},

	{ '3', 4, 4},
	{ '2', 8, 0},
	{ '3', 8, 4},
	{ '5', 4, 4},
	{ '3', 8, 4},
	{ '2', 8, 4},

	{ '2', 4, 4},
	{ '2', 8, 0},
	{ '1', 8, 4},
	{ '2', 4, 4},
};
#endif
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

static float get_time_of_note(const struct simple_input *input)
{
	const struct simple_input *next = input + 1;

	float time = 60.0 / TEMPO * 4.0 / input[0].duration;

	if (next->symbol == '.')
		time *= 1.5;
	else if (next->symbol == '-')
		time *= 2;
		
	printf("%s: time %f duration %d\n", __func__, time, input[0].duration);
	return time;
}

static void print_level()
{
	printf("LEVEL_1 %d\n", LEVEL_1);
	printf("LEVEL_6 %d\n", LEVEL_6);
}

static float get_freq_of_note(char *str) {
	int steps_from_a4;
	int octave;
	bool next = false;

    //int steps_from_a4[] = {-9, -8, -7, -6, -5, -4, -3, -2, -1, 0, 1, 2}; // C, C#/Db, D, D#/Eb, E, F, F#/Gb, G, G#/Ab, A, A#/Bb, B

	switch (str[0]) {
	case 'C':
		steps_from_a4 = -9;
		break;
	case 'D':
		steps_from_a4 = -7;
		break;
	case 'E':
		steps_from_a4 = -5;
		break;
	case 'F':
		steps_from_a4 = -4;
		break;
	case 'G':
		steps_from_a4 = -2;
		break;
	case 'A':
		steps_from_a4 = 0;
		break;
	case 'B':
		steps_from_a4 = 2;
		break;
	default:
		printf("Err: unsupport str %s\n", str);
		return -1;
		break;
	}

	do {
		switch (str[1 + !!next]) {
		case '#':
			steps_from_a4++;
			next = true;
			break;
		case 'b':
			steps_from_a4--;
			next = true;
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
			if (str[2 + !!next] != '\0') {
				printf("Err: unsupport str %s\n", str);
				return -1;
			}
			octave = atoi(&str[1 + !!next]);
			next = false;
			break;
		default:
			printf("Err: unsupport str %s\n", str);
			return -1;
		}
	
	} while (next);

#define REFERENCE_FREQ 440.0 // A4的频率
#define OCTAVE_RATIO 1.05946 // 两个半音之间的频率比
	// 计算总的半音步数
	int total_steps = steps_from_a4 + (octave - 4) * 12;
	// 计算频率
	double frequency;
	frequency = REFERENCE_FREQ * pow(OCTAVE_RATIO, total_steps);
	printf("steps_from_a4: %d, octave: %d, frequency: %f\n", steps_from_a4, octave, frequency);
	return frequency;
}

void get_chord(char *note) 
{
	
	int level, interval;
	int i;
	for (i = 0; i < ARRAY_SIZE(key_symbol); i++) {
		if (strcmp(note, key_symbol[i].name) == 0) {
			level = key_symbol[i].level;
			interval = key_symbol[i].interval;
			printf("key_symbol[i].name %s\n", key_symbol[i].name);
			level += 2;
			level %= 7;
			interval += 4;
			if (interval > 2)
				interval -= 12;
			for (i = 0; i < ARRAY_SIZE(key_symbol); i++) {
				if (key_symbol[i].level == level && key_symbol[i].interval == interval)
					printf("key_symbol[i].name %s\n", key_symbol[i].name);
			}
			level += 2;
			level %= 7;
			interval += 3;
			if (interval > 2)
				interval -= 12;
			for (i = 0; i < ARRAY_SIZE(key_symbol); i++) {
				if (key_symbol[i].level == level && key_symbol[i].interval == interval)
					printf("key_symbol[i].name %s\n", key_symbol[i].name);
			}
			break;
		}
	}



}
static char *get_symbol_by_note(const char *note, int *octave)
{

}

static char *get_note_by_symbol(const char *str, int *octave)
{
	char input = str[0];
	switch (input) {
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
		break;
	default:
		//printf("INFO: unsupport str %s\n", str);
		return NULL;
	}
	int level = atoi(&input);

	*octave = DEAFULT_OCTAVE;
	int new_level = key_symbol[KEY_SIGNATURE].level + level - 1;
	new_level %= 7;
	int interval = key_symbol[KEY_SIGNATURE].interval + step[level - 1].step - step[LEVEL_1].step;
	if (interval > 2) {
		interval -= 12;
		(*octave)++;

	}
//	printf("new_level %d\n", new_level);
//	printf("interval %d\n", interval);

	int i;
	char *note = NULL;
	for (i = 0; i < ARRAY_SIZE(key_symbol); i++) {
		if (key_symbol[i].interval == interval && key_symbol[i].level == new_level)
			note = key_symbol[i].name;
	}

	if (!note) {
		printf("Err: no key symbol is matched.\n");
		return NULL;
	} else {
		get_chord(note);
	
	}
	return note;
}

int main(int argc, char **argv) {
	int i, j;

	// 低音5（E4）的频率大约是330 Hz
	// 持续时间为3秒
//	float hz = 330;
	float time;

	uint8_t key;


#if 0
	for (i = 0; i < ARRAY_SIZE(input); i++) {
		printf("symbol %c\n", input[i].symbol);
		key = get_roll_call_by_simple(input[i].symbol, input[i].pitch);
		if (key < 0)
			continue;
		//time = 4.0 / input[i].duration;
		time = get_time_of_note(&input[i]);
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

	for (i = 0; i < ARRAY_SIZE(input); i++) {
		const char *str = &input[i].symbol;
		if (strlen(str) > 1) {
			printf("Err len %ld\n", strlen(str));
			return -1;
		}

		int octave;
		char *note = get_note_by_symbol(str, &octave);
		if (!note)
			continue;
		char tmp[20];
		sprintf(tmp, "%s%d", note, octave);
		printf("%s's note is: %s\n", str, tmp);

		float frequency = get_freq_of_note(tmp);
		if (frequency < 0)
			continue;
		time = get_time_of_note(&input[i]);
		play_sound(frequency, time);
	}

	return 0;
}

struct Triad {
	int root;
	int third;
	int fifth;
};

struct Triad triad [] = {
	{ 1,3,5 },
	{ 2,4,6 },
	{ 3,5,7 },
	{ 4,6,1 },
	{ 5,7,2 },
	{ 6,1,3 },
	{ 7,2,4 },
};
