#ifndef PARSER_H
#define PARSER_H

#include "dag.h"

// read and parse the given Patterson data file. Store the resulting
// dag in `g'. Return 0 on success and -1 on failure.
int parse_patterson(const char *fp, dag **g);

#endif // PARSER_H
