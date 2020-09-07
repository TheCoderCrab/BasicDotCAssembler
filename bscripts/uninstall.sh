#!/bin/sh

# Arg 0 should be the installation directory

if [ "$#" -ge 1 ]; then
    DC_INSTALL_DIR=$1
else
    DC_INSTALL_DIR="/opt/bdca/"
fi


echo -e "\e[1m\e[32mUninstalling basic dotc assembler\e[0m\e[39m"

if [ ${EUID:-$(id -u)} -eq 0 ]; then # Should be run as root
    [ -f /usr/bin/bdca ] && rm /usr/bin/bdca
    [ -d $HOME/.local/share/bdca ] && rm -r "$HOME/.local/share/bdca/"
    echo -e "\e[1m\e[32mDone\e[0m\e[39m"
    [ -d "$DC_INSTALL_DIR" ] && rm -r "$DC_INSTALL_DIR" # This is here because the uninstall 
                                                        # script is probably inside the install
                                                        # directory so it should be removed last
else
    echo "Please run as root!"
    exit 0
fi 
