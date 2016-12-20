##########################################################
'''
This script is for reading data from ms sql database,
and store all the data into a python list.
The final data of this script is 'df'.

Make sure update correct SERVER, DATABASE, UID, PWD in dbConfig file.
'''
###########################################################

from dbConfig import *
import pyodbc

# establish connection
cnxn = pyodbc.connect('DRIVER={SQL Server};SERVER=%s;DATABASE=%s;UID=%s;PWD=%s' % (SERVER, DATABASE, UID, PWD))
cursor = cnxn.cursor()
cursor.execute("SELECT [record],[Floornum],[areaname],[roomname],\
               [sitename],[labelprinteddatetime],[samplecollecteddatetime]\
               ,[statusname],[colorname],[Phenotype],[Summary_status]\
               ,[speciesname],[resultentrydatetime],[resultcomment]\
               ,[cavnumber],[PCR_Tested],[PCR_Results],[Load_Dte]\
               FROM [DS_HSDM_Prod].[INFC].[EnvironmentalSample]")
rows = cursor.fetchall()
if not rows:
    # not sure whether syntax correct or not
    cursor.execute("SELECT [record],[Floornum],[areaname],[roomname],\
               [sitename],[labelprinteddatetime],[samplecollecteddatetime]\
               ,[statusname],[colorname],[Phenotype],[Summary_status]\
               ,[speciesname],[resultentrydatetime],[resultcomment]\
               ,[cavnumber],[PCR_Tested],[PCR_Results],[Load_Dte]\
               FROM [EnvironmentalSample]")
    rows = cursor.fetchall()

df = []
df.append(['record', 'Floornum', 'areaname', 'roomname', \
           'sitename', 'labelprinteddatetime', 'samplecollecteddatetime', \
           'statusname', 'colorname', 'Phenotype', 'Summary_status', \
           'speciesname', 'resultentrydatetime', 'resultcomment', \
           'cavnumber', 'PCR_Tested', 'PCR_Results', 'Load_Dte'])

for row in rows:
    if not row: continue
    df.append([row.record,row.Floornum,row.areaname,row.roomname,\
               row.sitename,row.labelprinteddatetime,\
               row.samplecollecteddatetime,row.statusname,\
               row.colorname,row.Phenotype,row.Summary_status,\
               row.speciesname,row.resultentrydatetime,row.resultcomment,\
               row.cavnumber,row.PCR_Tested,row.PCR_Results,row.Load_Dte])
