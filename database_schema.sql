CREATE TABLE IF NOT EXISTS urls(
    shortened_version TEXT NOT NULL PRIMARY KEY,
    original_url TEXT NOT NULL
);