#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define REFERENCE_FREQ 440.0 // A4的频率
#define OCTAVE_RATIO 1.05946 // 两个半音之间的频率比

// 函数声明
double note_to_frequency(const char *note, int octave);

int main(int arg, char **argv) {
    const char *note = argv[1];
    int octave = atoi(argv[2]);
    double frequency = note_to_frequency(note, octave);
    printf("The frequency of %s%d is %.2f Hz\n", note, octave, frequency);
    return 0;
}

// 函数定义
double note_to_frequency(const char *note, int octave) {
    int note_steps[] = {-9, -8, -7, -6, -5, -4, -3, -2, -1, 0, 1, 2}; // C, C#/Db, D, D#/Eb, E, F, F#/Gb, G, G#/Ab, A, A#/Bb, B
    int steps_from_a4 = 0;
    double frequency;

#if 0
    // 将音符名转换为音级索引
    for (int i = 0; i < 12; ++i) {
        if ((note[0] == 'C' && note[1] == '\0') || (note[0] == 'C' && note[1] == ' ' && note[2] == '#')) {
            if (note[1] == '#')
		    steps_from_a4 = note_steps[i] + 1;
            else
		    steps_from_a4 = note_steps[i];
            break;
        } else if ((note[0] == 'C' && note[1] == ' ' && note[2] == 'b') || (note[0] == 'D' && note[1] == 'b')) {
            steps_from_a4 = note_steps[i];
            break;
        } else if ((note[0] == 'D' && note[1] == '\0') || (note[0] == 'D' && note[1] == ' ' && note[2] == '#')) {
            steps_from_a4 = note_steps[i];
            break;
        } else if ((note[0] == 'E' && note[1] == 'b') || (note[0] == 'E' && note[1] == '\0')) {
            steps_from_a4 = note_steps[i];
            break;
        } else if ((note[0] == 'E' && note[1] == ' ' && note[2] == '#')) {
            steps_from_a4 = note_steps[i] + 1;
            break;
        } else if ((note[0] == 'F' && note[1] == '\0') || (note[0] == 'F' && note[1] == ' ' && note[2] == '#')) {
            steps_from_a4 = note_steps[i];
            break;
        } else if ((note[0] == 'G' && note[1] == '\0') || (note[0] == 'G' && note[1] == ' ' && note[2] == '#')) {
            steps_from_a4 = note_steps[i];
            break;
        } else if ((note[0] == 'A' && note[1] == 'b') || (note[0] == 'A' && note[1] == '\0')) {
            steps_from_a4 = note_steps[i];
            break;
        } else if ((note[0] == 'A' && note[1] == ' ' && note[2] == '#')) {
            steps_from_a4 = note_steps[i] + 1;
            break;
        } else if ((note[0] == 'B' && note[1] == '\0') || (note[0] == 'B' && note[1] == ' ' && note[2] == 'b')) {
            steps_from_a4 = note_steps[i];
            break;
        }
    }
#endif
        if ((note[0] == 'C' && note[1] == '\0')) {
		    steps_from_a4 = note_steps[0];
        } else if ((note[0] == 'C' && note[1] == '#') || (note[0] == 'D' && note[1] == 'b')) {
		    steps_from_a4 = note_steps[1];
        } else if ((note[0] == 'D' && note[1] == '\0')) {
		    steps_from_a4 = note_steps[2];
        } else if ((note[0] == 'D' && note[1] == '#') || (note[0] == 'E' && note[1] == 'b')) {
		    steps_from_a4 = note_steps[3];
        } else if ((note[0] == 'E' && note[1] == '\0')) {
		    steps_from_a4 = note_steps[4];
        } else if ((note[0] == 'F' && note[1] == '\0')) {
		    steps_from_a4 = note_steps[5];
        } else if ((note[0] == 'F' && note[1] == '#') || (note[0] == 'G' && note[1] == 'b')) {
		    steps_from_a4 = note_steps[6];
        } else if ((note[0] == 'G' && note[1] == '\0')) {
		    steps_from_a4 = note_steps[7];
        } else if ((note[0] == 'G' && note[1] == '#') || (note[0] == 'A' && note[1] == 'b')) {
		    steps_from_a4 = note_steps[8];
        } else if ((note[0] == 'A' && note[1] == '\0')) {
		    steps_from_a4 = note_steps[9];
        } else if ((note[0] == 'A' && note[1] == '#') || (note[0] == 'B' && note[1] == 'b')) {
		    steps_from_a4 = note_steps[10];
        } else if ((note[0] == 'B' && note[1] == '\0')) {
		    steps_from_a4 = note_steps[11];
        }
    // 计算总的半音步数
    int total_steps = steps_from_a4 + (octave - 4) * 12;
    // 计算频率
    frequency = REFERENCE_FREQ * pow(OCTAVE_RATIO, total_steps);

    return frequency;
}
