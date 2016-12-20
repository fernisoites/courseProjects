import mysql.connector
from mysql.connector import errorcode

DB_NAME = 'environmentDB'

cnx = mysql.connector.connect(user='root', 
                              host='127.0.0.1',
                              database='environmentDB')
cursor = cnx.cursor()

# store any new table information in a dictionary called 'TABLES'.
# define all the required fields for each form.
TABLES = {}
TABLES['index_table'] = (
    "CREATE TABLE `index_table` ("
    "  `department` varchar(16) NOT NULL,"
    "  `location` int(10) NOT NULL,"
    "  `sink` varchar(30) NOT NULL,"
    "  `date` date NOT NULL,"
    "  `drain_swab_chromagar` varchar(32) NOT NULL,"
    "  `indirect_pos_neg` varchar(255) NOT NULL,"
    "  `organism_ID_swab` varchar(255) NOT NULL,"
    "  `CAV_drain_swab` int(10) NOT NULL,"
    "  `entry_id` int(10) AUTO_INCREMENT,"
    "  `pTrap_water_chromagar` varchar(32) NOT NULL,"
    "  `indirect_pos_neg_drain` varchar(255) NOT NULL,"
    "  `organism_ID_water` varchar(255) NOT NULL,"
    "  `CAV_pTrap_water` int(10) NOT NULL,"
    "  `comments` varchar(255) NOT NULL,"
    "  PRIMARY KEY (`entry_id`)"
    ") ENGINE=InnoDB")


# execute the building table funciton, and report error if table exists.
for name, ddl in TABLES.iteritems():
    try:
        print "Creating table {}: ".format(name),
        cursor.execute(ddl)
    except mysql.connector.Error as err:
        if err.errno == errorcode.ER_TABLE_EXISTS_ERROR:
            print("already exists.")
        else:
            print(err.msg)
    else:
        print("OK")

cursor.close()
cnx.close()
