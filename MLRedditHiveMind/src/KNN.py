import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

#%matplotlib inline
from sklearn.cluster import KMeans
from sklearn import datasets

centers = [[1, 1], [-1, -1], [1, -1]]
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import pickle
import glob
import sys
#converscore is used for scale the score into reasonable range. The score larger than
#30 we will just let it be 30. After scaling, the score of the data is maximum to be 30
def convertScore(row,minVal,maxVal):
    score = row['score']
    if score<30  : return score
    return 30
for file in glob.glob("../processedData/*Data.pickle"):
#read data from /processData
  subject=("../processData/" + file)[17:-7]
  americaDataFrame=pd.read_pickle(subject + ".pickle")    
#scale "score" into "new score"
  maxVal = americaDataFrame['score'].max()
  minVal = americaDataFrame['score'].min()
  americaDataFrame['newScore']= americaDataFrame.apply(convertScore,args=[minVal,maxVal],axis=1)
  americaDataFrame.columns = ['body', 'score', 'overallpol','stdPol', 'overallSub', 'stdSub', 'polRange', 'subRange', 'wordCount', 'bigWords', 'sentLen', 'targetPol', 'targetSub','newScore']

  Y = americaDataFrame['newScore'].values
  X = americaDataFrame[['newScore','overallpol', 'overallSub', 'wordCount']].values
#set the estimators
  estimators = {'k_means_iris_3': KMeans(n_clusters=3),
                'k_means_iris_8': KMeans(n_clusters=8),
                'k_means_iris_bad_init': KMeans(n_clusters=3, n_init=1,
                                                init='random')}
  fignum = 1
  for name, est in estimators.items():
      fig = plt.figure(fignum, figsize=(4, 3))
      plt.clf()
      ax = Axes3D(fig, rect=[0, 0, .95, 1], elev=48, azim=134)
      plt.cla()
      est.fit(X)
      labels = est.labels_
#select features
      ax.scatter(X[:, 3], X[:, 1], X[:, 2], c=labels.astype(np.float))
#plot the KNN figure
      ax.w_xaxis.set_ticklabels([])
      ax.w_yaxis.set_ticklabels([])
      ax.w_zaxis.set_ticklabels([])
      ax.set_xlabel('wordCount')
      ax.set_ylabel('overallpol')
      ax.set_zlabel('overallSub')
      fignum = fignum + 1
  fig = plt.figure(fignum, figsize=(1, 1  ))
  plt.clf()
  ax = Axes3D(fig, rect=[0, 0, .95, 1], elev=48, azim=134)

  plt.cla()
  plt.show()
  sys.exit()
  for name, label in [(0, 0),
                      (1, 1),
                      (2, 2)]:
      ax.text3D(X[Y == label, 3].mean(),
                X[Y == label, 0].mean() + 1.5,
                X[Y == label, 2].mean(), name,
                horizontalalignment='center',
                bbox=dict(alpha=.5, edgecolor='w', facecolor='w'))
  # Reorder the labels to have colors matching the cluster results
  Y = np.choose(Y, [1, 2, 0]).astype(np.float)
  ax.scatter(X[:, 3], X[:, 0], X[:, 2], c=Y)
  ax.w_xaxis.set_ticklabels([])
  ax.w_yaxis.set_ticklabels([])
  ax.w_zaxis.set_ticklabels([])
  ax.set_xlabel('stdPol')
  ax.set_ylabel('overallSub')
  ax.set_zlabel('wordCount')
  plt.show()