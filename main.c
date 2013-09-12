#include "mazegen.h"
#include "mazesolve.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define TOP 1
#define BOTTOM 2
#define LEFT 3
#define RIGHT 4
#define MAX 1000
struct room
{
	int col;
	int row;
	int set_size; //also same as level in bfs.
	int index;
	int dist;
	struct room* parent; 		//pointer to the name of the set that contains this node i.e the corresponding entry in the component array
};

struct q
{
	int room_no;
	struct q* next;
};
struct walls
{
	int wall_no;
	int room1_x, room1_y;
	int room2_x, room2_y;
	struct walls* next;
};

int level=0;
int type;  //type=1 for type I, type=2 for type II
int check_if_number(char* a, char* b);
void which_two_values_to_pass(int tpos, int bpos, int lpos, int rpos, int* v1, int* flag_v1, int* v2, int* flag_v2);
int parse_command_line(char* a, int* count, int* value);
void check_width_and_height(char* a, char* b);

/*mazegen functions*/
void mazegen(int ,int , int, int , int, int , int , FILE*); 
int addToarr(struct walls**, int, int);
void append(struct walls**, struct walls*, int);
void MakeUnionFind(struct room*, int);
struct room* find(struct room*, int, struct room*);
int unionFind(struct room*, struct room*);
void display(int, int, struct walls*, int, int, int, int);
int isWallPresent(int, struct walls*);

/*mazesolve functions*/
void mazesolve(FILE*);
void append_to_room(struct room* r, struct room* room1, struct room* room2);
void check_entrance_and_exit(int lines, char** ch, int tpos, int* lpos, int* rpos, int* bpos, int* entrance, int* entrance_flag, int* exitt, int* exit_flag, int* c1, int);
void bfs(struct room* r, int source, int target, int w, int h, int lines, int entrance, int entrance_flag, int exitt, int exit_flag, int t, char** ch);
void prepend(struct q** s, int u);
void append_to_q(struct q** Q, int i);
void find_source_and_target_rooms(int* source, int* target, int entrance, int entrance_flag, int exitt, int exit_flag, int w, int h);
void display_maze_bfs(char** ch, struct q* returnQ, int w, int h, int lines, int entrance,int entrance_flag,int exitt, int exit_flag, int t1, int mazetype);
int dequeue(struct q**);
void dijkstra_append_to_room(struct room* r, struct room* room1, struct room* room2, char dist);
void dijkstra(struct room* r, int source, int target, int w, int h, int lines, int entrance,int entrance_flag,int exitt,int exit_flag,int t1, char** ch);
int find_least_node_in_dist(int* dist,int* visited,int w,int h);

int main(int argc, char* argv[])
{
	// hw4 mazegen w h [-info] [-top=tpos] [-bottom=bpos] [-left=lpos] [-right=rpos] [rndfile]
	//hw4 mazesolve [file]
	int i,w,h, info_flag=0, flag,num;
	int tpos=0,bpos=0,lpos=0,rpos=0;
	int count=0;
	int v1=0,v2=0, flag_v1, flag_v2;
	FILE* fp;
	if(argc<2)
	{
		fprintf(stderr,"%s\n","Incorrect Usage");
		exit(0);
	}
	if(strcmp(argv[1],"mazegen")==0)
	{
		if(argc>8 || argc<4) 
		{
			fprintf(stderr,"%s\n","Correct Usage : hw4 mazegen w h [-info] [-top=tpos] [-bottom=bpos] [-left=lpos] [-right=rpos] [rndfile]");
			exit(0);
		}
		check_width_and_height(argv[2],argv[3]);
		w=atoi(argv[2]);
		h=atoi(argv[3]);
		if(argc==4)
		{
			fp=stdin;
		}
		for(i=4;i<argc;i++)
		{
			if(argv[i][0] == '-')
			{
			
				if(strcmp(argv[i],"-info") == 0)
				{
					//printf("*************************\n");
					info_flag=1;
					if(argc==5)
					fp=stdin;
				}
				else
				{
					flag=parse_command_line(argv[i], &count, &num);
					if(flag==0)
					{
						fprintf(stderr,"%s\n","Invalid argument. Correct Usage: hw4 mazegen w h [-info] [-top=tpos] [-bottom=bpos] [-left=lpos] [-right=rpos] [rndfile]");
						exit(0);
					}
					if(count >2)
					{
						fprintf(stderr,"%s\n","Either none or exactly two of -top, -bottom, -left, and -right can be specified.");
						exit(0);
					}
					if(flag==TOP) tpos=num;
					else if(flag==BOTTOM) bpos=num;
					else if(flag==LEFT) lpos=num;
					else if(flag==RIGHT) rpos=num;
				}
			}
			else
			{
				if(count==1)
				{
					fprintf(stderr,"%s\n","Either none or exactly two of -top, -bottom, -left, and -right can be specified.");
					exit(0);
				}
				if(count==2 && info_flag==1)
				{
					if(argc==7) fp=stdin;
					else
					{
						fp=fopen(argv[argc-1],"rb");
						if(fp==NULL)
						{
							fprintf(stderr,"%s\n","Cannot open file!");
							exit(0);
						}
					}
				}
				else if(count==2 && info_flag==0)
				{
					if(argc==6) fp=stdin;
					else
					{
						fp=fopen(argv[argc-1],"rb");
						if(fp==NULL)
						{
							fprintf(stderr,"%s\n","Cannot open file!");
							exit(0);
						}
					}
				
				}
				else if(count==0 && info_flag==1)
				{
					if(argc==5) fp=stdin;
					else
					{
						fp=fopen(argv[argc-1],"rb");
						if(fp==NULL)
						{
							fprintf(stderr,"%s\n","Cannot open file!");
							exit(0);
						}
					}
				
				}
				else
					{
						fp=fopen(argv[argc-1],"rb");
						if(fp==NULL)
						{
							fprintf(stderr,"%s\n","Cannot open file!");
							exit(0);
						}
					}
				
				
			}
		}
		if(count==0)
		{
			//set -top=1 and -bottom=w
			tpos=1; bpos=w;
			if(!info_flag) if(argc==4) fp=stdin;
		}
		if(count==1)
		{
			fprintf(stderr,"%s\n","Either none or exactly two of -top, -bottom, -left, and -right can be specified.");
			exit(0);
		}
		if(count==2)
		{
			if(info_flag)
			{
				if(argc==7) fp=stdin;
			}
			else if(argc==6) fp=stdin;
		}
		which_two_values_to_pass(tpos,bpos,lpos,rpos, &v1, &flag_v1, &v2, &flag_v2);
		mazegen(w,h, info_flag, v1,v2,flag_v1,flag_v2, fp);
	}
	else if(strcmp(argv[1],"mazesolve")==0) // ./hw4 mazesolve [file]
	{
		if(argc>3)
		{
			fprintf(stderr,"%s\n","Correct Usage: ./hw4 mazesolve [file]");
			exit(3);
		}
		if(argc==2) fp=stdin;
		else if(argc==3)
		{
			fp=fopen(argv[argc-1],"rb");
			if(fp==NULL)
			{
				fprintf(stderr,"%s\n","Cannot open file!");
				exit(3);
			}
		}
		mazesolve(fp);
	}
	else
	{
		printf("Incorrect Usage: Argument 1 should either be mazegen or mazesolve\n");
		exit(0);
	}
	return 1;
}

