import sqlite3
import json
import traceback
import os
from PIL import Image
import io
import base64
import json
import random
battle_db = "/var/jail/home/team5/battle.db"
profemon_db = "/var/jail/home/team5/profemon.db"
dir_path = '/var/jail/home/team5/images'
json_path = '/var/jail/home/team5/jsons'

def jpg2bin(jpg_file, width=96, height=120):
    img = Image.open(jpg_file)
    img = img.resize((width, height))
    output = io.BytesIO()
    img.save(output, format='JPEG')
    return output.getvalue()

def jpg2bin_small(jpg_file, width=32, height=40):
    img = Image.open(jpg_file)
    img = img.resize((width, height))
    output = io.BytesIO()
    img.save(output, format='JPEG')
    return output.getvalue()

def get_profemon_image(name, size = 'big'):
    file_name = '{}.jpg'.format(name)
    file_path = os.path.join(dir_path,file_name)
    binary = jpg2bin(file_path)
    if size == 'small':
        binary = jpg2bin_small(file_path)
    encodedbin = base64.standard_b64encode(binary)
    encodedbin = encodedbin.decode('ascii')
    return len(binary), encodedbin

def calculate_move_damage(move):
    return random.randint(10,40)


def request_handler(req):
    response = "No response."
    conn = sqlite3.connect(battle_db)
    c = conn.cursor()

    def get_game_status(game_id): #Check if people have joined the game
        c.execute('''SELECT user2 FROM meta WHERE game_id = ?;''', (game_id,))
        game = c.execute('SELECT user2 FROM meta WHERE game_id = ?;', (game_id,)).fetchone()
        if game == None: # Then this player is the first to join this room
            return 0
        elif game[0] == 'n/a': # Then this player is the second to join this room
            return 1
        else:
            return 2


    def battle_res(name1, name2, move1, move2, c, game_id): #Calculate the result of the battle
        data = c.execute('''SELECT user1, user2, name1, name2, hp1, hp2 FROM meta WHERE game_id = ?;''', (game_id,)).fetchone()
        user1, user2, name1, name2, now_hp1, now_hp2 = data[0], data[1], data[2], data[3], data[4], data[5]

        conn2 = sqlite3.connect(profemon_db)
        c2 = conn2.cursor()
        stats = c2.execute('''SELECT move1, move2, move3, move4, hp, atk, def, spatk, spdef, spd FROM profemon WHERE name = ?;''', (name1,)).fetchone()
        move1, move2, move3, move4, hp = stats[0], stats[1], stats[2], stats[3], stats[4]
        atk, deff, spatk, spdef, spd = stats[5], stats[6], stats[7], stats[8], stats[9]
        stats = c2.execute('''SELECT hp, atk, def, spatk, spdef, spd FROM profemon WHERE name = ?;''', (name2,)).fetchone()
        hp2, atk2, deff2, spatk2, spdef2, spd2 = stats[0], stats[1], stats[2], stats[3], stats[4], stats[5]

        hp1_1, hp2_1 = now_hp1, now_hp2 # hp1_1, hp2_1 means the respective hp of profemon 1,2 after first_round
        display_text_1, displayer_text_2 = "",""

        if spd == spd2: # Tie breaker
            spd += random.randint(0,1)
        if spd > spd2: # Profemon one moves first
            hp2_1 -= calculate_move_damage(move1)
            display_text_1 = {f'{name1} used {move1}'}
        else: #Profemon two moves first
            hp1_1 -= calculate_move_damage(move2)
            display_text_1 = {f'{name2} used {move2}'}

        hp1_2, hp2_2 = hp1_1, hp2_1 # The final hp after both moves
        if spd <= spd2: # Profemon one moves second
            hp2_2 -= calculate_move_damage(move1)
            display_text_2 = {f'{name1} used {move1}'}
        else: #Profemon two moves second
            hp1_2 -= calculate_move_damage(move2)
            display_text_2 = {f'{name2} used {move2}'}
        dic = {"move1":{"player_hp":hp1_1,"opponent_hp":hp2_1,"display_text":display_text_1},
        "move2":{"player_hp":hp1_2,"opponent_hp":hp2_2,"display_text":display_text_2}}
        
        return dic


    try:
        reset = req['values']['reset']
        c.execute('''DROP TABLE meta;''')
        c.execute('''DROP TABLE battle;''')
    except:
        pass
    if req['method'] == 'POST':
        try:
            delete = req['values']['delete']
            game_id = req['values']['game_id']
            if delete == 'true':
                c.execute('''DELETE FROM meta WHERE game_id = ?;''', (game_id,))
                conn.commit()
                conn.close()
                return f"Game {game_id} is deleted"
        except:
            pass
        try: # Joining the game
            user = req['values']['user']
            user = int(user)
            game_id = req['values']['game_id']
            name = req['values']['profemon']
            c.execute('CREATE TABLE IF NOT EXISTS meta (game_id int, user1 int, name1 text, user2 int, name2 text,hp1 int, hp2 int);')
            status = get_game_status(game_id)
            conn2 = sqlite3.connect(profemon_db)
            c2 = conn2.cursor()
            hp, = c2.execute('SELECT hp FROM profemon WHERE name = ?', (name, )).fetchone()
            conn2.close()
            if status == 0:
                c.execute('''INSERT INTO meta VALUES (?,?,?,'n/a','n/a',?,0);''', (game_id, user, name, hp,))
                response = f"{user} successfully joined game {game_id} with profemon {name}."
            elif status == 1:
                c.execute('''UPDATE meta SET user2 = ?, name2 = ?, hp2 = ? WHERE game_id = ?;''', (user, name, hp, game_id, ))
                response = f"{user} successfully joined game {game_id} with profemon {name}. The game is ready."
            elif status == 2:
                response = "Can't join an ongoing game!"
        #except ValueError or KeyError: #Why the heck is this KeyError not caught
        except Exception as e:
            pass
        try:
            #The user has passed in a move
            move = req['values']['move']

            data = c.execute('''SELECT user1, user2, name1, name2, hp1, hp2 FROM meta WHERE game_id = ?;''', (game_id,)).fetchone()
            user1, user2, name1, name2, now_hp1, now_hp2 = data[0], data[1], data[2], data[3], data[4], data[5]

            conn2 = sqlite3.connect(profemon_db)
            c2 = conn2.cursor()
            stats = c2.execute('''SELECT move1, move2, move3, move4, hp, atk, def, spatk, spdef, spd FROM profemon WHERE name = ?;''', (name1,)).fetchone()
            move1, move2, move3, move4, hp = stats[0], stats[1], stats[2], stats[3], stats[4]
            atk, deff, spatk, spdef, spd = stats[5], stats[6], stats[7], stats[8], stats[9]
            stats = c2.execute('''SELECT hp, atk, def, spatk, spdef, spd FROM profemon WHERE name = ?;''', (name2,)).fetchone()
            hp2, atk2, deff2, spatk2, spdef2, spd2 = stats[0], stats[1], stats[2], stats[3], stats[4], stats[5]
            conn2.close()

            # Check if the opponent has posted any move in the MOVE table
            c.execute('''CREATE TABLE IF NOT EXISTS move (game_id int, move1 text, move2 text)''')
            move_1 = c.execute('''SELECT move1 FROM move WHERE game_id = ?;''', (game_id,))
            move_2 = c.execute('''SELECT move2 FROM move WHERE game_id = ?;''', (game_id,))

            if move1 != None or move2 != None: # Then the opponent already passed in a move
                # We generate the json file
                if move1 != None:
                    move1 = move
                else:
                    move2 = move
                dic = battle_res(name1,name2,move1,move2,c,game_id) if user == user1 else battle_res(name2,name1,move2,move1,c,game_id)
                json1_path = os.path.join(json_path,f'{game_id}1.json') #user1-centric
                json2_path = os.path.join(json_path,f'{game_id}2.json') #user2-centric
                with open(json1_path, 'w') as outfile:
                    json.dump(dic, outfile)
                with open(json2_path, 'w') as outfile:
                    dic['move1']['player_hp'], dic['move1']['opponent_hp'] = dic['move1']['opponent_hp'], dic['move1']['player_hp']
                    dic['move2']['player_hp'], dic['move2']['opponent_hp'] = dic['move2']['opponent_hp'], dic['move2']['player_hp']
                    json.dump(dic, outfile)
                response = "Both players moved! Calculating the result..."
            
            else:
                c.execute('''INSERT INTO move VALUES (?, ?, ?));''', (game_id, 'n/a' if user != user1 else move, 'n/a' if user != user2 else move))
                response = "Still waiting for the opponent to move"

        except Exception as e:
            return traceback.format_exc() 
    elif req['method'] == 'GET':
