#ifndef __TABLE_H__
#define __TABLE_H__
struct Triad {
	int root;
	int third;
	int fifth;
};

struct Seventh {
	int root;
	int third;
	int fifth;
	int seventh;
};

struct Ninth {
	int root;
	int third;
	int fifth;
	int seventh;
	int ninth;
};

struct LevelChord {
	int level;
	int chord;
};

struct thread_arg {
	struct simple_table *table;
	struct LevelChord *lc;
};

struct thread_play_input_args {
	struct simple_input in[5];
	int in_len;
};

void table_print_input(struct simple_table *table);
void table_print_section(struct simple_table *table);
int table_play_song(struct simple_table *table);
void table_init(const char *filename, struct simple_table *table);
void chord_match_degree(struct simple_table *table);
void table_init_third_chord_match_degree(struct simple_table *table, int index);
void table_init_seventh_chord_match_degree(struct simple_table *table,
					   int index);
void table_init_ninth_chord_match_degree(struct simple_table *table, int index);
bool get_the_section_chord(struct simple_table *table, int chord,
			   int index, uint8_t * bitmap);
int get_notes_by_input(const struct simple_input *input, char *notes);
void play_input(struct simple_input *in);
void *thread_play_chord(void *arg);
#endif //__TABLE_H__
