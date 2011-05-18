import urllib2
import gzip
import csv
import os
import io

DATA_DIR = 'data'
DATA_SP500 = 'sp500_18_may_2011.csv'
DATA_QUOTES = 'quotes.csv'

def parse_csv_handle(handle):
    data = handle.read()
    reader = csv.reader(data.split(os.linesep))
    rows = []
    first = True
    try:
        for row in reader:
            # Skip thea headers
            if first:
                first = not first
                continue
            rows.append(row)
    except csv.Error, e:
        sys.exit('file %s, line %d: %s' % (filename, reader.line_num, e))

    return rows

def get_sp500_symbols():
    with open(DATA_DIR + '/' + DATA_SP500) as f:
        return parse_csv_handle(f)

def get_symbol_history(symb, start=None, stop=None):
    a = b = ''
    url = "http://ichart.finance.yahoo.com/table.csv?s=%s" % symb

    # The month is zero-indexed, so adjust months accordingly
    if start is not None:
        start[0] -= 1
        a = '&a=%d&b=%d&c=%d' % start

    if stop is not None:
        stop[0]  -= 1
        b = '&d=%d&e=%d&f=%d' % start

    url += a + b + '&g=d&ignore=.csv'

    # Request gzipped data and unzip the data when gets here
    bs = urllib2.urlopen(urllib2.Request(url, headers={"User-Agent": "Mozilla/5.0 (X11; U; Linux i686) Gecko/20071127 Firefox/2.0.0.11", "Accept-Encoding": "gzip"})).read()
    bi = io.BytesIO(bs)
    gf = gzip.GzipFile(fileobj=bi, mode="rb")
    rows = parse_csv_handle(gf)

    # Prepend symbol column to each row
    for i, row in enumerate(rows):
        rows[i] = [symb] + rows[i]

    return rows

def save_history(writer, rows):
    writer.writerows(rows)

def main():
    # Process data
    i = 0
    with open(DATA_DIR + '/' + DATA_QUOTES, 'wb') as o:
        writer = csv.writer(o, quoting=csv.QUOTE_ALL)
        for symbol, text in get_sp500_symbols():
            i += 1
            print '%03d. Getting data for %s(%s)' % (i, text, symbol)
            save_history(writer, get_symbol_history(symbol))

if __name__ == '__main__':
    main()
