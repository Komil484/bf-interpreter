#include <stdlib.h>
#include <stdio.h>

#define INSTR_INIT_SIZE 1024
#define DATA_SIZE 30000

int main(int argc, char *argv[])
{
    unsigned char *data = calloc(DATA_SIZE, 1);
    unsigned int data_pointer = 0;

    unsigned char *instr = malloc(INSTR_INIT_SIZE);
    unsigned int instr_pointer = 0;

    unsigned int instr_size = INSTR_INIT_SIZE;
    unsigned int instr_len = 0;

    if (argc == 1) {
        printf("Usage: bf <input file> [dump file]\n");
        return 0;
    }

    char *ifilename = argv[1];
    FILE *ifile = fopen(ifilename, "r");
    if (ifile == NULL) {
        printf("File \"%s\" not found\n", ifilename);
        return 1;
    }

    {
        int unclosed_brackets = 0;
        int c;
        while ((c = getc(ifile)) != EOF) {
            switch (c) {
            case '>':
            case '<':
            case '+':
            case '-':
            case '.':
            case ',':
            case '[':
            case ']':
                if (c == '[') {
                    ++unclosed_brackets;
                } else if (c == ']') {
                    --unclosed_brackets;
                    if (unclosed_brackets < 0) {
                        printf("File contains unmatched '['\n");
                        return 1;
                    }
                }

                if (instr_len >= instr_size) {
                    instr_size *= 2;
                    instr = realloc(instr, instr_size);
                    if (instr == NULL) {
                        printf("Insufficient memory to load \"%s\"\n", ifilename);
                        return 1;
                    }
                }

                instr[instr_len++] = c;
                break;
            default:
                break;
            }
        }

        if (unclosed_brackets > 0) {
            printf("File contains unmatched ']'\n");
            return 1;
        }
    }

    while (instr_pointer < instr_len) {
        switch (instr[instr_pointer]) {
        case '>':
            ++data_pointer;
            data_pointer %= DATA_SIZE;
            break;
        case '<':
            data_pointer += DATA_SIZE - 1;
            data_pointer %= DATA_SIZE;
            break;
        case '+':
            ++data[data_pointer];
            break;
        case '-':
            --data[data_pointer];
            break;
        case '.':
            putchar(data[data_pointer]);
            break;
        case ',':
            getchar();
            break;
        case '[': {
            int unclosed_brackets = 0;
            do {
                if (instr[instr_pointer] == '[') {
                    ++unclosed_brackets;
                } else if (instr[instr_pointer] == ']') {
                    --unclosed_brackets;
                }
                ++instr_pointer;
            } while (unclosed_brackets > 0 && instr_pointer < instr_len);
            if (instr_pointer > instr_len) {
                printf("bf interpreter bug: couldn't match bracket\n");
                exit(2);
            }
        } break;
        case ']': {
            int unopened_brackets = 0;
            do {
                if (instr[instr_pointer] == '[') {
                    --unopened_brackets;
                } else if (instr[instr_pointer] == ']') {
                    ++unopened_brackets;
                }
                --instr_pointer;
            } while (unopened_brackets > 0 && instr_pointer >= 0);
            if (instr_pointer < -1) {
                printf("bf interpreter bug: couldn't match bracket\n");
                exit(2);
            }
        } break;
        default:
            break;
        }
        ++instr_pointer;
    }

    if (argc >= 3) {
        FILE *ofile = fopen(argv[2], "w");
        if (ofile == NULL) {
            printf("Could not create/overwrite file \"%s\"\n", argv[2]);
        }
        int data_end = DATA_SIZE;
        while (data[--data_end] == 0) { }

        for (unsigned int i = 0; i <= data_end; ++i) {
            fprintf(ofile, "%.2X,", data[i]);
        }
    }
}
