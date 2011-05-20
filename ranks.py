
#all rank functions should return a single real value, that can be used for sorting
#x is a vector of real values (can be integers if y is regularly sampled)
#    representing when the samples in y were taken
#y is a vector of real values, as large as x, where each entry is the 
#    sample value at the corresponding x time



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



#something a bit different
def somethingExotic( x, y ):
    s = len(x)			#assumes x and y are the same size
    halfS = s/2

    firstHalf_x = x[0:halfS]
    firstHalf_y = y[0:halfS]
    secondHalf_x = x[halfS+1:s]
    secondHalf_y = y[halfS+1:s]


    
#################################################################################################
## END 
#################################################################################################

x_pass = range(0,500)
y_pass = []
#for xVal in x_pass:
for ii in range(0,len(x_pass)):
    x_pass[ii] = x_pass[ii] / 10.0
    xVal = x_pass[ii]
    y_pass.append( 3.2*xVal + 1.8 )

#print y_pass
y_pass[len(y_pass)-1] = y_pass[len(y_pass)-1] + 2;

x = expectedRatioGain( x_pass, y_pass )
print x


