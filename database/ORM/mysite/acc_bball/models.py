from django.db import models

# Create your models here.
from django.db import models

class State(models.Model):
    STATE_ID = models.AutoField(primary_key=True)
    NAME = models.CharField(max_length=256)

class Color(models.Model):
    COLOR_ID = models.AutoField(primary_key=True)
    NAME = models.CharField(max_length=256)

class Team(models.Model):
    TEAM_ID = models.AutoField(primary_key=True)
    NAME = models.CharField(max_length=256)
    STATE_ID = models.ForeignKey(State, on_delete=models.SET_NULL, null=True)
    COLOR_ID = models.ForeignKey(Color, on_delete=models.SET_NULL, null=True)
    WINS = models.IntegerField()
    LOSSES = models.IntegerField()

class Player(models.Model):
    PLAYER_ID = models.AutoField(primary_key=True)
    TEAM_ID = models.ForeignKey(Team, on_delete=models.CASCADE)
    UNIFORM_NUM = models.IntegerField()
    FIRST_NAME = models.CharField(max_length=256)
    LAST_NAME = models.CharField(max_length=256)
    MPG = models.IntegerField()
    PPG = models.IntegerField()
    RPG = models.IntegerField()
    APG = models.IntegerField()
    SPG = models.DecimalField(max_digits=5,decimal_places=1)
    BPG = models.DecimalField(max_digits=5,decimal_places=1)