void mazesolve(FILE* fp)
{
	char c;
	int h,w,t=0, t1=0, lines=1,i,j;
	char* ch[65];
	int top_flag=0,tpos=0,bpos=0,lpos=0,rpos=0,c1=0;
	int entrance=0, exitt=0, entrance_flag=0, exit_flag=0;
	struct room* r;
	int source=0,target=0;
	int room1=0, room2=0, m1, room_count=0;
	while((c=fgetc(fp))!='\n')
	{
		//append(&cList,c);
		if(c==' ') //entrance point
		{
			tpos=(t+1)/2;
			top_flag=1;
		}
		t++;
	}
	if(t%2==0)
	{
		fprintf(stderr,"%s\n","Line in maze does not contain odd number of characters");
		exit(4);
		//return;
	}
	w=(t-1)/2;
	//printf("w=%d and t=%d\n",w,t);
	if(w==0 || w>64)
	{
		fprintf(stderr,"%s\n","Invalid Width. Correct Usage: 0<width<=64");
		exit(5);
		//return;
	}
	if(top_flag)
	{
		entrance=tpos;
		entrance_flag=TOP;
		c1++;
	}
	t1=0;
	
	ch[1]=(char*)malloc(t+1);
	if(ch[1]==NULL)
	{
		printf("Malloc failed!\n");
		exit(2);
	}
	//printf("Here t=%d\n",t);
	for(i=0;(c=fgetc(fp))!=EOF;i++)
	{
		if(c=='\n')
		{
			if(t1!=t)
			{
				//printf("t1=%d and t=%d\n",t1,t);
				fprintf(stderr,"%s\n","Invalid maze. Lines not of same length.");
				exit(3);
				//return;
			}
			lines++;
			ch[lines]=(char*)malloc(t+1);
			if(ch[lines]==NULL)
			{
				printf("Malloc failed!\n");
				exit(2);
			}
			i=0;
			t1=0;
		}
		else{
		ch[lines][i]=c;
		if(c!=' ' && c!='+' && c!='|' && c!='-' && (c<'0' || c>'9'))
		{
			fprintf(stderr,"%s\n","Invalid entry in the maze");
			exit(2);
		}
		t1++;
		}
	}
	//must have odd number of lines.
	if(lines%2==0)
	{
		fprintf(stderr,"%s\n","Invalid maze. Number of lines should be an odd number.");
		exit(3);
		//return;
	}
	h=lines/2;
	if(h==0 || h>32)
	{
		fprintf(stderr,"%s\n","Invalid Height. Correct Usage: 0<height<=32");
		exit(5);
		//return;
	}
	
	//printf("w= %d and h= %d and t=%d and lines=%d\n",w,h,t,lines);
	//no. of removable walls = w*(h-1) horizontal + (w-1)*h vertical

	//find entrance and exit and the respective flags.
	check_entrance_and_exit(lines,ch,tpos,&lpos,&rpos,&bpos,&entrance,&entrance_flag,&exitt,&exit_flag, &c1, w);
	
	//printf("Entrance = %d, entrance_flag=%d, Exit=%d, exit_flag=%d\n",entrance,entrance_flag,exitt,exit_flag);
	r=(struct room*)malloc(sizeof(struct room) * (w*h));
	if(r==NULL)
	{
		printf("Malloc failed!\n");
		exit(2);
	}
	MakeUnionFind(r, w*h);
	
	
	for(i=1;i<lines-1;i++)
	{
			if(i%2!=0)  //if i is odd, that means i'm looking for vertical walls which are present at even indices.
			{
				for(j=2;j<t-1;j++)
				{
					room_count=0;
					if(j%2==0)
					{
						if(i==1)
						{
							if(ch[i][j]==' ')
							{
								type=1;
								room1=(j-1)/2;
								room2=room1+1;
								append_to_room(r,&r[room1],&r[room2]);
							}
							else if(ch[i][j]>='0' && ch[i][j]<='9')
							{
								type=2;
								room1=(j-1)/2;
								room2=room1+1;
								dijkstra_append_to_room(r,&r[room1],&r[room2],ch[i][j]);
							}
							
							else if(ch[i][j]!='+' && ch[i][j]!='|' && ch[i][j]!='-')
							{
								fprintf(stderr,"%s\n","Invalid entry in the maze");
								exit(2);
							}
							else if(ch[i][j]=='-' && ch[i][j+1]=='-')
							{
								fprintf(stderr,"%s\n","Two symbols (+, -, |) cannot be consecutive");
								exit(2);
							}
						
						}
						else if(ch[i][j+1]==' ' || (ch[i][j+1]>='0' && ch[i][j+1]<='9'))
						{
							//printf("ch[%d][%d] = %c\n",i,j,ch[i][j]);
							//type=1;
							m1=	i;
							while(m1!=1)
							{
								room_count++;
								m1-=2;
							}
							room1=((j-1)/2)+ (room_count*w);
							room2=room1+1;
						
							if(type==1) append_to_room(r, &r[room1], &r[room2]);
							if(type==2) dijkstra_append_to_room(r, &r[room1], &r[room2],ch[i][j+1]);
						}
						else if(ch[i][j+1]!='+' && ch[i][j+1]!='|' && ch[i][j+1]!='-')
						{
							fprintf(stderr,"%s\n","Invalid entry in the maze");
							exit(2);
						}
						else if(ch[i][j+1]=='-' && ch[i][j+2]=='-')
							{
								fprintf(stderr,"%s\n","Two symbols (+, -, |) cannot be consecutive");
								exit(2);
							}
						
					}
					else if(ch[i][j]!='+' && ch[i][j]!='|' && ch[i][j]!='-' && ch[i][j]!=' ' && (ch[i][j]<'0' || ch[i][j]>'9'))
					{
						fprintf(stderr,"%s\n","Invalid entry in the maze");
						exit(2);
					}
						
				}
			}
			else if(i%2==0)	//i'm looking for horizonatal walls.
			{
				for(j=1;j<t-1;j++)
				{
					room_count=0;
					if(j%2!=0)
					{
						if(ch[i][j+1]==' ' || (ch[i][j+1]>='0' && ch[i][j+1]<='9')) 
						{
							//printf("ch[%d][%d] = %c\n",i,j+1,ch[i][j+1]);
							m1=i;
							while(m1!=2)
							{
								room_count++;
								m1-=2;
							}
						
							room1=((j-1)/2)+ (room_count*w);
							room2=room1+w;
							//if(room1==15) printf("HERE\n");
							if(type==1) append_to_room(r,&r[room1],&r[room2]);
							if(type==2) dijkstra_append_to_room(r, &r[room1], &r[room2],ch[i][j+1]);
						}
						else if(ch[i][j+1]!='+' && ch[i][j+1]!='|' && ch[i][j+1]!='-')
						{
							fprintf(stderr,"%s\n","Invalid entry in the maze");
							exit(2);
						}	
						else if(ch[i][j+1]=='-' && ch[i][j+2]=='-')
							{
								fprintf(stderr,"%s\n","Two symbols (+, -, |) cannot be consecutive");
								exit(2);
							}
					}
				}
			}
	}
	
	//print_list(r,w,h);
	//check if all rooms are connected or not.
	for(i=0;i<(w*h);i++)
	{
		if(r[i].parent==NULL)
		{
			fprintf(stderr,"%s\n","Error. Invalid maze. Not all rooms are connected");
			exit(2);
		}
	}
	
	//find source and target room numbers.
	find_source_and_target_rooms(&source,&target,entrance,entrance_flag,exitt,exit_flag,w,h);
	
	if(type==1)
	{
		bfs(r,source,target,w,h,lines,entrance,entrance_flag,exitt,exit_flag,t,ch);
		//returnQ has rooms traversed from source to target.
		//display first line.
		//print return Q
		
	}
	else if(type==2)
	{
		dijkstra(r,source,target,w,h,lines,entrance,entrance_flag,exitt,exit_flag,t,ch);
	}
	
}
void bfs(struct room* r,int source,int target, int w,int h,int lines,int entrance,int entrance_flag,int exitt,int exit_flag,int t1,char** ch)
{
	int *visited, *dist, *previous;
	int max_level=0,u,i,n,v, row1=0,col1=0,row2=0,col2=0, final_target;
	struct q* Q;
	struct q* returnQ;
	
	struct room* temp;
	n=w*h;
	visited=(int*)malloc(sizeof(int) * n);
	dist=(int*)malloc(sizeof(int)*n);
	previous=(int*)malloc(sizeof(int)*n);
	Q=NULL;
	returnQ=NULL;
	for(i=0;i<n;i++)
	{
		visited[i]=0;
		dist[i]=MAX;
		previous[i]=-1;
		
	}
	
	visited[source]=1;
	dist[source]=0;
	
	append_to_q(&Q,source);
	while(Q!=NULL)
	{
		u=dequeue(&Q);
		//if(u==target) { target_flag= u} ;
		temp=&r[u];
		while(temp->parent!=NULL)
		{
			temp=temp->parent;
			v=temp->index;
			if(visited[v] ==0)
			{
				previous[v]=u;
				visited[v]=1;
				append_to_q(&Q,v);
				dist[v]=dist[u]+1;
				r[v].set_size= r[u].set_size+1;
				max_level=r[v].set_size;
			}
		}
		
		
	}
	u=target;
	while(previous[u]!=-1)
	{
		//insert u at the beginning of returnQ.
		prepend(&returnQ,u);
		u=previous[u];
	}
	prepend(&returnQ,u);
	
	//printf("return queue : ");
	
	while(source>(w-1))
	{
		row1++;
		source-=w;
	}
	row1=row1+1;
	col1=source+1;
	final_target=target;
	while(target>(w-1))
	{
		row2++;
		target-=w;
	}
	row2=row2+1;
	col2=target+1;
	if(dist[final_target]==MAX)
	{
		fprintf(stderr,"%s\n","Invalid Entry");
		exit(2);
	}
	
	printf("+");
	for(i=2;i<t1;i+=2)
	{
		if(i==(entrance*2) && entrance_flag==TOP) printf("@");
		else printf("-");
		printf("+");
	}
	printf("\n");
	display_maze_bfs(ch,returnQ,w,h,lines,entrance,entrance_flag,exitt,exit_flag,t1,type);  
	printf("Starts in room at row = %d and column = %d.\n",row1,col1);
	printf("Ends in room at row = %d and column = %d.\n",row2,col2);
	printf("Cost of shortest path = %d.\n",dist[final_target]);
	printf("Maximum level = %d.\n",max_level-1);
}

