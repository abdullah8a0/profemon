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

def the_formula(atk,deff,power):
    return int((22 * power * atk / deff / 50 + 2) * random.randint(85,100) / 100)

def calculate_move_damage(move,player,opponent):
    conn2 = sqlite3.connect(profemon_db)
    c2 = conn2.cursor()
    stats = c2.execute('''SELECT atk, def, spatk, spdef FROM profemon WHERE name = ?;''', (player,)).fetchone()
    atk, deff, spatk, spdef = stats[0], stats[1], stats[2], stats[3]
    stats = c2.execute('''SELECT atk, def, spatk, spdef FROM profemon WHERE name = ?;''', (opponent,)).fetchone()
    atk2, deff2, spatk2, spdef2 = stats[0], stats[1], stats[2], stats[3]
    phy_or_spec, power, acc = c2.execute('''SELECT phy_or_spec, pow, acc FROM moves WHERE move_name = ?''', (move,)).fetchone()
    hit = random.randint(1,100);
    if hit > acc * 100:
        return 0
    damage = 0
    if phy_or_spec == 'physical':
        damage = the_formula(atk,deff2,power);
    else:
        damage = the_formula(spatk,spdef2,power);
    return damage


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
        # Here the names should be absolute, meaning that name1 is the profemon used by the first user that entered the room
        hps = c.execute('''SELECT hp1, hp2 FROM meta WHERE game_id = ?;''', (game_id,)).fetchone()
        now_hp1, now_hp2 = hps[0], hps[1]

        conn2 = sqlite3.connect(profemon_db)
        c2 = conn2.cursor()
        stats = c2.execute('''SELECT move1, move2, move3, move4, hp, atk, def, spatk, spdef, spd FROM profemon WHERE name = ?;''', (name1,)).fetchone()
        moves, hp = stats[:4], stats[4]
        atk, deff, spatk, spdef, spd = stats[5], stats[6], stats[7], stats[8], stats[9]
        stats = c2.execute('''SELECT hp, atk, def, spatk, spdef, spd FROM profemon WHERE name = ?;''', (name2,)).fetchone()
        hp2, atk2, deff2, spatk2, spdef2, spd2 = stats[0], stats[1], stats[2], stats[3], stats[4], stats[5]
        conn2.close()

        hp1_1, hp2_1 = now_hp1, now_hp2 # hp1_1, hp2_1 means the respective hp of profemon 1,2 after first_round
        display_text_1, display_text_2 = "",""

        if spd == spd2: # Tie breaker
            spd += random.randint(0,1)
        if spd > spd2: # Profemon one moves first
            damage = calculate_move_damage(move1, name1, name2)
            display_text_1 = f'{name1} used {move1}'
            if damage == 0:
                display_text_1 += '\nBut it missed!'
            hp2_1 -= damage
            if hp2_1 <= 0:
                display_text_2 = f'{name2} fainted!'
                hp2_1 = 0
        else: #Profemon two moves first
            damage = calculate_move_damage(move2, name2, name1)
            display_text_1 = f'{name2} used {move2}'
            if damage == 0:
                display_text_1 += '\nBut it missed!'
            hp1_1 -= damage
            if hp1_1 <= 0:
                hp1_1 = 0
                display_text_2 = f'{name1} fainted!'


        hp1_2, hp2_2 = hp1_1, hp2_1 # The final hp after both moves
        
        dic = {"move1":{"player_hp":hp1_1,"opponent_hp":hp2_1,"display_text":display_text_1},
        "move2":{"player_hp":hp1_2,"opponent_hp":hp2_2,"display_text":display_text_2}}

        if hp1_1 <= 0 or hp2_1 <= 0: # If first move kills 
            return dic
            
        if spd <= spd2: # Profemon one moves second
            damage = calculate_move_damage(move1, name1, name2)
            display_text_2 = f'{name1} used {move1}'
            if damage == 0:
                display_text_2 += '\nBut it missed!'
            hp2_2 -= damage
            if hp2_2 <= 0:
                hp2_2 = 0
                display_text_2 += f'\n{name2} fainted!'
        else: #Profemon two moves second
            damage = calculate_move_damage(move2, name2, name1)
            display_text_2 = f'{name2} used {move2}'
            if damage == 0:
                display_text_2 += '\nBut it missed!'
            hp1_2 -= damage
            if hp1_2 <= 0:
                hp1_2 = 0
                display_text_2 += f'\n{name1} fainted!'

        # Update the new hp
        c.execute('''UPDATE meta SET hp1 = ?, hp2 = ? WHERE game_id = ?''',(hp1_2,hp2_2,game_id))
        
        # Create the dictionary to be converted to json
        dic["move2"] = {"player_hp":hp1_2,"opponent_hp":hp2_2,"display_text":display_text_2}
        
        return dic


    try: # Joining the game
        if 'reset' in req['values']:
            reset = req['values']['reset']
            c.execute('''DROP TABLE IF EXISTS meta;''')
            c.execute('''DROP TABLE IF EXISTS move;''')
            for json_file in os.listdir(json_path):
                os.remove(os.path.join(json_path,json_file)) 
        if req['method'] == 'POST':
            user = req['values']['user']
            user = int(user)
            game_id = req['values']['game_id']
            if 'profemon' in req['values']:
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
            else: 
                #The user has passed in a move
                move = req['values']['move']

                data = c.execute('''SELECT user1, user2, name1, name2, hp1, hp2 FROM meta WHERE game_id = ?;''', (game_id,)).fetchone()
                user1, user2, name1, name2, now_hp1, now_hp2 = data

                conn2 = sqlite3.connect(profemon_db)
                c2 = conn2.cursor()
                stats = c2.execute('''SELECT move1, move2, move3, move4, hp, atk, def, spatk, spdef, spd FROM profemon WHERE name = ?;''', (name1,)).fetchone()
                moves, hp = stats[:4], stats[4]
                atk, deff, spatk, spdef, spd = stats[5:]
                stats = c2.execute('''SELECT hp, atk, def, spatk, spdef, spd FROM profemon WHERE name = ?;''', (name2,)).fetchone()
                hp2, atk2, deff2, spatk2, spdef2, spd2 = stats
                conn2.close()

                # Check if the opponent has posted any move in the MOVE table
                c.execute('''CREATE TABLE IF NOT EXISTS move (game_id int, move1 text, move2 text)''')
                move1 = c.execute('''SELECT move1 FROM move WHERE game_id = ?;''', (game_id,)).fetchone()
                move2 = c.execute('''SELECT move2 FROM move WHERE game_id = ?;''', (game_id,)).fetchone()

                if move1 != None or move2 != None: # Then the opponent already passed in a move
                    # We generate the json file
                    if move1[0] == 'n/a':
                        move1,move2 = move,move2[0]
                    else:
                        move2,move1 = move,move1[0]
                    dic = battle_res(name1,name2,move1,move2,c,game_id) 
                    json1_path = os.path.join(json_path,f'{game_id}-{user1}-1.json') #user1-centric
                    json2_path = os.path.join(json_path,f'{game_id}-{user2}-2.json') #user2-centric
                    with open(json1_path, 'w') as outfile:
                        json.dump(dic, outfile)
                    with open(json2_path, 'w') as outfile:
                        dic['move1']['player_hp'], dic['move1']['opponent_hp'] = dic['move1']['opponent_hp'], dic['move1']['player_hp']
                        dic['move2']['player_hp'], dic['move2']['opponent_hp'] = dic['move2']['opponent_hp'], dic['move2']['player_hp']
                        json.dump(dic, outfile)
                    response = "Both players moved! Calculating the result..."
                    c.execute('''DELETE FROM move WHERE game_id = ?;''', (game_id,))
                    if dic['move2']['player_hp'] <= 0 or dic['move2']['opponent_hp'] <= 0:
                        c.execute('''DELETE FROM meta WHERE game_id = ?;''', (game_id,))
                
                else:
                    c.execute('''INSERT INTO move VALUES (?, ?, ?);''', (game_id, 'n/a' if user != user1 else move, 'n/a' if user != user2 else move))
                    response = "Still waiting for the opponent to move"


        elif req['method'] == 'GET':
            user = req['values']['user']
            user = int(user)
            game_id = req['values']['game_id']
            if 'init' not in req['values']: #So people are just asking for updates
                json1_path = os.path.join(json_path,f'{game_id}-{user}-1.json') #user1-centric
                json2_path = os.path.join(json_path,f'{game_id}-{user}-2.json') #user2-centric

                if os.path.exists(json1_path):
                    with open(json1_path) as json_file:
                        data = json.load(json_file)
                        response = str(data)
                    os.remove(json1_path) # burn after read
                elif os.path.exists(json2_path):
                    with open(json2_path) as json_file:
                        data = json.load(json_file)
                        response = str(data)
                    os.remove(json2_path)
                else:
                    response = "Still waitin for the oppo to move."

            else: #So people are asking for the image
                data = c.execute('''SELECT user1, user2, name1, name2, hp1, hp2 FROM meta WHERE game_id = ?;''', (game_id,)).fetchone()
                user1, user2, name1, name2, now_hp1, now_hp2 = data[0], data[1], data[2], data[3], data[4], data[5]
                status = get_game_status(game_id)
                if user == user2: # Swap
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
                    dic = {"player":{"len":len1,"image":image1,"moves":[move1,move2,move3,move4],"max_hp":hp}, "opponent":{"len":len2,"image":image2,"max_hp":hp2}}
                    json_res = json.dumps(dic)
                    response = str(json_res)
                    if 'len' in req['values']:
                        length = req['values']['len'] #So people are asking for the length of the json file 
                        response = len(response)
        
    except Exception as e:
        return traceback.format_exc() 

    conn.commit()
    conn.close()
    return response
