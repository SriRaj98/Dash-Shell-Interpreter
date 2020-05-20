


//NOTE : TRY running without the -O at the end of the gcc function you are using.




#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

int creat();
int wait();
//this method is used to print the error messages whenever it is needed
int error_msg()
{
char error_message[30] = "error has occured\n";
write(STDERR_FILENO, error_message,strlen(error_message));
return 0;
}

int  main(int argc,char *argv[]){
printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
printf("~~~~~~~~~~~DAllas SHell~~~~~~~~~~\n");
printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

char *lines;//this will take the input command
char *cur_path;//this is a temporary pointer used in !builtincmd case
int x = 1;//this keeps the while in loop
size_t size = 100;//this is required to get the current path
char *args[10];//this stores the commands after tokenisation
char sep[]= " \t\n";//these are sepeartaors used by thed by strtok function
char *path[20];//this is the array that stores all paths
int pathfilled = 2;// this keeps track of the number of paths updated in the array
path[0] = "";
FILE *content;//this stores the content of the batch file
bool has_re = NULL;//this keeps track is there is a redirection
content = NULL;
char* redirect_to = NULL ;//this stores the file name to which we have to redirect to
int char_read = NULL ;//this keeps track of the number of characters we have read
int working = 0;//this keeps track whther
pid_t childpid = 1; //this keeps the child pid
path[1] ="/bin";
//this checks if the args is 2 and and exit if not
if (argc == 2)

{
content = fopen(argv[1],"r");
	if(!content)
		{
		error_msg();
		exit(1);
		}
}
else if(argc > 2)
	{
	error_msg();
	exit(1);

	}
	//here the shell starts
while(x)
{
	bool builtincmd = true;
	int i = 0;
	printf("dash>");
	if(argc < 2)
	{
	getline(&lines,&size,stdin);
	}
	else
	{char_read = getline(&lines,&size,content);
	if(char_read < 1 )
		{
		fclose(content);
		exit(0);
		} 
	if(lines == NULL)
		{
		break;
		}
	}
//these are the variables that stores different types of tokens
	char *token;
	char *par_token;
	char *comm;

	while((par_token = strtok_r(lines,"&",&lines )))
	{
	i = 0;
	comm = strdup (par_token);
	token = strtok (comm,sep);
	while(token != NULL)
		{
		
			
			args[i] = token;
			token = strtok(NULL, sep);
			i++;
			args[i]= NULL;
		}
	//if we have a redirection using space
	int check_re ;
	int count_re;
	count_re = 0;
	check_re  = 0;
	while(args[check_re] != NULL)
	{

		if(strcmp(">",args[check_re]) == 0 )
			{
			has_re = true;
			count_re++;
			}
		check_re++;


	}
	if(count_re > 1)
		{
		error_msg();
		continue;

		}
	check_re = 0;
	//if it has a redirection
	if (has_re)
		{
		while(strcmp(">",args[check_re])!=0)
			{
			check_re++;
			}
		redirect_to = args[check_re + 1];
		args[check_re] = NULL;

		}
//if the given command is exit

	if (strcmp("exit",args[0] ) == 0)
		{
			if(argc == 2) 
				{fclose(content);}
			else
			{
				if(args[1] == NULL)
				{	
				printf("Shell Teriminated\n");
				exit(0);
				}
				else
				{
				error_msg();
				continue;
				}
			
			}
		}
//if the given command is cd
	else if(strcmp("cd",args[0] ) == 0)
		{
			printf("changing directory\n");
			int ccd;
			ccd = chdir(args[1]);
			if (ccd == -1 || args[2] != NULL)
			{
			error_msg();
			continue;
			}

		}
	else if (strcmp("path",args[0])==0)
		{
			int j = 1;
			while(args[j] != NULL)
				{
					path[pathfilled] = args[j];
					pathfilled++;
					
					if(pathfilled  < 20 )
						{
							path[pathfilled] = NULL;
						}
					j++;
				}
			printf("path updated\n");
			printf("path filled is %d\n",pathfilled);
		}
	else{builtincmd = false;}
	

	if(!builtincmd)
		{
		char *input;
		input = strdup(args[0]);
		for(i = 0 ; i< pathfilled ; i++)
		 {

			cur_path = malloc(strlen(path[i]) + strlen(input) + strlen("/")+1);
			cur_path = strdup(path[i]);
			if(strcmp(path[i], "" ) != 0)
				{
				strcat(cur_path, "/");
			}

				strcat(cur_path,input);
				working = access(cur_path, X_OK);
				if (working == 0)
					{
					   break;
			                }		
		 }
		
		 args[0] = strdup(cur_path);
		//this creates the child
		int detect_child = fork();
		if(!has_re)
		{
		if(detect_child == 0 )
			{
		        execv(args[0],args);
				if(working != -1)
					{
					error_msg();
					}
			exit(0);
			}

			
		}
		else
		{
		if (detect_child == 0)
			{
			int create;
			create = creat(redirect_to, 0644);
			dup2(create,STDOUT_FILENO);
			dup2(create,STDERR_FILENO);
			close(create);
			execv(args[0],args);
			error_msg();
			exit(0);
			}
			

		}

		}
	}
	if((working == -1) & (!has_re))
	{
	error_msg();
	}
	has_re = false;

	while((childpid = wait(NULL)) > 0);


}
return 0;
}
