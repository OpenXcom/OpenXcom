#pragma once
/* ADLPLAYER.H
 *
 * player functions for midi-like adlib music
 *
 */

//MAIN FUNCTION - instantly stops music
void func_mute();
//MAIN FUNCTION - runs 1 tick of music according tempo
void func_play_tick();
//MAIN FUNCTION - setup music for playing
void func_setup_music(unsigned char* music_ptr, int length);
//MAIN FUNCTION - initialize fade procedure
void func_fade();
bool func_is_music_playing();
void func_set_music_tempo(int value);
void func_set_music_volume(int value);
int func_get_polyphony();
void func_save_music_state(int i);
void func_load_music_state(int i);
