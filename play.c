#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include "main.h"
#include "table.h"

void play_sound(float hz, float time)
{
	char command[256];

	sprintf(command, "sox -q -n output.wav synth %f sine %f", time, hz);
	mc_debug("%s", command);
	system(command);

	sprintf(command, "aplay output.wav > /dev/null 2>&1");
	mc_debug("%s", command);
	system(command);
}

#if 0
void play_chord(char *note, int octave)
{
	char tmp[20];
//      octave += table->input[i].pitch;
	smc_debug(tmp, "%s%d", note, octave);
	mc_debug("%s's note is: %s\n", str, tmp);

	float frequency = get_freq_of_note(tmp);
	if (frequency < 0)
		continue;
	time = get_time_of_note(&table->input[i]);
	play_sound(frequency, time);
}
#endif
int table_play_song(struct simple_table *table)
{
	float time;
	for (int i = 0; i < table->len; i++) {
		const char *str = &table->input[i].symbol;
		if (strlen(str) > 1) {
			mc_err("Err len %ld", strlen(str));
			return -1;
		}

		int octave;
		const char *note = get_note_by_symbol(str, &octave);
		if (!note) {
			mc_warn("symbol '%s' continue", str);
			continue;
		} else if (note[0] == '0') {
			float beat_time = BEAT_TIME / table->input[i].duration;
			struct timespec req, rem;
			req.tv_sec = 0;
			req.tv_nsec = beat_time * 1000000000;
			nanosleep(&req, &rem);
			mc_info("symbol '%s' continue, delay %ld seconds",
				str, req.tv_nsec);
			continue;
		}
		char tmp[20];
		octave += table->input[i].pitch;
		sprintf(tmp, "%s%d", note, octave);
		mc_info("%s's note is: %s", str, tmp);

		float frequency = get_freq_of_note(tmp);
		if (frequency < 0)
			continue;
		time = get_time_of_note(&table->input[i]);
		play_sound(frequency, time);
	}

	return 0;
}

#if 0
void play_chord(char *note, int octave)
{
	get_chord_of_section();

	thread(play_chord_of_section());
	for (int i = 0; i < table->section[j].len; i++)
		play_note_of_section(note);

}
#endif
int table_play_song_section(struct simple_table *table)
{

	float time;
	for (int j = 0; j < table->section_count; j++) {
		/* Get chord of section */
		table_init_third_chord_match_degree(table, j);
		table_init_seventh_chord_match_degree(table, j);
		table_init_ninth_chord_match_degree(table, j);

		uint8_t bitmap = 0;
		if (get_the_section_chord(table, THIRD, j, &bitmap)) {
		} else if (get_the_section_chord(table, SEVENTH, j, &bitmap)) {
		} else if (get_the_section_chord(table, NINTH, j, &bitmap)) {
		}

		for (int i = 0; i < 7; i++) {
			if (bitmap & (1UL << i)) {
				const char symbol = i + '1';
				int octave;
				const char *note = get_note_by_symbol(&symbol, &octave);
				mc_info("bitmap = %x, symbol %c", bitmap, symbol);
				get_chord(note);
				break;
			}
		}
		/* Thread to play chord */
		//TODO


		/* Play note of section */
		for (int i = table->section_start[j];
		     i <= table->section_end[j]; i++) {
			const char *str = &table->input[i].symbol;
			if (strlen(str) > 1) {
				mc_err("Err len %ld", strlen(str));
				return -1;
			}

			int octave;
			const char *note = get_note_by_symbol(str, &octave);
			if (!note) {
				mc_warn("symbol '%s' continue", str);
				continue;
			} else if (note[0] == '0') {
				float beat_time = BEAT_TIME / table->input[i].duration;
				struct timespec req, rem;
				req.tv_sec = 0;
				req.tv_nsec = beat_time * 1000000000;
				nanosleep(&req, &rem);
				mc_info("symbol '%s' continue, delay %ld seconds",
					str, req.tv_nsec);
				continue;
			}
			char tmp[20];
			octave += table->input[i].pitch;
			sprintf(tmp, "%s%d", note, octave);
			mc_info("%s's note is: %s", str, tmp);

			float frequency = get_freq_of_note(tmp);
			if (frequency < 0)
				continue;
			time = get_time_of_note(&table->input[i]);
			play_sound(frequency, time);
		}
	}
	return 0;
}
