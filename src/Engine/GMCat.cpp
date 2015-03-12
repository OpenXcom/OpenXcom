/*
 * Copyright 2010-2015 OpenXcom Developers.
 *
 * This file is part of OpenXcom.
 *
 * OpenXcom is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenXcom is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "GMCat.h"
#include <vector>
#include "Music.h"

namespace OpenXcom
{

static inline unsigned read_uint32_le (const unsigned char *p)
{
return ((unsigned) p[0]) + (((unsigned) p[1]) << 8)
	+ (((unsigned) p[2]) << 16) + (((unsigned) p[3]) << 24);
}

/// MIDI sequence.
struct seq {
	unsigned size;
	const unsigned char *data;
};

/// MIDI track.
struct track {
	struct seq seq;
	unsigned channel;
};

/// MIDI stream.
struct gmstream {
	int tempo, nsubs, ntracks;
	struct seq subs [256];
	struct track tracks [256];
};

static int gmext_read_stream (struct gmstream *p,
	unsigned int n, const unsigned char *data)
{
	if (!n--)
		return -1;
	p->tempo = *data++;

	// subsequences
	if (!n--)
		return -1;
	p->nsubs = *data++;

	for (int i=0; i<p->nsubs; ++i) {
		if (n < 4)
			return -1;
		unsigned int s = read_uint32_le(data);
		if (s < 4)
			return -1;
		p->subs[i].size = s - 4;
		p->subs[i].data = data + 4;
		n -= s;
		data += s;
	}

	// tracks
	if (!n--)
		return -1;
	p->ntracks = *data++;

	for (int i=0; i<p->ntracks; ++i) {
		if (n-- < 5)
			return -1;
		p->tracks[i].channel = *data++;
		unsigned int s = read_uint32_le(data);
		if (s < 4)
			return -1;
		p->tracks[i].seq.size = s - 4;
		p->tracks[i].seq.data = data + 4;
		n -= s;
		data += s;
	}

	return n ? -1 : 0;
}

static inline void gmext_write_int16 (std::vector<unsigned char> &midi,
	unsigned int n)
{
	midi.push_back(n >> 8);
	midi.push_back(n);
}

static inline void gmext_write_delta (std::vector<unsigned char> &midi,
	unsigned int delta)
{
	unsigned char data[4];
	unsigned int i = 0;

	delta &= ((1<<28)-1);
	do {
		data[i++] = delta & 0x7F;
		delta >>= 7;
	} while (delta > 0);

	while (--i)
		midi.push_back(data[i] | 0x80);

	midi.push_back(data[0]);
}

static inline void gmext_write_tempo_ev (std::vector<unsigned char> &midi,
	unsigned int tempo)
{
	midi.push_back(0xFF);
	midi.push_back(0x51);
	midi.push_back(3);
	tempo = 60000000 / tempo;
	midi.push_back(tempo >> 16);
	midi.push_back(tempo >> 8);
	midi.push_back(tempo);
}

static inline void gmext_write_end_ev (std::vector<unsigned char> &midi)
{
	midi.push_back(0xFF);
	midi.push_back(0x2F);
	midi.push_back(0);
}

static const unsigned int volume [0x80] = {
	100,100,100,100,100, 90,100,100,100,100,100, 90,100,100,100,100,
	100,100, 85,100,100,100,100,100,100,100,100,100, 90,90, 110, 80,
	100,100,100, 90, 70,100,100,100,100,100,100,100,100,100,100,100,
	100,100, 90,100,100,100,100,100,100,120,100,100,100,120,100,127,
	100,100, 90,100,100,100,100,100,100, 95,100,100,100,100,100,100,
	100,100,100,100,100,100,100,115,100,100,100,100,100,100,100,100,
	100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,
	100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,
};

/// Output status.
struct output_status {
	unsigned int delta;
	unsigned int patch;
	unsigned char prevcmd;
};

static int gmext_write_sequence (std::vector<unsigned char> &midi,
	const struct gmstream *stream, unsigned int channel,
	const struct seq *seq, struct output_status *status)
{
	const unsigned char *data = seq->data;
	unsigned int left = seq->size;

	unsigned char cmd = -1;

	while (left) {

		// read delta
		unsigned int ndelta = 0;

		for (int i=0; ; ) {
			unsigned char c = *data++;
			left--;
			ndelta += c & 0x7F;
			if (!(c & 0x80))
				break;
			if ((++i == 4) || !left)
				return -1;
			ndelta <<= 7;
		}

		status->delta += ndelta;

		// read cmd byte
		if (!left)
			return -1;

		if (*data & 0x80) {
			// actual cmd byte
			cmd = *data++;
			left--;
			switch (cmd) {
				case 0xFF:	  // end track
				case 0xFD:	  // end subsequence
					return 0;
				case 0xFE:	  // insert subsequence
					if (!left--)
						return -1;
					if (*data >= stream->nsubs)
						// invalid subsequence
						return -1;
					if (gmext_write_sequence(midi,
						stream, channel,
						&stream->subs[*data++], status)
							== -1)
						return -1;
					cmd = 0;
					continue;
				default:
					cmd &= 0xF0;
			}
		} else if (cmd == 0)
			return -1;	  // invalid running mode

		if (!left--)
			return -1;
		unsigned char data1 = *data++;

		switch (cmd) {

			case 0x80:
			case 0x90: {
				if (!left--)
					return -1;
				unsigned char data2 = *data++;
				if (data2)
					data2 = (unsigned int) data2 *
						(channel==9 ? 80 : volume[status->patch]) >> 7;
				gmext_write_delta(midi, status->delta);
				midi.push_back(cmd | channel);
				midi.push_back(data1);
				midi.push_back(data2);
				} break;

			case 0xC0:
				if (data1 == 0x7E)
					return 0;	   // restart stream
				status->patch = data1;
				if ((data1 == 0x57) || (data1 == 0x3F))
					data1 = 0x3E;
				gmext_write_delta(midi, status->delta);
				midi.push_back(cmd | channel);
				midi.push_back(data1);
				break;

			case 0xB0: {
				if (!left--)
					return -1;
				unsigned char data2 = *data++;
				if (data1 == 0x7E)
					continue;
				if (!data1) {
					if (!data2)
						continue;
					gmext_write_delta(midi, status->delta);
					gmext_write_tempo_ev(midi, 2*data2);
					break;
				}
				if (data1 == 0x5B)
					data2 = 0x1E;
				gmext_write_delta(midi, status->delta);
				midi.push_back(cmd | channel);
				midi.push_back(data1);
				midi.push_back(data2);
				} break;

			case 0xE0: {
				if (!left--)
					return -1;
				unsigned char data2 = *data++;
				gmext_write_delta(midi, status->delta);
				midi.push_back(cmd | channel);
				midi.push_back(data1);
				midi.push_back(data2);
				} break;

			default:		// unhandled cmd byte
				return -1;
		}

		status->delta = 0;
	}

	return 0;
}

static int gmext_write_midi (const struct gmstream *stream,
	std::vector<unsigned char> &midi)
{
	// write MIDI file header
	static const unsigned char midi_file_signature[8] =
		{ 'M','T','h','d',0,0,0,6 };
	for (int i=0; i<8; ++i)
		midi.push_back(midi_file_signature[i]);
	gmext_write_int16(midi, 1);
	gmext_write_int16(midi, stream->ntracks + 1);
	gmext_write_int16(midi, 24);

	// write global tempo track
	static const unsigned char midi_track_header[8] =
		{ 'M','T','r','k',0,0,0,11 };
	for (int i=0; i<8; ++i)
		midi.push_back(midi_track_header[i]);
	gmext_write_delta(midi, 0);
	gmext_write_tempo_ev(midi, stream->tempo);
	gmext_write_delta(midi, 0);
	gmext_write_end_ev(midi);

	// write tracks
	for (int j=0; j<stream->ntracks; ++j) {

		// header
		for (int i=0; i<4; ++i)
			midi.push_back(midi_track_header[i]);

		size_t loffset = midi.size();
		for (int i=0; i<4; ++i)
			midi.push_back(0);

		// initial data
		static const unsigned char midi_track_init[8] =
			{ /* 0, 0xB0, */ 0x78, 0, 0, 0x79, 0, 0, 0x7B, 0 };
		midi.push_back(0);
		midi.push_back(0xB0 | stream->tracks[j].channel);
		for (int i=0; i<8; ++i)
			midi.push_back(midi_track_init[i]);

		// body
		struct output_status status = { 0, 0, 0 };
		if (gmext_write_sequence(midi, stream,
				stream->tracks[j].channel,
				&stream->tracks[j].seq, &status) == -1)
			return -1;

		// end of track
		gmext_write_delta(midi, status.delta);
		gmext_write_end_ev(midi);

		// rewrite track length
		unsigned char *p = &midi[loffset];
		size_t length = midi.size() - loffset - 4;
		p[0] = length >> 24;
		p[1] = length >> 16;
		p[2] = length >> 8;
		p[3] = length;
	}

	return 0;
}

/**
 * Loads a MIDI object into memory.
 * @param i Music number to load.
 * @return Pointer to the loaded music.
 */
Music *GMCatFile::loadMIDI(unsigned int i)
{
	Music *music = new Music;

	unsigned char *raw = static_cast<unsigned char*> ((void*)load(i));

	if (!raw)
		return music;

	// stream info
	struct gmstream stream;
	if (gmext_read_stream(&stream, getObjectSize(i), raw) == -1) {
		delete[] raw;
		return music;
	}

	std::vector<unsigned char> midi;
	midi.reserve(65536);

	// fields in stream still point into raw
	if (gmext_write_midi(&stream, midi) == -1) {
		delete[] raw;
		return music;
	}

	delete[] raw;

	music->load(&midi[0], midi.size());

	return music;
}

}
