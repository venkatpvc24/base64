#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>

#define MAX_LINE 10

static void b64_encode(unsigned char* in, unsigned char* out)
{
    char* base64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    uint32_t a = (unsigned char)(in[0]);
    uint32_t b = (unsigned char)(in[1]);
    uint32_t c = (unsigned char)(in[2]);

    uint32_t data_3b = (a << 0x10) + (b << 0x08) + c;

    out[0] = base64[(unsigned char)((data_3b >>  3 * 6) & 0x3f)];
    out[1] = base64[(unsigned char)((data_3b >>  2 * 6) & 0x3f)];
    out[2] = base64[(unsigned char)((data_3b >>  1 * 6) & 0x3f)];
    out[3] = base64[(unsigned char)((data_3b >>  0 * 6) & 0x3f)];

}

int base64(int argc, char** argv)
{
    bool encode = false, decode = false;
    char *in_file = NULL, *out_file = NULL;
    FILE *in, *out;
    for ( int i = 0; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            if (argv[i][1] == 'e') encode = true;
            if (argv[i][1] == 'd') decode = true;

            if (argv[i][1] == 'i') {
                if (argv[i+1] == NULL)
                {
                    perror("input file missing");
                    exit(0);
                }
                in_file = argv[i+1];
            }
            if (argv[i][1] == 'o') {
                if (argv[i+1] == NULL)
                {
                    perror("output file missing");
                    exit(0);
                }
                out_file = argv[i+1];
            }
        }
    }

    if (encode)
    {
        char data_in[3];
        char* data_out = (char*)malloc(sizeof(char) * 4);

        if (in_file)
        {
            in = fopen(in_file, "r");
            if (!in) perror("could not open file");
        }
        else
        {
            in = stdin;
        }
        if (out_file)
        {
            out = fopen(out_file, "wb");
            if (!out) perror("could not open file");
        }
        else
        {
            out = stdout;
        }

        size_t line = 0;
        int c, n = 0, count = 0;
        while((c = fgetc(in)) != EOF) {
            if (count < 3)
            {
                data_in[n++] = (char)c;
                count++;
            }
            if (count > 2)
            {
                b64_encode(data_in, data_out);
                // printf("%s\n", data_out);
                if ( line < 76 ) {
                    fwrite(data_out, 1, 4, out);
                }
                if (line == 72 )
                {
                    fputs("\r\n", out); line = 0;
                }
                line += 4;
                count = 0;
                n = 0;
            }
        }
    }

    fclose(in);
    fclose(out);

}