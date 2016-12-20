import sqlite3
import pandas as pd
from sqlalchemy import create_engine

# set up a connection to original sqlite database
sql_conn = sqlite3.connect('database.sqlite')

def Solution(subredditName,sql_conn):
    # a function take input as subreddit name we are interested
    # query from original database, fetch body and score
    # export the data into a new sqlite database
    # the name of new database is the same as subreddit name
    # the name of table is subredditname+1
    
    engineName = 'sqlite:///' + subredditName + '.sqlite'
    engine = create_engine(engineName)

    query = "SELECT body,score FROM May2015 WHERE subreddit = '" + subredditName + "'"
    Users = pd.read_sql(query, sql_conn)
    Users.drop_duplicates(inplace = True)

    pdFrame = pd.DataFrame(Users)

    pdFrame.to_sql(subredditName+'1',engine,index=False)


subredditOfInterest = ['worldnews',
                       'technology',
                       'politics']

for area in subredditOfInterest:
    Solution(area,sql_conn)
