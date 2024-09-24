#ifndef __PLAY_H__
#define __PLAY_H__
void play_sound(float hz, float time);
int table_play_song(struct simple_table *table);
int table_play_song_section(struct simple_table *table);
int table_play_test(struct simple_table *table);
#endif  //__PLAY_H__
