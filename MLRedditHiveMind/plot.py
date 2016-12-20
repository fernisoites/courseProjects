# -*- coding: utf-8 -*-
"""
Created on Tue May  3 23:55:56 2016

@author: chao
"""

import matplotlib.mlab as mlab
import matplotlib.pyplot as plt
from numpy import genfromtxt

subredditOfInterest = ['worldnews',
                       'technology',
                       'politics']
                       
for index in range(len(subredditOfInterest)):

    subredditName = subredditOfInterest[index]
    my_data = genfromtxt('data/'+subredditName+'.csv', delimiter=',',skip_header=1)
    print my_data
    plt.hist(my_data, 50, normed=1, facecolor='green', alpha=0.75)
    plt.xlabel('Smarts')
    plt.ylabel('Probability')
    plt.title(r'$\mathrm{Histogram\ of\ IQ:}\ \mu=100,\ \sigma=15$')
    #plt.axis([40, 160, 0, 0.03])
    #plt.grid(True)
    
    plt.show()