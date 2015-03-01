/* Computing probability of winning grand prize by Darky
 * Written for #VIzon on irc.rizon.net on 13/01/2014
 *
 * The newest (and fastest) grand prize calculator !
 *
 * Based on splotch's version, which was itself a huge improvement on my 1st version
 * Both of these older versions used recursion.
 *
 * Refer to http://s.rizon.net/VIzon for more info
 *
 */

/** CHANGE THESE VALUES TO YOUR LIKING, THEN COMPILE && EXECUTE **/ // e.g.: gcc -std=c99 vizon.c -o vizon && ./vizon
#define DRAWS 100
#define DAYS_LEFT 0
#define DAYS_WON 0
#define MULTIPLIER 1 // MINIMUM 1


#include <stdio.h>

#define NUM_PRIZES 5
long double P_PROB[NUM_PRIZES];
const int P_BANK[NUM_PRIZES] = { 120, 60, 30, 10, 0 };
const int P_WINS[NUM_PRIZES] = {  12,  6,  3,  1, 0 }; // divided by 10
const int P_MULT[NUM_PRIZES] = {   1,  1,  1,  0, 0 };

#define MAX_DRAWS DRAWS
#define MAX_MULT 3 // mult == 4 => 3 "mult" prizes won => minimum == 180 (30*1 + 30*2 + 30*3) => won
#define MAX_WINS 18 // Won at 18
#define MAX_BANK 170 // wins*10

long double memo[MAX_DRAWS][MAX_MULT][MAX_WINS][MAX_BANK];

long double binomial(int n, int k)
{
   long double retval = 1;
   if (k > n/2)
      k = n - k;
   for (int i = 1; i <= k; ++i)
      retval *= (long double) (n - i + 1) / i;
   return retval;
}

long double hypergeom(int n, int k, int b)
{
   return binomial(k,b) * binomial(n-k,k-b) / binomial(n,k);
}

void calcprobs()
{
   P_PROB[0] = hypergeom(29,6,6);
   P_PROB[1] = hypergeom(29,6,5);
   P_PROB[2] = hypergeom(29,6,4);
   P_PROB[3] = hypergeom(29,6,3);
   P_PROB[4] = hypergeom(29,6,2) + hypergeom(29,6,1) + hypergeom(29,6,0);
}

int main()
{
   calcprobs();

   int new_win, new_bank, new_mult;
   for (int draws_left = 0; draws_left < MAX_DRAWS; draws_left++)
   {
      for (int wins = 0; wins < MAX_WINS; wins++) // Total number of days (/10) you have already won
      {
         for (int bank = 0; bank <= wins * 10 && bank < MAX_BANK; bank++) // Can't have more days left than days you've already won
         {
            for (int mult = 0; mult < MAX_MULT; mult++) // There's a +1 somewhere else (so 1 <= mult <= 3)
            {
               if ((wins < 3 && mult > 0) || (wins < 9 && mult > 1)) break; // these are impossible to achieve in a normal setting (trust me)
               memo[draws_left][mult][wins][bank] = 0;
               for (int prize = 0; prize < NUM_PRIZES; prize++) // don't worry that's the last for loop
               {
                  new_win = wins + P_WINS[prize] * (1 + mult * P_MULT[prize]);
                  new_bank = bank + P_BANK[prize] * (1 + mult * P_MULT[prize]);
                  new_mult = mult + P_MULT[prize];

                  if (new_win >= 18) // won !
                     memo[draws_left][mult][wins][bank] += P_PROB[prize];

                  else if (draws_left >= 1 && draws_left % 3 != 2 && new_bank > 2) // 3 draws a week -> 2 + 2 + 3 = 7 days
                     memo[draws_left][mult][wins][bank] += P_PROB[prize] * memo[draws_left - 1][new_mult][new_win][new_bank - 2];

                  else if (draws_left >= 1 && new_bank > 3)
                     memo[draws_left][mult][wins][bank] += P_PROB[prize] * memo[draws_left - 1][new_mult][new_win][new_bank - 3];

                  else if (draws_left >= 1) // damn I forgot this clause at first... Made everything buggy :(
                     memo[draws_left][mult][wins][bank] += P_PROB[prize] * memo[draws_left - 1][0][0][0]; // Lost your VIP, but you continue playing

                  else; // nothing (probability of winning if you haven't won yet and decide to stop playing is... 0)
               }
            }
         }
      }
   }
   printf("VIP status :\n Multiplication factor: %d\n Total accumulated days of current VIP: %d\n Remaining time of current VIP: %d\n", MULTIPLIER, DAYS_WON, DAYS_LEFT);
   printf("Chances of winning grand prize after %d draws: %Lf%% or 1 / %Lf\n", DRAWS, memo[DRAWS - 1][MULTIPLIER - 1][DAYS_WON/10][DAYS_LEFT] * 100, 1 / memo[DRAWS - 1][MULTIPLIER - 1][DAYS_WON/10][DAYS_LEFT]);
}
