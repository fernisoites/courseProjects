Source file:

readData.py: 
    Read raw sqlite database file (30Gb), and select all data in subreddit of interest, output into new sqlite databases.
    
GenerateWordFrequency.py: 
    Iterating all 3 subreddits of interest, by reading data generated from readData.py. Rank the words by their number of apperance.

GenerateWordFrequencyNormalized.py: 
    Similar to GenerateWordFrequency.py, but normalize the ranking, by adding the score of each apperance as weight, so that the words appear in every post would be diluted.

helper.py: 
    The place to process stop words, serving GenerateWordFrequency.py and GenerateWordFrequencyNormalized.py.

RScriptForPlots.R:
    A script read data from every analysis, the only purpose is generating plots.
decisiontree.py:
    A script generate decision trees based on the data from processData.
KNN.py:
    A script can generate KNN figures based on the data from processData.  

NeuRual.ipynb :
    A script read data and generate neural network
