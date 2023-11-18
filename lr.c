#include <errno.h>
#include <error.h>
#include <getopt.h>
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>

#include <readline/readline.h>
#include <readline/history.h>

#define INPUT_BUF_SZ 2048

void print_table(void)
{
    printf("ya: %lc, a: %lc, b: %lc, v: %lc, g: %lc, d: %lc, ye: %lc, e: %lc, yo: %lc, o: %lc, zh: %lc, z: %lc\n"
           "i: %lc, j: %lc, kh,x: %lc, k: %lc, l: %lc, m: %lc, n: %lc, p: %lc, r: %lc, w: %lc, sh: %lc, s: %lc\n"
           "t: %lc, yu: %lc, u: %lc, f: %lc, ch: %lc, c: %lc, \": %lc, yy: %lc, ': %lc\n",
           L'\u044F', L'\u0430', L'\u0431', L'\u0432', L'\u0433', L'\u0434', L'\u044D', L'\u0435', L'\u0451', 
           L'\u043E', L'\u0436', L'\u0437', L'\u0438', L'\u0439', L'\u0445', L'\u043A', L'\u043B', L'\u043C',
           L'\u043D', L'\u043F', L'\u0440', L'\u0449', L'\u0448', L'\u0441', L'\u0442', L'\u044E', L'\u0443',
           L'\u0444', L'\u0447', L'\u0446', L'\u044A', L'\u044B', L'\u044C');  
}

wchar_t to_cyrillic(const char *str, int i)
{
    char prev = i != 0 ? str[i-1] : '\0';
    char next = str[i+1];

    switch(str[i]) {
    case 'a': {
        if(prev == 'y') {
            return L'\u044F';
        } else {
            return L'\u0430';
        }
    }
    case 'b':
        return L'\u0431';
    case 'v':
        return L'\u0432';
    case 'g':
        return L'\u0433';
    case 'd':
        return L'\u0434';
    case 'e': {
        if(prev == 'y') {
            return L'\u044D';
        } else {
            return L'\u0435';
        }
    }
    case 'o': {
        if(prev == 'y') {
            return L'\u0451';
        } else {
            return L'\u043E';
        }
    }
    case 'z': {
        if(next == 'h') {
            return L'\u0436';
        } else {
            return L'\u0437';
        }
    }
    case 'i': {
        if(prev == 'y') {
            return L'\u044B';
        } else {
            return L'\u0438';
        }
    }
    case 'j':
        return L'\u0439';
    case 'k': {
        if(next == 'h') {
            return L'\u0445';
        } else {
            return L'\u043A';
        }
    }
    case 'l':
        return L'\u043B';
    case 'm':
        return L'\u043C';
    case 'n':
        return L'\u043D';
    case 'p':
        return L'\u043F';
    case 'r':
        return L'\u0440';
    case 's': {
        if(next == 'h') {
            return L'\u0448';
        } else {
            return L'\u0441';
        }
    }
    case 't':
        return L'\u0442';
    case 'u': {
        if(prev == 'y') {
            return L'\u044E';
        } else {
            return L'\u0443';
        }
    }
    case 'w':
        return L'\u0449';
    case 'f':
        return L'\u0444';
    case 'c': {
        if(next == 'h') {
            return L'\u0447';
        } else {
            return L'\u0446';
        }
    }
    case '"':
        return L'\u044A';
    case '\'':
        return L'\u044C';
    // alternate codes
    case 'x':
        return L'\u0445';
    // only used in combination with other characters
    case 'y':
    case 'h':
        return '\0';
    default:
        if (str[i] < 0) // non-ascii value, better not return
            return '\0';
        return str[i];
    }

    return L'\0';
}

int create_xclip_pipe(void)
{
    pid_t pid;
    int fd[2];

    if (pipe(fd) == -1) {
        perror("pipe");
        return 1;
    }

    pid = fork();
    switch (pid) {
    case -1:
        perror("fork");
        return 1;
    case 0: {
        // turn child into xclip, reading from created pipe
        const char *xclip = "/usr/bin/xclip";
        if (access(xclip, F_OK) != 0) {
            perror(xclip);
            return 1;
        }

        // Read read-end of pipe into stdin
        close(fd[1]);
        dup2(fd[0], STDIN_FILENO);
        execl(xclip, xclip, "-selection", "clipboard", NULL);
        return 0;
    }
    default:
        close(fd[0]);
        return fd[1]; // return pipe address for main process to child
    }
}

int main(int argc, char **argv)
{
    bool use_xclip = true;

    int flag;
    while ((flag = getopt(argc, argv, "x")) != -1) {
        switch (flag) {
        case 'x':
            use_xclip = false;
            break;
        default:
            return 1;
        }
    }

    setlocale(LC_ALL, "");

    print_table();

    char *in, *out;
    while ((in = readline(": ")) != NULL) {
        if (strlen(in) == 0) {
            puts(""); // Simulate an emtpy string being entered
            free(in);
            continue;
        }

        add_history(in);

        out = malloc(strlen(in) * sizeof(wchar_t));
        *out = '\0';

        char *out_i = out;
        for (int i = 0; in[i] != '\0'; i++) {
            wchar_t c = to_cyrillic(in, i);
            if (c != L'\0') {
                out_i += sprintf(out_i, "%lc", c);
            }
        }

        puts(out);

        if (use_xclip) {
            int xclip_fd = create_xclip_pipe();
            dprintf(xclip_fd, "%s", out);
            close(xclip_fd);
        }

        free(in);
        free(out);
    }

    return 0;
}
