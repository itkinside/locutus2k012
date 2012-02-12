#ifndef DATABASE_H
#define DATABASE_H

struct track_info {
	char mbid[36];
	char *title;
	char *source_format;
	char *quality;
	int duration;
	// Our file data
	char *filename;
	char *location;
	char *extension;
	char fingerprint[36];
};

#endif
