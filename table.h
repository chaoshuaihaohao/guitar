#ifndef __TABLE_H__
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

void table_print_input(struct simple_table *table);
void table_print_section(struct simple_table *table);
int table_play_song(struct simple_table *table);
void table_init(struct simple_table *table);
void chord_match_degree(struct simple_table *table);
#endif  //__TABLE_H__
