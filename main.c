#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include "main.h"
#include "table.h"
#include "play.h"

static struct chord_stages stage[] = {
	{ LEVEL_1, 0 },
	{ LEVEL_2, 2 },
	{ LEVEL_3, 4 },
	{ LEVEL_4, 5 },
	{ LEVEL_5, 7 },
	{ LEVEL_6, 9 },
	{ LEVEL_7, 11 },
};

struct simple_note note[KEY_NUM] = {
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

int find_key_signature(char *value)
{
	for (int i = 0; i < ARRAY_SIZE(note); i++) {
		if (strcmp(value, note[i].name) == 0) {
			return note[i].key;
		}
	}
	return -1;
}

float get_time_by_input(const struct simple_input *input)
{
	const struct simple_input *next = input + 1;
	const struct simple_table *table = input->table;
	float beat_time = 60.0 / table->tempo;
	float time = beat_time * table->duration / input[0].duration;
	float count = 1;

	do {
		if (next->symbol == '.')
			count += 0.5;
		else if (next->symbol == '-')
			count++;
		else {
			break;
		}

	} while (++next);
	time *= count;

	mc_info("time %f", time);
	return time;
}

float get_freq_by_notes(char *notes)
{
	int steps_from_a4;
	int octave;
	bool next = false;

	if (!notes)
		return -1;
	//int steps_from_a4[] = {-9, -8, -7, -6, -5, -4, -3, -2, -1, 0, 1, 2}; // C, C#/Db, D, D#/Eb, E, F, F#/Gb, G, G#/Ab, A, A#/Bb, B

	switch (notes[0]) {
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
		mc_debug("Err: unsupport notes %s", notes);
		return -1;
		break;
	}

	do {
		switch (notes[1 + !!next]) {
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
			if (notes[2 + !!next] != '\0') {
				mc_err("Err: unsupport notes %s", notes);
				return -1;
			}
			octave = atoi(&notes[1 + !!next]);
			next = false;
			break;
		default:
			mc_err("Err: unsupport notes %s", notes);
			return -1;
		}

	} while (next);

#define REFERENCE_FREQ 440.0	// A4的频率
#define OCTAVE_RATIO 1.05946	// 两个半音之间的频率比
	// 计算总的半音步数
	int total_steps = steps_from_a4 + (octave - 4) * 12;
	// 计算频率
	double frequency;
	frequency = REFERENCE_FREQ * pow(OCTAVE_RATIO, total_steps);
#ifdef DEBUG
	mc_info("steps_from_a4: %d, octave: %d, frequency: %f", steps_from_a4,
		octave, frequency);
#endif
	return frequency;
}

#if 0
static char *get_target_note_by_note(const char *note_name, int level,
				     int steps_up_down)
{
	uint32_t i;
	int interval;
	int cur_level, target_level;
	char *target_note;

	for (i = 0; i < ARRAY_SIZE(note); i++) {
		if (strcmp(note_name, note[i].name) == 0) {
			cur_level = note[i].level;
			target_level = cur_level + level - LEVEL_1;
			target_level %= 7;
			interval =
			    note[i].steps_from_a4 + stage[target_level].step +
			    steps_up_down - stage[cur_level].step;
			if (interval >= 12) {
				interval -= 12;
			}
			if (interval < 0) {
				interval += 12;
			}
			break;
		}
	}
	for (i = 0; i < ARRAY_SIZE(note); i++) {
		if (note[i].steps_from_a4 == interval
		    && note[i].level == target_level)
			target_note = note[i].name;
	}

	if (!target_note) {
		mc_debug("Err: no key symbol is matched.");
		return NULL;
	}
	return target_note;
}

static char *get_third_note_by_note(const char *note_name)
{
	char *target_note;

	target_note = get_target_note_by_note(note_name, LEVEL_3, 0);
	if (!target_note) {
		mc_debug("Err: no key symbol is matched.");
		return NULL;
	}
	return target_note;
}

static char *get_fifth_note_by_note(const char *note_name)
{
	char *target_note;

	target_note = get_target_note_by_note(note_name, LEVEL_5, 0);
	if (!target_note) {
		mc_debug("Err: no key symbol is matched.");
		return NULL;
	}
	return target_note;
}

static char *get_seventh_note_by_note(const char *note_name)
{
	char *target_note;

	target_note = get_target_note_by_note(note_name, LEVEL_7, 0);
	if (!target_note) {
		mc_debug("Err: no key symbol is matched.");
		return NULL;
	}
	return target_note;
}
#endif

/*
 * input note name
 * output chord string
 */
void get_chord(const char *notes, int chord)
{
#if 1
	mc_info("note name %s", notes);
//      mc_info("note name %s%d", get_third_note_by_note(note_name), octave);
//      mc_info("note name %s%d", get_fifth_note_by_note(note_name), octave);
//      mc_info("note name %s%d", get_seventh_note_by_note(note_name), octave);
#else
	mc_info("note name %s: %s %s %s", note_name,
		get_third_note_by_note(note_name),
		get_fifth_note_by_note(note_name),
		get_seventh_note_by_note(note_name));
#endif
}

/*
 * return:
 *	 0: symbol '0'; 1: notes; -1: continue
 */

int get_notes_by_input(const struct simple_input *input, char *notes)
{
	char symbol = input->symbol;
	uint32_t i;
	char *note_name = NULL;
	int note_level;
	int interval;

	switch (symbol) {
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
		break;
	case '0':
		return -1;
	default:
		mc_info("skip special symbol '%c'", symbol);
		return -1;
	}
	int level = symbol - '1';

	int octave = DEAFULT_OCTAVE + input->pitch;
	note_level = note[input->table->key_signature].level + level - LEVEL_1;
	note_level %= 7;
	interval =
	    note[input->table->key_signature].steps_from_a4 +
	    stage[level].step - stage[LEVEL_1].step;
	if (interval >= 12) {
		interval -= 12;
		octave++;
	}
	if (interval < 0) {
		interval += 12;
		octave--;
	}

	for (i = 0; i < ARRAY_SIZE(note); i++) {
		if (note[i].steps_from_a4 == interval
		    && note[i].level == note_level)
			note_name = note[i].name;
	}

	if (!note_name) {
		mc_err
		    ("Err: no key symbol is matched to symbol %s, note_level %d, interval %d.",
		     note_name, note_level, interval);
		return -1;
	} else {
		//get_chord(note_name);

	}
	mc_info("note_name = %s, octave = %d", note_name, octave);
	sprintf(notes, "%s%d", note_name, octave);
	mc_info("notes = %s", notes);
	return 0;
}

int main(int argc, char **argv)
{
	struct simple_table table;

	table_init(argv[1], &table);
	chord_match_degree(&table);
#if 1
	table_play_song(&table);
#else
	table_play_song_section(&table);
#endif

	return 0;
}
