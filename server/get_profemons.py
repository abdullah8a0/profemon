from PIL import Image
import io
import os
import json
import sqlite3
import base64

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

def get_profemon_name_image_trio(uid,c):
    name = c.execute("SELECT name FROM profemon WHERE cipher = ?;",(uid,)).fetchone()
    name = name[0]
    #dir_path = os.path.dirname(os.path.realpath(__file__))
    
    file_name = '{}.jpg'.format(name)
    file_path = os.path.join(dir_path, file_name)
    #smallbin = jpg2bin_small(file_path)
    bigbin = jpg2bin(file_path)
    encodedbin = base64.standard_b64encode(bigbin)
    encodedbin = encodedbin.decode('ascii')
    return {'len': len(bigbin), 'name': name, 'image': encodedbin}

def request_handler(request):
    try:
        user = request['values']['user']
        conn = sqlite3.connect(profemon_db)
        c = conn.cursor()
        uids = c.execute("SELECT cipher FROM capture WHERE user = ?;",(user,)).fetchall()
        owned_profemons = []
        for uid in uids:
            owned_profemons.append(get_profemon_name_image_trio(uid[0],c))
        dic = {"count":len(owned_profemons), "team":owned_profemons}
        js = json.dumps(dic, indent = 4)
        conn.commit()
        conn.close()
        try:
            is_len = request['values']['len']
            return len(str(js))
        except:
            pass
        return js
    except ValueError or KeyError:
        return "418 I am a teapot"
    except FileNotFoundError as e:
        return str(e) + ", at least not in the current database"
