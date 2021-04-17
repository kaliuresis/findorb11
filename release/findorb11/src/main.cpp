/* Noita Orb 11 Chest Location Finder, written by kaliuresis */
/* The random generation functions are recreated from reading disassembled and decompiled code,
   so there might be cases where they do not match the in-game values.
   If you find bugs, please let me know on discord. */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

typedef unsigned int uint;

typedef uint8_t byte;
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef uint8 bool8;

#define ever (;;)

#define len(array) sizeof(array)/sizeof((array)[0])

int __thiscall Random(double * random_seed, int a, int b)

{
  int iVar1;

  iVar1 = (int)*random_seed * 0x41a7 + ((int)*random_seed / 0x1f31d) * -0x7fffffff;
  if (iVar1 < 1) {
    iVar1 = iVar1 + 0x7fffffff;
  }
  *random_seed = (double)iVar1;
  return a - (int)((double)((b - a) + 1) * (double)iVar1 * -4.656612875e-10);
}

uint64 SetRandomSeedHelper(double r)
{
    uint64 e = *(uint64*)&r;
    if(((e >> 0x20 & 0x7fffffff) < 0x7FF00000)
       && (-9.223372036854776e+18 <= r) && (r < 9.223372036854776e+18))
    {
        //should be same as e &= ~(1<<63); which should also just clears the sign bit,
        //or maybe it does nothing,
        //but want to keep it as close to the assembly as possible for now
        e <<= 1;
        e >>= 1;
        double s = *(double*) &e;
        uint64 i = 0;
        if(s != 0.0)
        {
            uint64 f = (((uint64) e) & 0xfffffffffffff) | 0x0010000000000000;
            uint64 g = 0x433 - ((uint64) e >> 0x34);
            uint64 h = f >> g;

            int j = -(uint)(0x433 < ((e >> 0x20)&0xFFFFFFFF) >> 0x14);
            i = (uint64) j<<0x20 | j;
            i = ~i & h | f << (((uint64) s >> 0x34) - 0x433) & i;
            i = ~-(uint64)(r == s) & -i | i & -(uint64)(r == s);
            // error handling, whatever
            // f = f ^
            // if((int) g > 0 && f )
        }
        return i & 0xFFFFFFFF;
    }

    //error!
    uint64 error_ret_val = 0x8000000000000000;
    return *(double*) &error_ret_val;
}

uint __fastcall SetRandomSeedHelper2(int param_1,int param_2,uint param_3)
{
  uint uVar1;
  uint uVar2;
  uint uVar3;

  uVar2 = (param_1 - param_2) - param_3 ^ param_3 >> 0xd;
  uVar1 = (param_2 - uVar2) - param_3 ^ uVar2 << 8;
  uVar3 = (param_3 - uVar2) - uVar1 ^ uVar1 >> 0xd;
  uVar2 = (uVar2 - uVar1) - uVar3 ^ uVar3 >> 0xc;
  uVar1 = (uVar1 - uVar2) - uVar3 ^ uVar2 << 0x10;
  uVar3 = (uVar3 - uVar2) - uVar1 ^ uVar1 >> 5;
  uVar2 = (uVar2 - uVar1) - uVar3 ^ uVar3 >> 3;
  uVar1 = (uVar1 - uVar2) - uVar3 ^ uVar2 << 10;
  return (uVar3 - uVar2) - uVar1 ^ uVar1 >> 0xf;
}

void SetRandomSeed(double* random_seed, uint world_seed, double x, double y)
{
    uint a = world_seed ^ 0x93262e6f;
    uint b = a & 0xfff;
    uint c = (a >> 0xc) & 0xfff;

    double x_ = x+b;

    double y_ = y+c;

    double r = x_*134217727.0;
    uint64 e = SetRandomSeedHelper(r);

    uint64 _x = (*(uint64*) &x_ & 0x7FFFFFFFFFFFFFFF);
    uint64 _y = (*(uint64*) &y_ & 0x7FFFFFFFFFFFFFFF);
    if(102400.0 <= *((double*) &_y) || *((double*) &_x) <= 1.0)
    {
        r = y_*134217727.0;
    }
    else
    {
        double y__ = y_*3483.328;
        double t = e;
        y__ += t;
        y_ *= y__;
        r = y_;
    }

    uint64 f = SetRandomSeedHelper(r);

    uint g = SetRandomSeedHelper2(e, f, world_seed);
    double s = g;
    s /= 4294967295.0;
    s *= 2147483639.0;
    s += 1.0;

    if(2147483647.0 <= s) {
        s = s*0.5;
    }
    Random(&s, 0, 0);

    uint h = world_seed&3;
    while(h)
    {
        Random(&s, 0, 0);
        h--;
    }
    *random_seed = s;
}

