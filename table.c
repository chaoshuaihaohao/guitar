#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include "guitar.h"
#include "table.h"

void table_print_input(struct simple_table *table)
{
	printf("Read %d entries from file.\n", table->len);
		for (int j = 0; j < table->len; j++) {
		printf("%c %d %d\n", table->input[j].symbol, table->input[j].duration, table->input[j].pitch);
	}
	double duration = 1.0 / QUARTER * BEAT;
	double beat_count = 0;
	int count = 0;
	int tmp = 0;
	for (int j = 0; j < table->len; j++) {
		switch (table->input[j].duration) {
		case 4:
			printf("   ");
			break;
		case 8:
			printf(" ");
			break;
		}
		printf("%c", table->input[j].symbol);
		if (table->input[j].duration)
			beat_count += 1.0 / table->input[j].duration;
		if (beat_count == duration) {
			beat_count = 0;
			printf("|");
			count++;
			if (count % 4 == 0) {
				printf("\n");
				for (int k = tmp; k <= j; k++) {
					switch (table->input[k].duration) {
					case 4:
						printf("    ");
						break;
					case 8:
						printf(" -");
						break;
					case 16:
						printf("=");
						break;
					}
					if (table->input[k].duration)
						beat_count += 1.0 / table->input[k].duration;
					if (beat_count == duration) {
						beat_count = 0;
						printf("|");
					}
				}
				tmp = j+1;
				printf("\n");
				beat_count = 0;

			}
		}
	}
	printf("\n");
}

static void table_init_section(struct simple_table *table)
{
	double section_beat = 1.0 / QUARTER * BEAT;
	double beat_count = 0;
	int section_count = 0;

	for (int j = 0; j < table->len; j++) {
		if (table->input[j].duration)
			beat_count += 1.0 / table->input[j].duration;
		if (beat_count == section_beat) {
			if (section_count == 0) {
				table->section_start[section_count] = 0;
			} else {
				table->section_start[section_count] = table->section_end[section_count - 1] + 1;
			}
			table->section_end[section_count] = j;
			beat_count = 0;
			section_count++;
		}
	}
	table->section_count = section_count;
}

void table_print_section(struct simple_table *table)
{
	for (int i = 0; i < table->section_count; i++) {
		if (i % 4 == 0)
			printf("\n");
		for (int j = table->section_start[i]; j <= table->section_end[i]; j++) {
			printf("%c ", table->input[j].symbol);
		}
		printf("| ");
	}

	printf("\n");
}

static int table_init_input(struct simple_table *table)
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

    return EXIT_SUCCESS;
}


int table_play_sound(struct simple_table *table)
{
	float time;
	for (int i = 0; i < table->len; i++) {
		const char *str = &table->input[i].symbol;
		if (strlen(str) > 1) {
			printf("Err len %ld\n", strlen(str));
			return -1;
		}

		int octave;
		const char *note = get_note_by_symbol(str, &octave);
		if (!note) {
			printf("symbol '%s' continue\n", str);
			continue;
		} else if (note[0] == '0') {
			float beat_time = BEAT_TIME / table->input[i].duration;
			struct timespec req, rem;
			req.tv_sec = 0;
			req.tv_nsec = beat_time * 1000000000;
			nanosleep(&req, &rem);
#ifdef DEBUG
			printf("symbol '%s' continue, delay %ld seconds\n", str, req.tv_nsec);
#endif
			continue;
		}
		char tmp[20];
		octave += table->input[i].pitch;
		sprintf(tmp, "%s%d", note, octave);
		printf("%s's note is: %s\n", str, tmp);

		float frequency = get_freq_of_note(tmp);
		if (frequency < 0)
			continue;
		time = get_time_of_note(&table->input[i]);
		play_sound(frequency, time);
	}

	return 0;
}