int dequeue(struct q** Q)
{
	int v=0;
	v=(*Q)->room_no;
	(*Q)=(*Q)->next;
	return v;
}
void display_maze_bfs(char** ch, struct q* returnQ, int w, int h, int lines, int entrance,int entrance_flag,int exitt, int exit_flag, int t, int mazetype)
{
	int i,j,h_count_1=0, h_count_2=0, room1_count=0, w_count_1, w_count_2, hmid, wmid;
	int room1, room2;
	struct q* temp;
	temp=returnQ;
	if(mazetype==2)
	{
		for(i=1;i<lines-1;i++)
		{
			for(j=1;j<t-1;j++)
			{
				if(i==1)
				{
					if(ch[i][j+1]>='0' && ch[i][j+1]<='9')
					{
						ch[i][j+1]=' ';
					}
				}
				else
				{
					if(ch[i][j+1]>='0' && ch[i][j+1]<='9')
					{
						ch[i][j+1]=' ';
					}
				}
			}
		}
	}
	while(temp->next!=NULL)
	{
		room1 = temp->room_no;
		room2=temp->next->room_no;
		
		if((room2==room1+1) || (room2==room1-1))
		{
			//adjacent rooms.
			if(room1>room2)
			{
				room1=room1+room2; //32
				room2=room1-room2; //24
				room1=room1-room2; //8
			}
			h_count_1=1;
			room1_count=room1;
			while(room1_count>(w-1))
			{
				(h_count_1)+=2;
				room1_count-=w;
			}
			h_count_2=h_count_1;
			w_count_1=(2*room1_count)+2;
			w_count_2=(w_count_1)+2;
			
			//room1(h_count_1,w_count_1) and room2(h_count_2,w_count_2)
			hmid=h_count_2;
			wmid=(w_count_1 + w_count_2)/2;
			
			if(h_count_1==1)
			{
				ch[h_count_1][w_count_1-1]='@';
				ch[h_count_2][w_count_2-1]='@';
				ch[hmid][wmid-1]='@';
			}
			else
			{
				ch[h_count_1][w_count_1]='@';
				ch[h_count_2][w_count_2]='@';
				ch[hmid][wmid]='@';
			}
		}
		else
		{
			h_count_1=1; 
			if(room2 < room1)
			{
				//swap
				room1=room1+room2; //32
				room2=room1-room2; //24
				room1=room1-room2; //8
			}
			room1_count=room1; //room1_count=10
			while(room1_count>(w-1)) //while(h_count_1 > 15)
			{
				(h_count_1)+=2;
				room1_count-=w;
			}
			//h_count_1=1
			h_count_2=(h_count_1)+2; //3
			
			w_count_1=((2*room1_count)+2); //22
 			w_count_2=w_count_1; //22
			
			//room1(h_count_1,w_count_1) , room2(h_count_2, w_count_2)
			//rrom1(1,2) room2(3,2)
			hmid=(h_count_1 + h_count_2)/2;
			wmid=w_count_1;
			
			if(h_count_1==1) ch[h_count_1][w_count_1 - 1]='@';
			else ch[h_count_1][w_count_1]='@';
			ch[h_count_2][w_count_2]='@';
			ch[hmid][wmid]='@';
			
		}
		temp=temp->next;
	}
	//check for LEFT and RIGHT
	for(i=1;i<lines-1;i+=2)
	{
		if(i==1){
			if(ch[i][0]==' ' && (entrance_flag==LEFT || exit_flag==LEFT))
			{
				ch[i][0]='@';
			}
			if(ch[i][t-1]==' ' && (entrance_flag==RIGHT || exit_flag==RIGHT))
			{
				ch[i][t-1]='@';
			}
		}
		else
		{
			if(ch[i][1]==' ' && (entrance_flag==LEFT || exit_flag==LEFT))
			{
				ch[i][1]='@';
			}
			if(ch[i][t]==' ' && (entrance_flag==RIGHT || exit_flag==RIGHT))
			{
				ch[i][t]='@';
			}
		}
	}
	
	for(i=1;i<lines-1;i++)
	{
		
			if(i==1)
			{
				for(j=0;j<t;j++)
				printf("%c",ch[i][j]);
				printf("\n");
			}
			else
			{
				for(j=1;j<=t;j++)
				printf("%c",ch[i][j]);
				printf("\n");
			}
			
	}
	
	//last line.
	printf("+");
	for(i=2;i<t;i+=2)
	{
		if(i==(exitt*2) && exit_flag==BOTTOM) printf("@");
		else printf("-");
		printf("+");
	}
	printf("\n");
		
}
void dijkstra(struct room* r, int source, int target, int w, int h, int lines, int entrance,int entrance_flag,int exitt,int exit_flag,int t1, char** ch)
{
	int *previous, *visited, *dist;
	int min,m,i,v,u,d;  // alt = length of the path from the root node to the neighbor node v if it were to go through u
	struct q *Q;
	struct q* returnQ;
	struct q* tempQ;
	struct room* rtemp;
	int row1=0,row2=0,final_target,col1=0,col2=0;
	returnQ=NULL;
	tempQ=NULL;
	Q=NULL;
	dist=NULL;
	dist=(int*)malloc(sizeof(int)*(w*h));
	previous=(int*)malloc(sizeof(int)*(w*h));
	visited=(int*)malloc(sizeof(int)*(w*h));
	for(i=0;i<(w*h);i++)
	{
		//initialize dist[i] to MAX
		dist[i]=MAX;
		//initialize previous[i] to something not-defined 
		previous[i]=-1;
		visited[i]=0;
	}
	/*
	dist[source] := 0 ;                                        // Distance from source to source
    q := the set of all nodes in Graph ;                       // All nodes in the graph are
                                                              // unoptimized - thus are in Q
	*/
	dist[source]=0;
	visited[source]=1;
	u=source;
	while(visited[target]==0)
	{
		min=MAX;
		//printf("u=%d\n",u);
		m=0;
		rtemp=&r[u];
		while(rtemp->parent!=NULL)
		{
			rtemp=rtemp->parent;
			//if(visited[rtemp->index]==1) continue;
			v=rtemp->dist;
			//printf("v = %d\n",v);
			d=dist[u]+ (rtemp->dist);
			//printf("d=%d\n",d);
			if(d< dist[rtemp->index] && visited[rtemp->index]==0)
			{
				dist[rtemp->index]=d;
				previous[rtemp->index]=u;
			}
			if(min>dist[rtemp->index] && visited[rtemp->index]==0)
			{
				min=dist[rtemp->index];
				m=rtemp->index;
			}
		}
		visited[u]=1;
		m=u;
		u=find_least_node_in_dist(dist,visited,w,h);
		if(m==u)
		{
			fprintf(stderr,"%s\n","Invalid Entry");
			exit(2);
		}
	}
	u=target;
	while(previous[u]!=-1)
	{
		//insert u at the beginning of returnQ.
		prepend(&returnQ,u);
		//printf("u=%d\n",u);
		u=previous[u];
	}
	prepend(&returnQ,u);
	while(source>(w-1))
	{
		row1++;
		source-=w;
	}
	row1=row1+1;
	col1=source+1;
	final_target=target;
	while(target>(w-1))
	{
		row2++;
		target-=w;
	}
	row2=row2+1;
	col2=target+1;
	if(dist[final_target]==91)
	{
		fprintf(stderr,"%s\n","Invalid Entry");
		exit(4);
	}
		
		printf("+");
		for(i=2;i<t1;i+=2)
		{
			if(i==(entrance*2) && entrance_flag==TOP) printf("@");
			else printf("-");
			printf("+");
		}
		printf("\n");
		display_maze_bfs(ch,returnQ,w,h,lines,entrance,entrance_flag,exitt,exit_flag,t1,type);  
		printf("Starts in room at row = %d and column = %d.\n",row1,col1); 
		printf("Ends in room at row = %d and column = %d.\n",row2,col2);
		printf("Cost of shortest path = %d.\n",dist[final_target]);  
}

