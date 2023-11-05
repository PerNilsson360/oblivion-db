// MIT license
//
// Copyright 2023 Per Nilsson
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the “Software”), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#include <getopt.h>
#include <memory>
#include <iostream>
#include <fstream>
#include "Json.hh"
#include "Schema.hh"

namespace {

void
printHelp() {
    std::cout << "Usage: jsch --schema=json-schema-file" << std::endl;
    std::cout << "Validates json data against a json schema file." << std::endl;
    std::cout << "JSON data is read from stdin and the result is printed on stdout." << std::endl;
}
    
}

int
main (int argc, char* argv[])
{
    std::string schemaFile;
    int c;
    while (true) {
        static struct option long_options[] = {
            {"help",    no_argument,       0, 'h'},
            {"version", no_argument,       0, 'v'},
            {"schema",  required_argument, 0, 's'},
            {0, 0, 0, 0}
        };
      
        /* getopt_long stores the option index here. */
        int option_index = 0;

        c = getopt_long (argc, argv, "hvs:", long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1) {
            break;
        }

        switch (c) {
        case 0:
            /* If this option set a flag, do nothing else now. */
            if (long_options[option_index].flag != 0) {
                break;
            }
            printf ("option %s", long_options[option_index].name);
            if (optarg) {
                printf (" with arg %s", optarg);
                printf ("\n");
            }
            break;
        case 'h':
            printHelp();
            return 0;
        case 'v':
            std::cout << "TODO: add version" << std::endl;
            return 0;
        case 's':
            schemaFile = optarg;
            break;
        case '?':
            /* getopt_long already printed an error message. */
            break;
        default:
            return -1;
        }
    }

    /* Print any remaining command line arguments (not options). */
    if (optind < argc) {
        printf ("non-option ARGV-elements: ");
        while (optind < argc)
            printf ("%s ", argv[optind++]);
        putchar ('\n');
    }

    if (schemaFile.empty()) {
        printHelp();
        return -1;
    }

    bool result(false);        
    try {
        
        std::unique_ptr<const Value> schema(Json::parse(schemaFile));
        // empty string means read from stdin
        std::unique_ptr<const Value> data(Json::parse(""));
        std::unique_ptr<const SchemaNode> schemaNode(parse(*schema));
        result = schemaNode->validate(*data);
    } catch (const std::exception& e) {
        std::cerr << "jsch got exception: " << e.what();
    }
    return result ? 0 : -1;
}
