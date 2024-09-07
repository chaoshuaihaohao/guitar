#ifndef __GUITAR_H__

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
	uint8_t key;	//high 4 is pitch, low 4 bit is key
};

struct simple_input {
	char symbol;
	int duration;
	int pitch;//音高
};

#define MAX_INPUT_SIZE 1024
#define MAX_SECTION_SIZE 256

struct simple_table {
	struct simple_input input[MAX_INPUT_SIZE];
	int len;
	int section_start[MAX_SECTION_SIZE];	//the end input index of section
	int section_end[MAX_SECTION_SIZE];
	int section_count;
};


#endif  //__GUITAR_H__
