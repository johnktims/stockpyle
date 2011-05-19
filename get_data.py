#!/usr/bin/python

import urllib2
import sqlite3
import gzip
import csv
import os
import io

DATA_DIR = 'data'
DATA_DB = 'data.db'

def parse_csv_handle(handle):
    data = handle.read()
    reader = csv.reader(data.split(os.linesep))
    rows = []
    first = True
    try:
        for row in reader:
            # Skip the header row
            if first:
                first = not first
                continue
            rows.append(row)
    except csv.Error, e:
        sys.exit('file %s, line %d: %s' % (filename, reader.line_num, e))

    return rows

def get_symbols(conn):
    c = conn.cursor()
    sql = "SELECT `symbol`, `name` FROM `symbols`"
    rows = []
    c.execute(sql)
    for row in c:
        rows.append(row)
    return rows

def get_symbol_history(conn, symb, start=None, stop=None):
    a = b = ''
    url = "http://ichart.finance.yahoo.com/table.csv?s=%s" % symb

    c = conn.cursor()
    sql = """
        SELECT STRFTIME('%m/%d/%Y', DATE(MAX(`date`), '+1 day'))
        FROM `quotes`
        WHERE `symbol`=? AND
              (SELECT MAX(`date`)
               FROM `quotes`) != DATE('now')
        """
    c.execute(sql, (symb,))
    x = c.fetchone()
    if x[0] is not None:
        start = x[0].split('/')
    c.close()

    c = conn.cursor()
    sql = "SELECT STRFTIME('%m/%d/%Y', DATE('now'))"
    c.execute(sql)
    x = c.fetchone()
    if x[0] is not None and start is not None:
        stop = x[0].split('/')
    c.close()

    # The month is zero-indexed, so adjust months accordingly
    if stop is not None:
        start = [int(x) for x in start]
        start[0] -= 1
        a = '&a=%d&b=%d&c=%d' % (start[0], start[1], start[2])

    if stop is not None:
        stop = [int(x) for x in stop]
        stop[0] -= 1
        b = '&d=%d&e=%d&f=%d' % (stop[0], stop[1], stop[2])

    url += a + b + '&g=d&ignore=.csv'

    # Request gzipped data and unzip the data when gets here
    user_agent = "Mozilla/5.0 (X11; U; Linux i686) Gecko/20071127 Firefox/2.0.0.11"
    headers = {'User-Agent': user_agent,
               'Accept-Encoding': 'gzip'}
    bs = None
    try:
        bs = urllib2.urlopen(urllib2.Request(url, headers=headers)).read()
    except urllib2.HTTPError:
        return False
    bi = io.BytesIO(bs)
    gf = gzip.GzipFile(fileobj=bi, mode="rb")
    rows = parse_csv_handle(gf)

    c = conn.cursor()
    sql = """
        INSERT INTO `quotes`
            (`symbol`, `date`, `open`, `high`,
             `low`, `close`, `volume`, `adj_close`)
        VALUES(?,?,?,?,?,?,?,?)
        """
    # Prepend symbol column to each row
    for i, row in enumerate(rows):
        if len(rows[i]) != 7:
            continue
        rows[i] = [symb] + rows[i]
        c.execute(sql, rows[i])
    conn.commit()
    c.close()

    return rows

def main():
    conn = sqlite3.connect(DATA_DIR + '/' + DATA_DB)
    c = conn.cursor()

    # Process data
    i = 0
    for symbol, name in get_symbols(conn):
        i += 1
        print '%03d. Getting data for %s(%s)' % (i, name, symbol)
        if not get_symbol_history(conn, symbol):
            print "    -> Cache up-to-date for %s(%s)" % (name, symbol)

if __name__ == '__main__':
    main()
