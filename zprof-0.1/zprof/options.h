#ifndef _OPTIONS_H
#define _OPTIONS_H

#include "ccport.h"

enum {
  COND_OPT,
  HELP_OPT,
  OUTPUT_OPT,
  PREFIX_OPT,
  PREPROCESS_OPT,
  SILENT_OPT,
  TYPEDEF_OPT,
  VERSION_OPT
};

typedef struct {
  ConstString output;
  ConstString prefix;
  Boolean isCond;
  Boolean isPreprocess;
  Boolean isSilent;
} Options;

extern CONST Options *CONST optionsP;

#endif /* ifndef _OPTIONS_H */
