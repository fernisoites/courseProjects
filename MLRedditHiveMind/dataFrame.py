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

class decisionTree(object):
    def __init__(self, fileName, header = 0):
        #Pull in data
        self.data=pd.read_sql(fileName)
        self.data=pd.DataFrame(self.data)

    def replace(self,col,tobe,be):
        self.data[col]=self.data[col].replace(to_replace=tobe,value=be)

    def dupCol(self,oldCol,newCol):
        self.data[newCol] = self.data[oldCol]
    
    def impute(self, col):
        #Impute medians to address NaN
        imput=preprocessing.Imputer(missing_values='NaN',strategy='median',axis = 1)
        tmp=imput.fit_transform(self.data[col]).T
        self.data[col]=tmp
	
    def printData(self):
        print self.data.head()

    def dropCol(self,col):
        #Drop first column
        self.data=self.data.drop(col,axis=1)

    def createTestSet(self,xSet,y):
	    #Create trainning and testing sets
        self.X=self.data.ix[:,xSet]
        self.Y=self.data.ix[:,y]
        self.X_train, self.X_test, self.Y_train, self.Y_test = train_test_split(self.X, self.Y, test_size=0.3, random_state=0)


    def scale(self):
        """ The data is scaled in preparation for creating the DT.  WHY ?  
        WHAT IS THE DIFFERENCE BETWEEN SCALING AND NORMALIZATION?"""
        sc = StandardScaler()
        sc=sc.fit(X_train)
        self.X_train = sc.transform(self.X_train)
        self.X_test = sc.transform(self.X_test)

    def buildTree(self,depth):
        #Here, we define the parameters of our tree and use a feature selection algorithm (RFE) to pick out the strongest features.

        self.tree = DecisionTreeClassifier(criterion = 'entropy', max_depth=depth, random_state=0)
        selector = RFE(self.tree, 2, step=1)
        selector = selector.fit(self.X_train, self.Y_train)
        selector.support_
        selector.ranking_

    def fitTree(self):
        # We then fit the tree to our training data 

        self.tree.fit(self.X_train, self.Y_train)

    def visTree(self):
        # Now we visualize our tree

        export_graphviz(self.tree, out_file='tree.dot',feature_names=range(9))

    def pred(self):
        # Let's make a prediction

        self.Y_pred=self.tree.predict(self.X_test)

    def confusionMatrix(self):
        # Now we calculate our accuracy and create a confusion matrix of our results

        print('Accuracy: %.2f' % accuracy_score(self.Y_test,self.Y_pred))
        
        confmat=confusion_matrix(y_true=self.Y_test, y_pred=self.Y_pred)
        print(confmat)



##################################################
#  feature selection
##################################################

fileName = "pima2.csv"
d = decisionTree(fileName)

d.printData()


