# -*- coding: utf-8 -*-
"""
  File: researcher_dia_plugins_API.py

  Плагин для изучения API python dia.
  Доступ к свойствам графических объектов, наблюдение реакции, 
  вывод в консоль отладочной информации.

  path: <home_dir>/.dia/python/

"""

import dia


def research(data, flags) :
  print "argument 'data' = ", data
  print "argument 'flags' = ", flags
  print "sorted_selected = ", dia.active_display().diagram.get_sorted_selected()
  
  objs = dia.active_display().diagram.get_sorted_selected()
  print " "
  
  for obj in objs:
    print "obj.type = ", obj.type
    print "obj.properties = ", obj.properties.keys() 
    print "prop obj_bb = ", obj.properties['obj_bb'].value
    print "prop meta = ",obj.properties.get('meta').value


dia.register_action ("ObjectsResearch", "researcher",
		     "/DisplayMenu/Objects", 
		     research)

