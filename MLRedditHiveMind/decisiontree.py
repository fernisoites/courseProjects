import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import os, pickle
import glob
import numpy as np
import pandas as pd
from sklearn.tree import DecisionTreeClassifier
from sklearn.cross_validation import train_test_split
from sklearn import datasets
from sklearn.metrics import accuracy_score
from sklearn.metrics import confusion_matrix
from sklearn import preprocessing
from sklearn.metrics import accuracy_score
from sklearn.metrics import confusion_matrix
from sklearn.preprocessing import StandardScaler
from sklearn.feature_selection import RFE
from sklearn.tree import export_graphviz
from sklearn.metrics import accuracy_score
from sklearn.metrics import confusion_matrix
from sklearn.linear_model import LogisticRegression
from sklearn.datasets import load_iris
from sklearn.feature_selection import SelectFromModel

#scale the score into binary value
def convertScore(row,minVal,maxVal):
    score = row['score']
    if score<10: return 0
    return 1
    
def readData(subject):
    df = pd.read_pickle(subject + '.pickle')
    
for file in glob.glob("processedData/*Data.pickle"):
    subject=("../processData/" + file)[15:-7]
    americaDataFrame = pd.read_pickle(subject+ '.pickle')
    maxVal = americaDataFrame['score'].max()
    minVal = americaDataFrame['score'].min()
    print maxVal,minVal
    americaDataFrame['newScore']= americaDataFrame.apply(convertScore,args=[minVal,maxVal],axis=1)
    print americaDataFrame.head()
    americaDataFrame.columns = ['body', 'score', 'overallpol','stdPol', 'overallSub', 'stdSub', 'polRange', 'subRange', 'wordCount', 'bigWords', 'sentLen', 'targetPol', 'targetSub','newScore']
    Y = americaDataFrame['newScore'].values
    X = americaDataFrame[['overallpol', 'overallSub', 'stdSub','sentLen']].values

    X_train, X_test, Y_train, Y_test = train_test_split(X, Y, test_size=0.3, random_state=0)


    tree = DecisionTreeClassifier(criterion = 'entropy', max_depth=3, random_state=0)
    selector = RFE(tree, 2, step=1)
    selector = selector.fit(X_train, Y_train)
    selector.support_
    selector.ranking_
    tree.fit(X_train, Y_train)
    Y_pred=tree.predict(X_test)
    print('Accuracy: %.2f' % accuracy_score(Y_test,Y_pred))
        
    confmat=confusion_matrix(y_true=Y_test, y_pred=Y_pred)
    print(confmat)
    export_graphviz(tree, out_file= subject + '.dot',feature_names=['overallpol','stdPol', 'overallSub', 'stdSub', 'polRange', 'subRange', 'wordCount', 'bigWords', 'sentLen', 'targetPol', 'targetSub'])



