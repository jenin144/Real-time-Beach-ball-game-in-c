#include "header.h"
#include "constants.h"



#define NUM_CHILDREN 5

char *fifo_PTOCH1= "/tmp/PTOCH1"; // main to parent
char *fifo10 = "/tmp/fifo10"; // parent to child 0 
char *fifo11 = "/tmp/fifo11";
char *fifo12 = "/tmp/fifo12";
char *fifo13 = "/tmp/fifo13";
char *fifo14 = "/tmp/fifo14";
char *fifo15 = "/tmp/fifo15"; // child 5 to parent 
char *fifop1p2 = "/tmp/fifop1p2";//  parent(team leader) to parent(team leader)
char *fifop2p1 = "/tmp/fifop2p1";//  parent(team leader) to parent(team leader)
int number = 0;		
int ballcount=0;
char source[20] ; 
char dest[20] ; 

int performAction(int energy);
void signal_handler(int sig);
void Finishsignal_handler(int sig);
void startsignal_handler(int sig);

typedef void (*sighandler_t)(int);
sighandler_t sigset(int sig, sighandler_t disp);

	 pid_t gl_pid;	

int main(int argc, char** argv) {

 	FILE* pid_read_fileT = fopen("teamPID.txt", "r");
		for (int j = 0; j <= 3; ++j) {
			 fscanf(pid_read_fileT, "%d", &gl_pid);
					}
		fscanf(pid_read_fileT, "%d", &gl_pid); // Read the next PID
			fclose(pid_read_fileT);

 	FILE* pid_read_filegl = fopen("openglid.txt", "r");
			 fscanf(pid_read_filegl, "%d", &gl_pid);
			 fclose(pid_read_filegl);


		

    if (argc < 1) {
        perror("Argc less than 1\n");
        exit(-1);
    }
    
    char msg_s[BUFSIZ];

    remove(fifo10);
    if (mkfifo(fifo10, S_IFIFO | 0777) == -1){
        perror("Fifo Error\n");
        exit(-1);
    }
    
    remove(fifo11);
    if (mkfifo(fifo11, S_IFIFO | 0777) == -1){
        perror("Fifo Error\n");
        exit(-1);
    }
        remove(fifo12);
    if (mkfifo(fifo12, S_IFIFO | 0777) == -1){
        perror("Fifo Error\n");
        exit(-1);
    }
        remove(fifo13);
    if (mkfifo(fifo13, S_IFIFO | 0777) == -1){
        perror("Fifo Error\n");
        exit(-1);
    }
        remove(fifo14);
    if (mkfifo(fifo14, S_IFIFO | 0777) == -1){
        perror("Fifo Error\n");
        exit(-1);
    }
            remove(fifo15);
    if (mkfifo(fifo15, S_IFIFO | 0777) == -1){
        perror("Fifo Error\n");
        exit(-1);
    }
               remove(fifop1p2);
    if (mkfifo(fifop1p2, S_IFIFO | 0777) == -1){
        perror("Fifo Error\n");
        exit(-1);
    }
                 remove(fifop2p1);
    if (mkfifo(fifop2p1, S_IFIFO | 0777) == -1){
        perror("Fifo Error\n");
        exit(-1);
    }   
    
    
/////////////  

int leaderEnergy = atoi(argv[1]); 

    sleep(0.6);

    pid_t parent_id = getpid();
    pid_t players[NUM_CHILDREN];
    int energy[NUM_CHILDREN];
    FILE* pid_file = fopen("pids1.txt", "w"); // Open the file in write mode
    FILE* ballA = fopen("ballsA.txt", "w"); // Open the file in write mode

    if (pid_file == NULL) {
        perror("Error opening pids.txt\n");
        exit(-1);
    }
    
      srand((unsigned) getpid());

    for (int i = 0; i < NUM_CHILDREN; ++i) {
        players[i] = fork();
        energy[i]=rand() % (100 - 90 + 1) + 90;

        if (players[i] == -1) {
            perror("No memory\n");
            exit(-5);
            
            
            
/////////children///////////////            
            
            
        } else if (players[i] == 0) {
        
        
        switch(i){
        
        
        case 0:
        strcpy(source , fifo10);
        strcpy(dest , fifo11);
        break;
        
        case 1:
                strcpy(source , fifo11);
        strcpy(dest , fifo12);
        break;
        
        
        case 2:
                strcpy(source , fifo12);
        strcpy(dest , fifo13);
        break;
        
        case 3:
                    strcpy(source , fifo13);
        strcpy(dest , fifo14);
        break;
        case 4:
            
        strcpy(source , fifo14);
        strcpy(dest , fifo15);
        break;

        
        
        }
        
        
        
        while(1){
            int f;
	    char msg_r[BUFSIZ] = "";
        
        
        
            if (sigset(SIGUSR1, signal_handler) == SIG_ERR) {
                perror("Signal Error\n");
                exit(-1);
            }
		    pause();
		
		
		
		
	
            
            // extract the ball number from the ball msg
            


    
		    
	// read the messege using fifo when you wakeup	                
                if ((f = open(source, O_RDONLY | O_NONBLOCK)) == -1){
		perror("Open Error\n");
		exit(-1);
		    }else {

		if ( read(f, msg_r, sizeof(msg_r)) == -1){
		    perror("Read Error\n");
		    exit(-1);
		}

	   	 }close(f);
	   	 
   	 

		     
            printf("Child %d (ID: %d) received signal<<< msgsss>>> = %s \n ", i, getpid() , msg_r);
            fflush(stdout);
            
          
               f = open(dest, O_RDONLY | O_NONBLOCK);

		    if ((f = open(dest, O_WRONLY | O_NONBLOCK)) == -1){
			perror("Open Error\n");
			exit(-1);
		    }else {

	       
		if ( write(f, msg_r, sizeof(msg_r)) == -1){
		    perror("Write Error\n");
		    exit(-1);
		}



	    }
	    close(f);
            
            

            
            	    
	    int baallnumber;
            
            // extract the ball number from the ball msg
            

	    sscanf(msg_r, "%d", &baallnumber);
            
            
            

            if (i < NUM_CHILDREN - 1) {
                // Read the next PID from the file
                FILE* pid_read_file = fopen("pids1.txt", "r");
                pid_t next_pid;
                for (int j = 0; j <= i; ++j) {
                    fscanf(pid_read_file, "%d", &next_pid);
                }
                fscanf(pid_read_file, "%d", &next_pid); // Read the next PID
                fclose(pid_read_file);

                printf("Child %d (ID: %d) passing signal to Child %d (ID: %d) with energy = %d\n", i, getpid(), i + 1, next_pid,energy[i]);
                fflush(stdout);
                 energy[i]= performAction(energy[i]);
                kill(next_pid, SIGUSR1);               
                // id ball 
                
                
    
         	   fprintf(ballA, "%d\n",baallnumber); // Save child PID to the file
       		  fflush(ballA); // Flush the buffer to ensure immediate write to disk
       				 

                
                ///
                kill(gl_pid,SIGUSR1);
            } else {
                printf("Child %d (ID: %d) passing signal to Parent (ID: %d)\n", i, getpid(), parent_id);
                fflush(stdout);
                 energy[i]= performAction(energy[i]);
                kill(parent_id, SIGUSR1);
                
                 fprintf(ballA, "%d\n",baallnumber); // Save child PID to the file
       		 fflush(ballA); // Flush the buffer to ensure immediate write to disk
                
                kill(gl_pid,SIGUSR1);
            }
            


            
            
        }
        
        
        }
         else {
        
            fprintf(pid_file, "%d\n", players[i]); // Save child PID to the file
            printf("-----> The team 1 file, create the player energy for player %d = %d with id %d\n",i,energy[i], players[i]);
           // energy[i]= performAction(energy[i]);
        }
    }

    fclose(pid_file); // Close the file
    fclose(ballA); // Close the file
//////////////parent////////////


    if (parent_id == getpid()) { // If you are the team leader, catch the signal
    
    while(1){
    
    
    
        if (sigset(SIGINT, startsignal_handler) == SIG_ERR) {
            perror("Signal Error\n");
            exit(-1);
        }
        
        
        
      if (sigset(SIGUSR1, Finishsignal_handler) == SIG_ERR) {
            perror("Signal Error\n");
            exit(-1);
        }    
        

        
      		 pause();
            leaderEnergy= performAction(leaderEnergy);

    }
}

    return 0;
}





