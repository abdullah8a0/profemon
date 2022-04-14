import sqlite3
import datetime
profemon_db = "/var/jail/home/team5/profemon.db"
def request_handler(request):
    if request["method"] == "POST":
        try:
            user = request['form']['name']
            cipher = request['form']['cipher']
            hp, atk, deff, spatk, spdef, spd, move1, move2, move3, move4 = 0,0,0,0,0,0,1,2,3,4
            try:
                hp = request['form']['hp']
                atk = request['form']['atk']
                deff = request['form']['def']
                spatk = request['form']['spatk']
                spdef = request['form']['spdef']
                spd = request['form']['spd']
            except:
                pass
            try:
                move1 = request['form']['move1']
                move2 = request['form']['move2']
                move3 = request['form']['move3']
                move4 = request['form']['move4']
            except:
                pass
            #First check if the scanned professor exists in our database
            conn = sqlite3.connect(profemon_db)
            c = conn.cursor()
            c.execute('''CREATE TABLE IF NOT EXISTS profemon (name text, cipher text, hp int, atk int, def int, spatk int, spdef int, spd int, move1 int, move2 int, move3 int, move4 int);''')
            if user == 'delete':
                c.execute('''DELETE FROM profemon WHERE cipher = (?);''', (cipher,))
            else:
                c.execute('''INSERT into profemon VALUES (?,?,?,?,?,?,?,?,?,?,?,?);''', (user,cipher,hp,atk,deff,spatk,spdef,spd,move1,move2,move3,move4))
            conn.commit() 
            conn.close() 
            if user == 'delete':
                return "Successfully deleted!"
            return "Successfully created!"
        except Exception as e:
            return e
    else:
        # GET request with single UID passed in -> retrieve associated profemon
        try:
            cipher = request['values']['cipher']
            conn = sqlite3.connect(profemon_db)
            c = conn.cursor()
            c.execute('''CREATE TABLE IF NOT EXISTS profemon (name text, cipher text, hp int, atk int, def int, spatk int, spdef int, spd int, move1 int, move2 int, move3 int, move4 int);''')
            profemon = c.execute('''SELECT name FROM profemon WHERE cipher = ?;''', (cipher,)).fetchone()
            conn.close()
            return profemon[0]
        except:
            pass
        # GET request with nothing passed in -> retrieve all profemon
        try:
            conn = sqlite3.connect(profemon_db)
            c = conn.cursor()
            c.execute('''CREATE TABLE IF NOT EXISTS profemon (name text, cipher text, hp int, atk int, def int, spatk int, spdef int, spd int, move1 int, move2 int, move3 int, move4 int);''')
            profedex = c.execute('''SELECT name, cipher FROM profemon;''').fetchall()
            conn.close()
            name_string = ''
            for i in profedex:
                name_string += i[0] + ': ' + i[1] + '\n'
            return name_string
        except Exception as e:
            return e
