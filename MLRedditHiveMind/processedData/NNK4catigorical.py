######################################
#Read data from pickle
######################################
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import pickle
# encode the result
def encodeData(input, prediction):
    for i in range(len(input)):
        if input[i] >= 5 :
            input[i] = 3
        elif input[i] <= -5:
            input[i] = 0
        elif input[i] < 5 and input[i] >= 0:
            input[i] = 2
        else:
            input[i] = 1
    for i in range(len(prediction)):
        if prediction[i] >= 5 :
            prediction[i] = 3
        elif prediction[i] <= -5:
            prediction[i] = 0
        elif prediction[i] < 5 and input[i] >= 0:
            prediction[i] = 2
        else:
            prediction[i] = 1
def readData(file_name):
    dataFrame = pd.read_pickle( file_name + ".pickle")
    dataFrame.columns = ['body', 'score', 'overallpol','stdPol', 'overallSub', 'stdSub', 'polRange', 'subRange', 'wordCount', 'bigWords', 'sentLen', 'targetPol', 'targetSub']
    y = dataFrame['score'].values
    x = dataFrame[['overallpol', 'stdSub', 'overallSub', 'sentLen']].values
    return y, x
#read data
files = ['americaData', 'AppleData', 'chinaData', 'ISIS_Data','ObamaData','RepublicansData','SandersData','TeslaData','WindowsData'];
for j in range(len(files)):
    #scale data
    from sklearn import preprocessing
    import numpy as np
    y, x = readData(files[j])
    y_np = np.array(y)
    x_np = np.array(x)
    # standard normalization
    # from sklearn.preprocessing import StandardScaler
    # scy = StandardScaler()
    # scx = StandardScaler()
    # scy = scy.fit(y)
    # scx = scx.fit(x)
    # X = scx.transform(x)
    # Y = scy.transform(y)
    # X.reshape(-1, 1)
    # Y.reshape(-1, 1)
    X = preprocessing.scale(x_np)
    Y = preprocessing.scale(y_np)
    ######################################
    #setup the dataset (supervised classification training) for neural network
    ######################################
    from pybrain.utilities import percentError
    from pybrain.tools.shortcuts import buildNetwork
    from pybrain.supervised.trainers import BackpropTrainer
    from pybrain.structure.modules import SoftmaxLayer
    from pybrain.datasets.classification import ClassificationDataSet
    from pybrain.datasets import SupervisedDataSet


    ds = SupervisedDataSet(4, 1)
    for i in range(len(X)):
        ds.addSample(X[i], Y[i])
    # #split the dataset
    trainData, testData = ds.splitWithProportion(0.60)

    # ###################################
    # #Creating a Neural Network
    # ###################################
    # # build nerual net with 4 inputs, 5 hidden neuron and 1 output neuron
    net = buildNetwork(4,5,1,bias=True)
    trainer = BackpropTrainer(net, trainData)
    train_error = trainer.trainUntilConvergence(dataset = trainData, maxEpochs = 50)

    # #evaluate the error rate on training data
    from sklearn.metrics import accuracy_score
    from sklearn.metrics import mean_squared_error
    train_out = net.activateOnDataset(trainData) #return the output


    #encodeData(train_out, trainData['target'])

    #calculate the error
    train_error = percentError( train_out, trainData['target'])
    train_mse = mean_squared_error(trainData['target'], train_out)
    test_out = net.activateOnDataset(testData)
    test_error = percentError( test_out, testData['target'])
    test_mse = mean_squared_error(testData['target'], test_out)
    print files[j] + 'neural network traindata MSE %.2f' % (train_mse)
    print files[j] + 'neural network testdata MSE %.2f' % (test_mse)









