import sqlite3
import time

battle_db = "/var/jail/home/team5/battle.db"
def request_handler(request):
    if request["method"] == "POST":
        p = request["form"]["me"]
        try:
            int(p)
        except:
            return "Error: Invalid input, p1 must be integers"
        # DB battle.bd TABLE meta, (game_id int, user1 text, name1 text, user2 text, name2 text,hp1 int, hp2 int)
        # find game_id corresponting to p
        conn = sqlite3.connect(battle_db)
        c = conn.cursor()
        c.execute("SELECT game_id FROM meta WHERE user1=?", (p,))
        game_id = c.fetchone()
        if game_id is None:
            c.execute("SELECT game_id FROM meta WHERE user2=?", (p,))
            game_id = c.fetchone()
        if game_id is None:
            return "Error: Invalid input, p1 not found"
        game_id = game_id[0] 
        conn.commit()
        conn.close()
        return str(game_id)

def generate_ids(p1,p2):
    # game id = p1 + p2 + time
    return str(p1) + str(p2) + str(time.time())