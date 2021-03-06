#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>


#define MAX_LINE 10


static void b64_encode(const char in[3], unsigned char* out, int len)
{
    char* base64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    uint32_t a = (unsigned char)(in[0]);
    uint32_t b = (unsigned char)(in[1]);
    uint32_t c = (unsigned char)(in[2]);

    uint32_t data_3b = (a << 0x10) + (b << 0x08) + c;

    out[0] = base64[(unsigned char)((data_3b >>  3 * 6) & 0x3f)];
    out[1] = base64[(unsigned char)((data_3b >>  2 * 6) & 0x3f)];
    out[2] = len == 1 ? '=' : base64[(unsigned char)((data_3b >>  1 * 6) & 0x3f)];
    out[3] = len == 1 || len == 2 ? '=' : base64[(unsigned char)((data_3b >>  0 * 6) & 0x3f)];

}

int main(int argc, char** argv)
{
    bool encode = false, decode = false;
    char *in_file = NULL, *out_file = NULL;
    unsigned int type = 0;
    FILE *in, *out;
    size_t line_length = 0, line = 0;
    int opt = 0;
    char* end = NULL;
    while( (opt = getopt(argc, argv, "t:edi:o:l:")) != -1)
    {
        switch(opt)
        {
            case 't':
                type = strtol(optarg, &end, 10);
                break;
            case 'i':
                in_file = optarg;
                break;
            case 'o':
                out_file = optarg;
                break;
            case 'e':
                encode = true;
                break;
            case 'd':
                decode = true;
                break;
            case 'l': {
                int num = strtol(optarg, &end, 10);
                line = num ? num : 0;
                if (line < MAX_LINE) {
                    line = MAX_LINE;
                }
            }
                break;
            case '?':
                printf("unknown command line arg\n");
                printf("Usage -t 32/64 -e or -d [-i file.txt] [-o base64.txt] [-l 32]\n");
                return EXIT_FAILURE;
        }
    }
    if (encode)
    {
        unsigned char data_in[3];
        unsigned char data_out[4];

        if (in_file)
        {
            in = fopen(in_file, "rb");
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

        int c, count = 0;
        while(!feof(in)) {
            while( count < 3)
            {
                if ((c = fgetc(in)) == EOF) break;
                data_in[count] = (char)c;
                count++;
            }
            // int len = count_filled_bytes(data_in);
            b64_encode(data_in, data_out, count);
            memset(data_in, 0, sizeof data_in);
            if ( line_length < line ) {
                size_t size = fwrite(data_out, 1, 4, out);
                if (size != 4)
                {
                    fprintf(stderr, "error writing to file");
                    return EXIT_FAILURE;
                }
            }
            if (line_length >= line )
            {
                fputs("\r\n", out); line_length = 0;
            }
            line_length += 4;
            count = 0;
        }
    }

    fclose(in);
    fclose(out);

}
