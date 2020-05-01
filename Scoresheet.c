#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#define VERSION "0.3"
#define BALLS_PER_OVER 6
#define TOTAL_WICKETS 10

/* Use struct for innings */
struct Innings {
        unsigned int runs;
        unsigned short wickets;
        unsigned int balls;
	unsigned int extras;
};

struct Batsman {
	unsigned int runs;
	unsigned int balls;
	bool in;
	bool out;
	char name[20];

}batsman[11];

struct Bowler {
	unsigned int runs;
	unsigned int balls;
	unsigned short wickets;
	char name[20];
}bowler[11];


void welcome();
void print_score(struct Innings innings);
void print_batsman(struct Batsman batsman);
void switch_strike(int *striker, int *non_striker);
void print_scorecard(struct Batsman batsman[11], struct Innings innings);
void setup_batsmen(struct Batsman *player);
void setup_bowlers(struct Bowler *bowlers);
int select_next_batsman(struct Batsman *player);
int select_bowler(struct Bowler *bowlers, int last_bowler);
void print_bowler(struct Bowler bowler);

int main() {
	welcome();
	/* Declare variables */
	struct Innings innings;
	innings.runs = 0;
	innings.wickets = 0;
	innings.balls = 0;
	innings.extras = 0;
	/* Enter the bowlers */
	setup_bowlers(bowler);
	/* Enter the batsmen */
	setup_batsmen(batsman);
	int striker = 0, non_striker = 1;
	batsman[striker].in = true;
	batsman[non_striker].in = true;
	int cur_bowler = select_bowler(bowler, -1);
	printf("Match ready to commence.\n");
	/* Setup batsmen */
	char input[3];
	while(fgets(input, 3, stdin) != NULL & innings.wickets < TOTAL_WICKETS) {
		/* Skip enter */
		if (input[0] == '\n') continue;
		if (input[0] == 'x') return 0;
		if (input[0] == 's') {
			print_scorecard(batsman, innings);
			continue;
		}
		/* Boolean for legal delivery, so we don't switch strike on a no-ball etc. */
		bool legal = true;
		/* Handle input */
		if (input[0] == '.') {
			innings.balls += 1;
			batsman[striker].balls++;
		} else if (input[0] == '!') {
			innings.balls += 1;
			innings.wickets += 1;
			bowler[cur_bowler].wickets++;
			batsman[striker].in = false;
			batsman[striker].out = true;
			striker = select_next_batsman(batsman);
			batsman[striker].in = true;
		} else if (input[0] == 'b' | input[0] == 'l') {
			/* Handle byes and leg byes */
			/* Skip first character, then send rest to atoi */
			int byes = atoi(&input[1]);
			if (byes == 0) {
				printf("You must specify the number of byes.\n");
			}
			innings.runs += byes;
			innings.extras += byes;
			innings.balls++;
			bowler[cur_bowler].balls++;
			batsman[striker].balls++;
		} else if (input[0] == 'w' | input[0] == 'n') {
			legal = false;
			/* Handle wides and no balls in the exact same way */
			int wides = atoi(&input[1]);
			/* Always at least one */
			if (wides == 0) wides = 1;
			innings.runs += wides;
			innings.extras += wides;
			/* Don't increment balls */
			/* if > 1 the batsmen have run, so increment their score + switch strike */
			/* May need to add some further complexity to handle running byes on a wide/no ball */
			if (wides > 1) {
				batsman[striker].runs += wides - 1;
				if (wides - 1 % 2 != 0) {
					switch_strike(&striker, &non_striker);
				}
			}
		} else {
			innings.balls += 1;
			int runs_scored = input[0] - '0';
			/* Convert to int. This works because '0' is also an int, and spec guarantees numbers are sequential */
			innings.runs += runs_scored;
			batsman[cur_bowler].runs += runs_scored;
			batsman[cur_bowler].balls++;
			bowler[cur_bowler].balls++;
			bowler[cur_bowler].runs += runs_scored;
			/* Change the strike if uneven number of balls */
			if (runs_scored % 2 != 0) {
				switch_strike(&striker, &non_striker);
			}
		}
		/* If end of over, switch strike, even if already switched */
		if (legal & innings.balls % BALLS_PER_OVER == 0) {
			switch_strike(&striker, &non_striker);
			cur_bowler = select_bowler(bowler, cur_bowler);
		}
		print_score(innings);
		print_batsman(batsman[striker]);
		print_batsman(batsman[non_striker]);
		print_bowler(bowler[cur_bowler]);
	}
	
	return 0;
}

