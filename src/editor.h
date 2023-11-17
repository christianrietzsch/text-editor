#ifndef EDITOR_H
  #include <unistd.h>
  #include <sys/ioctl.h>
  #include <errno.h>
  #include <string.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <termios.h>

  #define EDITOR_VERSION "0.0.1"

  #define CTRL_KEY(k) ((k) & 0x1f)
  #define BUFFER_INIT {NULL, 0}

  struct editorConfig {
    int cx, cy;
    int screenrows;
    int screencols;
    struct termios orig_termios;
  };

  struct editorConfig E;
    struct buffer {
    char *b;
    int len;
  };
#endif
