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

#if 1
	sprintf(command, "sox -q -n output.wav synth %f sine %f", time, hz);
	mc_debug("%s", command);
	system(command);

	sprintf(command, "aplay output.wav > /dev/null 2>&1");
	mc_debug("%s", command);
	system(command);
#else
	sprintf(command, "sox -q -n -d synth %f sine %f", time, hz);
	mc_debug("%s", command);
	system(command);
#endif
}

int table_play_song(struct simple_table *table)
{
	for (int i = 0; i < table->len; i++) {
		mc_info("Symbol: '%c', Duration: %2d, Pitch: %2d",
		       table->input[i].symbol, table->input[i].duration,
		       table->input[i].pitch);
		play_input(&table->input[i]);
	}
	return 0;
}

void *play_chord(void *arg)
{
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

	for (int j = 0; j < table->section_count; j++) {
		/* Get chord of section */
		table_init_third_chord_match_degree(table, j);
		table_init_seventh_chord_match_degree(table, j);
		table_init_ninth_chord_match_degree(table, j);

		uint8_t bitmap = 0;
		int chord = -1;
		for (int i = 0; i < CHORD_NUM; i++) {
			if (get_the_section_chord(table, i, j, &bitmap)) {
				chord = i;
				break;
			}
		}
		struct LevelChord lc;
		for (int i = 0; i < 7; i++) {
			if (bitmap & (1UL << i)) {
				lc.level = i;
				char symbol = i + '1';
				mc_info("bitmap = %x, symbol %c", bitmap,
					symbol);
				struct simple_input in;
				in.symbol = symbol;
				in.pitch = 0;
				in.duration = 4;
				char notes[10];
				get_notes_by_input(&in, notes);
//				get_chord(notes, chord);
				break;
			}
		}
		/* Thread to play chord */
		if (chord != -1) {
			lc.chord = chord;
			struct thread_arg arg;
			arg.table = table;
			arg.lc = &lc;
			status = pthread_create(&thread_id, NULL, thread_play_chord, &arg);
			if (status != 0) {
				fprintf(stderr, "Error creating thread: %d\n", status);
				exit(1);
			}
		}
		/* Play note of section */
		for (int i = table->section_start[j];
		     i <= table->section_end[j]; i++) {
			play_input(&table->input[i]);
		}
		if (chord != -1) {
			// 等待线程完成
			status = pthread_join(thread_id, NULL);
			if (status != 0) {
				fprintf(stderr, "Error joining thread: %d\n", status);
				exit(1);
			}
		}

		printf("Thread finished execution.\n");

	}
	return 0;
}
