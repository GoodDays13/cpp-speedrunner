extends Node2D

func _ready():
	var level = {}
	level["version"] = 1
	level["objects"] = []
	for item in get_children():
		var test = {}
		test["position"] = {
			"x": round(item.transform.origin.x)/128,
			"y": -round(item.transform.origin.y)/128
		}
		test["scale"] = {
			"x": round(item.transform.x.x * 16) / 16,
			"y": round(item.transform.y.y * 16) / 16
		}
		test["color"] = {
			"r": item.color.r,
			"g": item.color.g,
			"b": item.color.b,
			"a": item.color.a,
		}
		if "player" in item.get_meta("tags"):
			level["player"] = test
			continue
			
		
		test["tags"] = item.get_meta("tags")
		level["objects"].append(test)

	var json = JSON.stringify(level, "\t")
	var file = FileAccess.open("res://output.json", FileAccess.WRITE)
	var binary = FileAccess.open("res://output.bin", FileAccess.WRITE)
	binary.store_float(level['player']['position']['x'])
	binary.store_float(level['player']['position']['y'])
	binary.store_float(level['player']['scale']['x'])
	binary.store_float(level['player']['scale']['y'])
	binary.store_float(level['player']['color']['r'])
	binary.store_float(level['player']['color']['g'])
	binary.store_float(level['player']['color']['b'])
	binary.store_float(level['player']['color']['a'])
	for object in level['objects']:
		binary.store_float(object['position']['x'])
		binary.store_float(object['position']['y'])
		binary.store_float(object['scale']['x'])
		binary.store_float(object['scale']['y'])
		binary.store_float(object['color']['r'])
		binary.store_float(object['color']['g'])
		binary.store_float(object['color']['b'])
		binary.store_float(object['color']['a'])
		var flags = 0
		var tag_map = {'floor': 0, 'end': 1, 'kill': 2, 'no_draw': 3}
		for tag in object['tags']:
			if tag in tag_map:
				flags |= 1 << tag_map[tag]
		binary.store_32(flags)


	file.store_string(json)
	
	get_tree().quit()