int find_least_node_in_dist(int* dist,int* visited,int w,int h)
{
	int i=0,min=MAX,u=0;
	int n=w*h;
	for(i=0;i<n;i++)
	{
		if(visited[i]==0)
		{
			if(min>dist[i])
			{
				min=dist[i];
				u=i;
			}
		}
	}
	return u;
}
void prepend(struct q** s, int u)
{
	struct q* temp;
	struct q* r;
	if((*s)==NULL)
	{
		temp=(struct q*)malloc(sizeof(struct q));
		temp->room_no=u;
		temp->next=NULL;
		(*s)=temp;
	}
	else
	{
		r=(struct q*)malloc(sizeof(struct q));
		r->room_no=u;
		r->next=(*s);
		(*s)=r;
	}
}

void append_to_q(struct q** Q, int i)
{
	struct q* temp;
	struct q* r;
	if((*Q)==NULL)
	{
		temp=(struct q*)malloc(sizeof(struct q));
		temp->room_no=i;
		temp->next=NULL;
		(*Q)=temp;
	}
	else
	{
		temp=(*Q);
		while(temp->next!=NULL) temp=temp->next;
		r=(struct q*)malloc(sizeof(struct q));
		r->room_no=i;
		r->next=NULL;
		temp->next=r;
	}
}
void find_source_and_target_rooms(int* source, int* target, int entrance, int entrance_flag, int exitt, int exit_flag, int w, int h)
{
	int entrance_room, exit_room;
	switch(entrance_flag)
	{
		case 1:  //TOP
			entrance_room=entrance-1;
			break;
			
		case 2:  //BOTTOM
			entrance_room=entrance-1+((h-1)*w);
			break;
			
		case 3: //LEFT
			entrance_room=(entrance-1)*w;
			break;
			
		case 4: //RIGHT
			entrance_room=(entrance*w)-1;
			break;
			
		default: 
			break;
	}
	switch(exit_flag)
	{
		case 1: //TOP
			exit_room=exitt-1;
			break;
			
		case 2: //BOTTOM
			exit_room=exitt-1+((h-1)*w);
			break;
			
		case 3: //LEFT
			exit_room=(exitt-1)*w;
			break;
			
		case 4: //RIGHT
			exit_room=(exitt*w)-1;
			break;
			
		default: 
			break;
	}
	*source=entrance_room;
	*target=exit_room;
}

