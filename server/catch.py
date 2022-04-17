import sqlite3
import datetime
def bounding_box(point_coord,box):
    pass

def translate(point, origin):
    return (point[0] - origin[0], point[1] - origin[1])
def intersect(p0, p1,p2):
    x1, x2, y1, y2 = p1[0] - p0[0], p2[0] - p0[0], p1[1] - p0[1], p2[1] - p0[1]
    if y1 * y2 >= 0:
        print("False")
        return False
    x = (x1 * y2 - y1 * x2) / (y2 - y1)
    print(bool(x > 0))
    return bool(x > 0)

def within_area(point_coord,poly):
    cnt = 0
    for i in range(len(poly)):
        if intersect(point_coord, poly[i], poly[(i + 1) % len(poly)]):
            cnt += 1
    return bool(cnt % 2);
profemon_db = "/var/jail/home/team5/profemon.db"
locations={
    "Student Center":[(-71.095863,42.357307),(-71.097730,42.359075),(-71.095102,42.360295),(-71.093900,42.359340),(-71.093289,42.358306)],
    "Dorm Row":[(-71.093117,42.358147),(-71.092559,42.357069),(-71.102987,42.353866),(-71.106292,42.353517)],
    "Simmons/Briggs":[(-71.097859,42.359035),(-71.095928,42.357243),(-71.106356,42.353580),(-71.108159,42.354468)],
    "Boston FSILG (West)":[(-71.124664,42.353342),(-71.125737,42.344906),(-71.092478,42.348014),(-71.092607,42.350266)],
    "Boston FSILG (East)":[(-71.092409,42.351392),(-71.090842,42.343589),(-71.080478,42.350900),(-71.081766,42.353771)],
    "Stata/North Court":[(-71.091636,42.361802),(-71.090950,42.360811),(-71.088353,42.361112),(-71.088267,42.362476),(-71.089769,42.362618)],
    "East Campus":[(-71.089426,42.358306),(-71.090885,42.360716),(-71.088310,42.361017),(-71.087130,42.359162)],
    "Vassar Academic Buildings":[(-71.094973,42.360359),(-71.091776,42.361770),(-71.090928,42.360636),(-71.094040,42.359574)],
    "Infinite Corridor/Killian":[(-71.093932,42.359542),(-71.092259,42.357180),(-71.089619,42.358274),(-71.090928,42.360541)],
    "Kendall Square":[(-71.088117,42.364188),(-71.088225,42.361112),(-71.082774,42.362032)],
    "Sloan/Media Lab":[(-71.088203,42.361017),(-71.087044,42.359178),(-71.080071,42.361619),(-71.082796,42.361905)],
    "North Campus":[(-71.11022,42.355325),(-71.101280,42.363934),(-71.089950,42.362666),(-71.108361,42.354484)],
    "Technology Square":[(-71.093610,42.363157),(-71.092130,42.365837),(-71.088182,42.364188),(-71.088267,42.362650)]
}


def sign(x):
    if x > 0:
        return 1
    elif x == 0:
        return 0
    else:
        return -1

def get_area(point_coord,locations):
    for place in locations.keys():
        if(within_area(point_coord,locations[place])):
            return place
    return "Off Campus"


def request_handler(request):
    if request["method"] == "GET": # Not changed. Please don't use GET request.
        try:
            erase = request['values']['erase']
            conn = sqlite3.connect(profemon_db)
            c = conn.cursor()
            c.execute('''DELETE FROM catch WHERE user = ?;''', (erase,))
            c.execute('''DELETE FROM catch WHERE cipher = ?;''', (erase,))
            conn.commit()
            conn.close()
            return "Deleted " + erase
        except Exception as e:
            pass
        try:
            conn = sqlite3.connect(profemon_db)
            c = conn.cursor()
            #c.execute('''DELETE FROM catch WHERE profemon_name = 'AndiLiuTheLegendary' OR profemon_name = 'AndiLiu';''')
            c.execute('''CREATE TABLE IF NOT EXISTS catch (user text, cipher text, lat float, lon float, time timestamp, profemon_name text);''')
            database = c.execute('''SELECT * FROM catch;''').fetchall()
            conn.commit()
            conn.close()
            return database
        except Exception as e:
            return e

        return get_area([lon,lat],locations)
    else: # POST a form
        try:
            lat = float(request['form']['lat'])
            lon = float(request['form']['lon'])
            user = request['form']['user']
            cipher = request['form']['cipher']
        
            #First check if the scanned professor exists in our database
            conn = sqlite3.connect(profemon_db)
            c = conn.cursor()
            c.execute('''CREATE TABLE IF NOT EXISTS profemon (name text, cipher text, hp int, atk int, def int, spatk int, spdef int, spd int, move1 int, move2 int, move3 int, move4 int);''')
            name = c.execute('''SELECT name FROM profemon WHERE cipher = ?;''', (cipher,)).fetchone()
            if name == None:
                return "Profemon doesn't exist"
            name = name[0]
        
            #If the profemon does exist in our database, check if it was already caught by the user
            c.execute('''CREATE TABLE IF NOT EXISTS catch (user text, cipher text, lat float, lon float, time timestamp, profemon_name text);''')
            past = c.execute('''SELECT * FROM catch WHERE user = ? AND cipher = ?;''',(user, cipher, )).fetchone()
            if past != None:
                return "Already caught " + name + " before!"

            #If not caught by this user, then insert this into our record
            c.execute('''INSERT into catch VALUES (?,?,?,?,?,?);''', (user,cipher,lat,lon,datetime.datetime.now(),name))
            conn.commit() 
            conn.close() 
            return "Successfully captured " + name + "!"
        except Exception as e:
            return e
