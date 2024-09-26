#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_INPUT_SIZE 1024

enum LEVEL {
	LEVEL_1,
	LEVEL_2,
	LEVEL_3,
	LEVEL_4,
	LEVEL_5,
	LEVEL_6,
	LEVEL_7,
};
struct simple_note {
	int key;
	char name[10];
	int level;
	int steps_from_a4;
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
	KEY_SIGNATURE_NUM,
};
static struct simple_note note[] = {
	{ KEY_SIGNATURE_C_FLAT, "Cb", LEVEL_1, 0 },
	{ KEY_SIGNATURE_C, "C", LEVEL_1, 1 },
	{ KEY_SIGNATURE_C_SHARP, "C#", LEVEL_1, 2 },
	{ KEY_SIGNATURE_D_FLAT, "Db", LEVEL_2, 2 },
	{ KEY_SIGNATURE_D, "D", LEVEL_2, 3 },
	{ KEY_SIGNATURE_D_SHARP, "D#", LEVEL_2, 4 },	//没有D#调，因为D#
//      E#后面是F##，没有这个音.
	{ KEY_SIGNATURE_E_FLAT, "Eb", LEVEL_3, 4 },
	{ KEY_SIGNATURE_E, "E", LEVEL_3, 5 },
	{ KEY_SIGNATURE_F_FLAT, "Fb", LEVEL_4, 5 },
	{ KEY_SIGNATURE_E_SHARP, "E#", LEVEL_3, 6 },
	{ KEY_SIGNATURE_F, "F", LEVEL_4, 6 },
	{ KEY_SIGNATURE_F_SHARP, "F#", LEVEL_4, 7 },
	{ KEY_SIGNATURE_G_FLAT, "Gb", LEVEL_5, 7 },
	{ KEY_SIGNATURE_G, "G", LEVEL_5, 8 },
	{ KEY_SIGNATURE_G_SHARP, "G#", LEVEL_5, 9 },
	{ KEY_SIGNATURE_A_FLAT, "Ab", LEVEL_6, 9 },
	{ KEY_SIGNATURE_A, "A", LEVEL_6, 10 },
	{ KEY_SIGNATURE_A_SHARP, "A#", LEVEL_6, 11 },
	{ KEY_SIGNATURE_B_FLAT, "Bb", LEVEL_7, 11 },
	{ KEY_SIGNATURE_B, "B", LEVEL_7, 0 },
	{ KEY_SIGNATURE_B_SHARP, "B#", LEVEL_7, 1 },
};

struct simple_input {
	int symbol;
	int duration;
	int pitch;		// 音高
};

struct simple_table {
	struct simple_input input[MAX_INPUT_SIZE];
	int len;		// input len
	int key_signature;
	int duration;
	int beat;
};

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))
static int find_key_signature(char *value)
{
	for (int i = 0; i < ARRAY_SIZE(note); i++) {
		if (strcmp(value, note[i].name) == 0) {
			return note[i].key;
		}
	}
	return -1;
}

int get_key_signature(const char *filename, int *key_signature)
{
	char line[256];
	char value[256];

	FILE *fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("Error opening file");
		return -1;
	}
	// key signature
	while (fgets(line, sizeof(line), fp) != NULL) {
		if (strncmp(line, "key_signature=", strlen("key_signature=")) ==
		    0) {
			int result = sscanf(line, "key_signature=%s", value);
			//              printf("result = %d\n", result);
			if (result != 1) {
				fprintf(stderr,
					"Error reading key signature\n");
				fclose(fp);
				return -1;
			}
		}
	}
	*key_signature = find_key_signature(value);
	if (*key_signature == -1) {
		fprintf(stderr, "Error finding key signature\n");
		fclose(fp);
		return -1;
	}
	fclose(fp);
	return 0;
}

int get_duration_beat(const char *filename, int *duration, int *beat)
{
	char line[256];

	FILE *fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("Error opening file");
		return -1;
	}
	// 获取 duration 和 beat
	while (fgets(line, sizeof(line), fp) != NULL) {
		if (strncmp(line, "duration/beat=", strlen("duration/beat=")) ==
		    0) {
			int result =
			    sscanf(line, "duration/beat=%d/%d", duration,
				   beat);
			if (result != 2) {
				fprintf(stderr,
					"Error reading duration and beat\n");
				fclose(fp);
				return -1;
			}
		}
	}

	fclose(fp);
	return 0;
}

int get_input(const char *filename, struct simple_input *input, int *input_len)
{
	char line[256];
	int count = 0;

	FILE *fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("Error opening file");
		return -1;
	}

	while (fgets(line, sizeof(line), fp) != NULL) {
		int result = sscanf(line, "%d %d %d", &input[count].symbol,
				    &input[count].duration,
				    &input[count].pitch);
		if (result == 3) {
			count++;
		} else {
			//              fprintf(stderr, "Failed to parse line: %s, count %d\n",
			//                      line, count);
			continue;	// 或者处理错误
		}
	}

	*input_len = count;
	fclose(fp);
	return 0;
}

int parse_table(const char *filename, struct simple_table *table)
{
	char value[256];
	char line[256];
	int ret;

	ret = get_key_signature(filename, &table->key_signature);
	if (ret)
		return ret;
	ret = get_duration_beat(filename, &table->duration, &table->beat);
	if (ret)
		return ret;
	ret = get_input(filename, table->input, &table->len);
	if (ret)
		return ret;

	return 0;
}

int main(int argc, char **argv)
{
	struct simple_table table = { 0 };
	parse_table(argv[1], &table);

	// 打印解析的数据，以验证正确性
	printf("Key Signature: %d\n", table.key_signature);
	printf("Duration: %d\n", table.duration);
	printf("Beat: %d\n", table.beat);
	printf("Table Len: %d\n", table.len);
	printf("Input Data:\n");
	for (int i = 0; i < table.len; i++) {
		printf("Symbol: %d, Duration: %d, Pitch: %d\n",
		       table.input[i].symbol, table.input[i].duration,
		       table.input[i].pitch);
	}

	return 0;
}
