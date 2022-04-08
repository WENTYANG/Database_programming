import os
os.environ.setdefault("DJANGO_SETTINGS_MODULE", "mysite.settings")
import django
django.setup()

import sys
from acc_bball.models import State, Color, Team, Player

import query_funcs
from django.db import connection

def dropTables():
    cursor = connection.cursor()

    cursor.execute('TRUNCATE TABLE {0} RESTART IDENTITY CASCADE;'.format(Player._meta.db_table))
    cursor.execute('TRUNCATE TABLE {0} RESTART IDENTITY CASCADE;'.format(Team._meta.db_table))
    cursor.execute('TRUNCATE TABLE {0} RESTART IDENTITY CASCADE;'.format(Color._meta.db_table))
    cursor.execute('TRUNCATE TABLE {0} RESTART IDENTITY CASCADE;'.format(State._meta.db_table))

    # # Player.objects.all().delete()
    # Player.objects.all().raw('TRUNCATE TABLE {0} RESTART IDENTITY CASCADE;'.format(Player._meta.db_table))
    # # Team.objects.all().delete()
    # Team.objects.all().raw('TRUNCATE TABLE {0} RESTART IDENTITY CASCADE;'.format(Team._meta.db_table))
    # # Color.objects.all().delete()
    # Color.objects.all().raw('TRUNCATE TABLE {0} RESTART IDENTITY CASCADE;'.format(Color._meta.db_table))
    # # State.objects.all().delete()
    # State.objects.all().raw('TRUNCATE TABLE {0} RESTART IDENTITY CASCADE;'.format(State._meta.db_table))
    
    # Player.objects.all().raw('TRUNCATE TABLE acc_bball_player')
    # Team.objects.all().delete()
    # Color.objects.all().delete()
    # State.objects.all().raw('TRUNCATE TABLE acc_bball_state')

    # cursor.execute('DROP TABLE IF EXISTS "{0}" '.format(Player._meta.db_table))
    # cursor.execute('DROP TABLE IF EXISTS "{0}" '.format(Team._meta.db_table))
    # cursor.execute('DROP TABLE IF EXISTS "{0}" '.format(Color._meta.db_table))
    # cursor.execute('DROP TABLE IF EXISTS "{0}" '.format(State._meta.db_table))

    # cursor.execute('CREATE TABLE "{0}"'.format(State._meta.db_table))
    # cursor.execute('CREATE TABLE "{0}" '.format(Color._meta.db_table))
    # cursor.execute('CREATE TABLE "{0}" '.format(Team._meta.db_table))
    # cursor.execute('CREATE TABLE "{0}" '.format(Player._meta.db_table))
    return

def populatePlayer():
    ifs = open("player.txt")
    lines = ifs.readlines()
    for line in lines:
        PLAYER_ID, TEAM_ID, UNIFORM_NUM, FIRST_NAME, LAST_NAME, MPG, PPG, RPG, APG, SPG, BPG = line.split(' ')
        Player.objects.create(TEAM_ID=Team.objects.get(TEAM_ID=TEAM_ID), UNIFORM_NUM=UNIFORM_NUM, FIRST_NAME=FIRST_NAME, LAST_NAME=LAST_NAME, MPG=MPG, PPG=PPG, RPG=RPG, APG=APG, SPG=SPG, BPG=BPG.strip('\n'))
    ifs.close()
    return

def populateTeam():
    ifs = open("team.txt")
    lines = ifs.readlines()
    for line in lines:
        TEAM_ID, NAME, STATE_ID, COLOR_ID, WINS, LOSSES = line.split(' ')
        # print("Matching state id", STATE_ID)
        Team.objects.create(NAME=NAME, STATE_ID=State.objects.get(STATE_ID=STATE_ID), COLOR_ID=Color.objects.get(COLOR_ID=COLOR_ID), WINS=WINS, LOSSES=LOSSES.strip('\n'))
    ifs.close()
    return

def populateState():
    ifs = open("state.txt")
    lines = ifs.readlines()
    for line in lines:
        STATE_ID, NAME = line.split(' ')
        State.objects.create(NAME=NAME.strip('\n'))
        # print("state name:", NAME.strip('\n'))
    ifs.close()
    return

def populateColor():
    ifs = open("color.txt")
    lines = ifs.readlines()
    for line in lines:
        COLOR_ID, NAME = line.split(' ')
        # print("color name:", NAME.strip('\n'))
        Color.objects.create(NAME=NAME.strip('\n'))
    ifs.close()
    return

def main():
    dropTables()
    populateColor()
    populateState()
    populateTeam()
    populatePlayer()
    query_funcs.main()

if __name__ == "__main__":
    main()