void table_init(struct simple_table *table)
{
	table_init_input(table);
	table_init_section(table);
}
//index: section index
void table_init_third_chord_match_degree(struct simple_table *table, int index)
{
	struct Triad third_chord [] = {
	       { 1,3,5 },
	       { 2,4,6 },
	       { 3,5,7 },
	       { 4,6,1 },
	       { 5,7,2 },
	       { 6,1,3 },
	       { 7,2,4 },
	};

	for (int j = table->section_start[index]; j <= table->section_end[index]; j++) {
		if (table->input[j].symbol == '.' || table->input[j].symbol == '-')
			continue;
		int tmp = table->input[j].symbol - '0';
		table->degree[THIRD][index].symbol_count++;
		printf("%d ", tmp);
		for (int k = 0; k < 7; k++) {
			if (tmp == third_chord[k].root || tmp == third_chord[k].third || tmp == third_chord[k].fifth)
				table->degree[THIRD][index].score[k]++;
		}
	}
	printf("\n");
	for (int q = 0; q < 7; q++)
		printf("level %d table->degree[THIRD]: %f, score %d symbol_count %d\n", q + 1,(float)table->degree[THIRD][index].score[q] / table->degree[THIRD][index].symbol_count, table->degree[THIRD][index].score[q], table->degree[THIRD][index].symbol_count);
}

//index: section index
void table_init_seventh_chord_match_degree(struct simple_table *table, int index)
{
	struct Seventh seventh_chord [] = {
	       { 1,3,5,7 },
	       { 2,4,6,1 },
	       { 3,5,7,2 },
	       { 4,6,1,3 },
	       { 5,7,2,4 },
	       { 6,1,3,5 },
	       { 7,2,4,6 },
	};

	for (int j = table->section_start[index]; j <= table->section_end[index]; j++) {
		if (table->input[j].symbol == '.' || table->input[j].symbol == '-')
			continue;
		int tmp = table->input[j].symbol - '0';
		table->degree[SEVENTH][index].symbol_count++;
		printf("%d ", tmp);
		for (int k = 0; k < 7; k++) {
			if (tmp == seventh_chord[k].root || \
			    tmp == seventh_chord[k].third || \
			    tmp == seventh_chord[k].fifth || \
			    tmp == seventh_chord[k].seventh)
				table->degree[SEVENTH][index].score[k]++;
		}
	}
	printf("\n");
	for (int q = 0; q < 7; q++)
		printf("level %d table->degree[SEVENTH]: %f, score %d symbol_count %d\n", q + 1,(float)table->degree[SEVENTH][index].score[q] / table->degree[SEVENTH][index].symbol_count, table->degree[SEVENTH][index].score[q], table->degree[SEVENTH][index].symbol_count);
}

//index: section index
void table_init_ninth_chord_match_degree(struct simple_table *table, int index)
{
	struct Ninth ninth_chord [] = {
	       { 1,3,5,7,2 },
	       { 2,4,6,1,3 },
	       { 3,5,7,2,4 },
	       { 4,6,1,3,5 },
	       { 5,7,2,4,6 },
	       { 6,1,3,5,7 },
	       { 7,2,4,6,1 },
	};


	for (int j = table->section_start[index]; j <= table->section_end[index]; j++) {
		if (table->input[j].symbol == '.' || table->input[j].symbol == '-')
			continue;
		int tmp = table->input[j].symbol - '0';
		table->degree[NINTH][index].symbol_count++;
		printf("%d ", tmp);
		for (int k = 0; k < 7; k++) {
			if (tmp == ninth_chord[k].root || \
			    tmp == ninth_chord[k].third || \
			    tmp == ninth_chord[k].fifth || \
			    tmp == ninth_chord[k].seventh || \
			    tmp == ninth_chord[k].ninth)
				table->degree[NINTH][index].score[k]++;
		}
	}
	printf("\n");
	for (int q = 0; q < 7; q++)
		printf("level %d table->degree[NINTH]: %f, score %d symbol_count %d\n", q + 1,(float)table->degree[NINTH][index].score[q] / table->degree[NINTH][index].symbol_count, table->degree[NINTH][index].score[q], table->degree[NINTH][index].symbol_count);
}

void chord_match_degree(struct simple_table *table)
{
	for (int i = 0; i < table->section_count; i++) {
		table_init_third_chord_match_degree(table, i);
		table_init_seventh_chord_match_degree(table, i);
		table_init_ninth_chord_match_degree(table, i);
	}
}

