#include "arg.h"

// -r for raw data file
// -m for metadata
// -h for help file
// -t for triggering of a probe [default = 0]
// -o for start idx offset [default = 0x00000000]

Arg arg_ProgramInput(int argc, char *argv[]){
	int i;
	Arg p = {NULL, NULL, 0, 0};
	if (argc < 2)
		return p;
	for (i = 1; i < argc; i++){
		if (argv[i][0] == '-')
			switch(argv[i][1]){
				case 'r':
					p.input = argv[i + 1];
					break;
				case 'm':
					p.metadata = argv[i + 1];
					break;
                case 't':
					sscanf(argv[i + 1], "%d", &p.trigger);
                    break;
				case 'h':
					printf("-r for raw data file.\n");
                    printf("-m for metadata.\n");
                    printf("-h for help file.\n");
                    printf("-t for triggering of a probe [default = 0].\n");
                    printf("-o for start idx offset [default = 0x00000000].\n");
					break;
				case 'o':
					if (strstr(argv[i + 1], "0x"))
						sscanf(argv[i + 1], "0x%X", &p.data_offset);
					else
						sscanf(argv[i + 1], "%d", &p.data_offset);
					break;
			}
	}
    return p;
}
