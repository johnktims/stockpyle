
#all rank functions should return a single real value, that can be used for sorting
#x is a vector of real values (can be integers if y is regularly sampled)
#    representing when the samples in y were taken
#y is a vector of real values, as large as x, where each entry is the 
#    sample value at the corresponding x time

import urllib2
import sqlite3
import gzip
import csv
import os
import io



#finds the slope and y-intercept of a line through 
#the data that minimizes the sum of the squared errors
def linearRegression( x, y ):

    n = len( y );

    S_x = sum( x );
    S_y = sum( y );

    xx = []
    for xVal in x:
        xx.append( xVal*xVal )

    yy = []
    for yVal in y:
        yy.append( yVal*yVal )

    xy = []
    for ii in range(0,len(x)):
        xy.append( x[ii] * y[ii] )

    S_xx = sum( xx );
    S_yy = sum( yy );
    S_xy = sum( xy );


    beta_hat = ((n*S_xy) - (S_x*S_y)) / ((n*S_xx) - (S_x*S_x));
    alpha_hat = ((1.0/n)*S_y) - (beta_hat*(1.0/n)*S_x);

    #s_2_e = (1.0/(n*(n-2))) * ((n*S_yy) - (S_y*S_y) - (beta_hat*beta_hat)*((n*S_xx) - (S_x*S_x)) );
    #s_2_beta = (n*s_2_e*s_2_e) / ((n*S_xx) - (S_x*S_x));
    #s_2_alpha = (s_2_beta*s_2_beta)*(1/n)*S_xx;

    #print beta_hat
    #print alpha_hat

    return [beta_hat, alpha_hat]
#################################################################################################
## END linearRegression(...)
#################################################################################################



#does basic linear regression and retrns the slope as the score
def slopeRank( x, y ):
    r = linearRegression( x, y )
    return r[0]
#################################################################################################
## END slopeRank(...)
#################################################################################################



#returns expected percentage gain/loss based on the line in the data
def expectedRatioGain( x, y ):
    r = linearRegression( x, y )
    slope = r[0]
    y_int = r[1]

    #find largest x (most recent)
    max_x_set = 0
    may_x = 0
    corresp_y = 0.1

    for ii in range(0, len(x)):
        if( (max_x_set == 0) or (x[ii] > max_x) ):
            max_x_set = 1
            max_x = x[ii]
            corresp_y = y[ii]

    lineValue = slope*max_x + y_int
    distanceBelowLine = lineValue - corresp_y	#negative if above projected line
    #print [slope, y_int]
    #print [lineValue, distanceBelowLine]
    #print [max_x, corresp_y]
    
    return distanceBelowLine / corresp_y
#################################################################################################
## END expectedRatioGain(...)
#################################################################################################


#def findTopRanker( conn ):
#    cur = conn.cursor()
#    cur.execute( "SELECT * from quotes" )
#
    #(`symbol`, `date`, `open`, `high`, `low`, `close`, `volume`, `adj_close`)
#
    #for ii in range(0,10):
    #    quotes = cur.fetchone()
    #    print quotes
    #    print quotes[1]
#
#    for row in rows:
        

def main():
    DATA_DIR = 'data'
    DATA_DB = 'data.db'

    conn = sqlite3.connect(DATA_DIR + '/' + DATA_DB)
    conn.row_factory = sqlite3.Row
    cur = conn.cursor()

    #(`symbol`, `date`, `open`, `high`, `low`, `close`, `volume`, `adj_close`)
    f = open('data/sp500_18_may_2011.csv').read()
    reader = csv.reader(f.split(os.linesep))
    for company in reader:
        if len(company) > 0:
            symbol = company[0]
            cur.execute( "SELECT * FROM `quotes` WHERE `symbol` = ? AND `date` > DATE('now','-30 days') ORDER BY `date` DESC", (symbol,) )
            allRows = cur.fetchall()

            timeVector = []
            openVector = []
            highVector = []
            lowVector = []
            closeVector = []
            ii = 0
            for row in allRows:
                timeVector.append( ii )		#make this based on row[1], (date entry)
                openVector.append( row['open'] )
                highVector.append( row['high'] )
                lowVector.append( row['low'] )
                closeVector.append( row['close'] )
                ii = ii +1

            print [symbol, expectedRatioGain(timeVector, openVector), expectedRatioGain(timeVector, highVector), expectedRatioGain(timeVector, lowVector), expectedRatioGain(timeVector, closeVector)]



    #print allRows




if __name__ == '__main__':
    main()

