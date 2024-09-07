#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "guitar.h"

//#define KEY_SIGNATURE  KEY_SIGNATURE_C_SHARP  //: 调号
#define KEY_SIGNATURE  KEY_SIGNATURE_F 		//: 调号
//#define KEY_SIGNATURE  KEY_SIGNATURE_B_FLAT 	//: 调号

#define TEMPO 120	//节拍

#define DEAFULT_OCTAVE	4

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))

static struct chord_stages stage[] = {
	{LEVEL_1, 0},
	{LEVEL_2, 2},
	{LEVEL_3, 4},
	{LEVEL_4, 5},
	{LEVEL_5, 7},
	{LEVEL_6, 9},
	{LEVEL_7, 11},
};

static struct simple_note note[] = {
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

static char *get_symbol_by_note(const char *note, int *octave)
{

	return NULL;
}

static int get_level_and_steps_by_symbol(const char *str)
{
	int level = -1;

	switch (*str) {
	case '1':
		level = LEVEL_1;
		break;
	case '2':
		level = LEVEL_2;
		break;
	case '3':
		level = LEVEL_3;
		break;
	case '4':
		level = LEVEL_4;
		break;
	case '5':
		level = LEVEL_5;
		break;
	case '6':
		level = LEVEL_6;
		break;
	case '7':
		level = LEVEL_7;
		break;
	default:
		printf("Unknown symbol %c\n", *str);
		break;
	}
	return level;
}

static int get_input_from_file(struct simple_table *table)
{
    FILE *file;
    char filename[] = "song_juhuatai";

	memset(table, 0, sizeof(struct simple_table));

    // 打开文件
    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    // 读取文件直到末尾或数组满
    while (table->len < MAX_INPUT_SIZE && !feof(file)) {
        if (fscanf(file, "%c %d %d\n", &table->input[table->len].symbol, &table->input[table->len].duration, &table->input[table->len].pitch) == 3) {
            // 成功读取一行
            table->len++;
        } else {
            // 读取失败或到达文件末尾
            break;
        }
    }

    // 关闭文件
    fclose(file);

    // 打印读取的数据
    printf("Read %d entries from file.\n", table->len);
    for (int j = 0; j < table->len; j++) {
        printf("%c %d %d\n", table->input[j].symbol, table->input[j].duration, table->input[j].pitch);
    }

    return EXIT_SUCCESS;
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

static char *get_target_note_by_note(const char *note_name, int level, int steps_up_down)
{
	int i;
	int interval;
	int cur_level, target_level;
	char *target_note;

	for(i = 0; i < ARRAY_SIZE(note); i++) {
		if (strcmp(note_name, note[i].name) == 0) {
			cur_level = note[i].level;
			target_level = cur_level + level - LEVEL_1;
			target_level %= 7;
			interval = note[i].steps_from_a4 + stage[target_level].step + steps_up_down - stage[cur_level].step;
			if (interval > 3) {
				interval -= 12;
			}
			if (interval < -10) {
				interval += 12;
			}
			break;
		}
	}
	for (i = 0; i < ARRAY_SIZE(note); i++) {
		if (note[i].steps_from_a4 == interval && note[i].level == target_level)
			target_note = note[i].name;
	}

	if (!target_note) {
		printf("Err: no key symbol is matched.\n");
		return NULL;
	}
	return target_note;
}

static char *get_third_note_by_note(const char *note_name)
{
	char *target_note;

	target_note = get_target_note_by_note(note_name, LEVEL_3, 0);
	if (!target_note) {
		printf("Err: no key symbol is matched.\n");
		return NULL;
	}
	return target_note;
}

static char *get_fifth_note_by_note(const char *note_name)
{
	char *target_note;

	target_note = get_target_note_by_note(note_name, LEVEL_5, 0);
	if (!target_note) {
		printf("Err: no key symbol is matched.\n");
		return NULL;
	}
	return target_note;
}

static char *get_seventh_note_by_note(const char *note_name)
{
	char *target_note;

	target_note = get_target_note_by_note(note_name, LEVEL_7, 0);
	if (!target_note) {
		printf("Err: no key symbol is matched.\n");
		return NULL;
	}
	return target_note;
}

void get_chord(char *note_name) 
{
	printf("%s %s %s\n", note_name, get_third_note_by_note(note_name), get_fifth_note_by_note(note_name));
}

static char *get_note_by_level(const int level)
{
	switch (level) {
	case LEVEL_1:
	case LEVEL_2:
	case LEVEL_3:
	case LEVEL_4:
	case LEVEL_5:
	case LEVEL_6:
	case LEVEL_7:
		break;
	default:
		//printf("INFO: unsupport str %s\n", str);
		return NULL;
	}

	int new_level = note[KEY_SIGNATURE].level + level;
	new_level %= 6;
	int interval = note[KEY_SIGNATURE].steps_from_a4 + stage[level].step - stage[LEVEL_1].step;
	if (interval > 3) {
		interval -= 12;
	}
//	printf("new_level %d\n", new_level);
//	printf("interval %d\n", interval);

	int i;
	char *note_name = NULL;
	for (i = 0; i < ARRAY_SIZE(note); i++) {
		if (note[i].steps_from_a4 == interval && note[i].level == new_level)
			note_name = note[i].name;
	}

	if (!note_name) {
		printf("Err: no key symbol is matched.\n");
		return NULL;
	} else {
		get_chord(note_name);
	}
	return note_name;
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
	int level = atoi(&input) - 1;

	*octave = DEAFULT_OCTAVE;
	int note_level = note[KEY_SIGNATURE].level + level - LEVEL_1;
	note_level %= 7;
	int interval = note[KEY_SIGNATURE].steps_from_a4 + stage[level].step - stage[LEVEL_1].step;
	if (interval > 3) {
		interval -= 12;
		(*octave)++;
	}
//	printf("new_level %d\n", new_level);
//	printf("interval %d\n", interval);

	int i;
	char *note_name = NULL;
	for (i = 0; i < ARRAY_SIZE(note); i++) {
		if (note[i].steps_from_a4 == interval && note[i].level == note_level)
			note_name = note[i].name;
	}

	if (!note_name) {
		printf("Err: no key symbol is matched.\n");
		return NULL;
	} else {
		get_chord(note_name);
	
	}
	return note_name;
}


int main(int argc, char **argv) {
	int i;


	struct simple_table table;

	int ret = get_input_from_file(&table);
	if (ret < 0)
		return -1;
	float time;

#if 0
	uint8_t key;
	for (i = 0; i < ARRAY_SIZE(input); i++) {
		printf("symbol %c\n", input[i].symbol);
		key = get_roll_call_by_simple(input[i].symbol, input[i].pitch);
		if (key < 0)
			continue;
		//time = 4.0 / input[i].duration;
		time = get_time_of_note(&input[i]);
#if 1
	for (int j = 0; j < ARRAY_SIZE(Pitch_Name); j++) {
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

#if 1
	for (i = 0; i < table.len; i++) {
		const char *str = &table.input[i].symbol;
		if (strlen(str) > 1) {
			printf("Err len %ld\n", strlen(str));
			return -1;
		}

		int octave;
		char *note = get_note_by_symbol(str, &octave);
		if (!note) {
			printf("symbol '%s' continue\n", str);
			continue;
		}
		char tmp[20];
		sprintf(tmp, "%s%d", note, octave);
		printf("%s's note is: %s\n", str, tmp);

		float frequency = get_freq_of_note(tmp);
		if (frequency < 0)
			continue;
		time = get_time_of_note(&table.input[i]);
		play_sound(frequency, time);
	}
#endif
	for (i = 0; i < table.len; i++) {
		const char *str = &table.input[i].symbol;
		if (strlen(str) > 1) {
			printf("Err len %ld\n", strlen(str));
			return -1;
		}
	}
	return 0;
}

