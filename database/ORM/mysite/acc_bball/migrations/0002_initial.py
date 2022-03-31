# Generated by Django 4.0.2 on 2022-03-16 23:40

from django.db import migrations, models
import django.db.models.deletion


class Migration(migrations.Migration):

    initial = True

    dependencies = [
        ('acc_bball', '0001_initial'),
    ]

    operations = [
        migrations.CreateModel(
            name='Color',
            fields=[
                ('COLOR_ID', models.AutoField(primary_key=True, serialize=False)),
                ('NAME', models.CharField(max_length=256)),
            ],
        ),
        migrations.CreateModel(
            name='State',
            fields=[
                ('STATE_ID', models.AutoField(primary_key=True, serialize=False)),
                ('NAME', models.CharField(max_length=256)),
            ],
        ),
        migrations.CreateModel(
            name='Team',
            fields=[
                ('TEAM_ID', models.AutoField(primary_key=True, serialize=False)),
                ('NAME', models.CharField(max_length=256)),
                ('WINS', models.IntegerField()),
                ('LOSSES', models.IntegerField()),
                ('COLOR_ID', models.ForeignKey(null=True, on_delete=django.db.models.deletion.SET_NULL, to='acc_bball.color')),
                ('STATE_ID', models.ForeignKey(null=True, on_delete=django.db.models.deletion.SET_NULL, to='acc_bball.state')),
            ],
        ),
        migrations.CreateModel(
            name='Player',
            fields=[
                ('PLAYER_ID', models.AutoField(primary_key=True, serialize=False)),
                ('UNIFORM_NUM', models.IntegerField()),
                ('FIRST_NAME', models.CharField(max_length=256)),
                ('LAST_NAME', models.CharField(max_length=256)),
                ('MPG', models.IntegerField()),
                ('PPG', models.IntegerField()),
                ('RPG', models.IntegerField()),
                ('APG', models.IntegerField()),
                ('SPG', models.DecimalField(decimal_places=1, max_digits=5)),
                ('BPG', models.DecimalField(decimal_places=1, max_digits=5)),
                ('TEAM_ID', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='acc_bball.team')),
            ],
        ),
    ]
