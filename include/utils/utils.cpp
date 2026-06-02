#ifndef _UTILS_
#define _UTILS_

#include <cassert>
#include <stdio.h>
#include <Windows.h>

#define _do_
#define _then_

static void summonConsole(){
    if (AllocConsole()) {
        FILE* fDummy;
        freopen_s(&fDummy, "CONOUT$", "w", stdout);
        freopen_s(&fDummy, "CONOUT$", "w", stderr);
        freopen_s(&fDummy, "CONIN$", "r", stdin);

        // OUTPUT MODE (colors etc.)
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD outMode = 0;
        GetConsoleMode(hOut, &outMode);
        outMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, outMode);

        // INPUT MODE (FIX FREEZE)
        HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
        DWORD inMode = 0;
        GetConsoleMode(hIn, &inMode);

        inMode |= ENABLE_EXTENDED_FLAGS;      // required
        inMode &= ~ENABLE_QUICK_EDIT_MODE;    // disable freeze
        inMode &= ~ENABLE_INSERT_MODE;        // optional

        SetConsoleMode(hIn, inMode);
    }
}

/*
>>> .ini formate:-

[head_1]
element_1 = val(say, int)
element_2 = val(say, float)

[head_2]
element_1 = val(say, char)
element_2 = val(say, const char*)

>>> fmt:-

"[head_1] : %d, %f"
"[head_2] : %c, %s"

type_specifiers: %d %f %c %s

NOTE: %d    = int*
      %f    =    float*
      %c    =    char*
      %s    =    char*

      %zu   =    size_t*

      %b    =    _Bool*

      %-    =    skip

*/
/*
static void pasrse_ini(const char* file_path, const char* fmt, ...){

    FILE* file = fopen(file_path, "r");

    va_list args;
    va_start(args, fmt);

    assert(*fmt == '[');

    char buffer[32] = {'\0'};
    int buffer_idx = 0;

#define reset_buffer(buffer, idx)                                              \
    memset(buffer, 0, 32);                                                     \
    idx = 0;

    char ch = ' ';
    while(*fmt && (ch = fgetc(file)) > 0){

        // for comments
        if(ch == ';')
            while(ch != '\n' && ch != '\r' && ch != EOF)  _do_  ch = fgetc(file);

        while(ch != '[' && ch != EOF)  _do_  ch = fgetc(file);
        while(ch != ']'){
            ch = fgetc(file);
            buffer[buffer_idx++] = ch;
        }

        assert(memcmp(fmt++, buffer, buffer_idx) == 0);
        fmt += buffer_idx;

        reset_buffer(buffer, buffer_idx);

        assert(*fmt == ' ');
        assert(*(fmt++) == ':');
        assert(*(fmt++) == ' ');

        // move to data in file
        while(!isalpha(ch) && ch != EOF)   _do_   ch = fgetc(file);
        while(ch != '=' && ch != EOF)      _do_   ch = fgetc(file);
        ch = fgetc(file);

        while(isspace(ch))  _do_  ch = fgetc(file);
        // now, ch = data's first char


        while(1){
            // get data in buffer
            while (ch != '\n') {
              ch = fgetc(file);
              buffer[buffer_idx++] = ch;
            }
            buffer[buffer_idx] = '\0'; // remove '\n' from buffer


            if(*fmt == '%'){
                fmt++;
                switch(*fmt){
                    case 'd': {} break;
                    case 'f': {} break;
                    case 'c': {} break;
                    case 's': {} break;
                    case 'z': {} break;
                    case 'b': {} break;
                    case '-': {
                        fmt++;
                        while(isspace(*fmt))                              _do_   fmt++;
                        if(*fmt == '[' || *fmt == '\0' || *fmt == EOF)   _then_  break;



                    } break;
                    default:
                        fprintf(stderr, "unidentified type specifier %c\n", *fmt);
                }

                if(*(fmt++) == ',' && *(fmt++) == ' ')
                    continue;
                else
                    break;
            }
        }
    }

    va_end(args);
    fclose(file);
#undef reset_buffer
}
*/

#endif