#include "../include/editor.h"

void abAppend(struct buffer* buffer, const char* string, int len) {
  //appends a string to the buffer
  char* new = realloc(buffer->b, buffer->len + len);
  if(new == NULL) return;
  memcpy(&new[buffer->len], string, len);
  buffer->b = new;
  buffer->len += len;
}

void abFree(struct buffer* buffer) {
  //free the memory of the buffer
  free(buffer->b);
}

void die(const char* message) {
  //clear the screen and display an error message
  write(STDOUT_FILENO, "\x1b[2J",4);
  write(STDOUT_FILENO, "\x1b[H", 3);
  perror(message);
  exit(1);
}

void disableRawMode() {
  //disable Raw(Modal)-Mode
  if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1)
    die("tcsetattr");
}

void enableRawMode() {
  //enable Raw(Modal)-Mode
  if(tcgetattr(STDIN_FILENO, &E.orig_termios) == -1) die("tcgetattr");
  atexit(disableRawMode);
  struct termios raw = E.orig_termios;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;
  if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw)) die("tcsetattr");
}

char editorReadKey() {
  //reads a keypress and returns it as a character
  int nread;
  char c;
  while((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if(nread == -1 && errno != EAGAIN) die("read");
  }
  return c;
}

int getWindowSize(int* rows, int* cols) {
  //saves the size of the window in rows and cols
  struct winsize ws;

  if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
    return -1;
  } else {
    *cols = ws.ws_col;
    *rows = ws.ws_row;
    return 0;
  }
}
char* getRelativeCursorRowPosition(int current_line, int line) {
  return "12345";
}

void editorDrawRows(struct buffer* ab, int current_line) {
  //draws the rows with "~" at the beginning
  int y;
  for(y = 0; y < E.screenrows; y++) {
    char* row_inf = getRelativeCursorRowPosition(current_line, y);
    if(y == E.screenrows / 3) {
      char welcome[80];
      int welcomelen = snprintf(welcome, sizeof(welcome), "Editor --version %s", EDITOR_VERSION);
      if(welcomelen > E.screencols) welcomelen = E.screencols;
      int padding = (E.screencols - welcomelen) / 2;
      if(padding) {
        abAppend(ab, row_inf, BARRIER_LENGTH-1);
        padding--;
      }
      while(padding--) abAppend(ab, " ", 1);
      abAppend(ab, welcome, welcomelen);
    } else {
      abAppend(ab, row_inf, BARRIER_LENGTH-1);
    }
    abAppend(ab, "\x1b[K", 3);
    if(y < E.screenrows -1) {
      abAppend(ab, "\r\n", 2);
    }
  }
}

void editorRefreshScreen() {
  //clears and writes the buffer to the screen
  struct buffer ab = BUFFER_INIT;
  abAppend(&ab, "\x1b[?25l", 6);
  abAppend(&ab, "\x1b[H", 3);

  editorDrawRows(&ab, 0);
  char buf[32];
  snprintf(buf, sizeof(buf), "\x1b[%d;%dH", E.cy +1, E.cx +1);
  abAppend(&ab, buf, strlen(buf));
  abAppend(&ab, "\x1b[?25h", 6);

  write(STDOUT_FILENO, ab.b, ab.len);
  abFree(&ab);
}

void editorMoveCursor(char key) {
  //moves the cursor up/down and left/right based on a keypress
  switch(key) {
    case 'h':
    E.cx--;
    break;
    case 'd':
    E.cx++;
    break;
    case 'k':
    E.cy--;
    break;
    case 'j':
    E.cy++;
    break;
  }
}

void editorProcessKeypress() {
  //maps the pressed key to an action
  char c = editorReadKey();

  switch(c) {
    case CTRL_KEY('q'):
      write(STDOUT_FILENO, "\x1b[2J", 4);
      write(STDOUT_FILENO, "\x1b[H]", 3);
      exit(0);
      break;

    case 'h':
    case 'j':
    case 'k':
    case 'l':
      editorMoveCursor(c);
    break;
  }
}

void initEditor() {
  //initial state of the editor
  E.cx = BARRIER_LENGTH;
  E.cy = 0;
  if(getWindowSize(&E.screenrows, &E.screencols) == -1) die("getWindowSize");
}

int main() {
  enableRawMode();
  initEditor();
  while(1) {
    editorRefreshScreen();
    editorProcessKeypress();
  }
  return 0;
}
