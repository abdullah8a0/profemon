import sqlite3
import time
new_game_db = "/var/jail/home/team5/new_game.db"
def request_handler(request):
    if request["method"] == "POST":
        p1 = request["form"]["me"]
        p2 = request["form"]["them"]
        game_id = p1 + p2 + str(int(time.time()))
        conn = sqlite3.connect(new_game_db)
        c = conn.cursor()
        c.execute('''CREATE TABLE IF NOT EXISTS games (game_id text, player1 text, player2 text);''')
        c.execute('''INSERT into games VALUES (?,?,?);''', (game_id, p1, p2))
        conn.commit() 
        conn.close() 