/////// handlers ////////////
void signal_handler(int sig) { // player to player
    printf("The signal %d reached process with ID: %d\n", sig, getpid());
                fflush(stdout); 
                
                
       
                         
}


void startsignal_handler(int sig) {// main to team leader


		// counter of ball count .


		    printf("The signal %d reached to team 1 process with ID: %d\n", sig, getpid());
				fflush(stdout);

		ballcount++;
		  FILE* ballcount_file = fopen("ballcount.txt", "w"); // Open the file in write mode

		    if (ballcount_file == NULL) {
			perror("Error opening ballcount.txt\n");
			exit(-1);
		    }
		  fprintf(ballcount_file, "%d\n", ballcount); // Save child PID to the file
		    			fclose(ballcount_file);
	  
	  
	  

	FILE* energy_fileT = fopen("leaderEng.txt", "r");
			        int leaderEnergy;		

			 fscanf(energy_fileT, "%d", &leaderEnergy);
						
				fclose(energy_fileT);




	          
          /// when you wakeup read the messaage from your parent fifo then write the same message to your children fifo 
          
          
         //read
          // when fist signal came from the main read the fifo
                
		         int ff;
	    char msg_fromparent_or_otherteam[BUFSIZ] = "";



	    if ((ff = open(fifo_PTOCH1, O_RDONLY | O_NONBLOCK)) == -1){
		perror("Open Error\n");
		exit(-1);
	    }else {

		if ( read(ff, msg_fromparent_or_otherteam, sizeof(msg_fromparent_or_otherteam)) == -1){
		    perror("Read Error\n");
		    exit(-1);
		}

	    }

	    close(ff);

                
              
                 
          
          ////////////////////////////////////////////////////
          
                    //// or from team 2 leader.
           if(strcmp(msg_fromparent_or_otherteam , "") == 0){
            
        

	    if ((ff = open(fifop2p1, O_RDONLY | O_NONBLOCK)) == -1){
		perror("Open Error\n");
		exit(-1);
	    }else {

		if ( read(ff, msg_fromparent_or_otherteam, sizeof(msg_fromparent_or_otherteam)) == -1){
		    perror("Read Error\n");
		    exit(-1);
		}

	    }

	    close(ff);
            
                
            
            }    
            						//apeend ball id to the ball msg 

					    char tempppp[BUFSIZ+10];
					    sprintf(tempppp, "%d%s", number, msg_fromparent_or_otherteam);

					    // Copy the modified string back to msg_fromparent_or_otherteam
					    strncpy(msg_fromparent_or_otherteam, tempppp, sizeof(msg_fromparent_or_otherteam) - 1);
					    msg_fromparent_or_otherteam[sizeof(msg_fromparent_or_otherteam) - 1] = '\0'; // Null-terminate the string

    		
						  
							    //write 
						 int f = open(fifo10, O_RDONLY | O_NONBLOCK);



						    if ((f = open(fifo10, O_WRONLY | O_NONBLOCK)) == -1){
							perror("Open Error\n");
							exit(-1);
						    }else {

					       
						if ( write(f, msg_fromparent_or_otherteam, sizeof(msg_fromparent_or_otherteam)) == -1){
						    perror("Write Error\n");
						    exit(-1);
						}



					    }
						    close(f);
					   
					number++;	    
	    
	    
	    
	    
	                    FILE* pid_read_fileT = fopen("pids1.txt", "r");
	                pid_t player0pid;		

		 fscanf(pid_read_fileT, "%d", &player0pid);
					
			fclose(pid_read_fileT);
                                     
	  
	    printf("Parent (ID: %d) passing signal msg = %s to Child 0 (ID: %d)\n", getpid(),msg_fromparent_or_otherteam , player0pid);
                         
			leaderEnergy= performAction(leaderEnergy);
      			 kill(player0pid, SIGUSR1);
      			// kill (gl_pid, SIGUSR1);
 			kill (gl_pid, SIGSYS);

}

