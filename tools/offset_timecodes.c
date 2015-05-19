#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "ctype.h"

// By Andrew Johnson
//
// Subtract or add an offset to all of the timecodes in a day*.html.md file
// Useful if you annotate timecodes from the Twitch feed instead of the youtube one
// just run this tool on your .md file and subtract the prestream time.
//
// Compile for linux / unix:
// gcc offset_timecodes.c -o offset_timecodes
// Compile for msvc on windows:
// CL offset_timecodes.c

// Run executable on the commandline for usage instructions.

// Note, this is not robust at all... if you feed it a syntatically invalid file or
// a bogus offset, it will probably just segfault or print absurd values.

// Convert timecode (e.g. "1:30:26" or "0:21" or "36") into seconds
inline int timecodeToSeconds(char *timeStr)
{
	int numsecs = 0;
	int exp = 1;
	char *ltime = 0;
	int tval;
	int sign = 1;

	while(ltime != timeStr) {
		ltime = strrchr(timeStr, ':');
		if(ltime) {
			*ltime++ = 0;
		} else {
			ltime = timeStr;
		}
		tval = atoi(ltime);
		if(tval<0) {
			sign = -1;
			tval *= -1;
		}
		numsecs += tval * exp;
		exp *= 60;
	}

	return numsecs * sign;
}

int main(int argc, char **argv)
{
	const unsigned MAXLINESIZE = 32*1024;
	const unsigned SECONDS_IN_MINUTE = 60;
	const unsigned SECONDS_IN_HOUR = 3600;

	char line[MAXLINESIZE];
	char *linepart;
	int offset = 0;
	if(argc >= 2) {
		offset = timecodeToSeconds(argv[1]);
	} else {
		printf(
		"Usage:\n"
		"Subtract 20 seconds from all timecodes. Read from day030.html.md, output to newday030.html.md:\n"
		"	%s -20 < day030.html.md > newday030.html.md\n"
		"Add 10 seconds to all timecodes.:\n"
		"	%s 20 < day030.html.md > newday030.html.md\n"
		"Subtract one minute and 30 seconds from all timecodes:\n"
		"	%s -1:30 < day030.html.md > newday030.html.md\n"
		, argv[0], argv[0], argv[0]);
		return 1;
	}

	while( fgets(line, MAXLINESIZE, stdin) ) {
		linepart = strtok(line, "\"");
		int gotTimecode = 0;
		while( linepart ) {
			if(!gotTimecode && isdigit(linepart[0])) {
				int numsecs = timecodeToSeconds(linepart);
				numsecs += offset;

				unsigned newhours = numsecs / SECONDS_IN_HOUR;
				unsigned newminutes = (numsecs % SECONDS_IN_HOUR) / (SECONDS_IN_MINUTE);
				unsigned newseconds = (numsecs % SECONDS_IN_MINUTE);

				if(newhours) {
					printf("\"%i:%02i:%02i", newhours, newminutes, newseconds);
				} else {
					// Only print hours field if necessary
					printf("\"%02i:%02i", newminutes, newseconds);
				}
				gotTimecode = 1;
			} else {
				if(gotTimecode)
					fputs("\"",stdout);
				fputs(linepart, stdout);
			}
			linepart = strtok(NULL, "\"");
		}
	}
	return 0;
}

