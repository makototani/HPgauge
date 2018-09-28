/*
 * gomoku.c
 *
 *  Created on: 2017/06/30
 *      Author: b1016112 Daichi Matsumoto
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define GROUND_X 12
#define GROUND_Y 12
#define P1  1
#define P2  2
#define win 5

typedef struct{
	int n;
	int x;
	int y;
}heap;

typedef struct{
	int p;
    char *name;
}Player_ID;

int atkn=0;
int defn=0;

int judge(int p, int ground[GROUND_Y][GROUND_X]){
	int judge = 0;
	int sum=0;
	int i,j,k;

	for(i=1;i<GROUND_Y-1;i++){
		for(j=1;j<GROUND_X-1;j++){
			if(ground[i][j] == p) sum++;
			else sum=0;
			if(sum==win)judge=1;
			else if(sum>win)judge=0;
		}
		sum=0;
	}
	for(i=1;i<GROUND_X-1;i++){
		for(j=1;j<GROUND_Y-1;j++){
			if(ground[j][i] == p) sum++;
			else sum=0;
			if(sum==win)judge=1;
			else if(sum>win)judge=0;
		}
		sum=0;
	}

	for(j=1;j<GROUND_Y-1;j++){
		for( i=1, k=j ; (i<GROUND_X-1, k<GROUND_Y-1) ; i++, k++){
			if(ground[k][i] == p) sum++;
			else sum=0;
			if(sum==win)judge=1;
			else if(sum>win)judge=0;
		}
		sum=0;
	}

	for(j=1;j<GROUND_X-1;j++){
		for( i=j, k=1 ; (i<GROUND_X-1, k<GROUND_Y-1) ; i++, k++){
			if(ground[k][i] == p) sum++;
			else sum=0;
			if(sum==win)judge=1;
			else if(sum>win)judge=0;
		}
		sum=0;
	}

	for(j=GROUND_X-2;j>0;j--){
		for( i=j, k=1 ; i>0, k<GROUND_Y-1 ; i--, k++){
			if(ground[k][i] == p) sum++;
			else sum=0;
			if(sum==win)judge=1;
			else if(sum>win)judge=0;
		}
		sum=0;
	}

	for(j=1;j<GROUND_Y-1;j++){
		for( i=GROUND_X-2, k=j ; (i>0, k<GROUND_Y-1) ; i--, k++){
			if(ground[k][i] == p) sum++;
			else sum=0;
			if(sum==win)judge=1;
			else if(sum>win)judge=0;
		}
		sum=0;
	}

	for(i=1;i<GROUND_Y-1;i++){
		for(j=1;j<GROUND_X-1;j++){
			if(ground[i][j] == 0) sum++;
		}
	}
	if(sum==0)judge=2;
	return judge;
}

int judge_side_effective(int run_case,int ground[GROUND_Y][GROUND_X],int dist,int l,int m){
	int plus;
	int d=dist+1;
	switch(run_case){
	case 1:
		if(ground[l][m]==0&&ground[l-d][m-d]==0) plus=2;
		else plus=1;
		break;
	case 2:
		if(ground[l][m]==0&&ground[l+d][m+d]==0) plus=2;
		else plus=1;
		break;
	case 3:
		if(ground[l][m]==0&&ground[l-d][m+d]==0) plus=2;
		else plus=1;
		break;
	case 4:
		if(ground[l][m]==0&&ground[l+d][m-d]==0) plus=2;
		else plus=1;
		break;
	case 5:
		if(ground[l][m]==0&&ground[l][m-d]==0) plus=2;
		else plus=1;
		break;
	case 6:
		if(ground[l][m]==0&&ground[l][m+d]==0) plus=2;
		else plus=1;
		break;
	case 7:
		if(ground[l][m]==0&&ground[l-d][m]==0) plus=2;
		else plus=1;
		break;
	case 8:
		if(ground[l][m]==0&&ground[l+d][m]==0) plus=2;
		else plus=1;
		break;
	}
	return plus;
}

int N_case(int n_case){
	int move=0;
	switch(n_case){
	case 1:
		move=1;
		break;
	case 2:
		move=-1;
		break;
	case 3:
		move=0;
		break;
	}
	return move;
}

void player_exchange(int *player){
	switch(*player){
	case P1:
		*player = P2;
		break;
	case P2:
		*player = P1;
		break;
	}
}

void prot(int *player, int ground[GROUND_Y][GROUND_X] ,int s,int t){
	if(ground[s][t] !=0){
		printf("Already put\n");
		player_exchange(player);
	}else if(ground[s][t] == 0){
		if(*player == P1) ground[s][t] = 1;
		else if (*player == P2) ground[s][t] = 2;
	}
	for(int i=0;i<GROUND_Y;i++){
		for(int j=0;j<GROUND_X;j++){
			if(ground[i][j]==1) printf("  O  ");
			else if(ground[i][j]==2) printf("  X  ");
			else if(ground[i][j]==-1){
				if((i==0&&j==0)||(i==0&&j==GROUND_X-1)||(i==GROUND_Y-1&&j==0)||(i==GROUND_Y-1&&j==GROUND_X-1)) printf(" ### ");
				else if(i==0||i==GROUND_Y-1) printf(" #%2d ",j);
				else if(j==0||j==GROUND_X-1) printf(" #%2d ",i);

			}
			else printf("  -  ");
		}
		printf("\n");

	}
}

void CPU(int p, int ground[GROUND_Y][GROUND_X]){
	int out = 0;
	int sum=0;
	int def=0;
	int flag=0;
	int flag_emp=0;
	int effective=0;
	int more_effective=0;
	int emp=0;
	int run_case;
	int risk;
	int i,j,k,l,m;
	heap keep;
	heap beep;
	keep.n=0;beep.n=0;

	/*---def protocol---*/

	//puts("case1");
	for(def=0,j=1;j<GROUND_Y-1 && flag==0;j++){
		for( i=1, k=j ; (i<GROUND_X-1, k<GROUND_Y-1) && flag==0 ; i++, k++){
			if(ground[k][i] != p && ground[k][i] != 0 && ground[k][i] != -1)def++;
			else def=0;
			for(int min=win-2;min>0;min--){
				if(def==win-min && ground[k+1][i+1]==0){
					run_case=1;
					l=k+1;
					m=i+1;
					risk=judge_side_effective(run_case,ground,def,l,m);
					for(emp=0,more_effective=0,effective=0;ground[l+1][m+1]!=-1&&ground[l+1][m+1]!=p;effective++,l++,m++){
						if(ground[l+1][m+1]!=p&&ground[l+1][m+1]!=0){
							more_effective++;
							if(ground[l][m]==0) emp++;
						}
					}
					if(def+more_effective<win){
						if(keep.n<=def+more_effective+risk&&effective+def>=win-1){
							out=1;
							keep.n=def+more_effective+risk;
							keep.x=i+1;
							keep.y=k+1;
							flag=0;
						}
					}
					else{
						if(keep.n<=def+risk&&effective+def>=win-1){
							out=1;
							keep.n=def+risk;
							keep.x=i+1;
							keep.y=k+1;
							flag=0;
						}
					}
				}
				if(def==win-min && ground[k-def][i-def]==0 ){
					run_case=2;
					l=k-def;
					m=i-def;
					risk=judge_side_effective(run_case,ground,def,l,m);
					for(emp=0,more_effective=0,effective=0;ground[l-1][m-1]!=-1&&ground[l-1][m-1]!=p;effective++,l--,m--){
						if(ground[l-1][m-1]!=p&&ground[l-1][m-1]!=0){
							more_effective++;
							if(ground[l-1][m-1]==0) emp++;
						}
					}
					if(def+more_effective<win){
						if(keep.n<=def+more_effective+risk&&effective+def>=win-1){
							out=1;
							keep.n=def+more_effective+risk;
							keep.x=i-def;
							keep.y=k-def;
							flag=0;
						}
					}
					else{
						if(keep.n<=def+risk&&effective+def>=win-1){
							out=1;
							keep.n=def+risk;
							keep.x=i-def;
							keep.y=k-def;
							flag=0;
						}
					}
				}

			}
		}
		def=0;

	}

	//puts("case2");
	for(def=0,j=1;j<GROUND_X-1 && flag==0;j++){
		for( i=j, k=1 ; (i<GROUND_X-1, k<GROUND_Y-1) && flag==0 ; i++, k++){
			if(ground[k][i]!= p && ground[k][i] != 0 && ground[k][i] != -1)def++;
			else def=0;
			for(int min=win-2;min>0;min--){
				if(def==win-min && ground[k+1][i+1]==0){
					run_case=1;
					l=k+1;
					m=i+1;
					risk=judge_side_effective(run_case,ground,def,l,m);
					for(emp=0,more_effective=0,effective=0;ground[l+1][m+1]!=-1&&ground[l+1][m+1]!=p;effective++,l++,m++){
						if(ground[l+1][m+1]!=p&&ground[l+1][m+1]!=0){
							more_effective++;
							if(ground[l+1][m+1]==0) emp++;
						}
					}
					if(def+more_effective<win){
						if(keep.n<=def+more_effective+risk&&effective+def>=win-1){
							out=1;
							keep.n=def+more_effective+risk;
							keep.x=i+1;
							keep.y=k+1;
							flag=0;
						}
					}
					else{
						if(keep.n<=def+risk&&effective+def>=win-1){
							out=1;
							keep.n=def+risk;
							keep.x=i+1;
							keep.y=k+1;
							flag=0;
						}
					}
				}
				if(def==win-min && ground[k-def][i-def]==0 ){
					run_case=2;
					l=k-def;
					m=i-def;
					risk=judge_side_effective(run_case,ground,def,l,m);
					for(emp=0,more_effective=0,effective=0;ground[l-1][m-1]!=-1&&ground[l-1][m-1]!=p;effective++,l--,m--){
						if(ground[l-1][m-1]!=p&&ground[l-1][m-1]!=0){
							more_effective++;
							if(ground[l-1][m-1]==0) emp++;
						}
					}
					if(def+more_effective<win){
						if(keep.n<=def+more_effective+risk&&effective+def>=win-1){
							out=1;
							keep.n=def+more_effective+risk;
							keep.x=i-def;
							keep.y=k-def;
							flag=0;
						}
					}
					else{
						if(keep.n<=def+risk&&effective+def>=win-1){
							out=1;
							keep.n=def+risk;
							keep.x=i-def;
							keep.y=k-def;
							flag=0;
						}
					}
				}


			}
		}
		def=0;

	}

	//puts("case3");
	for(def=0,j=GROUND_X-2;j>0 && flag==0;j--){
		for(i=j, k=1 ; (i>0, k<GROUND_Y-1)  && flag==0; i--, k++){
			if(ground[k][i] != p && ground[k][i] != 0 && ground[k][i] != -1) def++;
			else def=0;

			for(int min=win-2;min>0;min--){
				if(def==win-min && ground[k+1][i-1]==0){
					run_case=3;
					l=k+1;
					m=i-1;
					risk=judge_side_effective(run_case,ground,def,l,m);
					for(emp=0,more_effective=0,effective=0;ground[l+1][m-1]!=-1&&ground[l+1][m-1]!=p;effective++,l++,m--){
						if(ground[l+1][m-1]!=p&&ground[l+1][m-1]!=0){
							more_effective++;
							if(ground[l+1][m-1]==0) emp++;
						}
					}
					if(def+more_effective<win){
						if(keep.n<=def+more_effective+risk&&effective+def>=win-1){
							out=1;
							keep.n=def+more_effective+risk;
							keep.x=i-1;
							keep.y=k+1;
							flag=0;
						}
					}
					else{
						if(keep.n<=def+risk&&effective+def>=win-1){
							out=1;
							keep.n=def+risk;
							keep.x=i-1;
							keep.y=k+1;
							flag=0;
						}
					}
				}
				if(def==win-min && ground[k-def][i+def]==0 ){
					run_case=4;
					l=k-def;
					m=i+def;
					risk=judge_side_effective(run_case,ground,def,l,m);
					for(emp=0,more_effective=0,effective=0;ground[l-1][m+1]!=-1&&ground[l-1][m+1]!=p;effective++,l--,m++){
						if(ground[l-1][m+1]!=p&&ground[l-1][m+1]!=0){
							more_effective++;
							if(ground[l-1][m+1]==0) emp++;
						}
					}
					if(def+more_effective<win){
						if(keep.n<=def+more_effective+risk&&effective+def>=win-1){
							out=1;
							keep.n=def+more_effective+risk;
							keep.x=i+def;
							keep.y=k-def;
							flag=0;
						}
					}
					else{
						if(keep.n<=def+risk&&effective+def>=win-1){
							out=1;
							keep.n=def+risk;
							keep.x=i+def;
							keep.y=k-def;
							flag=0;
						}
					}
				}


			}
		}
		def=0;

	}


	//puts("case4");
	for(def=0,j=1;j<GROUND_Y-1 && flag==0;j++){
		for( i=GROUND_X-2, k=j ; (i>0, k<GROUND_Y-1) && flag==0; i--, k++){
			if(ground[k][i] != p && ground[k][i] != 0 && ground[k][i] != -1)def++;
			else def=0;

			for(int min=win-2;min>0;min--){
				if(def==win-min && ground[k+1][i-1]==0){
					run_case=3;
					l=k+1;
					m=i-1;
					risk=judge_side_effective(run_case,ground,def,l,m);
					for(emp=0,more_effective=0,effective=0;ground[l+1][m-1]!=-1&&ground[l+1][m-1]!=p;effective++,l++,m--){
						if(ground[l+1][m-1]!=p&&ground[l+1][m-1]!=0){
							more_effective++;
							if(ground[l+1][m-1]==0) emp++;
						}
					}
					if(def+more_effective<win){
						if(keep.n<=def+more_effective+risk&&effective+def>=win-1){
							out=1;
							keep.n=def+more_effective+risk;
							keep.x=i-1;
							keep.y=k+1;
							flag=0;
						}
					}
					else{
						if(keep.n<=def+risk&&effective+def>=win-1){
							out=1;
							keep.n=def+risk;
							keep.x=i-1;
							keep.y=k+1;
							flag=0;
						}
					}
				}
				if(def==win-min && ground[k-def][i+def]==0 ){
					run_case=4;
					l=k-def;
					m=i+def;
					risk=judge_side_effective(run_case,ground,def,l,m);
					for(emp=0,more_effective=0,effective=0;ground[l-1][m+1]!=-1&&ground[l-1][m+1]!=p;effective++,l--,m++){
						if(ground[l-1][m+1]!=p&&ground[l-1][m+1]!=0){
							more_effective++;
							if(ground[l-1][m+1]==0) emp++;
						}
					}
					if(def+more_effective<win){
						if(keep.n<=def+more_effective+risk&&effective+def>=win-1){
							out=1;
							keep.n=def+more_effective+risk;
							keep.x=i+def;
							keep.y=k-def;
							flag=0;
						}
					}
					else{
						if(keep.n<=def+risk&&effective+def>=win-1){
							out=1;
							keep.n=def+risk;
							keep.x=i+def;
							keep.y=k-def;
							flag=0;
						}
					}
				}


			}
		}
		def=0;

	}


	//puts("case5");
	for(def=0,i=1;i<GROUND_Y-1 && flag==0;i++){
		for(j=1;j<GROUND_X-1 && flag==0;j++){
			if(ground[i][j] != p && ground[i][j] != 0 && ground[i][j] != -1)def++;
			else def=0;
			for(int min=win-2;min>0;min--){
				if(def==win-min && ground[i][j+1]==0){
					run_case=5;
					l=i;
					m=j+1;
					risk=judge_side_effective(run_case,ground,def,l,m);
					for(emp=0,more_effective=0,effective=0;ground[l][m+1]!=-1&&ground[l][m+1]!=p;effective++,m++){
						if(ground[l][m+1]!=p&&ground[l][m+1]!=0){
							more_effective++;
							if(ground[l][m+1]==0) emp++;
						}
					}
					if(def+more_effective<win){
						if(keep.n<=def+more_effective+risk&&effective+def>=win-1){
							out=1;
							keep.n=def+more_effective+risk;
							keep.x=j+1;
							keep.y=i;
							flag=0;
						}
					}
					else{
						if(keep.n<=def+risk&&effective+def>=win-1){
							out=1;
							keep.n=def+risk;
							keep.x=j+1;
							keep.y=i;
							flag=0;
						}
					}
				}
				if(def==win-min && ground[i][j-def]==0 ){
					run_case=6;
					l=i;
					m=j-def;
					risk=judge_side_effective(run_case,ground,def,l,m);
					for(emp=0,more_effective=0,effective=0;ground[l][m-1]!=-1&&ground[l][m-1]!=p;effective++,m--){
						if(ground[l][m-1]!=p&&ground[l][m-1]!=0){
							more_effective++;
							if(ground[l][m-1]==0) emp++;
						}
					}
					if(def+more_effective<win){
						if(keep.n<=def+more_effective+risk&&effective+def>=win-1){
							out=1;
							keep.n=def+more_effective+risk;
							keep.x=j-def;
							keep.y=i;
							flag=0;
						}
					}
					else{
						if(keep.n<=def+risk&&effective+def>=win-1){
							out=1;
							keep.n=def+risk;
							keep.x=j-def;
							keep.y=i;
							flag=0;
						}
					}
				}


			}
		}
		def=0;
	}

	//puts("case6");
	for(def=0,i=1;i<GROUND_X-1 && flag==0;i++){
		for(j=1;j<GROUND_Y-1 && flag==0;j++){
			if(ground[j][i] != p && ground[j][i] != 0 && ground[j][i] != -1)def++;
			else def=0;

			for(int min=win-2;min>0;min--){
				if(def==win-min && ground[j+1][i]==0){
					run_case=7;
					l=j+1;
					m=i;
					risk=judge_side_effective(run_case,ground,def,l,m);
					for(emp=0,more_effective=0,effective=0;ground[l+1][m]!=-1&&ground[l+1][m]!=p;effective++,l++){
						if(ground[l+1][m]!=p&&ground[l+1][m]!=0){
							more_effective++;
							if(ground[l+1][m]==0) emp++;
						}
					}
					if(def+more_effective<win){
						if(keep.n<=def+more_effective+risk&&effective+def>=win-1){
							out=1;
							keep.n=def+more_effective+risk;
							keep.x=i;
							keep.y=j+1;
							flag=0;
						}
					}
					else{
						if(keep.n<=def+risk&&effective+def>=win-1){
							out=1;
							keep.n=def+risk;
							keep.x=i;
							keep.y=j+1;
							flag=0;
						}
					}
				}
				if(def==win-min && ground[j-def][i]==0 ){
					run_case=8;
					l=j-def;
					m=i;
					risk=judge_side_effective(run_case,ground,def,l,m);
					for(emp=0,more_effective=0,effective=0;ground[l-1][m]!=-1&&ground[l-1][m]!=p;effective++,l--){
						if(ground[l-1][m]!=p&&ground[l-1][m]!=0){
							more_effective++;
							if(ground[l-1][i]==0) emp++;
						}
					}
					if(def+more_effective<win){
						if(keep.n<=def+more_effective+risk&&effective+def>=win-1){
							out=1;
							keep.n=def+more_effective+risk;
							keep.x=i;
							keep.y=j-def;
							flag=0;
						}
					}
					else{
						if(keep.n<=def+risk&&effective+def>=win-1){
							out=1;
							keep.n=def+risk;
							keep.x=i;
							keep.y=j-def;
							flag=0;
						}
					}
				}

			}

		}
		def=0;

	}

	/*--- atk protocol ---*/
	//puts("case7");
	for(sum=0,j=1;j<GROUND_Y-1 && flag==0;j++){
		for( i=1, k=j ; (i<GROUND_X-1, k<GROUND_Y-1) && flag==0 ; i++, k++){
			if(ground[k][i] == p)sum++;
			else sum=0;
			for(int min=win-2;min>0;min--){
				if(sum==win-min && ground[k+1][i+1]==0){
					run_case=1;
					l=k+1;
					m=i+1;
					risk=judge_side_effective(run_case,ground,sum,l,m);
					for(emp=0,more_effective=0,effective=0;ground[l+1][m+1]==0||ground[l+1][m+1]==p;effective++,l++,m++){
						if(ground[l+1][m+1]==p){
							more_effective++;
							if(ground[l+1][m+1]==0) emp++;
						}
					}
					if(sum+more_effective<win){
						if(beep.n<=sum+more_effective+risk&&effective+sum>=win-1){
							out=1;
							beep.n=sum+more_effective+risk;
							beep.x=i+1;
							beep.y=k+1;
							flag=0;
						}
					}
					else{
						if(beep.n<=sum+risk&&effective+sum>=win-1){
							out=1;
							beep.n=sum+risk;
							beep.x=i+1;
							beep.y=k+1;
							flag=0;
						}
					}
				}
				if(sum==win-min && ground[k-sum][i-sum]==0 && flag==0){
					run_case=2;
					l=k-sum;
					m=i-sum;
					risk=judge_side_effective(run_case,ground,sum,l,m);
					for(emp=0,more_effective=0,effective=0;ground[l-1][m-1]==0||ground[l-1][m-1]==p;effective++,l--,m--){
						if(ground[l-1][m-1]==p){
							more_effective++;
							if(ground[l-1][m-1]==0) emp++;
						}
					}
					if(sum+more_effective<win){
						if(beep.n<=sum+more_effective+risk&&effective+sum>=win-1){
							out=1;
							beep.n=sum+more_effective+risk;
							beep.x=i-sum;
							beep.y=k-sum;
							flag=0;
						}
					}
					else{
						if(beep.n<=sum+risk&&effective+sum>=win-1){
							out=1;
							beep.n=sum+risk;
							beep.x=i-sum;
							beep.y=k-sum;
							flag=0;
						}
					}
				}

			}
		}
		sum=0;

	}


	//puts("case8");
	for(sum=0,j=1;j<GROUND_X-1 && flag==0;j++){
		for( i=j, k=1 ; (i<GROUND_X-1, k<GROUND_Y-1) && flag==0 ; i++, k++){
			if(ground[k][i] == p)sum++;
			else sum=0;
			for(int min=win-2;min>0;min--){
				if(sum==win-min && ground[k+1][i+1]==0){
					run_case=1;
					l=k+1;
					m=i+1;
					risk=judge_side_effective(run_case,ground,sum,l,m);
					for(emp=0,more_effective=0,effective=0;ground[l+1][m+1]==0||ground[l+1][m+1]==p;effective++,l++,m++){
						if(ground[l+1][m+1]==p){
							more_effective++;
							if(ground[l+1][m+1]==0) emp++;
						}
					}
					if(sum+more_effective<win){
						if(beep.n<=sum+more_effective+risk&&effective+sum>=win-1){
							out=1;
							beep.n=sum+more_effective+risk;
							beep.x=i+1;
							beep.y=k+1;
							flag=0;
						}
					}
					else{
						if(beep.n<=sum+risk&&effective+sum>=win-1){
							out=1;
							beep.n=sum+risk;
							beep.x=i+1;
							beep.y=k+1;
							flag=0;
						}
					}
				}
				if(sum==win-min && ground[k-sum][i-sum]==0 && flag==0){
					run_case=2;
					l=k-sum;
					m=i-sum;
					risk=judge_side_effective(run_case,ground,sum,l,m);
					for(emp=0,more_effective=0,effective=0;ground[l-1][m-1]==0||ground[l-1][m-1]==p;effective++,l--,m--){
						if(ground[l-1][m-1]==p){
							more_effective++;
							if(ground[l-1][m-1]==0) emp++;
						}
					}
					if(sum+more_effective<win){
						if(beep.n<=sum+more_effective+risk&&effective+sum>=win-1){
							out=1;
							beep.n=sum+more_effective+risk;
							beep.x=i-sum;
							beep.y=k-sum;
							flag=0;
						}
					}
					else{
						if(beep.n<=sum+risk&&effective+sum>=win-1){
							out=1;
							beep.n=sum+risk;
							beep.x=i-sum;
							beep.y=k-sum;
							flag=0;
						}
					}
				}

			}
		}
		sum=0;

	}

	//puts("case9");
	for(sum=0,j=GROUND_X-2;j>0 && flag==0;j--){
		for( i=j, k=1 ; (i>0, k<GROUND_Y-1)  && flag==0; i--, k++){
			if(ground[k][i] == p)sum++;
			else sum=0;

			for(int min=win-2;min>0;min--){
				if(sum==win-min && ground[k+1][i-1]==0){
					run_case=3;
					l=k+1;
					m=i-1;
					risk=judge_side_effective(run_case,ground,sum,l,m);
					for(emp=0,more_effective=0,effective=0;ground[l+1][m-1]==0||ground[l+1][m-1]==p;effective++,l++,m--){
						if(ground[l+1][m-1]==p){
							more_effective++;
							if(ground[l+1][m-1]==0) emp++;
						}
					}
					if(sum+more_effective<win){
						if(beep.n<=sum+more_effective+risk&&effective+sum>=win-1){
							out=1;
							beep.n=sum+more_effective+risk;
							beep.x=i-1;
							beep.y=k+1;
							flag=0;
						}
					}
					else{
						if(beep.n<=sum+risk&&effective+sum>=win-1){
							out=1;
							beep.n=sum+risk;
							beep.x=i-1;
							beep.y=k+1;
							flag=0;
						}
					}
				}
				if(sum==win-min && ground[k-sum][i+sum]==0 && flag==0){
					run_case=4;
					l=k-sum;
					m=i+sum;
					risk=judge_side_effective(run_case,ground,sum,l,m);
					for(emp=0,more_effective=0,effective=0;ground[l-1][m+1]==0||ground[l-1][m+1]==p;effective++,l--,m++){
						if(ground[l-1][m+1]==p){
							more_effective++;
							if(ground[l-1][m+1]==0) emp++;
						}
					}
					if(sum+more_effective<win){
						if(beep.n<=sum+more_effective+risk&&effective+sum>=win-1){
							out=1;
							beep.n=sum+more_effective+risk;
							beep.x=i+sum;
							beep.y=k-sum;
							flag=0;
						}
					}
					else{
						if(beep.n<=sum+risk&&effective+sum>=win-1){
							out=1;
							beep.n=sum+risk;
							beep.x=i+sum;
							beep.y=k-sum;
							flag=0;
						}
					}
				}

			}
		}
		sum=0;

	}

	//puts("case10");
	for(sum=0,j=1;j<GROUND_Y-1 && flag==0;j++){
		for( i=GROUND_X-2, k=j ; (i>0, k<GROUND_Y-1) && flag==0; i--, k++){
			if(ground[k][i] == p)sum++;
			else sum=0;

			for(int min=win-2;min>0;min--){
				if(sum==win-min && ground[k+1][i-1]==0){
					run_case=3;
					l=k+1;
					m=i-1;
					risk=judge_side_effective(run_case,ground,sum,l,m);
					for(emp=0,more_effective=0,effective=0;ground[l+1][m-1]==0||ground[l+1][m-1]==p;effective++,l++,m--){
						if(ground[l+1][m-1]==p){
							more_effective++;
							if(ground[l+1][m-1]==0) emp++;
						}
					}
					if(sum+more_effective<win){
						if(beep.n<=sum+more_effective+risk&&effective+sum>=win-1){
							out=1;
							beep.n=sum+more_effective+risk;
							beep.x=i-1;
							beep.y=k+1;
							flag=0;
						}
					}
					else{
						if(beep.n<=sum+risk&&effective+sum>=win-1){
							out=1;
							beep.n=sum+risk;
							beep.x=i-1;
							beep.y=k+1;
							flag=0;
						}
					}
				}
				if(sum==win-min && ground[k-sum][i+sum]==0 && flag==0){
					run_case=4;
					l=k-sum;
					m=i+sum;
					risk=judge_side_effective(run_case,ground,sum,l,m);
					for(emp=0,more_effective=0,effective=0;ground[l-1][m+1]==0||ground[l-1][m+1]==p;effective++,l--,m++){
						if(ground[l-1][m+1]==p){
							more_effective++;
							if(ground[l-1][m+1]==0) emp++;
						}
					}
					if(sum+more_effective<win){
						if(beep.n<=sum+more_effective+risk&&effective+sum>=win-1){
							out=1;
							beep.n=sum+more_effective+risk;
							beep.x=i+sum;
							beep.y=k-sum;
							flag=0;
						}
					}
					else{
						if(beep.n<=sum+risk&&effective+sum>=win-1){
							out=1;
							beep.n=sum+risk;
							beep.x=i+sum;
							beep.y=k-sum;
							flag=0;
						}
					}
				}

			}
		}
		sum=0;

	}
	//puts("case11");
	for(sum=0,i=1;i<GROUND_Y-1 && flag==0;i++){
		for(j=1;j<GROUND_X-1 && flag==0;j++){
			if(ground[i][j] == p)sum++;
			else sum=0;
			for(int min=win-2;min>0;min--){
				if(sum==win-min && ground[i][j+1]==0){
					run_case=5;
					l=i;
					m=j+1;
					risk=judge_side_effective(run_case,ground,sum,l,m);
					for(emp=0,more_effective=0,effective=0;ground[l][m+1]==0||ground[l][m+1]==p;effective++,m++){
						if(ground[l][m+1]==p){
							more_effective++;
							if(ground[l][m+1]==0) emp++;
						}
					}
					if(sum+more_effective<win){
						if(beep.n<=sum+more_effective+risk&&effective+sum>=win-1){
							out=1;
							beep.n=sum+more_effective+risk;
							beep.x=j+1;
							beep.y=i;
							flag=0;
						}
					}
					else{
						if(beep.n<=sum+risk&&effective+sum>=win-1){
							out=1;
							beep.n=sum+risk;
							beep.x=j+1;
							beep.y=i;
							flag=0;
						}
					}
				}
				if(sum==win-min && ground[i][j-sum]==0 && flag==0){
					run_case=6;
					l=i;
					m=j-sum;
					risk=judge_side_effective(run_case,ground,sum,l,m);
					for(emp=0,more_effective=0,effective=0;ground[l][m-1]==0||ground[l][m-1]==p;effective++,m--){
						if(ground[l][m-1]==p){
							more_effective++;
							if(ground[l][m-1]==0) emp++;
						}
					}
					if(sum+more_effective<win){
						if(beep.n<=sum+more_effective+risk&&effective+sum>=win-1){
							out=1;
							beep.n=sum+more_effective+risk;
							beep.x=j-sum;
							beep.y=i;
							flag=0;
						}
					}
					else{
						if(beep.n<=sum+risk&&effective+sum>=win-1){
							out=1;
							beep.n=sum+risk;
							beep.x=j-sum;
							beep.y=i;
							flag=0;
						}
					}
				}

			}
		}
		sum=0;
	}

	//puts("case12");
	for(sum=0,i=1;i<GROUND_X-1 && flag==0;i++){
		for(j=1;j<GROUND_Y-1 && flag==0;j++){
			if(ground[j][i] == p)sum++;
			else sum=0;

			for(int min=win-2;min>0;min--){
				if(sum==win-min && ground[j+1][i]==0){
					run_case=7;
					l=j+1;
					m=i;
					risk=judge_side_effective(run_case,ground,sum,l,m);
					for(emp=0,more_effective=0,effective=0;ground[l+1][m]==0||ground[l+1][m]==p;effective++,l++){
						if(ground[l+1][m]==p){
							more_effective++;
							if(ground[l+1][m]==0) emp++;
						}
					}
					if(sum+more_effective<win){
						if(beep.n<=sum+more_effective+risk&&effective+sum>=win-1){
							out=1;
							beep.n=sum+more_effective+risk;
							beep.x=i;
							beep.y=j+1;
							flag=0;
						}
					}
					else{
						if(beep.n<=sum+risk&&effective+sum>=win-1){
							out=1;
							beep.n=sum+risk;
							beep.x=i;
							beep.y=j+1;
							flag=0;
						}
					}
				}
				if(sum==win-min && ground[j-sum][i]==0 && flag==0){
					run_case=8;
					l=j-sum;
					m=i;
					risk=judge_side_effective(run_case,ground,sum,l,m);
					for(emp=0,more_effective=0,effective=0;ground[l-1][m]==0||ground[l-1][m]==p;effective++,l--){
						if(ground[l-1][m]==p){
							more_effective++;
							if(ground[l-1][m]==0) emp++;
						}
					}
					if(sum+more_effective<win){
						if(beep.n<=sum+more_effective+risk&&effective+sum>=win-1){
							out=1;
							beep.n=sum+more_effective+risk;
							beep.x=i;
							beep.y=j-sum;
							flag=0;
						}
					}
					else{
						if(beep.n<=sum+risk&&effective+sum>=win-1){
							out=1;
							beep.n=sum+risk;
							beep.x=i;
							beep.y=j-sum;
							flag=0;
						}
					}
				}

			}
		}
		sum=0;
	}


