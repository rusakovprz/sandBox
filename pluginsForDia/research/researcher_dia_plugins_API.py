# -*- coding: utf-8 -*-
"""
  File: researcher_dia_plugins_API.py

  Плагин для изучения API python dia.
  Доступ к свойствам графических объектов, наблюдение реакции, 
  вывод в консоль отладочной информации.

"""

import dia


def research(data, flags) :
  print dia.active_display().diagram.get_sorted_selected()
  print data
  print flags
  
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