bool remove_equals(char** s)
{
    char* equals = strchr(*s, '=');
    if(equals)
    {
        *equals = 0;
        *s = equals+1;
    }
    return equals;
}

#define is_whitespace(c) ((c) == ' ' || (c) == '\t')

bool only_whitespace(char* s)
{
    while(*s++)
    {
        if(!is_whitespace(*s++)) return false;
    }
    return true;
}

double x = 0.0;
double y = 0.0;
uint world_seed = 0;
uint newgame = 0;

char error_message[200] = "";

#define check_n_args()                                                  \
    if(*a >= n_args)                                                    \
    {                                                                   \
        sprintf(error_message, ": missing argument, expected a number\n"); \
        return -1;                                                      \
    }

#define create_set_option_function(name, format)                        \
    int set_##name(int* a, int n_args, char** args)                     \
    {                                                                   \
        check_n_args();                                                 \
        remove_equals(args+*a);                                         \
        if(*args[*a] == 0) (*a)++;                                      \
        check_n_args();                                                 \
        int n = sscanf_s(args[*a], format, &name);                      \
        (*a)++;                                                         \
        return n != 1;                                                  \
    }

create_set_option_function(x, "%lf")
create_set_option_function(y, "%lf")
create_set_option_function(world_seed, "%u")
create_set_option_function(newgame, "%u")

struct command_option
{
    const char* name;
    int (*func)(int* a, int n_args, char** args);
};

const char help_string[] = "\
Usage: findorb11.exe [SEED] [OPTION]\n\
Searches for locations where a great chest will spawn orb_11 in Noita. Search takes place in a spiral around the specified starting position.\n\
\n\
Options:\n\
    seed            = [integer] - world seed, defaults to 0\n\
    ng, NG, newgame = [integer] - newgame number. equivalent to adding this number to the world seed, defaults to 0\n\
    x, X            = [double]  - starting x position, defaults to 0.0\n\
    y, Y            = [double]  - starting y position, defaults to 0.0\n\
";


int print_help(int* a, int nargs, char** args)
{
    printf(help_string);
    exit(0);
}

command_option options[] = {
    {"seed", set_world_seed},
    {"ng", set_newgame},
    {"NG", set_newgame},
    {"newgame", set_newgame},
    {"x", set_x},
    {"X", set_x},
    {"y", set_y},
    {"Y", set_y},
    {"help", print_help},
    {"--help", print_help},
    {"\\help", print_help},
};

int main(int n_args, char** args)
{
    for(int a = 1; a < n_args;)
    {
        switch(*args[a])
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            {
                int error = set_world_seed(&a, n_args, args);
                if(error)
                {
                    printf("invalid world seed%s", error_message);
                }
                break;
            }
            default:
            {
                char* option = args[a];
                if(!remove_equals(args+a) || only_whitespace(args[a]))
                {
                    a++;
                }
                for(int o = 0 ;;)
                {
                    if(strcmp(option, options[o].name) == 0)
                    {
                        int error = options[o].func(&a, n_args, args);
                        if(error)
                        {
                            printf("invalid setting for %s%s", option, error_message);
                            return -1;
                        }
                        break;
                    }
                    if(++o >= len(options))
                    {
                        printf("invalid option %s", option);
                        return -1;
                    }
                }
                break;
            }
        }
    }

    world_seed += newgame;

    double random_seed;

    double x_off = 0.0;
    double y_off = 0.0;

    double x_step = 1.0;
    double y_step = 0.0;

    double epsilon = 0.1;

    printf("Searching seed %u for orb chest in a spiral starting at x = %lf, y = %lf\n", world_seed, x, y);

    //search for a magic pixel in a spiral
    for ever
    {
        double x_seed = floor(x+x_off);
        double y_seed = floor(y+y_off);
        SetRandomSeed(&random_seed, world_seed, x_seed, y_seed);
        if(Random(&random_seed, 0, 100000) == 100000 && Random(&random_seed, 0, 1000) == 999)
        {
            printf("x = %lf, y = %lf\n", x_seed, y_seed);
            return 0;
        }

        x_off += x_step;
        y_off += y_step;
        if((fabs(fabs(x_off)-fabs(y_off)) < epsilon && x_step <= epsilon)
           || (fabs(x_off-1.0+y_off) < epsilon) && x_step > epsilon)
        { //turn
            double temp = x_step;
            x_step = -y_step;
            y_step = temp;
        }
    }
}