//	printf("%d %d\n",keep.n,beep.n);
	if(keep.n!=0||beep.n!=0){
		if(keep.n>beep.n  ){
			printf("DEF:%d,%d\n",keep.x,keep.y);
			prot(&p,ground,keep.y,keep.x);
			flag=1;
			defn++;
		}
		else if(keep.n<=beep.n || beep.n==5){
			printf("ATK:%d,%d\n",beep.x,beep.y);
			prot(&p,ground,beep.y,beep.x);
			flag=1;
			atkn++;
		}
	}
//	printf("ATK %d  DEF %d\n",atkn,defn);

	//puts("case13");
	if(keep.n==0&&beep.n==0){
		int sy,sx;
		int emp_count=0;
		for(i=1;i<GROUND_Y-1;i++){
			for(j=1;j<GROUND_X-1;j++){
				if(ground[i][j] == 0) emp_count++;
			}
		}
		if(emp_count>40&&emp_count!=100){
			while(flag==0){
				for(i=1;i<GROUND_Y-1&&flag==0;i++){
					for(j=1;j<GROUND_X-1&&flag==0;j++){
						if(out==0 && (ground[i][j]!=p&&ground[i][j]!=0&&ground[i][j]!=-1)) {
							srand((unsigned int)(time(NULL)));
							sy=i+N_case(rand()%3+1);
							sx=j+N_case(rand()%3+1);
							if(ground[sy][sx]==0){
								printf("NEUTRAL:%d,%d\n",sx,sy);
								prot(&p,ground,sy,sx);
								flag=1;
							}
						}
						else flag=0;
					}
				}

			}
		}
		else if(emp_count==100){
			while(flag==0){
				srand((unsigned int)(time(NULL)));
				sy=rand()%10+1;
				sx=rand()%10+1;
				if(out==0 && ground[sy][sx]==0) {
					printf("RAND:%d,%d\n",sx,sy);
					prot(&p,ground,sy,sx);
					flag=1;
				}
				else flag=0;
			}
		}
		else {
			while(flag==0){
				for(i=1;i<GROUND_Y-1&&flag==0;i++){
					for(j=1;j<GROUND_X-1&&flag==0;j++){
						if(out==0 && ground[i][j]==0) {
							printf("ORDER:%d,%d\n",j,i);
							prot(&p,ground,i,j);
							flag=1;
						}
						else flag=0;
					}
				}
			}
		}
	}

}



