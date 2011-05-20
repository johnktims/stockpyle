
#all rank functions should return a single real value, that can be used for sorting
#x is a vector of real values (can be integers if y is regularly sampled)
#    representing when the samples in y were taken
#y is a vector of real values, as large as x, where each entry is the 
#    sample value at the corresponding x time

import sqlite3

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



#does basic linear regression and returns the slope as the score
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
    conn = sqlite3.connect('data/data.db')
    conn.row_factory = sqlite3.Row

    symb_sql = '''
        SELECT `symbol`
        FROM `symbols`
    '''
    symb_cur = conn.cursor()
    symb_cur.execute(symb_sql)

    comp_sql = '''
        SELECT `symbol`, `date`, `open`,
               `high`, `low`, `close`,
               `volume`, `adj_close`
        FROM `quotes`
        WHERE `symbol` = ? AND
              `date` > DATE('now','-30 days')
        ORDER BY `date` DESC
    '''
    comp_cur = conn.cursor()

    for company in symb_cur:
        comp_cur.execute(comp_sql, tuple(company))

        timeVector = []
        openVector = []
        highVector = []
        lowVector = []
        closeVector = []
        ii = 0

        for row in comp_cur:
            #make this based on row[1], (date entry)
            timeVector.append(ii)

            openVector.append(row['open'])
            highVector.append(row['high'])
            lowVector.append(row['low'])
            closeVector.append(row['close'])
            ii += 1

        print [company[0],
            expectedRatioGain(timeVector, openVector),
            expectedRatioGain(timeVector, highVector),
            expectedRatioGain(timeVector, lowVector),
            expectedRatioGain(timeVector, closeVector)]


if __name__ == '__main__':
    main()