void welcome() {
	printf("Cricket Scorer\n");
	printf("Version %s\n", VERSION);
}

void print_score(struct Innings innings) {
	printf("%d/%d from %d.%d overs\n", innings.wickets, innings.runs, innings.balls / BALLS_PER_OVER, innings.balls % BALLS_PER_OVER);
	/* Calculate and display run rate */
	double divisor = (innings.balls / BALLS_PER_OVER) + ((innings.balls % BALLS_PER_OVER) / (double) BALLS_PER_OVER);
	double run_rate = innings.runs / divisor;
	printf("Run rate: %.2f\n", run_rate);	

}

void print_batsman(struct Batsman batsman) {
	printf("%s: %d (%d)\n", batsman.name, batsman.runs, batsman.balls);
}

void switch_strike(int *striker, int *non_striker) {
	int swap = *striker;
	*striker = *non_striker;
	*non_striker = swap;
}

void print_scorecard(struct Batsman batsman[11], struct Innings innings) {
	printf("Name\t\tRuns\tBalls\n");
	struct Batsman *p;
	p = &batsman[0];
	for(int i = 0; i < 11; i++) {
		printf("%s", p->name);
		/* Print asterisk if they are in */
		if (p->in) printf("*");
		printf("\t\t%d\t%d\n", p->runs, p->balls);
		p++;
	}
	printf("\n\n");
	printf("Extras: %d\n", innings.extras);
	printf("%d wickets for %d runs \n", innings.wickets, innings.runs);
}

void setup_batsmen(struct Batsman *player) {
	printf("Setup batting team\n\n");
	for (int i = 0; i < 11; i++) {
		player->runs = 0;
		player->balls = 0;
		player->in = false;
		player->out = false;
		printf("Batsman %d: ", i + 1);
		scanf("%s", player->name);
		player++;
	}
}

void setup_bowlers(struct Bowler *bowlers) {
	printf("Setup bowling team\n\n");
	for (int i = 0; i < 11; i++) {
		bowlers->runs = 0;
		bowlers->wickets = 0;
		bowlers->balls = 0;
		printf("Bowler %d: ", i + 1);
		scanf("%s", bowlers->name);
		bowlers++;
	}
}

int select_next_batsman(struct Batsman *players) {
	printf("Select next batsman:\n");
	/* Loop through the list and display any where out = false */
	int valid[11];
	for(int i = 0; i < 11; i++) {
		if ((players->out == false) & (players->in == false)) {
			printf("(%d) %s\n", i, players->name);
			/* Add to valid input array */
			valid[i] = true;
		} else {
			valid[i] = false;
		}
		players++;
	}
	/* User input for which index is batting next */
	/* Loop until valid input */
	while(true) {
		int index, *idx;
		idx = &index;
		printf(">");
		scanf("%d", idx);
		if(*idx >= 11) continue;
		if(valid[*idx] == true) return *idx;
		
	}
}

int select_bowler(struct Bowler *bowlers, int last_bowler) {
	printf("Select next bowler:\n");
	for(int i = 0; i < 11; i++) {
		if (i != last_bowler) {
			printf("(%d) %s\n", i, bowlers->name);
		}
		bowlers++;
	}
	while (true) {
		int index, *i;
		i = &index;
		printf(">");
		scanf("%d", i);
		if ((*i != last_bowler) & (*i < 11)) return *i;
	}
}

void print_bowler(struct Bowler bowler) {
	printf("%s: %d/%d\n", bowler.name, bowler.wickets, bowler.runs);
}
