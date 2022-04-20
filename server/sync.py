import sqlite3
new_game_db = "/var/jail/home/team5/new_game.db"

def request_handler(request):
    if request["method"] == "POST":
        p1 = request["form"]["me"]
        conn = sqlite3.connect(new_game_db)
        c = conn.cursor()
        c.execute('''CREATE TABLE IF NOT EXISTS games (game_id text, player1 text, player2 text);''')

        # find the game id corresponding to the player

        c.execute('''SELECT game_id FROM games WHERE player1 = ? OR player2 = ?;''', (p1,p1))
        game_id = c.fetchone()
        conn.commit()
        conn.close()
        if game_id is None:
            # return game_id
            return "You are not in a game"
        else:
            return game_id[0]
