#ifndef TAU_EXTERN_C_H
#define TAU_EXTERN_C_H

#ifdef __cplusplus
# define TAU_EXTERN_C_BEGIN extern "C" {
# define TAU_EXTERN_C_END }
#else
# define TAU_EXTERN_C_BEGIN
# define TAU_EXTERN_C_END
#endif

#endif
