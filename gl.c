#include "header.h"
#include "constants.h"

#define NUM_PLAYERS 6
#define WINDOW_WIDTH 870
#define WINDOW_HEIGHT 750
#define PI 3.14159265
#define SQUARE_SIZE 50
#define SQUARE_MARGIN 20
#define MAX_BALLS_PER_TEAM 20

int circleYA = 500;
int circleYB = 500;
int startXTeamA = 100;
int startYTeamA = 550;
int startXTeamB = 400;
int startYTeamB = 550;
int circleXA = 80;
int circleXB = 400;
int ballB=0;
int ballA=0;
int sizeA=MAX_BALLS_PER_TEAM;
int sizeB=MAX_BALLS_PER_TEAM;

int newcircleXA = 170;
int newcircleXB = 340;
int newcircleYA = 600;
int newcircleYB = 600;
int FPS = 1;
int indexA=0;
int indexB=0;
int doneA=0;
int doneB=0;
int flag2 =0; 
int flag =0;
void addBallA();
void addBallB();
void  removeBallA();
void  removeBallB();
int score_team1 = 0, score_team2 = 0;
char labelscore1[50]="0";
char labelscore2[50]="0";
int readBALLCOUNTFromFile(const char *filename) ;
void glutWMCloseFunc(void (*func)(void));
void handle_round_end(int signal);   

typedef void (*sighandler_t)(int);
sighandler_t sigset(int sig, sighandler_t disp);



typedef struct
{
  int player_number;
  int energy_level;
} Player;

typedef struct {
    float x;
    float y;
    float radius;
    int team; // 0 for A, 1 for  B
} Ball;

typedef struct {
    Player players[NUM_PLAYERS];
    int team_lead_energy;
    int ballCount; 
} Team;


Ball ballsA[MAX_BALLS_PER_TEAM];
Ball ballsB[MAX_BALLS_PER_TEAM];
Team TeamA, TeamB;
FILE *fileA;
FILE *fileB;




// Function to handle window closure
void closeCallback() {
    printf("Window is closing. Performing cleanup...\n");
    

    
             		fclose(fileB);
        		fclose(fileA);


    FILE *file = fopen("ballcount.txt", "w");
    if (file == NULL) {
        perror("Error opening file");
        exit(-10);
    }

    int value = 0;
    fprintf(file, "%d\n", value);

    fclose(file);
    
    
    int v =0;
    FILE *file2 = fopen("ballcount2.txt", "w");
    if (file2 == NULL) {
        perror("Error opening file");
        exit(-10);
    }


    fprintf(file2, "%d\n", v);

    fclose(file2);
    
         kill(getpid(), SIGTERM);
	    
   // empty fileballnumber
    
    
        FILE *fileballsA = fopen("ballsA.txt", "w"); // Open the file in write mode
    if (!fileballsA) {
        perror("Failed to open file");
    }

    fclose(fileballsA); // Close the file to truncate it
    
    
     FILE *fileballsB = fopen("ballsB.txt", "w"); // Open the file in write mode
    if (!fileballsA) {
        perror("Failed to open file");
    }

    fclose(fileballsB); // Close the file to truncate it
    

}


void handle_time_expired(int sig) {
    if (sig == SIGCHLD) {
        fflush(stdout);
        flag2= 1;
       glutPostRedisplay(); 
    }
}


void
initializePlayer (Player * player, int number)
{
  player->player_number = number;
  player->energy_level = 100;	
}


void
initializeTeam (Team * team, int team_lead_energy)
{
  int i;
  for (i = 0; i < NUM_PLAYERS; i++)
	{
	  initializePlayer (&(team->players[i]), i + 1);
	}
  team->team_lead_energy = team_lead_energy;
   team->ballCount = 0;
}



void drawHuman(int x, int y) {
    // Head (circle)

    int headRadius = 12; // Radius of the head
    int i;
    int triangleAmount = 20; // Number of triangles used to draw circle

    GLfloat twicePi = 2.0f * PI;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y+25 ); // Center of the head
    for(i = 0; i <= triangleAmount; i++) {
        glVertex2f(x + (headRadius * cos(i * twicePi / triangleAmount)),
                   y + 25 + (headRadius * sin(i * twicePi / triangleAmount)));
    }
    glEnd();

    // Body (rectangle)
    glBegin(GL_QUADS);
    glVertex2f(x - 5, y + 5); // Top left
    glVertex2f(x + 5, y + 5); // Top right
    glVertex2f(x + 5, y - 20); // Bottom right
    glVertex2f(x - 5, y - 20); // Bottom left
    glEnd();

    // Legs
    glBegin(GL_LINES);
    glVertex2f(x, y - 20);
    glVertex2f(x - 10, y - 40);
    glVertex2f(x, y - 20);
    glVertex2f(x + 10, y - 40);
    glEnd();

    // Arms
    glBegin(GL_LINES);
    glVertex2f(x, y + 5);
    glVertex2f(x - 15, y - 10);
    glVertex2f(x, y + 5);
    glVertex2f(x + 15, y - 10);
    glEnd();
}



