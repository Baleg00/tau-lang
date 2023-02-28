#ifndef TAU_ESC_SEQ_H
#define TAU_ESC_SEQ_H

#ifndef TAU_NO_ESC_SEQ
# define ESC_VALUE(STR) STR
#else
# define ESC_VALUE(STR) ""
#endif

#define ESC_FG_BLACK            ESC_VALUE("\033[30m") 
#define ESC_FG_RED              ESC_VALUE("\033[31m")
#define ESC_FG_GREEN            ESC_VALUE("\033[32m")
#define ESC_FG_YELLOW           ESC_VALUE("\033[33m")
#define ESC_FG_BLUE             ESC_VALUE("\033[34m")
#define ESC_FG_MAGENTA          ESC_VALUE("\033[35m")
#define ESC_FG_CYAN             ESC_VALUE("\033[36m")
#define ESC_FG_WHITE            ESC_VALUE("\033[37m")

#define ESC_FG_BRIGHT_BLACK     ESC_VALUE("\033[90m")
#define ESC_FG_BRIGHT_RED       ESC_VALUE("\033[91m")
#define ESC_FG_BRIGHT_GREEN     ESC_VALUE("\033[92m")
#define ESC_FG_BRIGHT_YELLOW    ESC_VALUE("\033[93m")
#define ESC_FG_BRIGHT_BLUE      ESC_VALUE("\033[94m")
#define ESC_FG_BRIGHT_MAGENTA   ESC_VALUE("\033[95m")
#define ESC_FG_BRIGHT_CYAN      ESC_VALUE("\033[96m")
#define ESC_FG_BRIGHT_WHITE     ESC_VALUE("\033[97m")

#define ESC_BG_BLACK            ESC_VALUE("\033[30m")
#define ESC_BG_RED              ESC_VALUE("\033[41m")
#define ESC_BG_GREEN            ESC_VALUE("\033[42m")
#define ESC_BG_YELLOW           ESC_VALUE("\033[43m")
#define ESC_BG_BLUE             ESC_VALUE("\033[44m")
#define ESC_BG_MAGENTA          ESC_VALUE("\033[45m")
#define ESC_BG_CYAN             ESC_VALUE("\033[46m")
#define ESC_BG_WHITE            ESC_VALUE("\033[47m")

#define ESC_BG_BRIGHT_BLACK     ESC_VALUE("\033[100m")
#define ESC_BG_BRIGHT_RED       ESC_VALUE("\033[101m")
#define ESC_BG_BRIGHT_GREEN     ESC_VALUE("\033[102m")
#define ESC_BG_BRIGHT_YELLOW    ESC_VALUE("\033[103m")
#define ESC_BG_BRIGHT_BLUE      ESC_VALUE("\033[104m")
#define ESC_BG_BRIGHT_MAGENTA   ESC_VALUE("\033[105m")
#define ESC_BG_BRIGHT_CYAN      ESC_VALUE("\033[106m")
#define ESC_BG_BRIGHT_WHITE     ESC_VALUE("\033[107m")

#define ESC_BOLD                ESC_VALUE("\033[1m")
#define ESC_BOLD_RESET          ESC_VALUE("\033[22m")

#define ESC_DIM                 ESC_VALUE("\033[2m")
#define ESC_DIM_RESET           ESC_VALUE("\033[22m")

#define ESC_ITALIC              ESC_VALUE("\033[3m")
#define ESC_ITALIC_RESET        ESC_VALUE("\033[23m")

#define ESC_UNDERLINE           ESC_VALUE("\033[4m")
#define ESC_UNDERLINE_RESET     ESC_VALUE("\033[24m")

#define ESC_BLINK               ESC_VALUE("\033[5m")
#define ESC_BLINK_RESET         ESC_VALUE("\033[25m")

#define ESC_INVERSE             ESC_VALUE("\033[7m")
#define ESC_INVERSE_RESET       ESC_VALUE("\033[27m")

#define ESC_HIDDEN              ESC_VALUE("\033[8m")
#define ESC_HIDDEN_RESET        ESC_VALUE("\033[28m")

#define ESC_STRIKE              ESC_VALUE("\033[9m")
#define ESC_STRIKE_RESET        ESC_VALUE("\033[29m")

#define ESC_RESET               ESC_VALUE("\033[0m")

#define ESC_FG_COLOR(ID)        ESC_VALUE("\033[38;5;" #ID "m")
#define ESC_BG_COLOR(ID)        ESC_VALUE("\033[48;5;" #ID "m")

#define ESC_FG_RGB(R, G, B)     ESC_VALUE("\033[38;2;" #R ";" #G ";" #B "m")
#define ESC_BG_RGB(R, G, B)     ESC_VALUE("\033[48;2;" #R ";" #G ";" #B "m")

#endif