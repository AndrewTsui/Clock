/*
 * GccApplication2.c
 *
 * Created: 4/18/2019 10:02:06 AM
 * Author : tsuiat
 */ 

#include <avr/io.h>
#include "avr.h"
#include "lcd.h"
#include <stdio.h>

typedef struct time {
	int hour;
	int min;
	int sec;
	int subsec;
	char shift;
} time;

typedef struct timer {
	int hour;
	int min;
	int sec;
	int subsec;
	int finished;
} timer;

typedef struct stopwatch {
	int hour;
	int min;
	int sec;
	int subsec;
	int lap;
} stopwatch;


typedef struct alarm {
	int hour;
	int min;
	int sec;
	char shift;
} alarm;

time t;
timer tr;
stopwatch s;
time stop_t;
alarm alarms[] = {{0, 0, 0, 'N'}, {0, 0, 0, 'N'}, {0, 0, 0, 'N'}, {0, 0, 0, 'N'}, {0, 0, 0, 'N'}};
int index = 0, alarm_flag = 0;

int get_key();
int is_pressed(int r, int c);
void update_time();
void update_timer();
void set_time();
void set_timer();
void print_timer_time();
void print_stand_time();
void play_note_alarm();
void play_note_timer();
void print_world_time();
void print_stop_time();
void set_alarm();
void run_stopwatch();
int check_alarm();

int main(void)
{
	SET_BIT(DDRB, 3);
	DDRC = 0;
    int k, flag = 0, timer_flag = 0;
	lcd_init();
	lcd_clr();
	char* welcome_msg = "LCD ON";
	lcd_puts2(welcome_msg);
	avr_wait(1000);
	lcd_clr();
	t.hour = 5, t.min = 35, t.sec = 00, t.subsec = 0, t.shift = 'P';
    while (1) 
    {
		lcd_clr();
		print_stand_time();
		k = get_key();
		if (flag == 0) {
			flag = 1;
			// t.shift = 'P';
			continue;
		}
		if(k == 4) { //set alarm
			alarm_flag = 1;
			avr_wait(200);
			set_time();
			avr_wait(200);
		}
		if(k == 8) { //set timer
			timer_flag = 1;
			avr_wait(200);
			// tr.hour = 0, tr.min = 1, tr.sec = 3, tr.subsec = 0;
			set_timer();
			avr_wait(200);
		}
		if(k == 12) { //stopwatch
			run_stopwatch();
		}
		if(k == 16) { //world time
			print_world_time();
		}
		if(timer_flag == 1) {
			print_timer_time();
			update_timer();
			update_time();
			if(tr.finished == 1) {
				timer_flag = 0;
				while ((k = get_key()) != 14) {
					play_note_timer();
					avr_wait(300);
					update_time();
				}
				tr.finished = 0;
			}
		}
		if(alarm_flag == 1) {
			int i = check_alarm();
			if( i == 1 ) {
				while((k = get_key()) != 14) {
					play_note_alarm();
					avr_wait(100);
					update_time();
					print_stand_time();
				}
				i = 0;
			}
		}
		update_time();
		avr_wait(100);
	}
}

int get_key() {
	int r, c;
	for (r = 0; r < 4; ++r) {
		for (c = 0; c < 4; ++c) {
			if (is_pressed(r, c)) {
				return (c * 4 + r) + 1;   // changed from default code
			}
		}
	}
	return 0;
}

int is_pressed(int r, int c) {
	DDRC = 0;
	SET_BIT(DDRC, c);
	CLR_BIT(PORTC, c);
	SET_BIT(PORTC, r+4);
	if (!GET_BIT(PINC, r+4)) {
		return 1;
	}
	return 0;
}

int check_alarm() {
	int i, count = 0;
	for(i = 0; i < index; ++i) {
		if(alarms[i].hour == 0 && alarms[i].min == 0 && alarms[i].sec == 0 && alarms[i].shift == 'N') {
			++count;
			continue;
		}
		else if(alarms[i].hour == t.hour && alarms[i].min == t.min && alarms[i].sec == t.sec && alarms[i].shift == t.shift) {
			alarms[i].hour = 0;
			alarms[i].min = 0;
			alarms[i].sec = 0;
			alarms[i].shift = 'N';
			return 1;
		}
	}
	if(count == 5)
		alarm_flag = 0;
	return 0;
}

