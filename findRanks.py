
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

    comp_sql = '''
        SELECT `date`, `open`,
               `high`, `low`, `close`,
               `volume`, `adj_close`
        FROM `quotes`
        WHERE `symbol` = ? AND
              `date` > DATE('now','-30 days')
        ORDER BY `date` DESC
    '''
    comp_cur = conn.cursor()

    allScores = []
    companyIndex = 0
    for company in symb_cur:
        comp_cur.execute(comp_sql, tuple(company))

        timeVector  = []
        openVector  = []
        highVector  = []
        lowVector   = []
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

        timeVector  = scipy.array(timeVector)
        openVector  = scipy.array(openVector)
        highVector  = scipy.array(highVector)
        lowVector   = scipy.array(lowVector)
        closeVector = scipy.array(closeVector)


        s1 = expectedRatioGain(timeVector, openVector)
        s2 = expectedRatioGain(timeVector, highVector)
        s3 = expectedRatioGain(timeVector, lowVector)
        s4 = expectedRatioGain(timeVector, closeVector)
        avgS = (s1+s2+s3+s4) / 4.0


        #symbolScores = [companyIndex,company[0],s1,s2,s3,s4,avgS, max([s1,s2,s3,s4])]
        symbolScores = [companyIndex,company[0],s1,s4,avgS]
        print symbolScores
        allScores.append( symbolScores )

        companyIndex = companyIndex+1


    for iii in range(2,len(symbolScores)):
        print 'index ' + str(iii)
        #allScores.sort(lambda x, y: cmp(x[iii],y[iii]))
        allScores.sort(lambda x, y: cmp(y[iii],x[iii]))
        #print allScores
        for asi in range(0, len(allScores)):
            print allScores[asi]



if __name__ == '__main__':
    main()
