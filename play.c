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

	float frequency = get_freq_by_notes(tmp);
	if (frequency < 0)
		continue;
	time = get_time_by_input(&table->input[i]);
	play_sound(frequency, time);
}
#endif
int table_play_song(struct simple_table *table)
{
	for (int i = 0; i < table->len; i++) {
		printf("Symbol: %c, Duration: %d, Pitch: %d\n",
		       table->input[i].symbol, table->input[i].duration,
		       table->input[i].pitch);
		char notes[10] = {0};
		if (get_notes_by_input(&table->input[i], notes)) {
			mc_warn("symbol '%c' continue", table->input[i].symbol);
			continue;
		} else if (table->input[i].symbol == '0') {
			float beat_time = BEAT_TIME / table->input[i].duration;
			struct timespec req, rem;
			req.tv_sec = 0;
			req.tv_nsec = beat_time * 1000000000;
			nanosleep(&req, &rem);
			mc_info("symbol '%c' continue, delay %ld seconds",
				table->input[i].symbol, req.tv_nsec);
			continue;
		}
		float frequency = get_freq_by_notes(notes);
		if (frequency < 0)
			continue;
		get_time_by_input(&table->input[i]);
		float time = get_time_by_input(&table->input[i]);
		play_sound(frequency, time);
	}
	return 0;
}

void *play_chord(void *arg) {
    // 播放 C 大三和弦 C4-E4-G4，持续时间为 2 秒

#if 0
	system("sox -n -d synth 0.5 sine 392 &");
	system("sox -n -d synth 0.5 sine 493 &");
	system("sox -n -d synth 0.5 sine 578 &");
#else
	system("sox -n -d synth 0.5 sine 349.232311 &");
	system("sox -n -d synth 0.5 sine 440 &");
	system("sox -n -d synth 0.5 sine 523.246546 &");


#endif
    return NULL;
}
int table_play_song_section(struct simple_table *table)
{
	pthread_t thread_id;
	int status;
	float time;

	for (int j = 0; j < table->section_count; j++) {
		/* Get chord of section */
		table_init_third_chord_match_degree(table, j);
		table_init_seventh_chord_match_degree(table, j);
		table_init_ninth_chord_match_degree(table, j);

		uint8_t bitmap = 0;
		int chord = 0;
		if (get_the_section_chord(table, THIRD, j, &bitmap)) {
			chord = THIRD;
		} else if (get_the_section_chord(table, SEVENTH, j, &bitmap)) {
			chord = SEVENTH;
		//} else if (get_the_section_chord(table, NINTH, j, &bitmap)) {
		} else {
			chord = NINTH;
		}

		for (int i = 0; i < 7; i++) {
			if (bitmap & (1UL << i)) {
				const char symbol = i + '1';
				int octave;
				const char *note = get_note_by_symbol(&symbol, &octave);
				mc_info("bitmap = %x, symbol %c", bitmap, symbol);
				get_chord(note, octave, chord);
				break;
			}
		}
		/* Thread to play chord */
		//TODO
	    // 创建线程
	    status = pthread_create(&thread_id, NULL, play_chord, NULL);
	    if (status != 0) {
		fprintf(stderr, "Error creating thread: %d\n", status);
		exit(1);
	    }

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

			float frequency = get_freq_by_notes(tmp);
			if (frequency < 0)
				continue;
			time = get_time_by_input(&table->input[i]);
			play_sound(frequency, time);
		}
    // 等待线程完成
    status = pthread_join(thread_id, NULL);
    if (status != 0) {
        fprintf(stderr, "Error joining thread: %d\n", status);
        exit(1);
    }

    printf("Thread finished execution.\n");

	}
	return 0;
}
