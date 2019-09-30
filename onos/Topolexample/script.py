try:
    import json
except ImportError:
    import simplejson as json
	

import requests

response = requests.get('http://localhost:8181/onos/v1/devices', auth=('onos', 'rocks'))


with open('flows.json') as json_file:
	data = json.load(json_file)

j = 0
print(len(data['paths'][0]['links']))
for i in data['paths'][0]['links']:
	str = json.dumps(i)
	print(i)
	j = j+1

print j
