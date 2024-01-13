#!/bin/bash
#
# (C) 2024 Urban Ottosson
# License: GNU General Public License v2 or later (GPLv2+)
# Repository: https://github.com/locupleto/crystal_htop
#
# Description: Example script that manages the execution of crystal_htop within  
# a flock monitor section, ensuring that only one instance runs at a time and 
# capturing system statistics in separate files. These statistics files will be 
# located in the directory specified by the HTOP_TEMP_DIR environment variable.

export HTOP_TEMP_DIR=/tmp
export HTOPRC=$HTOP_TEMP_DIR/htop_htoprc
lockfile="$HTOP_TEMP_DIR/crystal_htop.lock"

# Use flock to ensure single thread access in critical section below
(
    while true; do
        # Try to acquire a lock using file descriptor 200
        /opt/homebrew/bin/flock -n 200 || {
            # If the lock cannot be acquired (another instance is running)
            break
        }

        # Check if the screen session 'crystal_htop_session' is already running
        if ! screen -list | grep -q "crystal_htop_session"; then
            # Start 'crystal_htop' in a detached screen session named 'crystal_htop_session'
            screen -dmS crystal_htop_session htop-swap-bar.widget/crystal_htop

            # List the current screen sessions to file
            screen -ls > "$HTOP_TEMP_DIR/htop_session_list.txt"

            # Optionally report the total no of CPU cores in the system (e.g. 20)
            /usr/sbin/sysctl -n hw.ncpu > "$HTOP_TEMP_DIR/htop_num_cpus.txt"

            # Give htop a second to create the files we rely on next
            sleep 1
        fi

        # Exit the loop after executing the critical section
        break
    done
) 200>$lockfile