//***************************************************************************************************************************************************************************
void Finishsignal_handler(int sig) {// player 5 to team leader
    printf("-----end of the cycle ------The signal %d reached parent process with ID: %d\n", sig, getpid());
                  fflush(stdout);
    
     int ff;
   char msg_fromlastchild[BUFSIZ] = "";



	    if ((ff = open(fifo15, O_RDONLY | O_NONBLOCK)) == -1){
		perror("Open Error\n");
		exit(-1);
	    }else {

		if ( read(ff, msg_fromlastchild, sizeof(msg_fromlastchild)) == -1){
		    perror("Read Error\n");
		    exit(-1);
		}

	    }

	    close(ff);

   
   // write the msg to the team 2 leader fifo 
   printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>msgggggg from last child = %s\n" , msg_fromlastchild);
   
   	
   	   //write 
         int f = open(fifop1p2, O_RDONLY | O_NONBLOCK);



	    if ((f = open(fifop1p2, O_WRONLY | O_NONBLOCK)) == -1){
		perror("Open Error\n");
		exit(-1);
	    }else {

       strcpy(msg_fromlastchild,"BALL");
        if ( write(f, msg_fromlastchild, sizeof(msg_fromlastchild)) == -1){
            perror("Write Error\n");
            exit(-1);
        }



    }
	    close(f);
	    
pid_t parent_pid = getppid();
   	
   	
   	////////////// read team 2 pid then send the signal.
   
   

	       FILE* pid_read_fileT = fopen("teamPID.txt", "r");
		                pid_t next_pid;	
		for (int j = 0; j <= 2; ++j) {
			 fscanf(pid_read_fileT, "%d", &next_pid);
					}
		fscanf(pid_read_fileT, "%d", &next_pid); // Read the next PID

			fclose(pid_read_fileT);
			
			
			FILE* energy_fileT = fopen("leaderEng.txt", "r");
	                int leaderEnergy;		

		 fscanf(energy_fileT, "%d", &leaderEnergy);
					
			fclose(energy_fileT);

	leaderEnergy= performAction(leaderEnergy);
        kill(next_pid, SIGINT);
       kill(gl_pid,SIGXCPU);	
	
	ballcount--;	
	FILE* ballcount_file = fopen("ballcount.txt", "w"); // Open the file in write mode
	    if (ballcount_file == NULL) {
		perror("Error opening ballcount.txt\n");
		exit(-1);
		    }
	  fprintf(ballcount_file, "%d\n", ballcount); // Save child PID to the file
	  fclose(ballcount_file);

	if (ballcount ==0){
		leaderEnergy= performAction(leaderEnergy);
		kill(parent_pid, SIGUSR1);
		}
			
}





int performAction(int energy) {

	    // Calculate pause duration based on player's energy level and ball count
	    
	    
	    int energyPause =((100-energy)/3)+1;//*1000000;//rand() % (5) + 1;// max_sleep - (energy / 100) * (max_sleep - min_sleep); //rand()%(energy);
	 sleep(energyPause); // Simulate pause
	   
	   
	   
	   	if (energy > 40 ){
	    // Randomly determine energy consumption rate within a range
	    int randomConsumption = rand() % (5) + 1;
	    
	    
	    // Consume energy for the action using the random consumption rate
	     return energy = energy - randomConsumption; // Decrease player's energy level by the random consumption rate
		}
		else { return energy ; } 

	}

