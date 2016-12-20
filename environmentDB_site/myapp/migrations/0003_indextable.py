# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('myapp', '0002_auto_20150812_1336'),
    ]

    operations = [
        migrations.CreateModel(
            name='IndexTable',
            fields=[
                ('sampler', models.CharField(max_length=255)),
                ('department', models.CharField(max_length=16)),
                ('location', models.IntegerField()),
                ('sink', models.CharField(max_length=30)),
                ('date', models.DateField()),
                ('drain_swab_chromagar', models.CharField(max_length=32)),
                ('indirect_pos_neg', models.CharField(max_length=255)),
                ('organism_id_swab', models.CharField(max_length=255, db_column='organism_ID_swab')),
                ('cav_drain_swab', models.CharField(max_length=255, db_column='CAV_drain_swab')),
                ('entry_id', models.IntegerField(serialize=False, primary_key=True)),
                ('ptrap_water_chromagar', models.CharField(max_length=32, db_column='pTrap_water_chromagar')),
                ('indirect_pos_neg_drain', models.CharField(max_length=255)),
                ('organism_id_water', models.CharField(max_length=255, db_column='organism_ID_water')),
                ('cav_ptrap_water', models.CharField(max_length=255, db_column='CAV_pTrap_water')),
                ('comments', models.CharField(max_length=255)),
            ],
            options={
                'ordering': ['-entry_id'],
                'db_table': 'index_table',
                'managed': False,
            },
        ),
    ]
