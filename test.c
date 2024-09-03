#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

enum LEVEL {
	LEVEL_1,
	LEVEL_1_SHARP, LEVEL_2_FLAT = LEVEL_1_SHARP,
	LEVEL_2,
	LEVEL_2_SHARP, LEVEL_3_FLAT = LEVEL_2_SHARP,
	LEVEL_3,
	LEVEL_4,
	LEVEL_4_SHARP, LEVEL_5_FLAT = LEVEL_4_SHARP,
	LEVEL_5,
	LEVEL_5_SHARP, LEVEL_6_FLAT = LEVEL_5_SHARP,
	LEVEL_6,
	LEVEL_6_SHARP, LEVEL_7_FLAT = LEVEL_6_SHARP,
	LEVEL_7,
};

enum KEY_SIGNATURE {
	KEY_SIGNATURE_C,
	KEY_SIGNATURE_C_SHARP, KEY_SIGNATURE_D_FLAT = KEY_SIGNATURE_C_SHARP,
	KEY_SIGNATURE_D,
	KEY_SIGNATURE_D_SHARP, KEY_SIGNATURE_E_FLAT = KEY_SIGNATURE_D_SHARP,
	KEY_SIGNATURE_E,
	KEY_SIGNATURE_F,
	KEY_SIGNATURE_F_SHARP, KEY_SIGNATURE_G_FLAT = KEY_SIGNATURE_F_SHARP,
	KEY_SIGNATURE_G,
	KEY_SIGNATURE_G_SHARP, KEY_SIGNATURE_A_FLAT = KEY_SIGNATURE_G_SHARP,
	KEY_SIGNATURE_A,
	KEY_SIGNATURE_A_SHARP, KEY_SIGNATURE_B_FLAT = KEY_SIGNATURE_A_SHARP,
	KEY_SIGNATURE_B,
};


static void print_level()
{
	printf("LEVEL_1 %d\n", LEVEL_1);
	printf("LEVEL_6 %d\n", LEVEL_6);
	printf("LEVEL_6_SHARP %d\n", LEVEL_6_SHARP);
	printf("LEVEL_7_FLAT %d\n", LEVEL_7_FLAT);
}


static int get_notes(char *str) {
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

	printf("steps_from_a4 %d\n", steps_from_a4);
	printf("octave %d\n", octave);
#define REFERENCE_FREQ 440.0 // A4的频率
#define OCTAVE_RATIO 1.05946 // 两个半音之间的频率比
	// 计算总的半音步数
	int total_steps = steps_from_a4 + (octave - 4) * 12;
	// 计算频率
	double frequency;
	frequency = REFERENCE_FREQ * pow(OCTAVE_RATIO, total_steps);
	printf("frequency %f\n", frequency);
}

int main(int argc, char **argv)
{
	char *str = argv[1];

	get_notes(str);
}
