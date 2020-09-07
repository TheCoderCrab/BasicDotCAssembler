 
#!/bin/sh

echo -e "\e[1m\e[32mInstalling basic dotc assembler standard modules\e[0m\e[39m"

mkdir -p $HOME/.local/share/bdca/modules
cp -r modules/* $HOME/.local/share/bdca/modules
echo -e "\e[1m\e[32mDotCVM standard modules installed\e[0m\e[39m"
