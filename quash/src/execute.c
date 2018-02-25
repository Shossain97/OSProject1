/**
 * @file execute.c
 *
 * @brief Implements interface functions between Quash and the environment and
 * functions that interpret an execute commands.
 *
 * @note As you add things to this file you may want to change the method signature
 */

#include "execute.h"

#include <stdio.h>

#include "quash.h"
#include "deque.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define READ 0
#define WRITE 1
/*just checkign if git works*/
// Remove this and all expansion calls to it
/**
 * @brief Note calls to any function that requires implementation
 */
#define IMPLEMENT_ME()                                                  \
  fprintf(stderr, "IMPLEMENT ME: %s(line %d): %s()\n", __FILE__, __LINE__, __FUNCTION__)

/***************************************************************************
 * Interface Functions
 ***************************************************************************/
 typedef struct process{
   pid_t pid;//it looks like a struct for this was not necessary but I'm not changing code now
 }process;

IMPLEMENT_DEQUE_STRUCT(processList, process);
PROTOTYPE_DEQUE(processList, process);
IMPLEMENT_DEQUE(processList, process);

 typedef struct Job{
 	processList processes;//the queue for processes
 	int id;
  char* cmd;
  bool needsFree;
	//pipes
}Job;

void destructor_job(Job* aJob){
  destroy_processList(&aJob->processes);
  if(aJob->needsFree){
    free(aJob->cmd);
  }
}

 IMPLEMENT_DEQUE_STRUCT(JobQueue, Job);
 PROTOTYPE_DEQUE(JobQueue, Job);
 IMPLEMENT_DEQUE(JobQueue, Job);

JobQueue Jobs;
int curJobId=0;
int wasCreated=0;
static int pipes[2][2];
// Return a string containing the current working directory.
char* get_current_directory(bool* should_free) {
  // TODO: Get the current working directory. This will fix the prompt path.
  // HINT: This should be pretty simple
  //IMPLEMENT_ME();

  // Change this to true if necessary
  *should_free = true;
  //char* cwd = get_current_dir_name(3);

  return get_current_dir_name();
}

// Returns the value of an environment variable env_var
const char* lookup_env(const char* env_var) {
  // TODO: Lookup environment variables. This is required for parser to be able
  // to interpret variables from the command line and display the prompt
  // correctly
  // HINT: This should be pretty simple
  //IMPLEMENT_ME();

  // TODO: Remove warning silencers
  //(void) env_var; // Silence unused variable warning

  return getenv(env_var);
}

// Check the status of background jobs
void check_jobs_bg_status() {
  if(wasCreated==0||is_empty_JobQueue(&Jobs)){
    //printf("No background Jobs!\n");
    return;
  }
  Job job;
  processList curProcesses;
  process curProcess;
  int processListSize;
  bool finished;
  //pid_t processPid;
  pid_t runningPid;
  int status;


  int jobsSize=length_JobQueue(&Jobs);
  for(int i=0;i<jobsSize;i++){


    job=pop_front_JobQueue(&Jobs);
    curProcesses=job.processes;
    finished=true;
    //stillRunning=false;
    processListSize=length_processList(&curProcesses);
    for(int j=0;j<processListSize;j++){
      curProcess = pop_front_processList(&curProcesses);
      runningPid=waitpid(curProcess.pid, &status, WNOHANG);
      if(runningPid==0){
        //add back the process and job because they're still running
        push_back_processList(&curProcesses, curProcess);
        push_back_JobQueue(&Jobs, job);
        finished=false;
        break;//we done so we can break
      }
      else{
        //push process on back in case if processes coming next aren't finished
        push_back_processList(&curProcesses, curProcess);
      }

    }
    if(finished){
      print_job_bg_complete(job.id, peek_front_processList(&curProcesses).pid, job.cmd);
      job.needsFree=true;
      destructor_job(&job);
    }

  }
  // TODO: Once jobs are implemented, uncomment and fill the following line
  // print_job_bg_complete(job_id, pid, cmd);
}

// Prints the job id number, the process id of the first process belonging to
// the Job, and the command string associated with this job
void print_job(int job_id, pid_t pid, const char* cmd) {
  printf("[%d]\t%8d\t%s\n", job_id, pid, cmd);
  fflush(stdout);
}

// Prints a start up message for background processes
void print_job_bg_start(int job_id, pid_t pid, const char* cmd) {
  printf("Background job started: ");
  print_job(job_id, pid, cmd);
}

// Prints a completion message followed by the print job
void print_job_bg_complete(int job_id, pid_t pid, const char* cmd) {
  printf("Completed: \t");
  print_job(job_id, pid, cmd);
}

/***************************************************************************
 * Functions to process commands
 ***************************************************************************/
// Run a program reachable by the path environment variable, relative path, or
// absolute path
void run_generic(GenericCommand cmd) {
  // Execute a program with a list of arguments. The `args` array is a NULL
  // terminated (last string is always NULL) list of strings. The first element
  // in the array is the executable
  char* exec = cmd.args[0];
  char** args = cmd.args;

  // TODO: Remove warning silencers
  //(void) exec; // Silence unused variable warning
  //(void) args; // Silence unused variable warning

  // TODO: Implement run generic
  //IMPLEMENT_ME();
  execvp(exec, args );

  perror("ERROR: Failed to execute program");
}

