import os
os.environ.setdefault("DJANGO_SETTINGS_MODULE", "mysite.settings")
import django
django.setup()

import sys
from acc_bball.models import State, Color, Team, Player

def add_player(team_id,jersey_num,first_name,last_name,mpg,ppg,rpg,apg,spg,bpg):
    Player.objects.create(TEAM_ID=Team.objects.get(TEAM_ID = team_id), UNIFORM_NUM=jersey_num,FIRST_NAME=first_name,LAST_NAME=last_name,MPG=mpg,PPG=ppg,RPG=rpg,APG=apg,SPG=spg,BPG=bpg)
    return

def add_team(name, state_id, color_id, wins, losses):
    Team.objects.create(NAME = name, STATE_ID = State.objects.get(STATE_ID = state_id), COLOR_ID = Color.objects.get(COLOR_ID = color_id), WINS = wins, LOSSES = losses)

def add_state(name):
    State.objects.create(NAME = name)
    return

def add_color(name):
    Color.objects.create(NAME = name)
    return

def query1(use_mpg,min_mpg,max_mpg,use_ppg,min_ppg,max_ppg,use_rpg,min_rpg,max_rpg,use_apg,min_apg,max_apg,use_spg,min_spg,max_spg,use_bpg,min_bpg,max_bpg):
    ans = Player.objects.all()
    if (use_mpg):
        ans = ans.filter(MPG__gte = min_mpg, MPG__lte = max_mpg)
    if (use_ppg):
        ans = ans.filter(PPG__gte = min_ppg, PPG__lte = max_ppg)
    if (use_rpg):
        ans = ans.filter(RPG__gte = min_rpg, RPG__lte = max_rpg)
    if (use_apg):
        ans = ans.filter(APG__gte = min_apg, APG__lte = max_apg)
    if (use_spg):
        ans = ans.filter(SPG__gte = min_spg, SPG__lte = max_spg)
    if (use_bpg):
        ans = ans.filter(BPG_gte = min_bpg, BPG__lte = max_bpg)
    print("PLAYER_ID TEAM_ID UNIFORM_NUM FIRST_NAME LAST_NAME MPG PPG RPG "
            "APG SPG BPG")
    for p in ans:
        # print(p.PLAYER_ID+" "+p.TEAM_ID.TEAM_ID+" "+p.UNIFORM_NUM+" "+p.FIRST_NAME+" "+p.LAST_NAME+" "+p.MPG+" "+p.PPG+" "+p.RPG+" "+p.APG+" "+p.SPG+" "+p.BPG)
        print(p.PLAYER_ID, p.TEAM_ID.TEAM_ID, p.UNIFORM_NUM, p.FIRST_NAME, p.LAST_NAME, p.MPG, p.PPG, p.RPG, p.APG, p.SPG, p.BPG)
    return

def query2(team_color):
    ans = Team.objects.filter(COLOR_ID__NAME = team_color)
    print("NAME")
    for t in ans:
        print(t.NAME)
    return

def query3(team_name):
    ans = Player.objects.filter(TEAM_ID__NAME = team_name).order_by('-PPG')
    print("FIRST_NAME LAST_NAME")
    for p in ans:
        print(p.FIRST_NAME, p.LAST_NAME)
    return

def query4(team_state, team_color):
    ans = Player.objects.filter(TEAM_ID__STATE_ID__NAME = team_state).filter(TEAM_ID__COLOR_ID__NAME = team_color)
    print("UNIFORM_NUM FIRST_NAME LAST_NAME")
    for p in ans:
        print(p.UNIFORM_NUM, p.FIRST_NAME, p.LAST_NAME)
    return

def query5(num_wins):
    ans = Player.objects.filter(TEAM_ID__WINS__gt = num_wins)
    print("FIRST_NAME LAST_NAME NAME WINS")
    for p in ans:
        print(p.FIRST_NAME, p.LAST_NAME, p.TEAM_ID.NAME, p.TEAM_ID.WINS)
    return

def main():
    query1(1, 35, 40, 0, 0, 0, 0, 5, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0)
    query1(0, 35, 40, 0, 0, 0, 1, 5, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0)
    query2("Green")
    query2("Orange")
    query3("Duke")
    query3("UNC")
    query4("MA", "Maroon")
    query4("NC","DarkBlue")
    query5(13)
    query5(10)
    # add_player(1, 5, "Wenting", "Yang", 25, 6, 5, 2, 0.7, 0.3)
    # add_team("TestTeam", 10, 7, 2, 16)
    # add_state("Zhejiang")
    # add_color("Brown")
    return

if __name__ == "__main__":
    main()