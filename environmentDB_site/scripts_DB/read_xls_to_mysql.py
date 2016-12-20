import xlrd, os
import mysql.connector

# read data from .xls spreadsheet, store them in "book".
book= xlrd.open_workbook("AprilMay2015env_samples.xlsx")

cnx = mysql.connector.connect(user='root', 
                              host='127.0.0.1',
                              database='environmentDB')
cursor = cnx.cursor()

# read data from spreadsheet, insert them into database.
for sheet in book.sheets():
    print sheet.name
    for r in range(1, sheet.nrows):
        department = sheet.cell(r,0).value
        location = sheet.cell(r,1).value
        sink = sheet.cell(r,2).value
        date = sheet.name
        drain_swab_chromagar = sheet.cell(r,4).value
        indirect_pos_neg = sheet.cell(r,5).value
        organism_ID_swab = sheet.cell(r,6).value
        CAV_drain_swab = sheet.cell(r,7).value
        
       # if sheet.cell(r,8) != '':
        pTrap_water_chromagar = sheet.cell(r,8).value
        indirect_pos_neg_drain = sheet.cell(r,9).value
        organism_ID_water = sheet.cell(r,10).value
        CAV_pTrap_water = sheet.cell(r,11).value
        comments = sheet.cell(r,12).value

        # Assign values from each row
        values = (department, location, sink, date, drain_swab_chromagar,
                  indirect_pos_neg, organism_ID_swab, CAV_drain_swab,
                  pTrap_water_chromagar, indirect_pos_neg_drain,
                  organism_ID_water, CAV_pTrap_water, comments)
        #pTrap_values = (department, location, sink, date, pTrap_water_chromagar, indirect_pos_neg_drain, organism_ID_water, CAV_pTrap_water, comments)

        # Execute sql Query
        if drain_swab_chromagar:
            
            query = ("INSERT INTO index_table "
               "(department, location, sink, date, drain_swab_chromagar, \
                     indirect_pos_neg, organism_ID_swab, CAV_drain_swab, \
                     pTrap_water_chromagar, indirect_pos_neg_drain, \
                     organism_ID_water, CAV_pTrap_water, comments) "
               "VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s)")
            cursor.execute(query,values)

        cnx.commit()
'''
        if pTrap_water_chromagar:

            pTrap_query = ("INSERT INTO pTrap "
               "(department, location, sink, date, pTrap_water_chromagar, indirect_pos_neg_drain, organism_ID_water, CAV_pTrap_water, comments) "
               "VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s)")
            cursor.execute(pTrap_query,pTrap_values) 
'''
        
    
 

cursor.close()
cnx.close()