void print_stand_time() {
	lcd_clr();
	lcd_puts2("CURRENT TIME");
	lcd_pos(1,0);
	char r[17];
	sprintf(r, "%02d:%02d:%02d%cM", t.hour, t.min, t.sec, t.shift);
	lcd_puts2(r);
}

void print_stop_time() {
	char r[17];
	char t[17];
	lcd_clr();
	lcd_pos(0,0);
	lcd_puts2("SW: ");
	sprintf(r, "%02d:%02d:%02d", s.hour, s.min, s.sec);
	lcd_puts2(r);
	lcd_pos(1,0);
	lcd_puts2("Lap: ");
	sprintf(t, "%02d:%02d:%02d", stop_t.hour, stop_t.min, stop_t.sec);
	lcd_puts2(t);
}

void print_timer_time() {
	lcd_clr();
	lcd_pos(0, 0);
	char s[17];
	sprintf(s, "TIMER");
	lcd_puts2(s);
	lcd_pos(1,0);
	char r[17];
	sprintf(r, "%02dH %02dM %02dS", tr.hour, tr.min % 60, tr.sec % 60);
	lcd_puts2(r);
}

void print_world_time() {
	char* places[] = {"Los Angeles", "New York", "London", "Dubai", "Tokyo"};
	int time_diff[] = {0, 3, 8, 11, 16};
	int k, i = 0;
	while((k = get_key()) != 14) {
		if(k == 13) { //scroll left
			if( i == 0)
				i = 4;
			else
				i--;
		}
		if(k == 15) { //scroll right
			if(i==4)
				i = 0;
			else
				i++;
		}
		char s[17];
		lcd_clr();
		sprintf(s, "%s +%dH", places[i], time_diff[i]);
		lcd_pos(0,0);
		lcd_puts2(s);
		char r[17];
		lcd_pos(1,0);
		int pre_time = t.hour+time_diff[i];
		char c;
		if (pre_time >= 12) {
			if (t.shift == 'P')
				c = 'A';
			else
				c = 'P';
		}
		else
			c = t.shift;
		int mod_time = (t.hour+time_diff[i]) % 12;
		if(mod_time == 0)
			mod_time = 12;
		sprintf(r, "%02d:%02d:%02d%cM", mod_time, t.min, t.sec, c);
		lcd_puts2(r);
		update_time();
		avr_wait(100);
	}
}

void run_stopwatch() {
	lcd_clr();
	lcd_pos(0,0);
	lcd_puts2("SW: 00:00:00");
	lcd_pos(1,0);
	lcd_puts2("Lap: 00:00:00");
	avr_wait(500);
	int k;
	stop_t.hour = 0, stop_t.min = 0, stop_t.sec = 0, stop_t.subsec = 0;
	s.hour = 0, s.min = 0, s.sec = 0, s.subsec = 0;
	while((k = get_key()) != 14) {
		if(k == 16) { //start/stop
			while((k = get_key()) != 15) {
				s.subsec++;
				if (s.subsec == 10) {
					s.subsec = 0;
					s.sec++;
				}
				if (s.sec == 60) {
					s.sec = 0;
					s.min++;
				}
				if (s.min == 60) {
					s.min = 0;
					s.hour++;
				}
				if(k == 13) {
					stop_t.hour = s.hour;
					stop_t.min = s.min;
					stop_t.sec = s.sec;
				}
				print_stop_time();
				avr_wait(100);		
			}
		}
		if (k == 12) {
			stop_t.hour = 0;
			stop_t.min = 0;
			stop_t.sec = 0;
			s.hour = 0;
			s.min = 0;
			s.sec = 0;
			print_stop_time();
		}
	}
	
}

