#ifndef __GUITAR_H__
#define __GUITAR_H__

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

struct chord_stages {
	int level;
	int step;
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
	KEY_NUM,
};

struct simple_note {
	int key;
	char name[10];
	int level;
	int steps_from_a4;
};

struct simple_char {
	char name[10];
	float hz;
	uint8_t key;		//high 4 is pitch, low 4 bit is key
};

struct simple_input {
	char symbol;
	int duration;
	int pitch;		//音高
	const struct simple_table *table;
};

#define MAX_INPUT_SIZE 1024
#define MAX_SECTION_SIZE 256

#define MATCH_DEGREE_SCORE_NUM	7
struct MatchDegree {
	int score[MATCH_DEGREE_SCORE_NUM];
	int symbol_count;
};

enum Chord_Type {
	THIRD,
	SEVENTH,
	NINTH,
	CHORD_NUM,
};

#define CHORD_TYPE 3

/*
 * @len: length of input
 */
struct simple_table {
	struct simple_input input[MAX_INPUT_SIZE];
	int len;		//input len
	int section_start[MAX_SECTION_SIZE];	//the end input index of section
	int section_end[MAX_SECTION_SIZE];
	int section_count;
	struct MatchDegree degree[CHORD_NUM][MAX_SECTION_SIZE];
	int key_signature;
	int duration;
	int beat;
	int tempo;
};

float get_freq_by_notes(char *str);
float get_time_by_input(const struct simple_input *input);
void play_sound(float hz, float time);
void get_chord(const char *notes, int chord);
int find_key_signature(char *value);

#define mc_err(f, arg...) \
        fprintf(stderr, "ERR:[%s:%d]: " f "\n", __func__, __LINE__, ##arg)
#define mc_info(f, arg...) \
        printf("INFO:[%s:%d]:" f "\n", __func__, __LINE__, ##arg)
#define mc_warn(f, arg...) \
        printf("WARN:[%s:%d]:" f "\n", __func__, __LINE__, ##arg)
#define mc_debug(f, arg...) \
//	printf("DBG:[%s:%d]:" f "\n", __func__, __LINE__, ##arg)

#endif //__GUITAR_H__
