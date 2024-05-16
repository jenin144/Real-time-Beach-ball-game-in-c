#include "header.h"
#include "constants.h"



time_t start_time;
char *fifo_PTOCH1 = "/tmp/PTOCH1";
char *fifo_PTOCH2 = "/tmp/PTOCH2";

void signal_handler1(int sig);
void signal_handler2(int sig);

typedef void (*sighandler_t)(int);
sighandler_t sigset(int sig, sighandler_t disp);


char msg_s[BUFSIZ];
pid_t teams[2];
pid_t gl_pid;
    pid_t team1_pid, team2_pid;	
volatile sig_atomic_t simulation_end = 0;

   int score_team1 = 0, score_team2 = 0;

   int current_round = 0, max_rounds = 0; // max_rounds will be read from the file Constants.txt

void handle_time_expired(int sig) {
    if (sig == SIGALRM) {
        printf("Time has expired. Ending simulation.\n");
        fflush(stdout);
        simulation_end = 1;
    }
}




void handle_round_end(int sig) {


kill(gl_pid,SIGPWR);

if (sig == SIGALRM) {
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
    
        time_t current_time = time(NULL);
        double elapsed_seconds = difftime(current_time, start_time);
        
   if (elapsed_seconds >= max_seconds) {
            kill(gl_pid,SIGCHLD);
            printf("Maximum simulation time exceeded.\n");
            simulation_end = 1;
            // Determine winner based on ballcount logic here
        }
        
        
        current_round++;

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

        printf("Round %d ended. Scores - Team 1: %d, Team 2: %d\n", current_round, score_team1, score_team2);
                fflush(stdout);

        
        // Check if the maximum number of rounds has been reached
        if ( score_team1 == max_rounds ||  score_team2 == max_rounds) {
            simulation_end = 1; // To stop the simulation
            alarm(0); // Cancel any pending alarms
        } else {
            alarm(5); // Schedule the next round in 10 seconds
        }
        
    }
}

///////////////////////////
int main(){



    FILE *file = fopen("constants.txt", "r");
    if (!file) {
        perror("Failed to open Constants file");
        return EXIT_FAILURE;
    }

    int max_seconds, max_rounds;
    if (fscanf(file, "%d\n%d", &max_seconds, &max_rounds) != 2) {
        fprintf(stderr, "Error reading constants from file\n");
        fclose(file);
        return EXIT_FAILURE;
    }
    fclose(file);

    printf("Simulation parameters: %d seconds per round, %d rounds lost allowed\n", max_seconds, max_rounds);
            fflush(stdout);

///////





  
    remove(fifo_PTOCH1);
    if (mkfifo(fifo_PTOCH1, S_IFIFO | 0777) == -1){ // crate fifo betwee parent & team1
        perror("Fifo Error\n");
        exit(-1);
    }
    
    
    
    remove(fifo_PTOCH2);
    if (mkfifo(fifo_PTOCH2, S_IFIFO | 0777) == -1){// crate fifo betwee parent & team2
        perror("Fifo Error\n");
        exit(-1);
    }
    
    

    pid_t parent_id = getpid();
    printf("Hello i'm the main file, my id is: %d\n", parent_id);
           fflush(stdout);



  	srand((unsigned) getpid());
      int energy1=rand() % (100 - 90 + 1) + 90;

      int energy2=rand() % (100 - 90 + 1) + 90;
    
    FILE* leaderenergyF = fopen("leaderEng.txt", "w"); // Open the file in  mode

    if (leaderenergyF == NULL) {
        perror("Error opening leaderEng.txt\n");
        exit(-1);
    }

         fprintf(leaderenergyF, "%d\n", energy1); // Save child PID to the file
         fprintf(leaderenergyF, "%d\n", energy2); // Save child PID to the file                        
            fclose(leaderenergyF);

    char energy1string[10]; // Assuming energy1 is an integer value
    sprintf(energy1string, "%d", energy1); // Convert energy1 to a string
    char energy2string[10]; // Assuming energy1 is an integer value
    sprintf(energy2string, "%d", energy2); // Convert energy1 to a string


    FILE* pid_file = fopen("teamPID.txt", "w"); // Open the file in write mode
    if (pid_file == NULL) {
        perror("Error opening pids.txt\n");
        exit(-1);
    }
    
    
    

 	FILE* pid_read_filegl = fopen("openglid.txt", "r");
			 fscanf(pid_read_filegl, "%d", &gl_pid);
			 fclose(pid_read_filegl);		 

    

for (int i = 0; i < 2; ++i) {
        teams[i] = fork();
        // the team will continue from here
        if(teams[i] == -1){
            perror("No memory\n");
            exit(-5);
        } else if(teams[i] == 0){
        
	if (i == 0) {
	     
		printf("Team 1 (PID: %d)\n", getpid()); // team 1 go to team1.c file 
		 execlp("./team1", "team1" , energy1string, NULL); // i can  pass any number of arguments
	 } else {
	 
	 	
		 printf("Team 2 (PID: %d)\n", getpid()); // team 2 go to team2.c file 
		 execlp("./team2", "team2",energy2string, NULL);
		}
		    // If exec fails
		    perror("Exec failed\n");
		    exit(-1);
		
        
        }else{
	if (i==0){
            printf("The main file, create the team id: %d with energy = %d\n", teams[i],energy1);
        }        
        else{
        
            printf("The main file, create the team id: %d with energy = %d\n", teams[i],energy2);
        }
           // printf("The main file, create the team id: %d\n", teams[i]);
            	fprintf(pid_file, "%d\n", teams[i]); // Save team PID to the file
        }
    }
    
      fclose(pid_file); // Close the file
        
     sleep(1);



	// Parent will write to the two fifos 

	   int f1 = open(fifo_PTOCH1, O_RDONLY | O_NONBLOCK);
	   int f2 = open(fifo_PTOCH2, O_RDONLY | O_NONBLOCK);



    if ((f1 = open(fifo_PTOCH1, O_WRONLY | O_NONBLOCK)) == -1){
        perror("Open Error\n");
        exit(-1);
    }else {

        strcpy(msg_s, "BALL");
        if ( write(f1, msg_s, sizeof(msg_s)) == -1){
            perror("Write Error\n");
            exit(-1);
        }



    }

    close(f1);


 if ((f2 = open(fifo_PTOCH2, O_WRONLY | O_NONBLOCK)) == -1){
        perror("Open Error\n");
        exit(-1);
    }else {

        strcpy(msg_s, "BALL");
        if ( write(f2, msg_s, sizeof(msg_s)) == -1){
            perror("Write Error\n");
            exit(-1);
        }
        



    }

    close(f2);


        printf(" the two balls were sent\n");
        fflush(stdout);
        alarm(5); // Start the first round
	for (int i=0 ; i<2 ; i++){
        kill(teams[i], SIGINT);
        }
        
        
                kill(gl_pid, SIGHUP);
	        kill(gl_pid, SIGBUS);



/////////////////////////////////////////////////


	
	while(1){
        
         if (sigset(SIGUSR1, signal_handler1) == SIG_ERR) {
            perror("Signal Error\n");
            exit(-1);
        }    
        
         if (sigset(SIGUSR2, signal_handler2) == SIG_ERR) {
            perror("Signal Error\n");
            exit(-1);
        }  
        
            signal(SIGALRM, handle_round_end);       
            alarm(5);
         
          start_time = time(NULL); // Record start time of the simulation
          
   
       /////////////////
          FILE* ballcount_read_fileT = fopen("ballcount.txt", "r");
	               int ballcount1;		

		 fscanf(ballcount_read_fileT , "%d", &ballcount1);
					
			fclose(ballcount_read_fileT );

       
       
        FILE* ballcount2_read_fileT = fopen("ballcount2.txt", "r");
	               int ballcount2;		

		fscanf(ballcount2_read_fileT , "%d", &ballcount2);
		fclose(ballcount2_read_fileT );


       
          
//**************************              
    // Wait for a signal indicating a team has lost or time has expired
    while (!simulation_end) {
        pause();
    }
    



	
    // terminate child processes 
    kill(team1_pid, SIGTERM);
    kill(team2_pid, SIGTERM);

    while (wait(NULL) > 0);




    printf("The game has ended.\n");
    return EXIT_SUCCESS;
    
//***************************
    
}




}

 ///////////////////////////////////////////////////////
  