void update_time() {
	t.subsec++;

	if (t.subsec == 10) {
		t.subsec = 0;
		t.sec++;
	}
	if (t.sec == 60) {
		t.sec = 0;
		t.min++;
	}
	if (t.min == 60) {
		t.min = 0;
		t.hour++;
	}
	if (t.hour == 12 && t.min == 0 && t.sec == 0 && t.subsec == 0 && t.shift == 'P') {
		t.shift = 'A';
	}
	else if (t.hour == 12 && t.min == 0 && t.sec == 0 && t.subsec == 0 && t.shift == 'A') {
		t.shift = 'P';
	}
	if (t.hour == 13 ) {
		t.hour = 1;
	}
}

void update_timer() {
	tr.subsec++;

	if (tr.subsec == 10) {
		tr.subsec = 0;
		tr.sec--;
	}	
	if (tr.sec > 0) {
		tr.hour = tr.sec / 3600;
		tr.min = tr.sec/60;
	}
	else {
		tr.finished =1;
		print_timer_time();
		return;
	}
}

void set_timer() {
	int time[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	lcd_clr();
	lcd_pos(0,0);
	lcd_puts2("SET TIMER");
	lcd_pos(1,0);
	lcd_puts2("00H 00M 00S");
	avr_wait(500);
	char buf[1];
	int k, i = 0;
	while (i < 11) {
		if (i == 2 || i == 7 || i == 3 || i == 6 || i == 10) { 
			if(i == 3 || i == 7) {
				lcd_pos(1, i);
				lcd_put(' ');
				avr_wait(200);
				++i;
				continue;
			}
			else if(i == 2) {
				lcd_pos(1, i);
				lcd_put('H');
				avr_wait(200);
				++i;
				continue;
			}
			else if(i == 6) {
				lcd_pos(1, i);
				lcd_put('M');
				avr_wait(200);
				++i;
				continue;
			}
			else {
				lcd_pos(1, i);
				lcd_put('S');
				avr_wait(200);
				++i;
				continue;
			}
		}
		while((k = get_key()) != 16) {
			if (k == 15) {
				if( i == 0 ) {
					if(time[i] == 2)
						time[i] = 0;
					else
						time[i]++;
				}
				else if (i == 1) {
					if(time[0] == 0 || time[0] == 1)
					{
						if(time[i] == 9)
						time[i] = 0;
						else
						time[i]++;
					}
					else {
						if(time[i] == 3)
						time[i] = 0;
						else
						time[i]++;
					}
				}
				else if (i == 4 || i == 8) {
					if (time[i] == 5)
					time[i] = 0;
					else
					time[i]++;
				}
				else if (i == 5 || i == 9) {
					if (time[i] == 9)
					time[i] = 0;
					else
					time[i]++;
				}
			}
			else if (k == 13) {
				if( i == 0 ) {
					if(time[i] == 0)
						time[i] = 2;
					else
						time[i]--;
				}
				else if (i == 1) {
					if(time[0] == 0 || time[0] == 1)
					{
						if(time[i] == 0)
						time[i] = 9;
						else
						time[i]--;
					}
					else {
						if(time[i] == 0)
						time[i] = 3;
						else
						time[i]--;
					}
				}
				else if (i == 4 || i == 8) {
					if (time[i] == 0)
					time[i] = 5;
					else
					time[i]--;
				}
				else if (i == 5 || i == 9) {
					if (time[i] == 0)
					time[i] = 9;
					else
					time[i]--;
				}
			}
			if (k == 12) {
				avr_wait(200);
				if (i == 0)
				continue;
				if (i == 4 || i == 8) {
					i = i - 3;
					continue;
				}
				i--;
				avr_wait(200);
				continue;
			}
			lcd_pos(1, i);
			lcd_put(' ');
			avr_wait(200);
			lcd_pos(1, i);
			sprintf(buf, "%d", time[i]);
			lcd_puts2(buf);
			avr_wait(200);
		}
		if (i == 1) {
			tr.hour = 10*time[0] + 1*time[1];
		}
		if (i == 5) {
			tr.min = 10*time[4] + 1*time[5];
		}
		if (i == 9) {
			tr.sec = 10*time[8] + 1*time[9];
		}
		tr.sec += (3600 * tr.hour);
		tr.sec += (60 * tr.min);
		tr.subsec = 0;
		++i;
		avr_wait(200);
	}
}
void set_time() {
	int time[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	char shift = 'P';
	lcd_clr();
	lcd_pos(0,0);
	if(alarm_flag == 1)
		lcd_puts2("SET ALARM");
	else
		lcd_puts2("SET TIME");
	lcd_pos(1,0);
	lcd_puts2("00:00:00PM");
	avr_wait(500);
	char buf[1];
	int k, i = 0;
	while (i < 9) {
		if (i == 2 || i == 5) {
			lcd_pos(1, i);
			lcd_put(':');
			avr_wait(200);
			++i;
			continue;
		}
		while((k = get_key()) != 16) {
			if (k == 15) {
				if( i == 0 ) {
					if(time[i] == 1)
						time[i] = 0;
					else
						time[i] = 1;
				}
				else if (i == 1) {
					if(time[0] == 0)
					{
						if(time[i] == 9)
							time[i] = 0;
						else
							time[i]++;
					}
					else {
						if(time[i] == 2)
							time[i] = 0;
						else
							time[i]++;
					}
				}
				else if (i == 3 || i == 6) {
					if (time[i] == 5)
						time[i] = 0;
					else
						time[i]++;
				}
				else if (i == 4 || i == 7) {
					if (time[i] == 9)
						time[i] = 0;
					else
						time[i]++;
				}
				else if (i == 8) {
					if (shift == 'A')
						shift = 'P';
					else
						shift = 'A';
				}
			}
			else if (k == 13) {
				if( i == 0 ) {
					if(time[i] == 0)
						time[i] = 1;
					else
						time[i] = 0;
				}
				else if (i == 1) {
					if(time[0] == 0)
					{
						if(time[i] == 0)
							time[i] = 9;
						else
							time[i]--;
					}
					else {
						if(time[i] == 0)
							time[i] = 2;
						else
							time[i]--;
					}
				}
				else if (i == 3 || i == 6) {
					if (time[i] == 0)
						time[i] = 5;
					else
						time[i]--;
				}
				else if (i == 4 || i == 7) {
					if (time[i] == 0)
						time[i] = 9;
					else
						time[i]--;
				}
				else if (i == 8) {
					if (shift == 'P')
						shift = 'A';
					else
						shift = 'P';
				}
			}
			if (k == 12) {
				avr_wait(200);
				if (i == 0)
					continue;
				if (i == 3 || i == 6) {
					i = i - 2;
					continue;
				}
				i--;
				avr_wait(200);
				continue;
			}
			lcd_pos(1, i);
			lcd_put(' ');
			avr_wait(200);
			lcd_pos(1, i);
			if (i != 8) {
				sprintf(buf, "%d", time[i]);
				lcd_puts2(buf);
			}
			else
				lcd_put(shift);
			avr_wait(200);
		}
		if (alarm_flag == 1) {
			if (i == 1) {
				alarms[index].hour = 10*time[0] + 1*time[1];
			}
			if (i == 4) {
				alarms[index].min = 10*time[3] + 1*time[4];
			}
			if (i == 7) {
				alarms[index].sec = 10*time[6] + 1*time[7];
			}
			if (i == 8) {
				alarms[index].shift = shift;
			}
		}
		else {
			if (i == 1) {
				t.hour = 10*time[0] + 1*time[1];
			}
			if (i == 4) {
				t.min = 10*time[3] + 1*time[4];
			}
			if (i == 7) {
				t.sec = 10*time[6] + 1*time[7];
			}
			if (i == 8) {
				t.shift = shift;
			} 
		}
		++i;
		avr_wait(100);
	}
	if(alarm_flag == 1)
		index++;
}

void play_note_alarm() {
	int i;
	int pitch_temp = 48;
	for(i = 0; i < 10; ++i)
	{
		SET_BIT(PORTB, 3);
		note_avr_wait(pitch_temp);
		CLR_BIT(PORTB, 3);
		note_avr_wait(pitch_temp);
	}
}
void play_note_timer() {
	int i;
	int pitch_temp = 48;
	for(i = 0; i < 600; ++i)
	{
		SET_BIT(PORTB, 3);
		note_avr_wait(pitch_temp);
		CLR_BIT(PORTB, 3);
		note_avr_wait(pitch_temp);
	}
}