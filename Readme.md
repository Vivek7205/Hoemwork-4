# **Project Title**

Job Scheduler

# **Project Description**

In this project we will implement a simple job scheduler that will execute non-interactive jobs (for example, jobs that do not have direct user interaction, jobs that can run in the background). At any given time, only P jobs should be executing, and P is provided as an argument to your program. If the user have more than P jobs submitted, then these additional jobs must wait until one of the P executing jobs are completed. We can assume that P is typically the number of cores that are available on a system and the user is executing one process per core and if there are more jobs than the available number of cores these jobs must wait for one of the processes to complete. When the user launch the program with P as the command-line argument, the program 
should print a prompt and wait for the user to enter commands.  
As the jobs are non-interactive, the output and error streams from the job must be 
written to separate files - <jobid>.out and <jobid>.err, where <jobid> is the appropriate 
job id that is associated with a job.  

# **Author**

1.Vivek Doppalapudi
2.Veda Mounica Makkapati

# **Getting Started**

>Prerequisites:
a) Visual studio code to build the code
b) c compiler
c) cisco any connect for VPN connection
d) Putty to connect to the server.
>Instructions for software building:
All these applications are made available in the internet, user can simply download and then proceed with installation. User needs to signin with the cisco credentinals inorder to access the Vulcan server with the help of putty.

# ** Running the project**

User can simply run with the following command:
CC=gcc
CFALGS=-g -O0 -Wall

User can also compile with the "make"  command
For the execution user just need to give "./pshed 2" command
