#! /bin/bash

PASS_PATH=""
HOSTNAME="127.0.0.1"
PROJECT_NAME="Override"

COMMAND="$1"
VM_USER="$2"

FILE_INJECT="$3"

project_name_bold="\033[1m$PROJECT_NAME\033[0m"
vm_user_bold="\033[1m$VM_USER\033[0m"
filename_bold="\033[1m$FILE_INJECT\033[0m"

function print_connection() {
	echo -e "Logging in to $project_name_bold as user $vm_user_bold"
}


function print_injection() {
	echo -e "Injecting $filename_bold to user $vm_user_bold in /tmp/$filename_bold"
}


find_latest_level() {
	for user in level00 level01 level02 level03 level04 level05 \
				level06 level07 level08 level09
	do
		if [ ! -s "./$user/flag" ]; then
			VM_USER="$user"
			vm_user_bold="\033[1m$VM_USER\033[0m"
			return
		fi
		VM_USER="end"
		if [ "$user" = "level00" ]; then
			VM_USER="level00"
		fi
	done
}

function open_user() {
	print_connection
	sshpass -f $PASS_PATH ssh -p 4242 $VM_USER@$HOSTNAME
}


function inject_file() {
	if [ -z "$FILE_INJECT" ]; then
		echo "Error : Missing inject file argument : Usage : ./override_toolkit.sh inject <target user> <source file path>"
		exit 1
	fi

	sshpass -f $PASS_PATH scp -P 4242 $FILE_INJECT $VM_USER@$HOSTNAME:/tmp/$(basename $FILE_INJECT) 2> /dev/null

	if [ $? -ne 0 ]; then
		echo "Failed to copy $(basename $FILE_INJECT) to /tmp/$(basename $FILE_INJECT)"
		exit 1
	fi

	print_injection "$(basename $FILE_INJECT)"
}


function extract_file() {
	sshpass -f $PASS_PATH scp -P 4242 $VM_USER@$HOSTNAME:/home/users/$VM_USER/$VM_USER ./binaries/. 2> /dev/null

	if [ $? -ne 0 ]; then
		echo -e "Failed to extract $vm_user_bold to ./binaries/$vm_user_bold"
		exit 1
	fi

	echo -e "Successfully extracted $vm_user_bold to ./binaries/$vm_user_bold"
}


function set_pass_path() {
	if [[ "$VM_USER" =~ ^(level00)$ ]]; then
		PASS_PATH="./tools/.level0_flag";
		return ;
	fi

	previous_user="$1"

	if [ ! -s "./$previous_user/flag" ]; then
		echo "Error : Flag has not been found yet, go back to work."
		exit 1
	fi
	PASS_PATH="./$previous_user/flag"
}


if [ -z "$COMMAND" ] || [ -z "$VM_USER" ]; then
	echo "Error : Missing argument : Usage : ./override_toolkit.sh <open|inject|extract> <target user> [file path]";
	exit 1
else
	if [[ "$VM_USER" =~ ^(latest)$ ]]; then
		find_latest_level
	fi

	if [[ "$VM_USER" =~ ^(level0[0-9]|end)$ ]]; then

		num=${VM_USER##*[!0-9]}
		prefix=${VM_USER%"$num"}

		previous_user="${prefix}0$((num - 1))"

		case "$VM_USER" in
		"end") set_pass_path "level09"; VM_USER="end"
		;;
		*) set_pass_path "$previous_user"
		esac

		case "$COMMAND" in
		"open") open_user;
		;;
		"inject") inject_file;
		;;
		"extract") extract_file;
		;;
		*) echo "Error : Invalid command : Usage : ./override_toolkit.sh <open|inject|extract> <target user> [source file path]"; exit 1;
		esac

	else
		echo "Error : Invalid user name"
		exit 1
	fi
fi
