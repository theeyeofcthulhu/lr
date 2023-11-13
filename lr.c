#include <stdio.h>
#include <errno.h>
#include <error.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>

#define INPUT_BUF_SZ 2048

void print_table(void)
{
    printf("ya: %lc, a: %lc, b: %lc, v: %lc, g: %lc, d: %lc, ye: %lc, e: %lc, yo: %lc, o: %lc, hz: %lc, z: %lc\n"
           "i: %lc, j: %lc, hk,x: %lc, k: %lc, l: %lc, m: %lc, n: %lc, p: %lc, r: %lc, ys: %lc, hs: %lc, s: %lc\n"
           "t: %lc, yu: %lc, u: %lc, f: %lc, hc: %lc, c: %lc, \": %lc, yy: %lc, ': %lc\n",
           L'\u044F', L'\u0430', L'\u0431', L'\u0432', L'\u0433', L'\u0434', L'\u044D', L'\u0435', L'\u0451', 
           L'\u043E', L'\u0436', L'\u0437', L'\u0438', L'\u0439', L'\u0445', L'\u043A', L'\u043B', L'\u043C',
           L'\u043D', L'\u043F', L'\u0440', L'\u0449', L'\u0448', L'\u0441', L'\u0442', L'\u044E', L'\u0443',
           L'\u0444', L'\u0447', L'\u0446', L'\u044A', L'\u044B', L'\u044C');  
}

wchar_t to_cyrillic(char c, char *carry)
{
    switch(c) {
    case 'a': {
        if(*carry == 'y') {
            *carry = '\0';
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
        if(*carry == 'y') {
            *carry = '\0';
            return L'\u044D';
        } else {
            return L'\u0435';
        }
    }
    case 'o': {
        if(*carry == 'y') {
            *carry = '\0';
            return L'\u0451';
        } else {
            return L'\u043E';
        }
    }
    case 'z': {
        if(*carry == 'h') {
            *carry = '\0';
            return L'\u0436';
        } else {
            return L'\u0437';
        }
    }
    case 'i':
        return L'\u0438';
    case 'j':
        return L'\u0439';
    case 'k': {
        if(*carry == 'h') {
            *carry = '\0';
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
        if(*carry == 'y') {
            *carry = '\0';
            return L'\u0449';
        } else if(*carry == 'h') {
            *carry = '\0';
            return L'\u0448';
        } else {
            return L'\u0441';
        }
    }
    case 't':
        return L'\u0442';
    case 'u': {
        if(*carry == 'y') {
            *carry = '\0';
            return L'\u044E';
        } else {
            return L'\u0443';
        }
    }
    case 'f':
        return L'\u0444';
    case 'c': {
        if(*carry == 'h') {
            *carry = '\0';
            return L'\u0447';
        } else {
            return L'\u0446';
        }
    }
    case '"':
        return L'\u044A';
    case 'y': {
        if(*carry == 'y') {
            *carry = '\0';
            return L'\u044B';
        } else {
            *carry = 'y';
            return L'\0';
        }
    }
    case '\'':
        return L'\u044C';
    case 'h':
        *carry = 'h';
        return L'\0';
    // alternate codes
    case 'x':
        return L'\u0445';
    default:
        return c;
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

    char in[INPUT_BUF_SZ];

    while (1) {
        // Simultaneously write output to "xclip" program,
        // putting into X copy/paste buffer
        int xclip_fd;
        if (use_xclip)
            xclip_fd = create_xclip_pipe();

        if (fgets(in, sizeof(in), stdin) == NULL) {
            error(1, 0, "fgets error or no characters read");
        }

        char *nl;
        if ((nl = strchr(in, '\n')) != NULL)
            *nl = '\0';

        char carry = '\0';
        for (char *i = in; *i; i++) {
            wchar_t c = to_cyrillic(*i, &carry);
            if (c != L'\0') {
                printf("%lc", c);
                if (use_xclip)
                    dprintf(xclip_fd, "%lc", c);
            }
        }

        printf("\n");
        if (use_xclip)
            close(xclip_fd);
    }

    return 0;
}