void drawCircle (int x, int y, int radius){
  glColor3f(0.5f, 0.0f, 0.0f); // Dark red colorfor Team 1
  int i;
  int triangleAmount = 20;		// Number of triangles used to draw circle
  GLfloat twicePi = 2.0f * PI;
  glBegin (GL_TRIANGLE_FAN);
  glVertex2f (x, y);			// Center of circle
  for (i = 0; i <= triangleAmount; i++)
	{
	  glVertex2f (x + (radius * cos (i * twicePi / triangleAmount)),
	  y + (radius * sin (i * twicePi / triangleAmount)));
	}
 		 glEnd ();
}



// Function to draw teams vertically with leader and players in same vertical line
void drawTeamsVertical (Team teamA, Team teamB) {
  int i;

  // Draw Team A Leader
  glColor3f(1.0, 0.5, 0.5); // Light red: R=1.0, G=0.5, B=0.5
  drawHuman(startXTeamA + 150, startYTeamA-50); // Team A leader
  // Label the Team A Leader
  glColor3f(0.0f, 0.0f, 0.0f); // Black for text
  glRasterPos2i(startXTeamA + 130, startYTeamA-10); // Position the text above the leader
  const char* leaderLabelA = "Team Leader";
  for (const char* c = leaderLabelA; *c != '\0'; c++) {
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, *c);
  }

  // Draw and Label Team A players
  for (i = 1; i < NUM_PLAYERS; i++) {
    int x = startXTeamA;
    int y = startYTeamA-50 - i * 100;
    int energy = teamA.players[i].energy_level;
    glColor3f(1.0, 0.0, 0.0); // Red color for Team A
    drawHuman(x + 75, y + 50); // Team A player
    // Label the player
    glColor3f(0.0f, 0.0f, 0.0f); // Black for text
    glRasterPos2i(x + 55, y + 50); // Position the text above the player
    char playerLabel[2];
    sprintf(playerLabel, "%d", i);
    for (char* c = playerLabel; *c != '\0'; c++) {
      glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, *c);
    }
  }

  // Draw Team B Leader
glColor3f(0.5, 0.7, 1.0); // Light blue: R=0.5, G=0.7, B=1.0
  //drawPlayerSquare(startXTeamB, startYTeamB-50);
    drawHuman(startXTeamB, startYTeamB-50);
  // Label the Team B Leader
  glColor3f(0.0f, 0.0f, 0.0f); // Black for text
  glRasterPos2i(startXTeamB-20, startYTeamB-10); // Position the text above the leader
  const char* leaderLabelB = "Team Leader";
  for (const char* c = leaderLabelB; *c != '\0'; c++) {
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, *c);
  }

  // Draw and Label Team B players
  for (i = 1; i < NUM_PLAYERS; i++) {
    int x = startXTeamB;
    int y = startYTeamB-50 - i * 100;
    int energy = teamB.players[i].energy_level;
    glColor3f(0.0, 0.0, 1.0); // Blue color for Team B
    drawHuman(x + 80, y + 50);
    
    
    // Label the player
    glColor3f(0.0f, 0.0f, 0.0f); // Black for text
    glRasterPos2i(x + 95, y + 50); // Position the text above the player
    char playerLabel[2];
    sprintf(playerLabel, "%d", i);
    for (char* c = playerLabel; *c != '\0'; c++) {
      glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, *c);
    }
  }
}





void update_ball_team_a() {


    int lineA;
   
    if (fscanf(fileA, "%d", &lineA) >=0) {
        ballA=lineA;
        
        }
 else {
	sleep(1);
     fscanf(fileA, "%d", &lineA);
     
  }
    
          if (ballsA[ballA].y<500){ 
	     ballsA[ballA].y -= 100;
        if   (ballsA[ballA].y<= 0)
             ballsA[ballA].y = 510;
           }
        glutPostRedisplay(); 
}