void dijkstra_append_to_room(struct room* r, struct room* room1, struct room* room2, char dist)
{
	//append room2 to room1's list and vice versa.
	struct room* temp1;
	struct room* temp2;
	struct room* t1;
	struct room* t2;
	int d;
	d=(int)(dist-'0');
	
	t1=room1;
	t2=room2;
	temp1=(struct room*)malloc(sizeof(struct room));
	temp2=(struct room*)malloc(sizeof(struct room));
	
	temp1->index=room1->index;
	temp1->dist=d;
	temp2->index=room2->index;
	temp2->dist=d;
	temp1->parent=NULL;
	temp2->parent=NULL;
	if(room1->parent==NULL)
	{
		room1->parent=temp2;
	}
	else if(room1->parent!=NULL)
	{
		while(t1->parent!=NULL)
		{
			t1=t1->parent;
		}
		t1->parent=temp2;
	}
	if((room2)->parent==NULL)
	{
		(room2)->parent=temp1;
	}
	else if(room2->parent!=NULL)
	{
		while(t2->parent!=NULL)
		{
			t2=t2->parent;
		}
		t2->parent=temp1;
	}
}


void append_to_room(struct room* r, struct room* room1, struct room* room2)
{
	//append room2 to room1's list and vice versa.
	struct room* temp1;
	struct room* temp2;
	struct room* t1;
	struct room* t2;
	t1=room1;
	t2=room2;
	temp1=(struct room*)malloc(sizeof(struct room));
	temp2=(struct room*)malloc(sizeof(struct room));
	
	temp1->index=room1->index;
	temp2->index=room2->index;
	temp1->parent=NULL;
	temp2->parent=NULL;
	if(room1->parent==NULL)
	{
		room1->parent=temp2;
	}
	else if(room1->parent!=NULL)
	{
		while(t1->parent!=NULL)
		{
			t1=t1->parent;
		}
		t1->parent=temp2;
	}
	if((room2)->parent==NULL)
	{
		(room2)->parent=temp1;
	}
	else if(room2->parent!=NULL)
	{
		while(t2->parent!=NULL)
		{
			t2=t2->parent;
		}
		t2->parent=temp1;
	}
}
void check_entrance_and_exit(int lines, char** ch, int tpos, int* lpos, int* rpos, int* bpos, int* entrance, int* entrance_flag, int* exitt, int* exit_flag, int* c1, int w)
{
	int j;
	for(j=1;j<lines;j+=2)
	{
		if(j==1)
		{
		if(ch[j][0] == ' ')
		{
			*lpos=(j+1)/2;
			if(tpos)
			{
				*exitt=*lpos; *exit_flag=LEFT;
			}
			else
			{
				*entrance=*lpos; *entrance_flag=LEFT;
			}
			(*c1)++;
		}
		}
		else
		{
			if(ch[j][1] == ' ')
		{
			*lpos=(j+1)/2;
			if(tpos)
			{
				*exitt=*lpos; *exit_flag=LEFT;
			}
			else if(*rpos)
			{
				*exitt=*lpos; *exit_flag=LEFT;
			}
			else
			{
				*entrance=*lpos; *entrance_flag=LEFT;
			}
			(*c1)++;
		}
		
		}
		if(j==1)
		{
		if(ch[j][2*w]==' ')
		{
			if((*c1)==2)
			{
				fprintf(stderr,"%s\n","Either two of -top, -bottom, -left, and -right should be specified.");
				exit(3);
			}
			*rpos=(j+1)/2;
			if(tpos)
			{
				*exitt=*rpos; *exit_flag=RIGHT;
			}
			else if(*lpos)
			{
				if((*lpos)<=(*rpos))
				{
					*exitt=*rpos; *exit_flag=RIGHT;
				}
				else
				{
					*exitt=*lpos; *exit_flag=LEFT;
					*entrance=*rpos; *entrance_flag=RIGHT;
				}
			}
			else
			{
				*entrance=*rpos; *entrance_flag=RIGHT;
			}
			(*c1)++;
		}
		}
		else
		{
		if(ch[j][2*w+1]==' ')
		{
			if((*c1)==2)
			{
				fprintf(stderr,"%s\n","Either two of -top, -bottom, -left, and -right should be specified.");
				exit(3);
				//return;
			}
			*rpos=(j+1)/2;
			if(tpos)
			{
				*exitt=*rpos; *exit_flag=RIGHT;
			}
			else if(*lpos)
			{
				if((*lpos)<=(*rpos))
				{
					*exitt=*rpos; *exit_flag=RIGHT;
				}
				else
				{
					*exitt=*lpos; *exit_flag=LEFT;
					*entrance=*rpos; *entrance_flag=RIGHT;
				}
			}
			else
			{
				*entrance=*rpos; *entrance_flag=RIGHT;
			}
			(*c1)++;
		}
		
		}
	}
	for(j=2;j<(2*w+1);j+=2)
	{
		if(ch[lines-1][j]==' ')
		{
			if((*c1)==2)
			{
				fprintf(stderr,"%s\n","Either two of -top, -bottom, -left, and -right should be specified.");
				exit(3);
				//return;
			}
			*bpos=j/2;
			*exitt=*bpos;
			*exit_flag=BOTTOM;
		}
	}
}

