#include "meta.h"

#define META_PARAMETERS 4
#define META_PARAMETERS_LEN 16

const char metadata[META_PARAMETERS][META_PARAMETERS_LEN] = {
    "total probes",
    "samplerate",
    "probe",
    "unitsize" // unused for now
};


SrMetaData* meta_InitStruct(void){
    SrMetaData *md = NULL;
	md = (SrMetaData*)calloc(1, sizeof(SrMetaData));
	if (!md)
		return NULL;
    return md;
}

int meta_GetData(char *filepath, SrMetaData *md){
	FILE *f = NULL;
	unsigned int i = 0;
	unsigned int size = 0;
	char *buffer = NULL;
	char *cp_buffer;

	// Open file
	f = fopen(filepath, "rb");
	if(!f)
		return -1;

    // Get size of file
	while (fgetc(f) != EOF)
        i++;
    if (!feof(f))
        return -1;

	size = i;
	rewind(f);

	buffer = (char*)malloc(sizeof(char) * size);
	if (!buffer)
		return -1;
    fread(buffer, sizeof(char), size, f);

	// Get total probes
	cp_buffer = strstr(buffer, metadata[META_TOTAL_PROBES]);
	cp_buffer = strchr(cp_buffer, '=');
	sscanf(cp_buffer+1, "%u", (unsigned int*)&md->total_probes);

	// Get sample rate
	cp_buffer = strstr(cp_buffer, metadata[META_SAMPLE_FREQ]);
	cp_buffer = strchr(cp_buffer, '=');
	sscanf(cp_buffer+1, "%u %c", (unsigned int*)&md->sample_freq, &md->sample_freq_unit);


	// Malloc probe names
	md->probe_name = (char**)malloc(sizeof(char*) * md->total_probes);
	md->probe_used = (unsigned char*)malloc(sizeof(unsigned char) * md->total_probes);
	for (i = 0; i < md->total_probes; i++){
		md->probe_name[i] = (char*)calloc(META_PROBE_NAME_LEN, sizeof(char));
		md->probe_used[i] = 0;
	}
	// Get probe names
	while ((cp_buffer = strstr(cp_buffer, metadata[META_PROBE]))){
		sscanf(cp_buffer + strlen(metadata[META_PROBE]), "%u", &i);
		cp_buffer = strchr(cp_buffer, '=');
		--i;
		sscanf(cp_buffer + 1, "%s", md->probe_name[i]);
		md->probe_used[i] = 1;
	}

    free(buffer);
    fclose(f);
	return 0;
}

void meta_FreeData(SrMetaData *md){
	unsigned char i;
	for (i = 0; i < md->total_probes; i++){
		free(md->probe_name[i]);
		md->probe_name[i] = NULL;
	}
	free(md->probe_name);
	free(md->probe_used);
	md->probe_name = NULL;
	md->probe_used = NULL;
}
