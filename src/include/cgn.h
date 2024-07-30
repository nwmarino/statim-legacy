// Copyright 2024 Nick Marino (github.com/nwmarino)

#ifndef STATIMC_CGN_H
#define STATIMC_CGN_H

/**
 * Initialize the code generator.
 * 
 * @param out_file_name The name of the output file.
 */
void cgn_init(const char *out_file_name);

void cgn_close();

#endif  // STATIMC_CGN_H