void mazegen(int w,int h, int info_flag, int entrance, int exitt, int flag_entrance, int flag_exit, FILE* fp)
{
	struct walls* arr;
	struct walls* temp;
	struct walls* node;
	struct room* room1;
	struct room* room2;
	struct room* r1;
	struct room* r2;
	
	int W, tempW, L, R,count=0;
	int i,j,no_of_iterations;
	int wall_count=0,x[4];
	//unsigned char ch[4];
	unsigned int X=0;
	int p=w*h;
	struct room* component; //an array of n=w*h sets, where component[i] is the name of the set containing node i
	component=(struct room*)malloc(p * sizeof(struct room));	
	arr=NULL;
	L=addToarr(&arr,w,h);
	//printf("Here L = %d\n",L); //number of removable walls.
	MakeUnionFind(component, (w*h));
	//fread(char buffer, size in bytes of each object read, number of objects to be read, file) 
	//The function fread advances the file position indicator for the stream by the number of bytes read.
	//Upon successful completion, fread() shall return the number of elements successfully read which is less than nitems only if a read error or end-of-file is 		encountered
	j=0;
	no_of_iterations=w*h - 1;
	while(count< no_of_iterations)
	{
		//printf("Iteration # %d\n",count);		
		for(i=0;i<4;i++)
		{
			x[i]=fgetc(fp);
			if(x[i]==EOF)
			{
				fprintf(stderr,"%s\n","Exhausted reading from file");
				exit(3);
			}
			//printf("%d\n",x[i]);
		}
		
		X=(x[0]<<24) + (x[1]<<16) + (x[2]<<8) + x[3];
		//printf("X=%u\n",X);		
		R=X%L;
		L--;
		//printf("R=%d\n",R);
		j++;		
		//use R as a zero-based index into arr
		//go to the Rth element in arr
		temp=arr;
		while((temp->wall_no==-1 || temp->wall_no==-2)) temp=temp->next;
		for(i=0;i<R;i++) 
		{
			//old=temp;
			if(temp->wall_no!=-1 && temp->wall_no!=-2) temp=temp->next;
			while(temp->wall_no==-1 || temp->wall_no==-2) 
			{
				temp=temp->next; 
			}
		}	
			
		//unlink temp
		//old->next=temp->next;
		//examine the wall that this element corresponds to. Let's call this wall W.
		tempW = W = temp->wall_no;
		wall_count=0;
		while(tempW > (2*w - 2))
		{
			tempW = tempW - ((2*w) - 1);
			wall_count++;
		}
		
		//find the node corresponding to wall tempW
		node=arr;
		for(i=0;i<tempW && node!=NULL;i++)
		{
			node=node->next;
		}
		
		//retrieve the room values for node
		room1 = (struct room*)malloc(sizeof(struct room));
		room2 = (struct room*)malloc(sizeof(struct room));
		
		room1->row = (node->room1_x) + wall_count;
		room2->row = (node->room2_x) + wall_count;
		room1->col = node->room1_y;
		room2->col = node->room2_y;
		
		//printf("Room1 (%d,%d) and Room2 (%d,%d)\n",room1->row,room1->col, room2->row, room2->col);
		
		
		r1=find(room1,w, component);
		r2=find(room2,w, component);
		//printf("r1(%d,%d) and r2(%d,%d)\n",r1->row,r1->col,r2->row,r2->col);
		//implement Find and UnionFind
		if(!unionFind(r1,r2)) //0 if the two rooms are not connected
		{
			//when r1 and r2 are not connected
			if(r1->set_size >= r2->set_size)
			{
				r2->parent=r1;
				r1->set_size+=r2->set_size;
			}
			else
			{
				r1->parent=r2;
				r2->set_size+=r1->set_size;
			}
			
			//since r1 and r2 are not connected when they enter this "if", we must remove wall W between them.
			
			if(info_flag)
			{
				printf("Wall %d removed.\n", W);
			}
			temp->wall_no=-1; // wall_no is -1 to indicate that the wall has been removed so print a space.
			count++;
		}
		else 
		{
			temp->wall_no=-2;
			//printf("*** Wall %d flag = -2 ****\n",W);
		}
	
	}
	
	display(w,h,arr,entrance,exitt,flag_entrance,flag_exit);
}
void display(int w, int h, struct walls* arr, int entrance, int exitt, int flag_entrance, int flag_exit)
{
	int i,j=0,k=0,l,m=0; //w=48, h=23, exit=18 flag_exit=4
	printf("+");
	for(i=1;i<=w;i++)
	{
		if((entrance == i) && (flag_entrance == TOP))
		{
			printf(" +");
		}
		else printf("-+");
	}
	printf("\n");
	for(i=1;i<=h;i++)
	{
		if((entrance==i && flag_entrance==LEFT) || (exitt==i && flag_exit==LEFT))
		{
			printf("  ");
		}
		else if(!(i==entrance && flag_entrance==RIGHT))
		printf("| ");
		else if(!(i==exitt && flag_exit==RIGHT))
		printf("| ");
		
		for(j=k;j<w-1+k;j++)
		{
			if(isWallPresent(j,arr)) //return 0 if wall_flag!=-1 for node j
			{
				printf("| ");
			}
			else
			{
				printf("  ");
			}
		}
		if((i==entrance && flag_entrance==RIGHT) || (i==exitt && flag_exit==RIGHT))
		{
			printf(" \n");
		}
		else printf("|\n");
		
		if(i!=h){
		printf("+");
		for(k=j;k<j+w;k++)
		{
			if(isWallPresent(k,arr))
			{
				printf("-+");
			}
			else printf(" +");
		}
		printf("\n");
		}
		else if(i==h)
		{
			printf("+");
			for(l=k;l<k+w;l++)
			{
				//l=93, k=93, w=16, h=4
				m=l-((h-1)*(2*w-1) - 1);  
				if(m==exitt && flag_exit==BOTTOM)
				{
					printf(" +");
				}
				else printf("-+");
			}
			printf("\n");
		}
		j=k;
	}
}

