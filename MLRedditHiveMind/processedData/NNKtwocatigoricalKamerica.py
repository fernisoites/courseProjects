######################################
#Read data from pickle
######################################
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import pickle
# cardataframe = pd.read_csv('https://archive.ics.uci.edu/ml/machine-learning-databases/car/car.data',header=None)
americaDataFrame = pd.read_pickle("americaData.pickle")

americaDataFrame.columns = ['body', 'score', 'overallpol','stdPol', 'overallSub', 'stdSub', 'polRange', 'subRange', 'wordCount', 'bigWords', 'sentLen', 'targetPol', 'targetSub']
Y = americaDataFrame['score'].values
X = americaDataFrame[['overallpol','stdPol', 'overallSub', 'stdSub', 'polRange', 'subRange', 'wordCount', 'bigWords', 'sentLen', 'targetPol', 'targetSub']].values

from sklearn.preprocessing import LabelEncoder
from sklearn.cross_validation import train_test_split
######################################
#setup the dataset (supervised classification training) for neural network
######################################
from pybrain.utilities import percentError
from pybrain.tools.shortcuts import buildNetwork
from pybrain.supervised.trainers import BackpropTrainer
from pybrain.structure.modules import SoftmaxLayer
from pybrain.datasets.classification import ClassificationDataSet
from pybrain.datasets import SupervisedDataSet
ds = SupervisedDataSet(11, 1)
for i in range(len(X)):
    ds.addSample(X[i], Y[i])
# #split the dataset
trainData, testData = ds.splitWithProportion(0.60)


# ###################################
# #Creating a Neural Network
# ###################################
# # build nerual net with 21 inputs, 5 hidden neuron and 6 output neuron
net = buildNetwork(11,5,1,bias=True)
trainer = BackpropTrainer(net, trainData)
trnerr, valerr = trainer.trainUntilConvergence(dataset = trainData, maxEpochs = 50)

# #evaluate the error rate on training data
from sklearn.metrics import accuracy_score
train_out = net.activateOnDataset(trainData) #return the output

# encode the result
for i in range(len(train_out)):
    if train_out[i] >= 0 :
        train_out[i] = 1
    else:
        train_out[i] = 0;
for i in range(len(trainData['target'])):
    if trainData['target'][i] >= 0 :
        trainData['target'][i] = 1
    else:
        trainData['target'][i] = 0
#calculate the error
train_error = percentError( train_out, trainData['target'])
train_acc = accuracy_score( train_out, trainData['target'])

#testdata
test_out = net.activateOnDataset(testData)
for i in range(len(test_out)):
    if test_out[i] >= 0 :
        test_out[i] = 1
    else:
        test_out[i] = 0;
for i in range(len(testData['target'])):
    if testData['target'][i] >= 0 :
        testData['target'][i] = 1
    else:
        testData['target'][i] = 0
test_error = percentError( test_out, testData['target'])
test_acc = accuracy_score( test_out, testData['target'])

print('neural network training accuracies %.2f'
     % (train_acc))
print('neural network test accuracies %.2f'
     % (test_acc))








