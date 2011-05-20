import sqlite3
import gzip
import csv
import os

# Connect to SQLite DB
conn = sqlite3.connect('data/data.db')

# Enable access by column names instead of indices
# (Get a dict instead of a list)
conn.row_factory = sqlite3.Row

c = conn.cursor()

# Create tables
sql = """
    CREATE TABLE `symbols`
        (`id` INTEGER PRIMARY KEY AUTOINCREMENT,
         `symbol` varchar, `name` varchar);

    CREATE TABLE `quotes`
        (`id` INTEGER PRIMARY KEY AUTOINCREMENT,
         `symbol` varchar, `date` date, `open` real,
         `high` real, `low` real, `close` real,
         `volume` integer, `adj_close` real);
    """
c.executescript(sql)

# Insert symbol data from gzipped symbol file
sql = """
    INSERT INTO `symbols`
    (`symbol`, `name`)
    VALUES
    (?, ?)
    """
try:
    f = open('data/sp500_18_may_2011.csv').read()
    reader = csv.reader(f.split(os.linesep))
    for row in reader:
        c.execute(sql, row)
except:
    pass
finally:
    conn.commit()
    c.close()
