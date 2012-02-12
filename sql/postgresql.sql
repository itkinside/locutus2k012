--
-- LOCUTUS 2k012 SQL SCRIPT
--

CREATE TABLE artist (	
	mbid character(36) PRIMARY KEY,
	name varchar NOT NULL
);

CREATE TABLE album (
	mbid character(36) PRIMARY KEY,
	name varchar NOT NULL,
	release_date date NOT NULL
);

CREATE TABLE artist_album (
	artist_mbid character(36) REFERENCES artist (mbid),
	album_mbid character(36) REFERENCES album (mbid),

	PRIMARY KEY (artist_mbid, album_mbid)
);

CREATE TABLE track (
	mbid character(36) PRIMARY KEY,
	title varchar NOT NULL,
	source varchar NOT NULL,
	quality varchar NOT NULL,
	duration integer NOT NULL,

	album_mbid character(36) REFERENCES album (mbid),

	location varchar NOT NULL,
	filename varchar NOT NULL,

	FOREIGN KEY (location, filename) REFERENCES file (location, filename)
);

CREATE TABLE file (
	filename varchar NOT NULL,
	location varchar NOT NULL,
	extension varchar NOT NULL,
	fingerprint character(36) NOT NULL, --acoustID
	sorted boolean NOT NULL,

	PRIMARY KEY (location, filename)
);
