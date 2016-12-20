import csv
import pandas
import os.path
import sys
import readMssql

# This script pulls data from v_record.csv and v_result.csv,
# And store them in python matrix

class csvData():
    def __init__(self,file1='v_record.csv',file2='v_result.csv'):
        # Make sure files exist
        self.validateFilenames(file1,file2)
        # read data separately from 2 files
        self.v_record = self.readData(file1)
        self.v_result = self.readData(file2)
        # Merge files by find 
        self.data = self.mergeData()
        self.attributesDict = {(self.data[0][i].replace('"','')):i for i in range(len(self.data[0]))}
        
    def validateFilenames(self, file1, file2):
        print "Reading data from csv files, %s and %s" % (file1,file2)
        try:
            assert os.path.isfile(file1)
            assert os.path.isfile(file2)
        except AssertionError:
            print 'File %s and %s not found, please make sure they are in root directory of this program.' %(file1, file2)
            print 'The default filenames for input are v_record.csv and v_result.csv, please rename your input files and run program without arguments.'
            print 'Alternatively, you can call this program by doing:'
            print 'python readFromCsv.py yourfilename1 yourfilename2'

    def readData(self,filename):
        res = pandas.read_csv(filename, delimiter = ',')
        res = [res.columns.values] + res.values.tolist()
        return res

    def mergeData(self):
        resDict = {}
        out = []
        for line in self.v_result:
            if line[5] == 'Negative':
                if line[1] not in resDict:
                    resDict[line[1]] = [line[5],'','']
            elif line[5] == 'Positive':
                resDict[line[1]] = [line[5],line[8],line[6]]

        for i,line in enumerate(self.v_record):
            if i==0:
                out.append((line.tolist()+['result','CAVnumber','speciesname']))
            else:
                if line[0] in resDict:
                    out.append((line[:]+resDict[line[0]]))
        return out

    def printColName(self):
        print "The attributes of file1 are: "
        for i,attribute in enumerate(self.v_record[0]):
            print str(i)+":", attribute+".",
        print
        
        print "The attributes of file2 are: "
        for i,attribute in enumerate(self.v_result[0]):
            print str(i)+":", attribute+".",
        print

        print "The attributes of mergedData are: "
        for i,attribute in enumerate(self.data[0]):
            print str(i)+":", attribute+".",
        print

    def removeMissingData(self):
        tmpData ,missingData = [] , []
        refPosNeg = {}
        refPosNeg['Positive'] = ['Positive','pos','POSITIVE']
        refPosNeg['Negative'] = ['Negative','neg','NEG','NG']
        for row in self.data:
            curRow = [char for char in row]

            # get ride of " in cells, this is very import for later steps
            for i in range(len(curRow)):
                curRow[i]=curRow[i].replace('"','')
            if row[2] not in refPosNeg['Positive']+refPosNeg['Negative']+["Results"]: 
                missingData.append(row)
                continue
            if row[4] == '':
                # no room number
                if row[7]!='' and row[8]!='': # room identifiable
                    curRow[4] = row[8]+row[7]
                else: # not able to find room information at all
                    missingData.append(row)
                    continue
            tmpData.append(curRow)
        return tmpData, missingData

    def removeDuplicate(self):
        ref = set()
        tmpData = []
        for row in self.data:
            curInfo = reduce(lambda x,y:x+y, row)
            if curInfo not in ref:
                ref.add(curInfo)
                tmpData.append(row)
            else: print row
        return  tmpData

    def compactData(self,attributeOfInterest):
        tmpData=[]
        for row in self.data:
            curRow = [char for char in row]
            tmpRow = []
            for item in attributeOfInterest:
                index = self.attributesDict[item]
                tmpRow.append(curRow[index])
            tmpData.append(tmpRow)
        return tmpData

    def writeCsv(self, filename):
        with open(filename,"wb") as csvfile:
            spamwriter = csv.writer(csvfile, delimiter=',',
                                quotechar='|', quoting=csv.QUOTE_MINIMAL)
            for line in self.data:
                spamwriter.writerow(line)


class sqlData():
    def __init__(self,df=readMssql.df):
        self.data = df
        self.attributesDict = {(self.data[0][i].replace('"','')):i for i in range(len(self.data[0]))}
        
    def printColName(self):
        print "The attributes are: "
        for i,attribute in enumerate(self.data[0]):
            print str(i)+":", attribute+".",
        print

    def removeMissingData(self):
        tmpData ,missingData = [] , []
        refPosNeg = {}
        refPosNeg['Positive'] = ['Positive','pos','POSITIVE']
        refPosNeg['Negative'] = ['Negative','neg','NEG','NG']
        for row in self.data:
            curRow = [char for char in row]

            # get ride of " in cells, this is very import for later steps
            for i in range(len(curRow)):
                curRow[i]=curRow[i].replace('"','')
            if row[2] not in refPosNeg['Positive']+refPosNeg['Negative']+["Results"]: 
                missingData.append(row)
                continue
            if row[4] == '':
                # no room number
                if row[7]!='' and row[8]!='': # room identifiable
                    curRow[4] = row[8]+row[7]
                else: # not able to find room information at all
                    missingData.append(row)
                    continue
            tmpData.append(curRow)
        return tmpData, missingData

    def removeDuplicate(self):
        ref = set()
        tmpData = []
        for row in self.data:
            curInfo = reduce(lambda x,y:x+y, row)
            if curInfo not in ref:
                ref.add(curInfo)
                tmpData.append(row)
            else: print row
        return  tmpData

    def compactData(self,attributeOfInterest):
        tmpData=[]
        for row in self.data:
            curRow = [char for char in row]
            tmpRow = []
            for item in attributeOfInterest:
                index = self.attributesDict[item]
                tmpRow.append(curRow[index])
            tmpData.append(tmpRow)
        return tmpData

    def writeCsv(self, filename):
        with open(filename,"wb") as csvfile:
            spamwriter = csv.writer(csvfile, delimiter=',',
                                quotechar='|', quoting=csv.QUOTE_MINIMAL)
            for line in self.data:
                spamwriter.writerow(line)

    def testAttributeOfInterest(self,attributeOfInterest):
        unmatch = []
        for attribute in attributeOfInterest:
            if attribute not in self.attributesDict:
                unmatch.append(attribute)
        if len(unmatch)>0:
            print "Sorry but the following attributes you choose don't exist in the data:"
            print unmatch
            print "Please select attrubutes from:"
            print self.attributesDict.values()
            print "And correct it in attributeOfInterest.txt."
            return False
        return True
                
if len(sys.argv)>1 and sys.argv[1]=='csv': 
    A = csvData()
    attributeOfInterest = [line.strip() for line in open('attributeOfInterest.txt').readlines()]
    if not A.testAttributeOfInterest(attributeOfInterest): sys.exit()
    A.data = A.compactData(attributeOfInterest)
    sys.exit()

A = sqlData()
attributeOfInterest = [line.strip() for line in open('attributeOfInterest.txt').readlines()]
if not A.testAttributeOfInterest(attributeOfInterest): sys.exit()
A.data = A.compactData(attributeOfInterest)
A.writeCsv('sqlData.csv')
