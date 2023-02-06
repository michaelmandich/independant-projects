import openpyxl
from openpyxl import Workbook
from datetime import datetime
from datetime import timedelta
# read the data from the excel spreadsheet
def Sort(sub_li):
    sub_li.sort(key = lambda x: x[1])
    return sub_li
data = openpyxl.load_workbook(filename = 'banddata.xlsx') # opens up predetermined file
data.iso_dates = True #get dates into format I want
sheet = data.active
x = 4 # starting line for reader to begin reading
thelist = [] # creates a list for the events
while x<37 : # end value where you want the reader to stop reading from
    artist = sheet['C'+str(x)].value # get artist starting
    date = sheet['D'+str(x)].value # get initial date value
    dt = datetime.strptime(str(date),"%Y-%m-%d %H:%M:%S") #getd date into format that can be >'d
    location = sheet['E'+str(x)].value # get location string
    jawn = (artist, dt, location) # put all 3 as an object for list
    thelist.append(jawn) # puts object in list
    x = x + 1 # iterating element because I'm not the best at Python.
thelist = Sort(thelist) # sorts the lsit in order of date, mixing up previously seperated bands to make for easier time sorting
print(thelist) # shows the list in all it's glory
