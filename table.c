#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <math.h>
#include <time.h>
#include "main.h"
#include "table.h"
#include "play.h"

static void table_init_section(struct simple_table *table)
{
	float section_beat = (float)table->beat / table->duration;
	float beat_count = 0;
	int section_count = 0;

	for (int j = 0; j < table->len; j++) {
		if (table->input[j].duration)
			beat_count += 1.0 / table->input[j].duration;
		if (beat_count == section_beat) {
			if (section_count == 0) {
				table->section_start[section_count] = 0;
			} else {
				table->section_start[section_count] =
				    table->section_end[section_count - 1] + 1;
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
		for (int j = table->section_start[i];
		     j <= table->section_end[i]; j++) {
			printf("%c ", table->input[j].symbol);
		}
		printf("| ");
	}

	printf("\n");
}

static int simple_check(int key)
{
	if (key == KEY_SIGNATURE_D_SHARP) {
		mc_err("ERR: unsupport key 'D#', Please use key Eb");
		return -1;
	}
	return 0;
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
			if (result != 1) {
				fprintf(stderr,
					"Error reading key signature\n");
				fclose(fp);
				return -1;
			}
		}
	}
	*key_signature = find_key_signature(value);
	if (*key_signature == -1 || simple_check(*key_signature)) {
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

int get_tempo(const char *filename, int *tempo)
{
	char line[256];

	FILE *fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("Error opening file");
		return -1;
	}
	// 获取 duration 和 beat
	while (fgets(line, sizeof(line), fp) != NULL) {
		if (strncmp(line, "tempo=", strlen("tempo=")) == 0) {
			int result = sscanf(line, "tempo=%d", tempo);
			if (result != 1) {
				fprintf(stderr, "Error reading tempo\n");
				fclose(fp);
				return -1;
			}
		}
	}

	fclose(fp);
	return 0;
}

int get_input(const char *filename, struct simple_input *input, int *input_len,
	      const struct simple_table *table)
{
	char line[256];
	int count = 0;

	FILE *fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("Error opening file");
		return -1;
	}

	while (fgets(line, sizeof(line), fp) != NULL) {
		int result = sscanf(line, "%c %d %d", &input[count].symbol,
				    &input[count].duration,
				    &input[count].pitch);
		input[count].table = table;
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

extern struct simple_note note[KEY_NUM];
int parse_table(const char *filename, struct simple_table *table)
{
	int ret;

	ret = get_key_signature(filename, &table->key_signature);
	if (ret)
		return ret;
	ret = get_duration_beat(filename, &table->duration, &table->beat);
	if (ret)
		return ret;
	ret = get_tempo(filename, &table->tempo);
	if (ret)
		return ret;
	ret = get_input(filename, table->input, &table->len, table);
	if (ret)
		return ret;

	// 打印解析的数据，以验证正确性
	mc_info("Key Signature: %s", note[table->key_signature].name);
	mc_info("Duration: %d", table->duration);
	mc_info("Beat: %d", table->beat);
	mc_info("Tempo: %d", table->tempo);
	mc_info("Table Len: %d", table->len);
	mc_info("Input Data:");
	for (int i = 0; i < table->len; i++) {
		mc_info("Symbol: '%c', Duration: %2d, Pitch: %2d",
			table->input[i].symbol, table->input[i].duration,
			table->input[i].pitch);
	}
	return 0;
}

void table_init(const char *filename, struct simple_table *table)
{
	parse_table(filename, table);
	table_init_section(table);
}

//index: section index
void table_init_third_chord_match_degree(struct simple_table *table, int index)
{
	struct Triad third_chord[] = {
		{ 1, 3, 5 },
		{ 2, 4, 6 },
		{ 3, 5, 7 },
		{ 4, 6, 1 },
		{ 5, 7, 2 },
		{ 6, 1, 3 },
		{ 7, 2, 4 },
	};

	for (int j = table->section_start[index];
	     j <= table->section_end[index]; j++) {
#if 1
		if (table->input[j].symbol == '.'
		    || table->input[j].symbol == '-'
		    || table->input[j].symbol == '0')
			continue;
#endif
		int tmp = table->input[j].symbol - '0';
		table->degree[THIRD][index].symbol_count++;
		printf("%d ", tmp);
		for (int k = 0; k < 7; k++) {
			if (tmp == third_chord[k].root
			    || tmp == third_chord[k].third
			    || tmp == third_chord[k].fifth)
				table->degree[THIRD][index].score[k]++;
		}
	}
	printf("\n");
	for (int q = 0; q < 7; q++)
		mc_debug
		    ("level %d table->degree[THIRD]: %f, score %d symbol_count %d",
		     q + 1,
		     (float)table->degree[THIRD][index].score[q] /
		     table->degree[THIRD][index].symbol_count,
		     table->degree[THIRD][index].score[q],
		     table->degree[THIRD][index].symbol_count);
}

//index: section index
void table_init_seventh_chord_match_degree(struct simple_table *table,
					   int index)
{
	struct Seventh seventh_chord[] = {
		{ 1, 3, 5, 7 },
		{ 2, 4, 6, 1 },
		{ 3, 5, 7, 2 },
		{ 4, 6, 1, 3 },
		{ 5, 7, 2, 4 },
		{ 6, 1, 3, 5 },
		{ 7, 2, 4, 6 },
	};

	for (int j = table->section_start[index];
	     j <= table->section_end[index]; j++) {
		if (table->input[j].symbol == '.'
		    || table->input[j].symbol == '-')
			continue;
		int tmp = table->input[j].symbol - '0';
		table->degree[SEVENTH][index].symbol_count++;
		printf("%d ", tmp);
		for (int k = 0; k < 7; k++) {
			if (tmp == seventh_chord[k].root ||
			    tmp == seventh_chord[k].third ||
			    tmp == seventh_chord[k].fifth ||
			    tmp == seventh_chord[k].seventh)
				table->degree[SEVENTH][index].score[k]++;
		}
	}
	printf("\n");
	for (int q = 0; q < 7; q++)
		mc_debug
		    ("level %d table->degree[SEVENTH]: %f, score %d symbol_count %d",
		     q + 1,
		     (float)table->degree[SEVENTH][index].score[q] /
		     table->degree[SEVENTH][index].symbol_count,
		     table->degree[SEVENTH][index].score[q],
		     table->degree[SEVENTH][index].symbol_count);
}

//index: section index
void table_init_ninth_chord_match_degree(struct simple_table *table, int index)
{
	struct Ninth ninth_chord[] = {
		{ 1, 3, 5, 7, 2 },
		{ 2, 4, 6, 1, 3 },
		{ 3, 5, 7, 2, 4 },
		{ 4, 6, 1, 3, 5 },
		{ 5, 7, 2, 4, 6 },
		{ 6, 1, 3, 5, 7 },
		{ 7, 2, 4, 6, 1 },
	};

	for (int j = table->section_start[index];
	     j <= table->section_end[index]; j++) {
		if (table->input[j].symbol == '.'
		    || table->input[j].symbol == '-')
			continue;
		int tmp = table->input[j].symbol - '0';
		table->degree[NINTH][index].symbol_count++;
		printf("%d ", tmp);
		for (int k = 0; k < 7; k++) {
			if (tmp == ninth_chord[k].root ||
			    tmp == ninth_chord[k].third ||
			    tmp == ninth_chord[k].fifth ||
			    tmp == ninth_chord[k].seventh ||
			    tmp == ninth_chord[k].ninth)
				table->degree[NINTH][index].score[k]++;
		}
	}
	printf("\n");
	for (int q = 0; q < 7; q++)
		mc_debug
		    ("level %d table->degree[NINTH]: %f, score %d symbol_count %d",
		     q + 1,
		     (float)table->degree[NINTH][index].score[q] /
		     table->degree[NINTH][index].symbol_count,
		     table->degree[NINTH][index].score[q],
		     table->degree[NINTH][index].symbol_count);
}

bool get_the_section_chord(struct simple_table *table, int chord,
			   int index, uint8_t *bitmap)
{
#if 1
	int max = table->degree[chord][index].score[0];	// 假设第一个元素是最大的
	int tip = 0;

	for (int j = 1; j < MATCH_DEGREE_SCORE_NUM; j++) {
		if (table->degree[chord][index].score[j] > max) {
			max = table->degree[chord][index].score[j];	// 找到更大的值，更新最大值
			tip = j;
		}
	}
	if (table->degree[chord][index].symbol_count == 0)
		return false;

	//if degree is 100%, means matched.
	if ((table->degree[chord][index].score[tip] /
	     table->degree[chord][index].symbol_count) || chord == NINTH) {
		mc_info("section %d mateched, chord %d", index, chord);
		for (int j = 0; j < MATCH_DEGREE_SCORE_NUM; j++) {
			if (table->degree[chord][index].score[j] == max) {
				mc_info(" option level[%d]", j + 1);
				*bitmap |= (1UL << j);
			}
		}
		return true;
	}

	return false;
#endif
}

void play_input(struct simple_input *in)
{
	const struct simple_table *table = in->table;
	char notes[10];

	if (get_notes_by_input(in, notes)) {
		if (in->symbol == '0') {
			float beat_time = 60.0 / table->tempo;
			float delay_time =
			    beat_time * table->duration /
			    in->duration;
			struct timespec req, rem;
			req.tv_sec = 0;
			req.tv_nsec = delay_time * 1000000000;
			nanosleep(&req, &rem);
			mc_info
			    ("symbol '%c' continue, delay %ld seconds",
			     in->symbol, req.tv_nsec / 1000000000);
		}
		return;
	}
	mc_info("%c's note is: %s", in->symbol, notes);

	float frequency = get_freq_by_notes(notes);
	if (frequency < 0)
		return;
	float time = get_time_by_input(in);
	play_sound(frequency, time);
}

void *thread_play_input(void *arg)
{
	struct simple_input *in = arg;
	play_input(in);
	return NULL;
}

int get_chord_by_level(struct thread_arg *arg)
{
	int level = arg->lc->level;
	int chord = arg->lc->chord;
	struct Ninth ninth_chord[] = {
		{ 1, 3, 5, 7, 2 },
		{ 2, 4, 6, 1, 3 },
		{ 3, 5, 7, 2, 4 },
		{ 4, 6, 1, 3, 5 },
		{ 5, 7, 2, 4, 6 },
		{ 6, 1, 3, 5, 7 },
		{ 7, 2, 4, 6, 1 },
	};
	mc_info("level = %d, chord = %d", level, chord);
	struct simple_input in[5] = {0};
	int in_len = 0;
#define THREAD_COUNT 5
	pthread_t threads[THREAD_COUNT];
	
	switch (chord) {
	case NINTH:
		in[4].symbol = ninth_chord[level].ninth + '0';
		in_len++;
		/* fallthrough */
	case SEVENTH:
		in[3].symbol = ninth_chord[level].seventh + '0';
		in_len++;
		/* fallthrough */
	case THIRD:
		in[2].symbol = ninth_chord[level].root + '0';
		in[1].symbol = ninth_chord[level].third + '0';
		in[0].symbol = ninth_chord[level].fifth + '0';
		in_len += 3;
		break;
	}
	for (int i = 0; i < in_len; i++) {
		in[i].pitch = 0;
		in[i].duration = 8;
		in[i].table = arg->table;
		mc_info("play chord %c >>>>>>>>>>>>>>>>>>>>>>>>>", in[i].symbol);
		if (pthread_create(&threads[i], NULL, thread_play_input, in) != 0) {
		    mc_err("pthread_create");
		    return -1;
		}
	}


	// 等待所有线程完成
	for (int i = 0; i < in_len; i++) {
		if (pthread_join(threads[i], NULL) != 0) {
		    mc_err("pthread_join");
		    return -1;
		}
	}
	return 0;
}

void *thread_play_chord(void *arg)
{
	struct thread_arg *t_arg = arg;
	get_chord_by_level(t_arg);
	return NULL;
}

void chord_match_degree(struct simple_table *table)
{
	bool matched;
	uint8_t bitmap = 0;
	for (int i = 0; i < table->section_count; i++) {
		table_init_third_chord_match_degree(table, i);
		table_init_seventh_chord_match_degree(table, i);
		table_init_ninth_chord_match_degree(table, i);

		matched = get_the_section_chord(table, THIRD, i, &bitmap);
		if (matched)
			continue;
		matched = get_the_section_chord(table, SEVENTH, i, &bitmap);
		if (matched)
			continue;
#if 1
		matched = get_the_section_chord(table, NINTH, i, &bitmap);
#endif
	}
}
