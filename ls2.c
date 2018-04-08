/* ls2.c
 * purpose list contents of directory or directories
 * action if no args, use . else list files in args
 * note uses stat and pwd.h and grp.h
 * BUG: try ls2 /tmp
 */
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>

void do_ls(char[]);
void dostat(char *);
void show_file_info(char *, struct stat *);
void mode_to_letters(int, char[]);
char * uid_to_name(utd_t);
char * gid_to_name(gid_t);


void main(int argc, char const *argv[])
{
	if(argc == 1){
		do_ls(".");
	}
	else{
		while(--argc){
			printf("%s:\n", * ++argv);
			do_ls(*argv);
		}
	}
}

void do_ls(char dirname[])
/*
 * list files in directory called dirname
 */
{
	DIR 	* dir_ptr;				//the directory
	struct 	dirent * direntp;		//each entry

	if((dir_ptr = opendir(dirname)) == NULL){
		fprintf(stderr, "ls2:cannot open %s\n", dirname);
	}
	else{
		while((direntp = readdir(dir_ptr)) != NULL){
			dostat(direntp -> d_name);
		}
		closedir(dir_ptr);
	}
}

void dostat(char * filename)
{
	struct stat info;

	if(stat(filename, &info) == -1){	//cannot stat
		perror(filename);
	}
	else{
		show_file_info(filename, &info);
	}
}

void show_file_info(char * filename, struct stat * info_p)
/*
 * display the info about filename. the info is sorted in struct at * info_p
 */
{
	//char *uid_to_name(), *ctime(), *gid_to_name(),*filemode();
	//void mode_to_letters();
	char modestr[11];

	mode_to_letters(info_p->st_mode, modestr);

	printf("%s", modestr);
	printf("%4d", (int)info_p -> st_nlink);
	printf("%-8s", uid_to_name(info_p->st_uid));
	printf("%-8s", gid_to_name(info_p->st_gid));
	printf("%8ld", (long)info_p->st_size);
	//printf("%s", 4 + ctime(&info_p->st_mtime));  
	printf("%s\n", filename);
}

/*
 * utility functions
 */

/*
 * This function takes a mode value and a char array
 * and puts into the char array the file type and the
 * nine letters that correspond to the bits in mode.
 * NOTE: It does not code setuid, setgid, and sticky
 * codes
 */
void mode_to_letters(int mode, char str[])
{
	strcpy(str, "---------");

	if(S_ISDIR(mode)) str[0] = 'd';
	if(S_ISCHR(mode)) str[0] = 'c';
	if(S_ISBLK(mode)) str[0] = 'b';

	if(mode & S_IRUSR) str[1] = 'r';	//for user
	if(mode & S_IWUSR) str[2] = 'w';
	if(mode & S_IXUSR) str[3] = 'x';

	if(mode & S_IRGRP) str[4] = 'r';	//for group
	if(mode & S_IWGRP) str[5] = 'w';
	if(mode & S_IXGRP) str[6] = 'x';

	if(mode & S_IROTH) str[7] = 'r';	//for other
	if(mode & S_IWOTH) str[8] = 'w';
	if(mode & S_IXOTH) str[9] = 'x';
}

#include <pwd.h>

char * uid_to_name(uid_t uid)
/*
 * returns pointers to username associated with uid,uses getpw()
 */
{
	struct passwd *getpwuid(), * pw_ptr;
	static char numstr[10];

	if((pw_ptr = getpwuid(uid)) == NULL){
		sprintf(numstr, "%d", uid);
		return numstr;
	}
	else{
		return pw_ptr->pw_name;
	}
}

#include <grp.h>

char * gid_to_name(gid_t gid)
/*
 * return poiters to group number gid. used getgrgid(3)
 */
{
	struct group * grp_ptr;
	static char gnumstr[10];

	if((grp_ptr = getgrgid(gid)) == NULL){
		sprintf(gnumstr,"%d",gid);
		return gnumstr;
	}
	else{
		return grp_ptr->gr_name;
	}
}