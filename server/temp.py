
def request_handler(request):
    if request["method"] == "POST":
        p1 = request["form"]["me"]
        p2 = request["form"]["them"]
        with open('/var/jail/home/team5/games.txt','w') as f:
            f.write(p1)
            f.write(" ")
            f.write(p2)
