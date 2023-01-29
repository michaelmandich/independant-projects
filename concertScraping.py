import csv
import requests
from bs4 import BeautifulSoup

# list of bands to search
bands = ["Goose","Dead and Company","Pigeons Playing Ping Pong"]

# csv file to write to
csv_file = open('concerts.csv', 'w', newline='')
csv_writer = csv.writer(csv_file)
csv_writer.writerow(['Band','Location','Date'])

# loop through each band
for band in bands:

    # Ticketmaster search
    url = 'https://www.ticketmaster.com/search?q='+band
    response = requests.get(url)
    soup = BeautifulSoup(response.text, 'lxml')

    # get all shows
    shows = soup.find_all('div', class_='event-listing-container')
    for show in shows:
        # get event name
        event_name = show.find('div', class_='event-listing-info').find('div', class_='event-title').text.strip()
        # get location
        location = show.find('div', class_='event-listing-info').find('div', class_='event-venue').text.strip()
        # get date
        date = show.find('div', class_='event-listing-info').find('div', class_='event-date').text.strip()
        # write to csv
        csv_writer.writerow([band, location, date])
   
    # SeatGeek search
    url = 'https://seatgeek.com/search?search='+band
    response = requests.get(url)
    soup = BeautifulSoup(response.text, 'lxml')

    # get all shows
    shows = soup.find_all('div', class_='event-listing-container')
    for show in shows:
        # get event name
        event_name = show.find('div', class_='event-listing-info').find('div', class_='event-title').text.strip()
        # get location
        location = show.find('div', class_='event-listing-info').find('div', class_='event-venue').text.strip()
        # get date
        date = show.find('div', class_='event-listing-info').find('div', class_='event-date').text.strip()
        # write to csv
        csv_writer.writerow([band, location, date])
    
    # vividseats.com search
    url = 'https://www.vividseats.com/concerts/'+band.replace(' ','-')+'-tickets.html'
    response = requests.get(url)
    soup = BeautifulSoup(response.text, 'lxml')

    # get all shows
    shows = soup.find_all('div', class_='event-listing-container')
    for show in shows:
        # get event name
        event_name = show.find('div', class_='event-listing-info').find('div', class_='event-title').text.strip()
        # get location
        location = show.find('div', class_='event-listing-info').find('div', class_='event-venue').text.strip()
        # get date
        date = show.find('div', class_='event-listing-info').find('div', class_='event-date').text.strip()
        # write to csv
        csv_writer.writerow([band, location, date])

    # bands' websites search
    url = 'http://www.'+band.replace(' ','')+'.com'
    response = requests.get(url)
    soup = BeautifulSoup(response.text, 'lxml')

    # get all shows
    shows = soup.find_all('div', class_='event-listing-container')
    for show in shows:
        # get event name
        event_name = show.find('div', class_='event-listing-info').find('div', class_='event-title').text.strip()
        # get location
        location = show.find('div', class_='event-listing-info').find('div', class_='event-venue').text.strip()
        # get date
        date = show.find('div', class_='event-listing-info').find('div', class_='event-date').text.strip()
        # write to csv
        csv_writer.writerow([band, location, date])

# close csv file
csv_file.close()