void Initiarize(int ground[GROUND_Y][GROUND_X]){
	for(int i=0;i<GROUND_Y;i++){
		for(int j=0;j<GROUND_X;j++){
			if(j==0 || i==0 || i== GROUND_Y-1 || j==GROUND_X-1) ground[i][j]= -1;
			else ground[i][j]=0;
		}
	}
}


void Finish(int stat,int *player,char *name){
	char *xxx;
	char *pre;
	char *ans;
	pre=ans=calloc(10,sizeof(char));
	for(int i=0;i<3;i++,ans++) *ans='x'; ans=pre;
	if(stat==1){
		switch(*player){
		case P1:
			printf("%s WIN\n",name);
			break;
		case P2:
			printf("CPU WIN\n");
			break;
		}
	}
	if(stat==2) printf("DRAW!\n");
	TOP:
	xxx=calloc(10,sizeof(char));
	printf("ENTER 'xxx'\n>");
	scanf("%s",xxx);
	if(strcmp(ans,xxx)==0){
		free(xxx);
		free(ans);
		free(pre);
		exit(0);
	}
	else goto TOP;
}


int main(void){

	int s,t;
	int ground[GROUND_Y][GROUND_X];
	int player = P1;
	char *name;

	Initiarize(ground);
	name=calloc(20,sizeof(char));
	printf("Enter your name.\n>");
	scanf("%s",name);

	TOP:

	switch(player){
	case P1:
	//	/*
		printf("%s\n",name);
		printf("INPUT X (%d~%d): ",1,GROUND_X-2);
		scanf("%d",&t);
		printf("INPUT Y (%d~%d): ",1,GROUND_Y-2);
		scanf("%d",&s);

		if(s<GROUND_Y-1 && t<GROUND_X-1 && s!=0 && t!=0){

			prot(&player,ground,s,t);
		}
		else {
			printf("NO GROUND\n");
			player_exchange(&player);

		}
		break;
	//	 */
	//	CPU(player,ground);
	//	break;
	case P2:
		printf("CPU\n");
		CPU(player,ground);
		break;
	}
	if(judge(player,ground) == 1||judge(player,ground) == 2) Finish(judge(player,ground),&player,name);

	player_exchange(&player);

	goto TOP;


	return 0;

}