void signal_handler1(int sig) { // player to player

		     int flag;
		    
		    do{
		    
		       int f1 = open(fifo_PTOCH1, O_RDONLY | O_NONBLOCK);
		    	     flag =0 ; 
		    if ((f1 = open(fifo_PTOCH1, O_WRONLY | O_NONBLOCK)) == -1){
			perror("Open Error\n");
			//exit(-1);
			   sleep(1);
			   flag = 1;
			  close(f1); 
			   break; 
			   
		    }else {

			strcpy(msg_s, "BALL");
			if ( write(f1, msg_s, sizeof(msg_s)) == -1){
			    perror("Write Error\n");
			   // exit(-1);
			   sleep(1);
			   flag = 1; 
			}



		    }

		    close(f1); 
		    
		    
		    }  while(flag != 0 );
    
    printf("The signal %d reached process with ID: %d\n", sig, getpid());
                fflush(stdout); 
                 printf("ANOTHER BALL IS SENT NOW to team 1 \n");
                fflush(stdout);
 	   kill(teams[0], SIGINT);
 	   kill(gl_pid, SIGHUP);
   
}



void signal_handler2(int sig) { // player to player
	 

	  int f2 = open(fifo_PTOCH2, O_RDONLY | O_NONBLOCK);

	    int flag;
	    
	    do{
	    	     flag =0 ; 
	    if ((f2 = open(fifo_PTOCH2, O_WRONLY | O_NONBLOCK)) == -1){
		perror("Open Error\n");
		//exit(-1);
	           sleep(1);
		   flag = 1;
	          close(f2); 
		   break; 
		   
	    }else {

		strcpy(msg_s, "BALL");
		if ( write(f2, msg_s, sizeof(msg_s)) == -1){
		    perror("Write Error\n");
		   // exit(-1);
		   sleep(1);
		   flag = 1; 
		}



	    }

	    close(f2); 
	    
	    
	    }  while(flag != 0 );
	    
	    
	    
	    
	    printf("The signal %d reached process with ID: %d\n", sig, getpid());
		        fflush(stdout); 
	     printf("####################################ANOTHER BALL IS SENT NOW to team 2 \n");
		        fflush(stdout);            
	    kill(teams[1], SIGINT);  
	    kill(gl_pid, SIGBUS);   
	    
	 
		              
	}
