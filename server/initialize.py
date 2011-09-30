#!/usr/bin/python

from datetime import datetime
import threading
import urllib2
import MySQLdb
import Queue
import csv
import sys
import os


g_host = raw_input('Host: ')
g_user = raw_input('User: ')
g_pass = raw_input('Pass: ')


def initialize_database():
    global g_host
    global g_user
    global g_pass

    conn = MySQLdb.connect(host=g_host,
                           user=g_user,
                           passwd=g_pass,
                           port=6612)
    cur = conn.cursor()

    # Create database
    cur.execute('DROP DATABASE IF EXISTS `stockpyle`')
    cur.execute('CREATE DATABASE `stockpyle`')
    cur.execute('USE `stockpyle`')

    # Create tables
    cur.execute('''
        CREATE TABLE `symbols` (
            `id` int(11) NOT NULL AUTO_INCREMENT,
            `symbol` varchar(13) NOT NULL,
            `sector` tinytext NOT NULL,
            `industry` tinytext NOT NULL,
            `title` tinytext NOT NULL,

            PRIMARY KEY (`id`),
            UNIQUE KEY `symbol` (`symbol`)
        ) ENGINE=InnoDB
    ''')

    cur.execute('''
        CREATE TABLE `quotes` (
            `id` int(11) NOT NULL AUTO_INCREMENT,
            `symbol_id` int(11) NOT NULL,
            `open` double NOT NULL,
            `high` double NOT NULL,
            `low` double NOT NULL,
            `close` double NOT NULL,
            `volume` int(11) NOT NULL,
            `date` datetime NOT NULL,

            PRIMARY KEY (`id`),
            FOREIGN KEY (`symbol_id`) REFERENCES symbols(id)
                ON UPDATE CASCADE
                ON DELETE CASCADE
        ) ENGINE=InnoDB
    ''')

    # Commit changes and clean up
    conn.commit()
    conn.close()
    

def download_nasdaq_csv(market):
    '''
        CSV fields:
            Symbol, Name, LastSale, MarketCap, IPOyear,
            Sector, Industry, Summary, Quote
    '''
    site = 'http://www.nasdaq.com'
    offset = 'screening'
    filename = 'companies-by-industry.aspx?exchange=%s&render=download'
    url = '/'.join((site, offset, filename % market))
    text = urllib2.urlopen(url).read()
    return csv.reader(text.split(os.linesep)[1:-1])
 

def download_wsj_csv(market):
    '''
        CSV fields:
            Name, Symbol, Open, High, Low, Close, Net Chg,
            % Chg, Volume, 52 Wk High, 52 Wk Low, Div, Yield,
            P/E,YTD % Chg
    '''
    site = 'http://online.wsj.com'
    offset = 'public/resources/documents'
    filename = '%s.csv'
    url = '/'.join((site, offset, filename % market))
    text = urllib2.urlopen(url).read()
    try:
        x = text.index('Name')
    except ValueError:
        return False
    return csv.reader(text[x:-1].split(os.linesep))


def download_google_csv(symbol):
    '''
        CSV fields:
            Date, Open, High, Low, Close, Volume
    '''
    site = 'http://finance.google.com'
    offset = 'finance'
    filename = 'historical?q=%s&output=csv'
    url = '/'.join((site, offset, filename % symbol))
    request = urllib2.Request(url)
    opener  = urllib2.build_opener()
    user_agent = 'Mozilla/5.0 (X11; Linux i686) ' \
                 'AppleWebKit/535.1 (KHTML, like Gecko) ' \
                 'Chrome/13.0.782.107 Safari/535.1'
    request.add_header('User-Agent', user_agent)
    text = opener.open(request).read()
    
    if len(text) < 1:
        raise ValueError
    return csv.reader(text.split(os.linesep)[1:])


