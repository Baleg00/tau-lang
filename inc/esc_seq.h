/**
 * \file
 * 
 * ANSI escape sequence macros.
*/

#ifndef TAU_ESC_SEQ_H
#define TAU_ESC_SEQ_H

#ifndef TAU_NO_ESC_SEQ
# define ESC_VALUE(STR) "\033[" STR
#else
# define ESC_VALUE(STR) ""
#endif

#define ESC_FG_BLACK            ESC_VALUE("30m") 
#define ESC_FG_RED              ESC_VALUE("31m")
#define ESC_FG_GREEN            ESC_VALUE("32m")
#define ESC_FG_YELLOW           ESC_VALUE("33m")
#define ESC_FG_BLUE             ESC_VALUE("34m")
#define ESC_FG_MAGENTA          ESC_VALUE("35m")
#define ESC_FG_CYAN             ESC_VALUE("36m")
#define ESC_FG_WHITE            ESC_VALUE("37m")

#define ESC_FG_BRIGHT_BLACK     ESC_VALUE("90m")
#define ESC_FG_BRIGHT_RED       ESC_VALUE("91m")
#define ESC_FG_BRIGHT_GREEN     ESC_VALUE("92m")
#define ESC_FG_BRIGHT_YELLOW    ESC_VALUE("93m")
#define ESC_FG_BRIGHT_BLUE      ESC_VALUE("94m")
#define ESC_FG_BRIGHT_MAGENTA   ESC_VALUE("95m")
#define ESC_FG_BRIGHT_CYAN      ESC_VALUE("96m")
#define ESC_FG_BRIGHT_WHITE     ESC_VALUE("97m")

#define ESC_BG_BLACK            ESC_VALUE("30m")
#define ESC_BG_RED              ESC_VALUE("41m")
#define ESC_BG_GREEN            ESC_VALUE("42m")
#define ESC_BG_YELLOW           ESC_VALUE("43m")
#define ESC_BG_BLUE             ESC_VALUE("44m")
#define ESC_BG_MAGENTA          ESC_VALUE("45m")
#define ESC_BG_CYAN             ESC_VALUE("46m")
#define ESC_BG_WHITE            ESC_VALUE("47m")

#define ESC_BG_BRIGHT_BLACK     ESC_VALUE("100m")
#define ESC_BG_BRIGHT_RED       ESC_VALUE("101m")
#define ESC_BG_BRIGHT_GREEN     ESC_VALUE("102m")
#define ESC_BG_BRIGHT_YELLOW    ESC_VALUE("103m")
#define ESC_BG_BRIGHT_BLUE      ESC_VALUE("104m")
#define ESC_BG_BRIGHT_MAGENTA   ESC_VALUE("105m")
#define ESC_BG_BRIGHT_CYAN      ESC_VALUE("106m")
#define ESC_BG_BRIGHT_WHITE     ESC_VALUE("107m")

#define ESC_BOLD                ESC_VALUE("1m")
#define ESC_BOLD_RESET          ESC_VALUE("22m")

#define ESC_DIM                 ESC_VALUE("2m")
#define ESC_DIM_RESET           ESC_VALUE("22m")

#define ESC_ITALIC              ESC_VALUE("3m")
#define ESC_ITALIC_RESET        ESC_VALUE("23m")

#define ESC_UNDERLINE           ESC_VALUE("4m")
#define ESC_UNDERLINE_RESET     ESC_VALUE("24m")

#define ESC_BLINK               ESC_VALUE("5m")
#define ESC_BLINK_RESET         ESC_VALUE("25m")

#define ESC_INVERSE             ESC_VALUE("7m")
#define ESC_INVERSE_RESET       ESC_VALUE("27m")

#define ESC_HIDDEN              ESC_VALUE("8m")
#define ESC_HIDDEN_RESET        ESC_VALUE("28m")

#define ESC_STRIKE              ESC_VALUE("9m")
#define ESC_STRIKE_RESET        ESC_VALUE("29m")

#define ESC_RESET               ESC_VALUE("0m")

#define ESC_FG_COLOR(ID)        ESC_VALUE("38;5;" #ID "m")
#define ESC_BG_COLOR(ID)        ESC_VALUE("48;5;" #ID "m")

#define ESC_FG_RGB(R, G, B)     ESC_VALUE("38;2;" #R ";" #G ";" #B "m")
#define ESC_BG_RGB(R, G, B)     ESC_VALUE("48;2;" #R ";" #G ";" #B "m")

#endif