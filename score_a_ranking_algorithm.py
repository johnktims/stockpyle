
import sqlite3
import scipy
from scipy import stats

from ranks import *



def main():
    conn = sqlite3.connect('data/data.db')
    conn.row_factory = sqlite3.Row

    symb_sql = '''
        SELECT `symbol`
        FROM `symbols`
    '''
    symb_cur = conn.cursor()
    symb_cur.execute(symb_sql)



    #units are samples, (days)
    whole_history_size = 90	#pull this many days back
    slide_size = 5		#when picking windows... shift this much
    window_size = 30		#the history size presented to the ranking algorithm
    max_own_window = 5		#maximum number of days that a purchase has to get to the predicted value (hold time)


    comp_sql = '''
        SELECT `date`, `open`,
               `high`, `low`, `close`,
               `volume`, `adj_close`
        FROM `quotes`
        WHERE `symbol` = ? AND
              `date` > DATE('now','-90 days')
        ORDER BY `date` DESC
    '''
    comp_cur = conn.cursor()


    companyIndex = 0

    #for each of the possible symbols
    overallError = 0.0
    allSymbolErrors = []
    for company in symb_cur:
        #print "PASSED: ", tuple([company[0], whole_history_size])
        #comp_cur.execute( comp_sql, tuple([company[0], whole_history_size]) )	#grab entire history
        comp_cur.execute(comp_sql, tuple(company))	#grab entire history


        ####################################################################
        ## step 1: build vectors of interest
        ####################################################################
        timeVector  = []
        openVector  = []
        highVector  = []
        lowVector   = []
        closeVector = []

        #for each sample in this symbol's history
        rowIndex = 0
        for row in comp_cur:
            timeVector.append(rowIndex)		#make this based on row[1], (date entry)

            openVector.append(row['open'])
            highVector.append(row['high'])
            lowVector.append(row['low'])
            closeVector.append(row['close'])
            rowIndex += 1

        timeVector  = scipy.array(timeVector)
        openVector  = scipy.array(openVector)
        highVector  = scipy.array(highVector)
        lowVector   = scipy.array(lowVector)
        closeVector = scipy.array(closeVector)

        vectorOfInterest = closeVector
        ####################################################################



        ####################################################################
        ## step 2: find this symbol's error term
        ####################################################################
        symbolError = 0.0
        leftSide = 0
        numberOfSlides = 0
        while( leftSide < (whole_history_size - window_size - max_own_window) ):
            rightSide = leftSide + window_size	#inclusive...
            currentWindow = vectorOfInterest[leftSide:(rightSide+1)]

            #insert the algorithm function to test
            predicted_up_value = expectedRatioGain(timeVector, currentWindow)

            #find the value closest to this prediction, within the next few days
            #and call their difference the error term
            futureValues = vectorOfInterest[(rightSide+1):(rightSide+1+max_own_window)]
            currentValue = currentWindow[len(currentWindow)-1]						#last element

            #find the minimum absolute value difference between 
            #the actual and predicted over the next few days
            error_term = 0.0
            #for each of the next few days
            allActuals = []
            for futureIndex in range(0,len(futureValues)):
                actual_percentage_up = (futureValues[futureIndex] - currentValue) / currentValue
                possible_replacement = abs( predicted_up_value - actual_percentage_up )
                allActuals.append( actual_percentage_up )
                if( (futureIndex == 0) or (possible_replacement < error_term) ):
                    error_term = possible_replacement

            print predicted_up_value, " ", allActuals, " ", error_term

            symbolError = symbolError + error_term

            leftSide = leftSide + slide_size
            numberOfSlides = numberOfSlides + 1


        #expected value of smallest distance between prediction and actual
        symbolError = symbolError / numberOfSlides
        ####################################################################

        print company, ": ", symbolError
        allSymbolErrors.append( symbolError )
        overallError = overallError + symbolError

    overallError = overallError / len(allSymbolErrors)
    print "allSymbolErrors: ", allSymbolErrors
    print "overallError: ", overallError


if __name__ == '__main__':
    main()

