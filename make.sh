#!/bin/zsh
if gcc -o bin/editor src/editor.c -Wall -Wextra -pedantic -std=c99 &> /dev/null
then
  ./bin/editor
  rm bin/editor  
fi