#        try:
#            meta = req['values']['meta']
#            data = c.execute('''SELECT)
        try:
            user = req['values']['user']
            user = int(user)
            game_id = req['values']['game_id']
            data = c.execute('''SELECT user1, user2, name1, name2, hp1, hp2 FROM meta WHERE game_id = ?;''', (game_id,)).fetchone()
            user1, user2, name1, name2, now_hp1, now_hp2 = data[0], data[1], data[2], data[3], data[4], data[5]
            if 'init' in req['values']: #So people are asking for the image
                status = get_game_status(game_id)
                if user == user2:
                    user1, user2, name1, name2, now_hp1, now_hp2 = user2, user1, name2, name1, now_hp2, now_hp1
                if status == 0:
                    response = "Game doesn't exist"
                elif status == 1:
                    response = "wait" 
                elif status == 2:
                    conn2 = sqlite3.connect(profemon_db)
                    c2 = conn2.cursor()
                    stats = c2.execute('''SELECT move1, move2, move3, move4, hp, atk, def, spatk, spdef, spd FROM profemon WHERE name = ?;''', (name1,)).fetchone()
                    move1, move2, move3, move4, hp = stats[0], stats[1], stats[2], stats[3], stats[4]
                    atk, deff, spatk, spdef, spd = stats[5], stats[6], stats[7], stats[8], stats[9]
                    stats = c2.execute('''SELECT hp, atk, def, spatk, spdef, spd FROM profemon WHERE name = ?;''', (name2,)).fetchone()
                    hp2, atk2, deff2, spatk2, spdef2, spd2 = stats[0], stats[1], stats[2], stats[3], stats[4], stats[5]
                    conn2.close()
                    len1, image1 = get_profemon_image(name1,size = 'small')
                    len2, image2 = get_profemon_image(name2,size = 'small')
                    #move1, move2, move3, move4 = "Astonish", "Confuse", "Double Team", "Hypnosis"
                    dic = {"player":{"len":len1,"image":image1,"moves":[move1,move2,move3,move4],"max_hp":hp}, "opponent":{"len":len2,"image":image2,"max_hp":hp}}
                    json_res = json.dumps(dic)
                    response = str(json_res)
                    try:
                        length = req['values']['len']
                        response = len(response)
                    except:
                        pass
            else: # They are just asking for updates about battle status
                json1_path = os.path.join(json_path,f'{game_id}1.json') #user1-centric
                json2_path = os.path.join(json_path,f'{game_id}2.json') #user2-centric

                if user == user1 and os.path.exists(json1_path):
                    with open(json1_path) as json_file:
                        data = json.load(json_file)
                        response = str(data)
                    os.remove(json1_path)
                if user == user2 and os.path.exists(json2_path):
                    with open(json2_path) as json_file:
                        data = json.load(json_file)
                        response = str(data)
                    os.remove(json2_path)
        except Exception as e:
            return traceback.format_exc() 
    conn.commit()
    conn.close()
    return response