void update_ball_team_b() {



    int lineB;
   
    if (fscanf(fileB, "%d", &lineB) >=0) {
        ballB=lineB;
        
        }
 else {
	sleep(1);
     fscanf(fileB, "%d", &lineB);
      }
  
      if(ballsB[ballB].y <500){
	ballsB[ballB].y -= 100;
        if (ballsB[ballB].y<= 0)
            ballsB[ballB].y = 510;
         }  
        glutPostRedisplay(); 
}

void leader_to_player_0B(){

fflush(stdout);
 // if (ballsB[ballB].y>=500){
 for(int i=0;i<MAX_BALLS_PER_TEAM;i++){
 if(ballsB[i].y >= circleYB + 10){

 fflush(stdout);
      ballsB[i].y -= 50;}
    }
      
       glutPostRedisplay(); 
 // }
}

void leader_to_player_0A(){

fflush(stdout);
  for(int i=0;i<MAX_BALLS_PER_TEAM;i++){
 if(ballsA[i].y >= circleYB + 10){

 fflush(stdout);
      ballsA[i].y -= 50;}
    }
     glutPostRedisplay(); 
}

void update_ball_from_lead_a_to_b() {
        removeBallA();
        addBallB();
           
	TeamA.ballCount--;
        TeamB.ballCount++;
	
	
	
        glutPostRedisplay(); 
}

void update_ball_from_lead_b_to_a() {
        removeBallB();
        addBallA();
        TeamA.ballCount++;
        TeamB.ballCount--;
     
		
        glutPostRedisplay(); 
}

void  removeBallB(){
  for(int i=0;i<MAX_BALLS_PER_TEAM;i++){
 if(ballsB[i].y >= 500){
 
     ballsB[i].radius=0; 
     break;}
    }
      
}

void  removeBallA(){
  for(int i=0;i<MAX_BALLS_PER_TEAM;i++){
 if(ballsA[i].y >= 500){
 
     ballsA[i].radius=0;
     break;}
    }
}

void addBallA() {
    indexA++;
    ballA=indexA;
    if(indexA<MAX_BALLS_PER_TEAM ){
    ballsA[ballA].x=circleXA + 130;
    ballsA[ballA].y=circleYA + 10;
    ballsA[ballA].radius=6;}
    else{ 
    perror("\nFull array of A balls\n");}

}

void addBallB() {
    
    indexB++;
    ballB=indexB;
    if(indexB<MAX_BALLS_PER_TEAM ){
    ballsB[ballB].x=circleXB + 60;
    ballsB[ballB].y=circleYB + 10;
    ballsB[ballB].radius=6; }
    
    else{
    perror("\nFull array of B balls\n");}

}

void ball_from_main_to_a(){

if(doneA==1 &&  TeamA.ballCount==0){
addBallA();
}

if(doneA==0 && TeamA.ballCount==0){
   doneA=1;
   ballA=indexA;
    ballsA[ballA].y=circleYA+10;
    ballsA[ballA].x=circleXA+130;
    TeamA.ballCount++;
  
}
        glutPostRedisplay(); 
}

void ball_from_main_to_b(){


if(doneB==1 && TeamB.ballCount==0){  ///doneB ---> first index (0) done
addBallB();
}
    if(doneB==0 && TeamB.ballCount==0){
    doneB=1;
    ballB=indexB;
    ballsB[ballB].y=circleYB+10;
    ballsB[ballB].x=circleXB+60;
    TeamB.ballCount++;
  }       
        glutPostRedisplay(); 

}
// Function to display
void display (){
  glClear (GL_COLOR_BUFFER_BIT);
  Team teamA, teamB;
  int teamALeadEnergy = rand () % 100 + 1;
  int teamBLeadEnergy = rand () % 100 + 1;

  initializeTeam (&teamA, teamALeadEnergy);
  initializeTeam (&teamB, teamBLeadEnergy);
  drawTeamsVertical (teamA, teamB);
  glColor3f(0.6f, 0.2f, 0.2f); // Darker shade of maroon
  drawHuman(350, WINDOW_HEIGHT - 45);
   
   for(int i=0;i<MAX_BALLS_PER_TEAM;i++){   
  drawCircle (ballsA[i].x, ballsA[i].y, ballsA[i].radius);
  drawCircle (ballsB[i].x, ballsB[i].y, ballsB[i].radius);
  }
  
  
  
// team1 & 2 lables
glColor3f(0.6f, 0.2f, 0.2f); // Darker shade of maroon
 glRasterPos2i(120, 600);
    const char *text1 = "TEAM 1";
    for (const char *c = text1; *c; ++c) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c); 
}
glColor3f(0.6f, 0.2f, 0.2f); // Darker shade of maroon
 glRasterPos2i(480, 600);
    const char *text2 = "TEAM 2";
    for (const char *c = text2; *c; ++c) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c); 
} 
  
  


 glColor3f(0.6f, 0.2f, 0.2f); // Darker shade of maroon
 glRasterPos2i(295, 60);
    const char *text = "SCORE";
    for (const char *c = text; *c; ++c) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
}
//*********

