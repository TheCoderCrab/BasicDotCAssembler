#!/bin/sh

# Arg 0 should be the installation directory

if [ "$#" -ge 1 ]; then
    DC_INSTALL_DIR=$1
else
    DC_INSTALL_DIR="/opt/bdca/"
fi

echo -e "\e[1m\e[32mInstalling Basic DotC Assembler\e[0m\e[39m"

if [ ${EUID:-$(id -u)} -eq 0 ]; then # Should be run as root
    mkdir -p $DC_INSTALL_DIR
    cp dotcvm "$DC_INSTALL_DIR/bdca"
    cp uninstall.sh "$DC_INSTALL_DIR/uninstall.sh"
    [ -f /usr/bin/bdca ] && rm /usr/bin/bdca
    ln -s "$DC_INSTALL_DIR/bdca" /usr/bin/bdca
    echo -e "\e[1m\e[32mInstalled to $DC_INSTALL_DIR\e[0m\e[39m"
else
    echo "Please run as root!"
    exit 0
fi 