// Print strings
void run_echo(EchoCommand cmd) {
  // Print an array of strings. The args array is a NULL terminated (last
  // string is always NULL) list of strings.
  char** str = cmd.args;


  // TODO: Remove warning silencers
//  (void) str; // Silence unused variable warning

  // TODO: Implement echo
  //IMPLEMENT_ME();
  //while(str)
  int iterator=0;
  while(str[iterator]!=NULL){
    printf("%s ",str[iterator]);
    iterator++;
  }
  printf("\n");
  // Flush the buffer before returning
  fflush(stdout);
}

// Sets an environment variable
void run_export(ExportCommand cmd) {
  // Write an environment variable
  const char* env_var = cmd.env_var;
  const char* val = cmd.val;

  // TODO: Remove warning silencers
  //(void) env_var; // Silence unused variable warning
  //(void) val;     // Silence unused variable warning

  // TODO: Implement export.
  // HINT: This should be quite simple.
  setenv(env_var, val,1);
}

// Changes the current working directory
void run_cd(CDCommand cmd) {
  // Get the directory name
  const char* dir = cmd.dir;
  char* oldDir;
  char* newDir;

  // Check if the directory is valid
  if (dir == NULL) {
    perror("ERROR: Failed to resolve path");
    return;
  }



  // TODO: Change directory
  oldDir=get_current_dir_name();
  chdir(dir);
  newDir=get_current_dir_name();

  // TODO: Update the PWD environment variable to be the new current working
  // directory and optionally update OLD_PWD environment variable to be the old
  // working directory.
  //IMPLEMENT_ME();
  setenv("PWD", newDir,1 );
  setenv("OLD_PWD", oldDir, 1);
  free(oldDir);
  free(newDir);
}

// Sends a signal to all processes contained in a job
void run_kill(KillCommand cmd) {
  int signal = cmd.sig;
  int job_id = cmd.job;

  // TODO: Remove warning silencers
  (void) signal; // Silence unused variable warning
  (void) job_id; // Silence unused variable warning

  // TODO: Kill all processes associated with a background job
  IMPLEMENT_ME();
}


// Prints the current working directory to stdout
void run_pwd() {
  // TODO: Print the current working directory
  //IMPLEMENT_ME();
  bool aBoolThatNeedsToBePassedForFreesThatHeSaidWouldBeHere;
  char* curDir=get_current_directory(&aBoolThatNeedsToBePassedForFreesThatHeSaidWouldBeHere);
  printf("%s\n", curDir);
  //Document said something about freeing but it's not here???
if(aBoolThatNeedsToBePassedForFreesThatHeSaidWouldBeHere){
  free(curDir);
}
  // Flush the buffer before returning
  fflush(stdout);
}

// Prints all background jobs currently in the job list to stdout
void run_jobs() {
  // TODO: Print background jobs
  IMPLEMENT_ME();

  // Flush the buffer before returning
  fflush(stdout);
}

/***************************************************************************
 * Functions for command resolution and process setup
 ***************************************************************************/

/**
 * @brief A dispatch function to resolve the correct @a Command variant
 * function for child processes.
 *
 * This version of the function is tailored to commands that should be run in
 * the child process of a fork.
 *
 * @param cmd The Command to try to run
 *
 * @sa Command
 */
void child_run_command(Command cmd) {
  CommandType type = get_command_type(cmd);

  switch (type) {
  case GENERIC:
    run_generic(cmd.generic);
    break;

  case ECHO:
    run_echo(cmd.echo);
    break;

  case PWD:
    run_pwd();
    break;

  case JOBS:
    run_jobs();
    break;

  case EXPORT:
  case CD:
  case KILL:
  case EXIT:
  case EOC:
    break;

  default:
    fprintf(stderr, "Unknown command type: %d\n", type);
  }
}

/**
 * @brief A dispatch function to resolve the correct @a Command variant
 * function for the quash process.
 *
 * This version of the function is tailored to commands that should be run in
 * the parent process (quash).
 *
 * @param cmd The Command to try to run
 *
 * @sa Command
 */
void parent_run_command(Command cmd) {
  CommandType type = get_command_type(cmd);

  switch (type) {
  case EXPORT:
    run_export(cmd.export);
    break;

  case CD:
    run_cd(cmd.cd);
    break;

  case KILL:
    run_kill(cmd.kill);
    break;

  case GENERIC:
  case ECHO:
  case PWD:
  case JOBS:
  case EXIT:
  case EOC:
    break;

  default:
    fprintf(stderr, "Unknown command type: %d\n", type);
  }
}