///ball count
    int ballcount1 = readBALLCOUNTFromFile("ballcount.txt");
    // Render the label with the constant value
    char label[50];
    snprintf(label, sizeof(label), "%d", ballcount1);
        glColor3f(0.6f, 0.2f, 0.2f); // Darker shade of maroon
    glRasterPos2f(740, 400); // Set position for label
    for (char *c = label; *c != '\0'; ++c) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
        
        
        glColor3f(0.6f, 0.2f, 0.2f); // Darker shade of maroon
 	glRasterPos2i(650, 450);
    const char *text2 = "TEAM 1 BALLCOUNT";
    for (const char *c = text2; *c; ++c) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
} 
  
        
        
    }


///ball count
    int ballcount2 = readBALLCOUNTFromFile("ballcount2.txt");
    // Render the label with the constant value
    char label2[50];
    snprintf(label2, sizeof(label2), "%d", ballcount2);
        glColor3f(0.6f, 0.2f, 0.2f); // Darker shade of maroon
    glRasterPos2f(740,280); // Set position for label
    for (char *c = label2; *c != '\0'; ++c) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
    }
    
            glColor3f(0.6f, 0.2f, 0.2f); // Darker shade of maroon
 	glRasterPos2i(650, 330);
    const char *text4 = "TEAM 2 BALLCOUNT";
    for (const char *c = text4; *c; ++c) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
} 

snprintf(labelscore1, sizeof(labelscore1), "%d", score_team1);
        glColor3f(0.6f, 0.2f, 0.2f); // Darker shade of maroon
    glRasterPos2f(215,30); // Set position for label
    for (char *c = labelscore1; *c != '\0'; ++c) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
   
    }
    
    /////////////////////////////////////display score
       
        // Render the label with the constant value
   // char labelscore2[50];
    snprintf(labelscore2, sizeof(labelscore2), "%d", score_team2);
        glColor3f(0.6f, 0.2f, 0.2f); // Darker shade of maroon
    glRasterPos2f(435,30); // Set position for label
    for (char *c = labelscore2; *c != '\0'; ++c) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);

    }
    
    
    // show result label when flag =1 ;
    
    if(flag){
    flag2=0;//<---
        //   const char *text3 = "Team 1 wins with a lower score ";    
	glColor3f(0.0f, 0.0f, 0.0f); // Black color
 		glRasterPos2i(190, 300);
    if (score_team1 < score_team2) {
    
   		 const char *text3 = "Team 1 wins with a lower score ";    
 	   for (const char *c = text3; *c; ++c) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
    }

    } else if (score_team2 < score_team1) {
    
       		 const char *text3 = "Team 2 wins with a lower score";
   	   for (const char *c = text3; *c; ++c) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);    
    }
    }
    
    }
    
    
      if(flag2){
      flag =0; //<--
        //   const char *text3 = "Team 1 wins with a lower score ";    
	glColor3f(0.0f, 0.0f, 0.0f); // Black color
 		glRasterPos2i(190, 300);
    if (ballcount1 < ballcount2) {
    
   		 const char *text4 = "Team 1 wins with a lower ball count";    
 	   for (const char *c = text4; *c; ++c) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
    
    }
    } else if (ballcount2 < ballcount1) {
    
       		 const char *text4 = "Team 2 wins with a lower ball count";
   	   for (const char *c = text4; *c; ++c) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
    
    }
    }
    else{
     const char *text4 = "ITS A TIE";
   	   for (const char *c = text4; *c; ++c) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
    }

    } 
    }

///
  

  
   glFlush ();
}