def insert_symbols(conn):
    markets = ('Nasdaq', 'NYSE', 'AMEX')

    # Get list of symbols from Nasdaq.com
    sql = '''INSERT INTO `symbols`
                 (`symbol`, `sector`, `industry`, `title`)
             VALUES
                 (%s,%s,%s,%s)'''

    num_total_symbols = 0
    num_filtered_symbols = 0

    for market in markets:
        print '---> Fetching %s data' % market

        # Get list of symbols from WSJ. We'll be using WSJ to update daily, so
        # why get information for symbols they don't track?
        wsj_symbols = [x[1] for x in download_wsj_csv(market)]

        # NASDAQ.com has more information, so use them to get sector info, etc.
        nasdaq_csv = [x for x in download_nasdaq_csv(market)]

        num_total_symbols += len(nasdaq_csv)
        for i, nasdaq_row in enumerate(nasdaq_csv):
            for wsj_symbol in wsj_symbols:
                if wsj_symbol == nasdaq_row[0]:
                    break
            else:
               del nasdaq_csv[i] 
        num_filtered_symbols += len(nasdaq_csv)

        for row in nasdaq_csv:
            cursor = conn.cursor()
            symbol = row[0].strip()
            title  = row[1]
            sector = row[5]
            industry = row[6]
            symbol = ':'.join((market, symbol))
            done = False
            while not done:
                try:
                    cursor.execute(sql, (symbol, sector, industry, title))
                    done = True
                except MySQLdb.OperationalError:
                    pass
                    
    conn.commit()

    print 'Found %s symbols.' % num_total_symbols
    print 'Downloading information for %s symbols' % num_filtered_symbols

def fetch_symbols(conn):
    cursor = conn.cursor()
    sql = "SELECT `id`, `symbol` FROM `symbols`"
    cursor.execute(sql)
    return cursor

class download_history(threading.Thread):
    def __init__(self, q):
        global g_host
        global g_user
        global g_pass

        threading.Thread.__init__(self)

        # Each thread requires its own connection to the database
        self.conn = MySQLdb.connect(host=g_host,
                                    user=g_user,
                                    passwd=g_pass,
                                    db='stockpyle',
                                    port=6612)
        self.q = q
        self.cursor = self.conn.cursor()
        self.sql = '''INSERT INTO `quotes`
                          (`symbol_id`,`open`,`high`,`low`,
                           `close`,`volume`,`date`)
                      VALUES
                          (%s,%s,%s,%s,%s,%s,%s)'''
        
    def run(self):
        while True:
            try:
                db_id, symbol = self.q.get(True, 2)
            except Queue.Empty:
                return

            print 'Downloading: %s' % symbol
            try:
                rows = download_google_csv(symbol)
            except urllib2.HTTPError:
                sys.stderr.write('Error getting `%s`; Removing from database.\n' % symbol)
                self.cursor.execute("DELETE FROM `symbols` WHERE `symbol`=%s", (symbol,))
                continue
            else:
                for row in rows:
                    # Skip blank rows. Usually the last row.
                    if len(row) < 1:
                        continue

                    date, open, high, low, close, volume = row
                    date = datetime.strptime(date, '%d-%b-%y')
                    #date = datetime.strptime(date, '%Y-%m-%d')
                    params = (db_id, open, high, low, close, volume, date)
                    done = False
                    while not done:
                        try:
                            self.cursor.execute(self.sql, params)
                            done = True
                        except MySQLdb.OperationalError:
                            pass
                self.conn.commit()
            finally:
                self.q.task_done()

def main():
    global g_host
    global g_user
    global g_pass

    print 'Initializing Database'
    initialize_database()

    conn = MySQLdb.connect(host=g_host,
                           user=g_user,
                           passwd=g_pass,
                           db="stockpyle",
                           port=6612)
    q = Queue.Queue()

    print 'Fetching symbols from Internet'
    insert_symbols(conn)

    print 'Fetching symbols and corresponding IDs from database'
    symbols = fetch_symbols(conn)

    print 'Spawning worker threads'
    for i in xrange(15):
        p = download_history(q)
        p.setDaemon(True)
        p.start()

    # Load jobs into the queue
    for i in symbols:
        q.put(i)

    print 'Joining threads'
    q.join()
    conn.commit()

    # Create cache tables
    #
    # MySQL Views aren't cached and are really slow. This will have to be
    # reconstructed daily by the update script.
    print 'Creating cache tables'
    days = (30, 60, 90)
    cur = conn.cursor()

    for day in days:
        print 'Creating %d_day_window cache table' % day
    
        cur.execute('''
            CREATE TABLE `%d_day_window`
                SELECT *
                FROM `quotes`
                WHERE `date` > DATE_SUB(NOW(), INTERVAL %d DAY)
                ORDER BY `date`
        ''' % (day, day))
        conn.commit()


if __name__ == "__main__":
    main()
