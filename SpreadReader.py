import openpyxl
from openpyxl import Workbook
from datetime import datetime
from datetime import timedelta
from dateutil import parser
# read the data from the excel spreadsheet
def Sort(sub_li):
    sub_li.sort(key = lambda x: x[1])
    return sub_li
data = openpyxl.load_workbook(filename = 'banddata.xlsx')
data.iso_dates = True
sheet = data.active
x = 4
thelist = []
while x<37 :
    artist = sheet['C'+str(x)].value
    date = sheet['D'+str(x)].value
    dt = datetime.strptime(str(date),"%Y-%m-%d %H:%M:%S")
    location = sheet['E'+str(x)].value
    jawn = (artist, dt, location)
    thelist.append(jawn)
    x = x + 1
thelist = Sort(thelist)
print(thelist)
