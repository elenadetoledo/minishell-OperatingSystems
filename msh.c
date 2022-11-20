//  MSH main file
// Write your msh source code here

//#include "parser.h"
#include <stddef.h>			/* NULL */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_COMMANDS 8


// files in case of redirection
char filev[3][64];

//to store the execvp second parameter
char *argv_execvp[8];

void siginthandler(int param)
{
	printf("****  Exiting MSH **** \n");
	signal(SIGINT, siginthandler);
	exit(0);
}

/**
 * Get the command with its parameters for execvp
 * Execute this instruction before run an execvp to obtain the complete command
 * @param argvv
 * @param num_command
 * @return
 */
void getCompleteCommand(char*** argvv, int num_command) {
	//reset first
	for(int j = 0; j < 8; j++)
		argv_execvp[j] = NULL;

	int i = 0;
	for ( i = 0; argvv[num_command][i] != NULL; i++)
		argv_execvp[i] = argvv[num_command][i];
}


/**
 * Main sheell  Loop  
 */
int main(int argc, char* argv[])
{
	//This variable is created for the function mycalc. It should be declared here to avoid the reset of the value
	int env_sum = 0;
	
	/**** Do not delete this code.****/
	int end = 0; 
	int executed_cmd_lines = -1;
	char *cmd_line = NULL;
	char *cmd_lines[10];

	if (!isatty(STDIN_FILENO)) {
		cmd_line = (char*)malloc(100);
		while (scanf(" %[^\n]", cmd_line) != EOF){
			if(strlen(cmd_line) <= 0) return 0;
			cmd_lines[end] = (char*)malloc(strlen(cmd_line)+1);
			strcpy(cmd_lines[end], cmd_line);
			end++;
			fflush (stdin);
			fflush(stdout);
		}
}

	/*********************************/

	char ***argvv = NULL;
	int num_commands;


	while (1){
		int status = 0;
		int command_counter = 0;
		int in_background = 0;
		signal(SIGINT, siginthandler);

		// Prompt 
		
		write(STDERR_FILENO, "MSH>>", strlen("MSH>>"));
		// Get command
		//********** DO NOT MODIFY THIS PART. IT DISTINGUISH BETWEEN NORMAL/CORRECTION MODE***************
		executed_cmd_lines++;
		if( end != 0 && executed_cmd_lines < end) {
			command_counter = read_command_correction(&argvv, filev, &in_background, cmd_lines[executed_cmd_lines]);
		}
		else if( end != 0 && executed_cmd_lines == end) {
			return 0;
		}
		else {
			command_counter = read_command(&argvv, filev, &in_background); //NORMAL MODE                                                                                                                                               
		}
		//************************************************************************************************


		/************************ STUDENTS CODE ********************************/
	   if (command_counter > 0) {
			if (command_counter > MAX_COMMANDS){
				printf("Error: Maximum number of commands is %d \n", MAX_COMMANDS);
			}
			else {
				// Print command
				//print_command(argvv, filev, in_background);
				//This function is used to iterate through all the commands enterd by the user. It takes values between 0 and command_counter-1.
				int ncommand = 0;
				
				 //This variable changes its value when a syntax error is detected.
				int error_syntax=0;
				//Here we use the function getCompleteCOmmand to fill up the arg_excevp for the first command.
				getCompleteCommand(argvv,ncommand);
				
				//Mycalc command
				if(strcmp(argv_execvp[0],"mycalc")==0){
				//Correct syntax: 
					if (argv_execvp[1] != NULL && argv_execvp[2] != NULL && argv_execvp[3] != NULL && argv_execvp[4] == NULL){
						int operand1 = atoi(argv_execvp[1]);
			      int operand2 = atoi(argv_execvp[3]);
			      if ((operand1 !=0 || (strcmp(argv_execvp[1], "0")==0))&&(operand2 !=0 || (strcmp(argv_execvp[3], "0")==0))){ //To check if they are integers 
						
						//When the operator is add:
						if (strcmp(argv_execvp[2],"add")==0){
			        int sum_result = operand1 + operand2;
			        env_sum = env_sum + sum_result; 
			        char env_sumtostring[20];
			        sprintf(env_sumtostring, "%d", env_sum); //transform env_sum from int to string
			        const char *env_var = env_sumtostring;
			        char *acc;
			        int set_error = setenv("Acc", env_var, 1); // stores env_var in Acc, we need the third argument to be different from zero.
			        if (set_error < 0){
			        	perror("Error with setenv function");
			        	return -1;
			        }
			        acc = getenv("Acc");
			        //If the getenv variable doesn't work it sets the string to false. This way we can check if there's any error.
			        if (strcmp(acc,"false")==0){
			        	perror("Error with the getenv function.\n");
			        	return -1;
			        }
			 				char add_message[100];
							snprintf(add_message, 100, "[OK] %s + %s = %d; Acc %s\n", argv_execvp[1], argv_execvp[3], sum_result, acc);
							int nwrite = write(STDERR_FILENO, add_message, strlen("[OK] s + s = d; Acc s\n")+3);
							if (nwrite < 0){
								perror("Error writing\n");
								return -1;
							}
							//If the operator is mod
						}else if(strcmp(argv_execvp[2], "mod") == 0){
							int mod_result = operand1 % operand2;
							int quotient_result = operand1/ operand2;
							char mod_message[100];
							snprintf(mod_message, 100, "[OK] %s %% %s = %d; Quotient %d\n", argv_execvp[1], argv_execvp[3], mod_result, quotient_result);
							int nwrite = write(STDERR_FILENO, mod_message, strlen("[OK] s + s = d; Acc s\n")+6);	
							if (nwrite < 0){
								perror("Error writing");
								return -1;
							}	
						}else{ //If operator is neither add nor mod
							error_syntax=1;
						}
					}else{ //The operands aren't integers.
						error_syntax=1;
					}
						
					}else{ //If either the operands or operator is NULL
						error_syntax=1;
					}
					if(error_syntax == 1){ //Incorrect structure
						char error_message[100] = "[ERROR] The structure of the command is mycalc <operand_1> <add/mod> <operand_2>\n";
		        if((write(STDERR_FILENO, error_message, strlen(error_message)))<0){
		        perror("There's been an error with the write system call.\n");
		        return -1;
		        }
          }		
          
          //Mycp command
					}else if(strcmp(argv_execvp[0],"mycp")== 0){
					//We check that the arguments are properly passed to the command
						if (argv_execvp[1] != NULL && argv_execvp[2] != NULL && argv_execvp[3] == NULL){
							int sourcef;
							int destf;
							//open file from which we will read (source file) 
		  				if ((sourcef = open(argv_execvp[1], O_RDONLY, 0666)) < 0){
								//control error when opening source file 
								char error_message[50] ="[ERROR] Error opening original file \n";
		            if((write(STDOUT_FILENO, error_message, strlen(error_message))) < 0 ){ 
									perror ("Error writing the message");
								}
							}else{ //The source has opened correctly
								char buffer[1024];//Buffer to store read data
								//Open file to write in and control the error
								if ((destf=open(argv_execvp[2], O_WRONLY | O_CREAT, 0666))<0){
									// Write error message in the STANDARD OUTPUT and control the error
									char error_message[50] = "[ERROR] Error opening copied file: \n";
		            	if((write(STDOUT_FILENO, error_message, strlen(error_message))) < 0 ){ 
										perror ("Error writing the message\n");
									}
									if(close(sourcef) < 0){
											perror("Error closing source file\n");
									}
								}else{//we opened destination file and source file correctly
									int nread; //Number of bytes read correctly
									int nwrite; //Number of bytes written correctly
									// Loop to read and write data
									while ((nread = read(sourcef, buffer, 1024)) > 0){
										
										if ((write(destf, buffer, nread)) < nread){ //We have written less bytes than what we've read and the write error is also handled. 
											perror("Error writing in the destination file\n");
										}
									}
									if (nread < 0){ //Since when it's finished reading it'll be = 0. It will only be <0 when there's been an error
										perror("Error reading\n");
										if(close(sourcef) < 0){
											perror("Error closing source file\n");
										}
										if(close(destf) < 0){
											perror("Error closing the destination file\n");
										}
									}else{
										//Successful process message
										char success_message[100];
										snprintf(success_message, 100, "[OK] Copy has been successful between %s and %s\n", argv_execvp[1], argv_execvp[2]);
										
										if((write(STDERR_FILENO, success_message, strlen(success_message))) < 0 ){
											perror ("Error writing the message\n");
										}
										//Control errors when closing the files
										if(close(sourcef) < 0){
											perror("Error closing source file\n");
										}
										if(close(destf) < 0){
											perror("Error closing the destination file\n");
										}
									}
							}
						}
					}else{
						//If argv_execvp[1] or argv_execvp[2] is NULL or argv_execvp[3] isn't NULL 
						char error_message[100] = "[ERROR] The structure of the command is mycp <original_file> <copied_file>\n";
		        if((write(STDOUT_FILENO, error_message, strlen(error_message)))<0){
		        perror("There's been an error with the write system call.\n");
		        } 
					}
				}else{ //command not mycalc or mycp, we need to do exec
				
				if (command_counter == 1){
				 //For a unique command
				//We should have an inmediate child of the minishell.
					int pid = fork();
				  //Check there's no errors creating the child
				  if (pid < 0){
				  	perror("An error has occurred when creating the child\n");
				  	return -1;
				  }
				  if (pid == 0){//For the child
				  //Now we must check all the files redirections. For the standard input, output and error we check if its content inside the filevv is equal to 0. In the case it isn't we must first close the file to allow the redirection.
				  	//For the input
				  	if(strcmp(filev[0],"0") != 0){
				  		if ((close(STDIN_FILENO)) <0){ //Control the error when closing the file
				  			perror("Error closing the file");
				  			return -1; 
				  		}
				  		//for the input we will only give the flag read only
				  		int ifid = open(filev[0],O_RDONLY,0666);
				  		if (ifid < 0){ //Control the error when opening the file
				  			perror("Error opening the file");
				  			return -1;
				  		}
				  	}
				  	//For the output
				  	if(strcmp(filev[1],"0") != 0){
				  		if ((close(STDOUT_FILENO)) <0){
				  			perror("Error closing the file");
				  			return -1; 
				  		}
				  		//for the output we give more flags
				  		int ofid = open(filev[1],O_CREAT | O_WRONLY ,0666);
				  		if (ofid < 0){
				  			perror("Error opening the file");
				  			return -1;
				  		}
				  	}
				  	//For the standard error
				  	if(strcmp(filev[2],"0") != 0){
				  		if ((close(STDERR_FILENO)) <0){
				  			perror("Error closing the file");
				  			return -1; 
				  		}
				  		//for the error output we give more flags
				  		int efid = open(filev[2],O_CREAT | O_WRONLY,0666);
				  		if (efid < 0){
				  			perror("Error opening the file");
				  			return -1;
				  		}
				  	} 
				  //To the function execvp we give the name of the program as first argument and next the whole vector of arguments for the program execution 
					int nexecvp = execvp(argv_execvp[0],argv_execvp);
					if(nexecvp <0){
						perror("There was an error executing the command execvp");
						return -1;
					}
					//Once this is done, we exit the child process
					exit(pid);
			    }else{ //When pid>0 --> for the parent child
							//We divide it into two cases. When it's running in background and when it's not.
							if (in_background == 0){
								//Case 0: Not in background. Here we must wait until the child process it's finished before going on.
							 //This would be captured in the child's exit function and it's recieved by the function wait in the parent.
									int pid = wait(&status);
									if (pid < 0){
										perror("Error in waiting syscall.");
										return -1;
									}
							}else{
								//Case 1: We're running in background. Wait isn't needed.
								//We print the pid of the child's process.
								printf("[%d]\n", pid);
							}
					}
			}else{ 
				//Command counter is >1
				int fd[2]; //This is the array of two file descriptors (for input and output purposes) used by pipe
				int ncommand = 0;
				int pids[9];
				int fid;
				//Save the value of the screen as a duplicate so that we don't loose it
				int screen = dup(STDOUT_FILENO); //Now standard output is the screen. We must save this before performing any changes
				if (screen < 0){ //Check there's no errors
					perror("Error duplicating descriptor");
					return -1;
				
				}
				
				
				
				while(ncommand < command_counter){ //Iterate through all the commands. ncommad is started at 0 while command counter is at least 1.
						
						if (ncommand != command_counter -1){ //Create a pipe for all but the last command
							int pipeid = pipe(fd); //we create the pipe and check there's no errors regarding its creation. We'll read from fd[0] and write to fd[1]
							if(pipeid <0){ 
								perror("Error regarding the pipe\n");
								return -1;
							}
						}
						
						int pid = fork(); //Fork to create childs of the processes
						if (pid < 0){ //Check there's no error
							perror("Error creating child\n");
							return -1;
						}
						
						if (pid==0){ //Child process of the different commands
						
						
							if (ncommand == 0){
							//If we are in the first command, we must replace the standard output by the  writing end of the pipe 
								
								//First we should make all the needed redirections if there's any
				if((strcmp(filev[0],"0"))!= 0){ 
							//Redirect from standard input
							//Close standard input
				  			if ((close(STDIN_FILENO)) <0){
									perror("Error closing the file");
									return -1; 
								}
							//Open filev[0] so that it occupies the posititon of standard input
								int fid = open(filev[0],O_RDONLY, 0666); 
								if (fid<0){
									perror("Error opening file");
									return -1;
								}
							}
				//We do exactly the same with standard output and standard error
				if((strcmp(filev[1],"0"))!= 0){ 
							//Redirect from standard output 
								if ((close(STDOUT_FILENO)) <0){
									perror("Error closing the file");
									return -1; 
								}
								int fid = open(filev[1],O_WRONLY | O_CREAT , 0666);
								if (fid<0){
									perror("Error opening file");
									return -1;
								}
							}
							
				if((strcmp(filev[2],"0"))!= 0){ 
							//Redirect from standard error
									if ((close(STDERR_FILENO)) <0){
										perror("Error closing the file");
										return -1; 
									}
									int fid = open(filev[2],O_WRONLY | O_CREAT, 0666); 
									if (fid<0){
										perror("Error opening file");
										return -1;
									}
								}
								//Set the writing end of the pipe to replace the standard output
								int nclose = close(STDOUT_FILENO);
								if (nclose < 0){
									perror("Error closing the file\n");
									return -1;
								}
								int dupp = dup(fd[1]);
								if (dupp < 0){ 
									perror("Error while duplicating the descriptor\n");
									return -1;
								}
								
								if ((close(fd[1])) < 0){
									perror("Error while closing the descriptor\n");
								}
								
							}else{ //If it's not the first process, we must redirect the input of the command from the output of the previous commands
								int nclose = close(STDIN_FILENO);
								if (nclose < 0){
									perror("Error closing the file\n");
									return -1;
								}
								int dupp = dup(screen);
								if (dupp < 0){
									perror("Error duplicating screen\n");
									return -1;
								}
								
								if ((close(screen)) < 0){
									perror("Error closing the screen\n");
									return -1;
								}

								//Now, we must deal with the pipe . Therefore, we must treat all of the commands but the last one, for which there exist no pipe
								
								//Set the writing end of the pipe as the STDOUT
								if (ncommand < (command_counter - 1)){ //If we are not in the last command
									int nclose = close(STDOUT_FILENO);
									if (nclose < 0){
										perror("Error opening the file\n");
									}
									int dupp = dup(fd[1]);
									if (dupp < 0){ 
										perror("Error while duplicating the descriptor");
										return -1;
									}
									//Close the pipe (boths end)to avoid errors
									if ((close(fd[0])) < 0){
										perror("Error closing the descriptor");
										return -1;
									}
									
									if ((close(fd[1])) < 0){
										perror("Error while closing the descriptor");
										return -1;
									}
								}else{
								  
										if ((strcmp(filev[1], "0")) != 0){
											if ((fid= open(filev[1], O_WRONLY | O_CREAT, 0666)) < 0){
											perror("Error opening the file");
											return -1;
										}
										if ((dup2(fid, STDOUT_FILENO)) < 0){
											perror("Error while duplicating the descriptor");
											return -1;
										}
										if ((close(fid)) < 0){
											perror("Error closing the file descriptor");
											return -1;
											}
										}
									
								}
							}
							
							//Complete the array argv_excvp of commands
							getCompleteCommand(argvv,ncommand);
							
							//Command execute
							execvp(argv_execvp[0],argv_execvp);
							//If this is shown, something didn't go as expeted.
							perror("Exec didn't work as expected");
							exit(pid);
								
							}else{ //Father process 
								//Here we catch the pids created by the exit syscall
								pids[ncommand]=pid;
								//Now we have to check if we are running in background or not
								if (in_background == 0){//If we are not in background
								int pid = wait(NULL);
								while (wait(&pid) > 0);
								if (pid < 0) {
									perror("Error waiting for child ");
									return -1;
								}
							}else{ //If we are running in background
								printf("[%d]\n", pid);
							
								
							}
							if (ncommand < (command_counter - 1)){
							
								//Update the standard input for the new process
								if ((screen = dup(fd[0])) < 0){
									perror("Error duplicating my descriptor");
								}
								//Close the pipe to avoid errors
								if ((close(fd[0])) < 0){
									perror("Error closing the descriptor");
								}
								
								if ((close(fd[1])) < 0){
									perror("Error closing the descriptor");
								}
								
							}	
								
								
								
						}
						
						ncommand++; //To iterate through all the commands
				}	
							
				//Another loop must be created for waiting 
				for(int i=ncommand;i<command_counter;i++){
					waitpid(pids[i],&status,0);
				}
				
			}
		}
		}
	}
}
	return 0;
}