// Function to initialize OpenGL
void init (){
glClearColor(1.0f, 0.8f, 0.8f, 1.0f); // Light pink background color
  glMatrixMode (GL_PROJECTION);
  gluOrtho2D (0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
}

int main (int argc, char **argv){

    TeamB.ballCount=0;   ////////////////////////////////////Note for me (Amal): must change it to zero after finishing the main problem
    TeamA.ballCount=0;

    
      fileA = fopen("ballsA.txt", "r"); // Open the file for reading
    if (fileA == NULL) {
        perror("Error opening ballsA file");
        return 0;
    }
    
    
       fileB = fopen("ballsB.txt", "r"); // Open the file for reading
    if (fileB == NULL) {
        perror("Error opening ballsB file");
        return 0;
    }

      
    ballsA[0].team=0;
    ballsB[0].team=1;
    ballsA[0].x=newcircleXA + 130;
    ballsA[0].y=newcircleYA + 10;
    ballsA[0].radius=6;
    ballsB[0].x=newcircleXB + 60;
    ballsB[0].y=newcircleYB + 10;
    ballsB[0].radius=6;

    
	  FILE* glpid_file = fopen("openglid.txt", "w"); // Open the file in write mode
	
		    if (glpid_file == NULL) {
			perror("Error opening glpid_file.txt\n");
			exit(-1);
		    }
  	fprintf(glpid_file, "%d\n", getpid()); // Save child PID to the file
    			fclose(glpid_file);
  


        // for ball downward for ball A 
        if (sigset(SIGUSR1, update_ball_team_a) == SIG_ERR) {
            perror("Signal Error\n");
            exit(-1);
        }
        // for ball downward for ball B 
        if (sigset(SIGUSR2, update_ball_team_b) == SIG_ERR) {
            perror("Signal Error\n");
            exit(-1);
        }
        

        // for ball from team lead A to B
        if (sigset(SIGXCPU, update_ball_from_lead_a_to_b) == SIG_ERR) {
            perror("Signal Error\n");
            exit(-1);
        }
        // for ball from team lead B to A
        if (sigset(SIGURG, update_ball_from_lead_b_to_a) == SIG_ERR) {
            perror("Signal Error\n");
            exit(-1);
        }
        
        if (sigset(SIGHUP, ball_from_main_to_a) == SIG_ERR) {
            perror("Signal Error\n");
            exit(-1);
        }
        
         if (sigset(SIGBUS, ball_from_main_to_b) == SIG_ERR) {
            perror("Signal Error\n");
            exit(-1);
        }
        
        
        if (sigset(SIGPWR, handle_round_end) == SIG_ERR) {
            perror("Signal Error\n");
            exit(-1);
        }
        
        if (sigset(SIGCHLD, handle_time_expired) == SIG_ERR) {
            perror("Signal Error\n");
            exit(-1);
        }
        
        
        if (sigset(SIGINT,leader_to_player_0B ) == SIG_ERR) {
            perror("Signal Error\n");
            exit(-1);
        }
         if (sigset(SIGSYS, leader_to_player_0A) == SIG_ERR) {
            perror("Signal Error\n");
            exit(-1);
        }

        
                

        
        
  
  srand (time (NULL));
  glutInit (&argc, argv);
  glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize (WINDOW_WIDTH, WINDOW_HEIGHT);
  glutCreateWindow ("Beach Game");
  glutDisplayFunc (display);
  init ();
  glutWMCloseFunc(closeCallback); 
  glutMainLoop ();

  return 0;
}

// Function to read the value from the file
int readBALLCOUNTFromFile(const char *filename) {
    int ballcount;

    FILE *file = fopen(filename, "r");
    if (file != NULL) {
        fscanf(file, "%d", &ballcount);
        fclose(file);
    }
    
    
    
    return ballcount;
}


void handle_round_end(int sig) {



FILE *file = fopen("constants.txt", "r");
    if (!file) {
        perror("Failed to open Constants file");
    }

    int max_seconds, max_rounds;
    if (fscanf(file, "%d\n%d", &max_seconds, &max_rounds) != 2) {
        fprintf(stderr, "Error reading constants from file\n");
        fclose(file);
    }
    fclose(file);


         FILE* ballcount_read_fileT = fopen("ballcount.txt", "r");
	               int ballcount1;		

		 fscanf(ballcount_read_fileT , "%d", &ballcount1);
					
			fclose(ballcount_read_fileT );

       
       
        FILE* ballcount2_read_fileT = fopen("ballcount2.txt", "r");
	               int ballcount2;		

		fscanf(ballcount2_read_fileT , "%d", &ballcount2);
		fclose(ballcount2_read_fileT );
      

        // Compare ball counts and update scores
        if (ballcount1 > ballcount2) {
            score_team1++;
        } else if (ballcount2 > ballcount1) {
            score_team2++;
        }


        
        // Check if the maximum number of rounds has been reached
        if ( score_team1 == max_rounds ||  score_team2 == max_rounds) {
 		flag =1;
 		 glutPostRedisplay(); 
        } 
        
 }

