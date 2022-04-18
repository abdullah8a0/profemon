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

def request_handler(req):
    response = "No response."
    conn = sqlite3.connect(battle_db)
    c = conn.cursor()
    def get_game_status(game_id):
        c.execute('''SELECT user2 FROM meta WHERE game_id = ?;''', (game_id,))
        game = c.execute('SELECT user2 FROM meta WHERE game_id = ?;', (game_id,)).fetchone()
        if game == None: # Then this player is the first to join this room
            return 0
        elif game[0] == 'n/a': # Then this player is the second to join this room
            return 1
        else:
            return 2
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
        try:
            user = req['values']['user']
            game_id = req['values']['game_id']
            name = req['values']['profemon']
            c.execute('CREATE TABLE IF NOT EXISTS meta (game_id int, user1 text, name1 text, user2 text, name2 text,hp1 int, hp2 int);')
            status = get_game_status(game_id)
            conn2 = sqlite3.connect(profemon_db)
            c2 = conn2.cursor()
            hp, = c2.execute('SELECT hp FROM profemon WHERE name = ?', (name, )).fetchone()
            conn2.close()
            if status == 0:
                c.execute('''INSERT INTO meta VALUES (?,?,?,'n/a','n/a',?,0);''', (game_id, user, name, hp + 100))
                response = user + " successfully joined game " + game_id + ' with profemon ' + name + '.'
            elif status == 1:
                c.execute('''UPDATE meta SET user2 = ?, name2 = ?, hp2 = ? WHERE game_id = ?;''', (user, name, hp + 100, game_id, ))
                response = user + " successfully joined game " + game_id + ' with profemon ' + name + ', the game is ready.'
            elif status == 2:
                response = "Can't join an ongoing game!"
        except ValueError or KeyError:
            return "Invalid POST format"
        except Exception as e:
            return e
    elif req['method'] == 'GET':
        try:
            user = req['values']['user']
            game_id = req['values']['game_id']
            data = c.execute('''SELECT user1, user2, name1, name2, hp1, hp2 FROM meta WHERE game_id = ?;''', (game_id,)).fetchone()
            user1, user2, name1, name2, now_hp1, now_hp2 = data[0], data[1], data[2], data[3], data[4], data[5]
            if user == user2:
                user1, user2, name1, name2, now_hp1, now_hp2 = user2, user1, name2, name1, now_hp2, now_hp1
            if 'init' in req['values']:
                status = get_game_status(game_id)
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
                    move1, move2, move3, move4 = "Astonish", "Confuse", "Double Team", "Hypnosis"
                    dic = {"player":{"len":len1,"image":image1,"moves":[move1,move2,move3,move4],"max_hp":hp}, "opponent":{"len":len2,"image":image2,"max_hp":hp}}
                    json_res = json.dumps(dic)
                    response = str(json_res)
                    try:
                        length = req['values']['len']
                        response = len(response)
                    except:
                        pass
            #If it is 
            else:
                conn2 = sqlite3.connect(profemon_db)
                c2 = conn2.cursor()
                stats = c2.execute('''SELECT move1, move2, move3, move4, hp, atk, def, spatk, spdef, spd FROM profemon WHERE name = ?;''', (name1,)).fetchone()
                move1, move2, move3, move4, hp = stats[0], stats[1], stats[2], stats[3], stats[4]
                atk, deff, spatk, spdef, spd = stats[5], stats[6], stats[7], stats[8], stats[9]
                stats = c2.execute('''SELECT hp, atk, def, spatk, spdef, spd FROM profemon WHERE name = ?;''', (name2,)).fetchone()
                hp2, atk2, deff2, spatk2, spdef2, spd2 = stats[0], stats[1], stats[2], stats[3], stats[4], stats[5]
                conn2.close()
                c.execute('''CREATE TABLE IF NOT EXISTS battle (game_id int, user text, move text);''')
                move = req['values']['move']
                opponent_move = c.execute('''SELECT move FROM battle WHERE game_id = ? AND user = ?;''', (game_id, user2,)).fetchone()
                if opponent_move != None:
                    first_hp1, first_hp2, first_text = now_hp1, now_hp2, " used "
                    second_hp1, second_hp2, second_text = now_hp1, now_hp2, " used "
                    
                    if spd == spd2:
                        spd2 += random.randint(0,1)
                    if spd >= spd2:
                        first_hp2 -= random.randint(10,40) #CHANGEME AFTER WE GOT THE MOVE DICTIONARY
                        first_text = name1 + first_text + move
                        second_hp2 = first_hp2
                        second_hp1 -= random.randint(10,40)
                        second_text = name2 + second_text + opponent_move[0]
                    else: 
                        first_hp1 -= random.randint(10,40) #CHANGEME AFTER WE GOT THE MOVE DICTIONARY
                        first_text = name2 + first_text + opponent_move[0]
                        second_hp1 = first_hp1
                        second_hp2 -= random.randint(10,40)
                        second_text = name1 + second_text + move 
                    json_list = [{"player_hp":first_hp1,"opponent_hp":first_hp2,"display_text":first_text}
                    ,{"player_hp":second_hp1,"opponent_hp":second_hp2,"display_text":second_text} ]
                    json_list = json.dumps(json_list)
                    c.execute('''DELETE FROM battle WHERE game_id = ?;''', (game_id, ))
                    response = str(json_list)
                else:
                    c.execute('INSERT INTO battle VALUES (?,?,?);', (game_id, user, move))

        except Exception as e:
            return traceback.format_exc() 
    conn.commit()
    conn.close()
    return response
