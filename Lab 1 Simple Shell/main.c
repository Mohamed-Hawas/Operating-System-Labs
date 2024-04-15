#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
// #include <sys/wait.h>
#include <signal.h>


char input[256] ;
char *args =(char*) malloc(sizeof(int)*16) ;
pid_t pid ;
int flag = 0 ;



// Functions
void get_input ();
void split_input();

//void log_handle(int sig);

int main(){
    
    
    char path [100];
    do{
    get_input();
    split_input();
    pid = fork();
    if(pid == -1 ){
        printf ("Error");
    } else if (pid == 0){//child
        if (!strcmp(args[0],"cd")){
            chdir(args[1]);
        } else if (!strcmp(args[0],"pwd")){
            getcwd(path,100);
            printf("%s\n",path);   
        } else {
            execvp(args[0],args);
        }
    } else { // parent
        if( flag == 0 ) {// background 
            waitpid(pid,NULL,0);
            }
    }

    } while (strcmp(args[0],"exit"));
    
    return 0 ;
}

void get_input(){
    do{
        printf("my shell : ");
        fgets(input , 256 ,stdin);

    } while (strlen(input)==1);
     
    if( ( strlen(input) > 0 ) && input[strlen(input)-1]=='\n' )
        input[strlen(input)-1]='\0';

}


void split_input(){
    
    char *mytoken ;
    int i = 0 ;
    mytoken = strtok(input ," ");
    
    while(mytoken != NULL){
        args[i++] = mytoken ;
        mytoken = strtok(NULL ," ");
    } 

    if(!strcmp(args[i-1],"&")){
        args[i-1]=NULL;
        flag = 1 ;
    }

}
