#pragma once
#include <string>
#include <math.h>
#include "str_a_num.hpp"

// Tone generation, conversation will follow "twelve-tone equal temperament".
// Format of note string should be follow "${note}${level}". Middle level is 3.
// Up plus 1, Down reduce 1.for example, "3A" means middle A note.
namespace vocaloid {

	// Middle A note is 440Hz
	#define MIDDLE_A_NOTE 440
	// Notes' name
	const char notes[12][3] = { "A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#" };

	float NoteToFreq(const char note[3]) {
		int k[128];
		k['A'] = 0; k['B'] = 2; k['C'] = 3; k['D'] = 5;
		k['E'] = 7; k['F'] = 8; k['G'] = 10;

		int n = k[note[0]];
		if (note[1] == '#') n++;
		if (note[1] == 'b') n--;

		int level = (note[2] ? note[2] : note[1]) - '0';
		n += (level - 4 - (note[0] >= 'C')) * 12;
		float f = 440.0f * powf(2.0, (float)n / 12.0f);
		return roundf(f * 1000) * 0.001f;
	}

	std::string FreqToNote(float f) {
		int n = (int)roundf(log2f(f / MIDDLE_A_NOTE) * 12.0f);
		int k = (n % 12 + 12) % 12;
		int level = 4 + (n - k) / 12 + (notes[k][0] >= 'C');
		std::string note;
		Number2Str(level, note);
		note = notes[k] + note;
		return note;
	}

	// Floor frequency according to "twelve-tone equal temperament"
	float FreqAdjust(float f) {
		int n = (int)round(log2f(f / MIDDLE_A_NOTE) * 12.0);
		return (float)MIDDLE_A_NOTE * powf(2.0, (float)n / 12.0f);
	}
}