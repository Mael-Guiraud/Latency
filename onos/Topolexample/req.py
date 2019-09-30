import requests
import json


#devices
res = requests.get('http://localhost:8181/onos/v1/devices',auth=('onos', 'rocks'))
data = json.loads(res.text)
fichier = open("devices", "w")
nb = len(data['devices'])
fichier.write(str(nb)+"\n")
for i in range(nb):
	fichier.write(str(i)+" "+data['devices'][i]['id']+"\n")

fichier.close()
#links
res = requests.get('http://localhost:8181/onos/v1/links',auth=('onos', 'rocks'))
data = json.loads(res.text)
fichier = open("links", "w")

nb = len(data['links'])
for i in range(nb):
	fichier.write(str(i)+" "+data['links'][i]['annotations']['latency']+" "+data['links'][i]['src']['device']+" "+data['links'][i]['src']['port']+" "+data['links'][i]['dst']['device']+" "+data['links'][i]['dst']['port']+"\n")

fichier.close()

#requette sur les intents
res = requests.get('http://localhost:8181/onos/v1/intents',auth=('onos', 'rocks'))
#parse le json
data = json.loads(res.text)

fichier = open("paths", "w")
nb = len(data['intents'])
for i in range(nb):
	url = "http://localhost:8181/onos/v1/intents/"+data['intents'][i]['appId']+"/"+data['intents'][i]['key']
	
	resI = requests.get(url,auth=('onos', 'rocks'))
	dataI = json.loads(resI.text)
	url =  "http://localhost:8181/onos/v1/paths/"+dataI['ingressPoint']['device']+"/"+dataI['egressPoint']['device']
	
	resP = requests.get(url,auth=('onos', 'rocks'))
	dataP = json.loads(resP.text)
	nbP = len(dataP['paths'][0]['links'])

	for j in range(nbP):
		fichier.write(str(i)+" "+str(nbP)+" "+dataP['paths'][0]['links'][j]['src']['device']+" "+dataP['paths'][0]['links'][j]['src']['port']+" "+dataP['paths'][0]['links'][j]['dst']['device']+" "+dataP['paths'][0]['links'][j]['dst']['port']+" "+dataP['paths'][0]['links'][j]['annotations']['latency']+" ")
	fichier.write('\n')


fichier.close()