/**
 * @brief Creates one new process centered around the @a Command in the @a
 * CommandHolder setting up redirects and pipes where needed
 *
 * @note Processes are not the same as jobs. A single job can have multiple
 * processes running under it. This function creates a process that is part of a
 * larger job.
 *
 * @note Not all commands should be run in the child process. A few need to
 * change the quash process in some way
 *
 * @param holder The CommandHolder to try to run
 *
 * @sa Command CommandHolder
 */
void create_process(CommandHolder holder, Job* aJob, int curProcessNum, processList* aProcessList) {
  // Read the flags field from the parser
  bool p_in  = holder.flags & PIPE_IN;
  bool p_out = holder.flags & PIPE_OUT;
  bool r_in  = holder.flags & REDIRECT_IN;
  bool r_out = holder.flags & REDIRECT_OUT;
  bool r_app = holder.flags & REDIRECT_APPEND; // This can only be true if r_out
                                               // is true
	int write_end =curProcessNum%2;
    	int read_end =(curProcessNum-1)%2;

  if(p_out){
    pipe(pipes[write_end]);
  }

  // TODO: Remove warning silencers
  //(void) p_in;  // Silence unused variable warning
  //(void) p_out; // Silence unused variable warning
  //(void) r_in;  // Silence unused variable warning
  //(void) r_out; // Silence unused variable warning
  //(void) r_app; // Silence unused variable warning

  // TODO: Setup pipes, redirects, and new process
  //printf("create_process is currently in progress\n");
  //IMPLEMENT_ME();

  pid_t pid = fork();
  if(pid==0){
    if(p_in){
      dup2(pipes[read_end][READ], STDIN_FILENO);
      close(pipes[read_end][READ]);
    }
    if(p_out){
      dup2(pipes[write_end][WRITE], STDIN_FILENO);
      close(pipes[write_end][WRITE]);
    }
    if(r_in){
      //file redirects
      int inFile=open(holder.redirect_in,0);//would not take O_RDONLY??
      dup2(inFile, STDIN_FILENO);
      close(inFile);
    }
    if(r_out){
      //file redirects probably some dups
      int outFile;
      if(r_app){
        outFile=open(holder.redirect_out,O_APPEND);
      }
      else{
        outFile=open(holder.redirect_out,O_WRONLY);
      }
      dup2(outFile,STDOUT_FILENO);
      close(outFile);
    }
    child_run_command(holder.cmd);// This should be done in the child branch of a fork
    exit(EXIT_SUCCESS);
  }
  else{
    if(p_out){
      close(pipes[write_end][WRITE]);
    }
    if(r_in){
      int inFile=open(holder.redirect_in,0);
      dup2(inFile, STDIN_FILENO);
      close(inFile);
    }
    if(r_out){
        int outFile;
      if(r_app){
        outFile=open(holder.redirect_out,O_APPEND);
      }
      else{
        outFile=open(holder.redirect_out,O_WRONLY);
      }
      dup2(outFile,STDOUT_FILENO);
      close(outFile);
  }
  process backgroundProcess;
  backgroundProcess.pid=pid;
  //backgroundProcess.cmd=holder.cmd;
  push_front_processList(aProcessList, backgroundProcess);
  parent_run_command(holder.cmd);
  //wait() // This should be done in the parent branch of
                                  // a fork

  //parent_run_command(holder.cmd); // This should be done in the parent branch of
                                  // a fork
  //child_run_command(holder.cmd); // This should be done in the child branch of a fork
}
}

// Run a list of commands
void run_script(CommandHolder* holders) {
  if (holders == NULL)
    return;

  check_jobs_bg_status();

  if (get_command_holder_type(holders[0]) == EXIT &&
      get_command_holder_type(holders[1]) == EOC) {
    end_main_loop();
    return;
  }

  CommandType type;
  Job newJob;

  newJob.processes=new_processList(20);



  // Run all commands in the `holder` array
  for (int i = 0; (type = get_command_holder_type(holders[i])) != EOC; ++i)
    create_process(holders[i], &newJob, i, &newJob.processes);

  if (!(holders[0].flags & BACKGROUND)) {
    // Not a background Job
    // TODO: Wait for all processes under the job to complete
    //IMPLEMENT_ME();
    //wait
    while(!is_empty_processList(&newJob.processes)){
      int status;
      if(waitpid(peek_back_processList(&newJob.processes).pid, &status, 0)!=-1){
        pop_back_processList(&newJob.processes);
      }
    }
    newJob.needsFree=false;
    destructor_job(&newJob);
    //printf("currently no waiting occurs in run_script line 381\n");
    //create_process(holders[i]);

    //get rid of job here
  }
  else {
    // A background job.
    // TODO: Push the new job to the job queue
    if(wasCreated==0){
      Jobs = new_JobQueue(20);
      wasCreated=1;
    }
    newJob.id=curJobId++;
    newJob.needsFree=true;
    newJob.cmd=get_command_string();
    push_back_JobQueue(&Jobs, newJob);

    //IMPLEMENT_ME();

    // TODO: Once jobs are implemented, uncomment and fill the following line
    print_job_bg_start(newJob.id, peek_front_processList(&newJob.processes).pid, newJob.cmd);
  }
}
