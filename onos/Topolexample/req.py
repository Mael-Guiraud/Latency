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
fichier.write(str(nb)+'\n')
for i in range(nb):
	fichier.write(str(i)+" "+data['links'][i]['annotations']['latency']+" "+data['links'][i]['src']['device']+" "+data['links'][i]['src']['port']+" "+data['links'][i]['dst']['device']+" "+data['links'][i]['dst']['port']+"\n")

fichier.close()

#requette sur les intents
res = requests.get('http://localhost:8181/onos/v1/intents',auth=('onos', 'rocks'))
#parse le json
data = json.loads(res.text)
cmpt = 0
fichier = open("paths", "w")
nb = len(data['intents'])
for i in range(nb):
	if data['intents'][i]['state'] == 'INSTALLED':
		cmpt+=1
cmpt
fichier.write(str(cmpt)+'\n')
for i in range(nb):
	if data['intents'][i]['state'] == 'INSTALLED':
		url = "http://localhost:8181/onos/v1/intents/"+data['intents'][i]['appId']+"/"+data['intents'][i]['key']
		
		resI = requests.get(url,auth=('onos', 'rocks'))
		dataI = json.loads(resI.text)
		if cmpt == 0:
			fichier2 = open("params", "w")
			dataI['constraints'][0]['period'] = dataI['constraints'][0].get('period', '7800')
			fichier2.write(str(dataI['constraints'][0]['latencyNanos'])+'\n'+dataI['constraints'][0]['period']+'\n')
			fichier2.close()
			cmpt = cmpt +1
		url =  "http://localhost:8181/onos/v1/paths/"+dataI['ingressPoint']['device']+"/"+dataI['egressPoint']['device']
		resP = requests.get(url,auth=('onos', 'rocks'))
		dataP = json.loads(resP.text)
		nbP = len(dataP['paths'][0]['links'])
		fichier.write(str(i)+" "+str(nbP)+" ")
		for j in range(nbP):
			fichier.write(dataP['paths'][0]['links'][j]['src']['device']+" "+dataP['paths'][0]['links'][j]['src']['port']+" "+dataP['paths'][0]['links'][j]['dst']['device']+" "+dataP['paths'][0]['links'][j]['dst']['port']+" ")
		fichier.write('\n')

fichier.close()




