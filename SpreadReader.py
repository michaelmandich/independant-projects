import openpyxl
from openpyxl import Workbook

# read the data from the excel spreadsheet 
data = openpyxl.load_workbook(filename = 'banddata.xlsx')
print(data.sheetnames)
data.iso_dates = True
sheet = data.active
print (sheet['C4'].value)
x = 3
thelist = []
while x<37 :
    artist = sheet['C'+str(x)].value
    date = sheet['D'+str(x)].value
    print(date)
    location = sheet['E'+str(x)].value
    jawn = (artist, date, location)
    thelist.append(jawn)
    x = x + 1
    

# loop through each row in the data
# print(thelist)
# print the list of objects 