int isWallPresent(int j, struct walls* arr)
{
	//wall j will be the jth element in arr
	int i;
	struct walls* t;
	t=arr;
	for(i=0;i<j;i++)
	t=t->next;
	
	if(t->wall_no == -1) return 0;
	else return 1;
}

struct room* find(struct room* r, int w, struct room* component)
{
	struct room* r1;
	//struct room* r2;
	//first locate the node in component
	int a,b;
	int room_num;
	a=(r)->row;
	b=(r)->col;
	room_num= (w*a) + b;
	r1=&component[room_num]; //the corresponding room in the component list
	while(r1->parent!=NULL) r1=r1->parent;
	
	return r1;
}
int unionFind(struct room* r1, struct room* r2)
{
	//r1=&component[room_num1]
	//r2=&component[room_num2]
	//if index of r1 and r2 in component is same, then components are connected, else they're not.
	//find index of r1 and r2 in component.
	if(r1==r2)
	{
		return 1;
	}
	else return 0;
}

int addToarr(struct walls** arr, int cols, int rows)
{
	//cols=4, rows=3	
	int L=0;
	int i,j,k;
	int room1_x=0, room1_y=0, room2_x=0, room2_y=1;
	struct walls* t;
	t=(struct walls*)malloc(sizeof(struct walls));
	//append the first 2w-1 elements with room1 and room2 info.
	for(i=0;i<cols-1;i++) //cols=16, walls 0 to 14 appended
	{
		//i is the wall number.
		t->room1_x=room1_x;
		t->room1_y=room1_y++;
		t->room2_x=room2_x;
		t->room2_y= room2_y++;
		L++;
		append(arr,t,i);
	}
	room1_x=0; room1_y=0; room2_x=1; room2_y=0;
	for(i=cols-1;i<(2*cols)-1;i++) //i=15; i<31 
	{
		t->room1_x=room1_x;
		t->room1_y=room1_y++;
		t->room2_x=room2_x;
		t->room2_y=room2_y++;
		L++;
		append(arr,t,i);
	}
	room1_x=0; room1_y=0; room2_x=0; room2_y=0;
	for(k=1;k<rows-1;k++)
	{
		//j=(((2*rows)-1)*(cols-1))-cols+((2*cols) -2); //j= 105-16+30=119
		j=i; //i=31; i=62
		for(i=((2*cols)-1)*k;i<j+(((2*cols)-1));i++) //i=31, i<62 ; i=62, i<93   31 times
		{
			t->room1_x=0;
			t->room1_y=0;
			t->room2_x=0;
			t->room2_y=0;
			L++;
			append(arr,t,i);
		}
	}
	
	//add cols -1 walls
	for(k=i;k<i+cols-1;k++)
	{
		t->room1_x=0;
		t->room1_y=0;
		t->room2_x=0;
		t->room2_y=0;
		L++;
		append(arr,t,k);
	}
	return L;
}
void append(struct walls** arr, struct walls* t, int wall_no)
{
	struct walls* temp;
	struct walls* new_wall;
	if(*arr==NULL)
	{
		temp=(struct walls*)malloc(sizeof(struct walls));
		temp->wall_no=wall_no;
		temp->room1_x=t->room1_x;
		temp->room1_y=t->room1_y;
		temp->room2_x=t->room2_x;
		temp->room2_y=t->room2_y;
		temp->next=NULL;
		*arr=temp;
	}
	else
	{
		temp=*arr;
		while(temp->next!=NULL) temp=temp->next;
		new_wall=(struct walls*)malloc(sizeof(struct walls));
		new_wall->room1_x=t->room1_x;
		new_wall->room1_y=t->room1_y;
		new_wall->room2_x=t->room2_x;
		new_wall->room2_y=t->room2_y;
		new_wall->wall_no=wall_no;
		new_wall->next=NULL;
		temp->next=new_wall;
	}
}

