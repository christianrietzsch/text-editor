#!/bin/zsh
if gcc -o editor src/editor.c -Wall -Wextra -pedantic -std=c99 &> /dev/null
then
  ./editor
rm editor  
else
  echo "Error!"
fi
