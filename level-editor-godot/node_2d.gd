extends Node2D

func _ready():
	var level = {}
	level["version"] = 1
	level["objects"] = []
	for item in get_children():
		var test = {}
		test["position"] = {
			"x": round(item.transform.origin.x)/128,
			"y": -item.transform.origin.y/128
		}
		test["scale"] = {
			"x": item.transform.x.x,
			"y": item.transform.y.y
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

	var json = JSON.stringify(level, "    ")
	var file = FileAccess.open("res://output.json", FileAccess.WRITE)
	file.store_string(json)
	
	get_tree().quit()
