#!/bin/sh
# $@ - files to be submitted

RED='\033[0;31m'
BLUE='\033[0;36m'
GREEN='\033[0;32m'
NC='\033[0m'

START_DIR=$(pwd)
DRYRUN_FILES_DIR=/web/cs2521/20T2/ass/ass2/hunt/dryrun/files
TEMP_DIR=""

# $@ - files to be submitted
main()
{
	if [ $# -eq 0 ]
	then
		echo -e "Error: no files provided"
		exit 1
	fi
	
	TEMP_DIR=$(mktemp -d)
	echo -e "${BLUE}Created a temp directory '$TEMP_DIR' for checking your submission${NC}"
	
	# copy submitted files into temporary directory
	echo -e "${BLUE}Copying your files into '$TEMP_DIR'${NC}"
	copy_into_temp "$@"
	
	# check that the submission includes all the specified files
	echo -e "${BLUE}Checking that your submission includes all the required files${NC}"
	check_required_files Makefile \
				dracula.c hunter.c \
				DraculaView.c DraculaView.h \
				HunterView.c HunterView.h \
				GameView.c GameView.h
	
	# check that the submission does not include certain provided files
	echo -e "${BLUE}Checking that your submission does NOT include files that shouldn't be modified${NC}"
	check_forbidden_files Game.h Places.c Places.h player.c dracula.h hunter.h
	
	echo -e "${BLUE}Copying our versions of files that shouldn't be modified into '$TEMP_DIR'${NC}"
	# copy certain provided files into the temporary directory
	copy_in_provided_files Game.h Places.c Places.h player.c dracula.h hunter.h
	
	echo -e "${BLUE}Copying in the remaining files that you weren't required to modify or submit${NC}"
	copy_in_if_missing Map.c Map.h Places.h Places.c
	
	echo -e "${BLUE}Checking that your code compiles${NC}"
	attempt_compile
	echo -e "${GREEN}Compilation succeeded!${NC}"
	
	echo -e ""
	echo -e "${GREEN}Dryrun succeeded!${NC}"
}

# Copies all given files into $TEMP_DIR
# Assumes $TEMP_DIR is an existing temporary directory
# Exits with an error if any files don't exist
# $@ - files to be copied
copy_into_temp()
{
	for f in "$@"
	do
		if [ ! -f "$f" ]
		then
			echo -e "${RED}Error: file '$f' doesn't exist${NC}"
			exit 1
		elif [ ! -r "$f" ]
		then
			echo -e "${RED}Error: file '$f' is not readable${NC}"
			exit 1
		fi
	done
	
	cp "$@" "$TEMP_DIR"
}

# Checks that each of the given files are in $TEMP_DIR
# Assumes $TEMP_DIR is an existing temporary directory
# Exits with an error if any of the files are missing
# $@ - compulsory files
check_required_files()
{
	cd "$TEMP_DIR"
	
	missing=""
	for f in "$@"
	do
		if [ ! -f "$f" ]
		then
			missing="$missing $f"
		fi
	done
	
	if [ ! -z "$missing" ]
	then
		echo -e "${RED}Error: your submission is missing these files:$missing${NC}"
		exit 1
	fi
}

check_forbidden_files()
{
	cd "$TEMP_DIR"
	
	forbidden=""
	for f in "$@"
	do
		if [ -f "$f" ]
		then
			forbidden="$forbidden $f"
		fi
	done
	
	if [ ! -z "$forbidden" ]
	then
		echo -e "${RED}Error: your submission should not include these files:$forbidden${NC}"
		exit 1
	fi
}

copy_in_provided_files()
{
	cd "$TEMP_DIR"
	
	missing=""
	for f in "$@"
	do
		if [ ! -f "$DRYRUN_FILES_DIR/$f" ]
		then
			missing="$missing $f"
		else
			cp "$DRYRUN_FILES_DIR/$f" "$TEMP_DIR"
		fi
	done
	
	if [ ! -z "$missing" ]
	then
		echo -e "${RED}Error: these files were missing:$missing"
		echo -e "Please contact course staff.${NC}"
		exit 1
	fi
}

copy_in_if_missing()
{
	missing=""
	for f in "$@"
	do
		if [ ! -f "$TEMP_DIR/$f" ]
		then
			if [ ! -f "$DRYRUN_FILES_DIR/$f" ]
			then
				missing="$missing $f"
			else
				cp "$DRYRUN_FILES_DIR/$f" "$TEMP_DIR"
			fi
		fi
	done
	
	if [ ! -z "$missing" ]
	then
		echo -e "${RED}Error: these files were missing:$missing"
		echo -e "Please contact course staff.${NC}"
		exit 1
	fi
}

attempt_compile()
{
	cd "$TEMP_DIR"
	
	make
	
	if [ "$?" -ne 0 ]
	then
		echo -e "${RED}Error: compilation error"
		echo -e "Please fix your code or add any missing files and resubmit.${NC}"
		exit 1
	fi
	
	if [ ! -x "dracula" ]
	then
		echo -e "${RED}Error: the compilation succeeded, but did not produce ./dracula."
		echo -e "Please fix your Makefile and resubmit.${NC}"
		exit 1
	fi
	
	if [ ! -x "hunter" ]
	then
		echo -e "${RED}Error: the compilation succeeded, but did not produce ./hunter."
		echo -e "Please fix your Makefile and resubmit.${NC}"
		exit 1
	fi
}

overwrite_files()
{
	missing=""
	for f in "$@"
	do
		if [ ! -f "$DRYRUN_FILES_DIR/$f" ]
		then
			missing="$missing $f"
		fi
		cp "$DRYRUN_FILES_DIR/$f" "$TEMP_DIR"
	done
	
	if [ ! -z "$missing" ]
	then
		echo -e "${RED}Error: these files were missing:$missing"
		echo -e "Please contact course staff.${NC}"
		exit 1
	fi
}

########################################################################

main "$@"