void MakeUnionFind(struct room* component, int n)
{
	//initialize struct room* c for each room in the component array to NULL, coz it points to itself initially. This results in n disjoint sets.
	int i;
	for(i=0;i<n;i++)
	{
		component[i].parent=NULL;
		component[i].set_size=1;
		component[i].index=i;
	}
}

void which_two_values_to_pass(int tpos, int bpos, int lpos, int rpos, int* v1, int* flag_v1, int* v2, int* flag_v2)
{
	if(tpos) {*v1 = tpos; *flag_v1= 1;}
	if(bpos)
	{
		*v2=bpos; *flag_v2= 2; 
	}
	if(lpos)
	{
		if(tpos) { *v2= lpos; *flag_v2= 3; }
		else if(bpos) { *v1=lpos; *flag_v1= 3; }
	}
	if(rpos)
	{
		if(tpos) { *v2=rpos; *flag_v2 = 4; }
		else if(bpos) {*v1=rpos; *flag_v1=4; }
		else if(lpos)
		{
			if(lpos>rpos)
			{
				*v2=lpos;
				*v1=rpos;
				*flag_v2=3;
				*flag_v1=4;
			}
			else
			{
				*v2=rpos;
				*flag_v2=4;
			}
		}
	}
}

int parse_command_line(char* a, int* count, int* value)
{
	// hw4 mazegen w h [-info] [-top=tpos] [-bottom=bpos] [-left=lpos] [-right=rpos] [rndfile]
	int i=0;
	int num=0;
	if(a[1]=='t' || a[1]=='l')
	{
		while(a[i]!= '=') i++;
		i++;
		while(a[i]!= '\0')
		{
			num=num*10 + (a[i] - '0');
			i++;
		}
		*value=num;
		if(num <1 && a[1]=='t') 
		{
			fprintf(stderr,"%s\n","Value of -top should not be less than 1");
			exit(2);
		}
		if(a[1]=='l' && (num<1 || num>32))
		{
			fprintf(stderr,"%s\n","Value of -left should be between 1 and h");
			exit(2);
		}
		(*count)++;
		if(a[1]=='t') return TOP;
		else return LEFT;
	}
	else if(a[1]=='b' || a[1]=='r')
	{
		(*count)++;
		while(a[i]!= '=') i++;
		i++;
		while(a[i]!='\0')
		{
			num=num*10 + (a[i] - '0');
			i++;
		}
		*value=num;
		if((a[1]=='b') && (num <=0 || num > 64))
		{
			fprintf(stderr,"%s\n","Value of -bottom is incorrect");
			exit(2);
		}
		if((a[1]=='r') && (num<=0 || num>32))
		{
			fprintf(stderr,"%s\n","Value of -right is incorrect");
			exit(2);
		}
		if(a[1]=='b') return BOTTOM;
		else return RIGHT;
	}
	else return 0;
}

void check_width_and_height(char* a, char* b)
{
	int x,y;
	if(!check_if_number(a,b))		// check if a and b are numbers
	{
		exit(1); 
	}
	x=atoi(a);
	y=atoi(b);
	
	if(x>64 || y>32)
	{
		if(x>64)
		fprintf(stderr,"%s\n","Invalid Width. Width should be less than 65");
		else
		fprintf(stderr,"%s\n","Invalid Width. Height should be less than 33");
		exit(1);
	}
	if(x==0 || y==0)
	{
		if(x==0)
		fprintf(stderr,"%s\n","Width can not be 0");
		else
		fprintf(stderr,"%s\n","Height can not be 0");
		exit(1);
	}
}

int check_if_number(char* a, char* b)
{
	int i,j;
	for(i=0;a[i]!='\0';i++)
	{
		if(a[i] <'0' || a[i]>'9')
		{
			fprintf(stderr,"%s\n","Invalid Width. Please enter a valid number");
			return 0;
		}
	}
	for(j=0;b[j]!='\0';j++)
	{
		if(b[j] <'0' || b[j]>'9')
		{
			fprintf(stderr,"%s\n","Invalid Height. Please enter a valid number");
			return 0;
		}
	}
	return 1;
}






