import seaborn as sns
sns.set(style="whitegrid", palette="pastel", color_codes=True)

# Load the example tips dataset
tips = sns.load_dataset("tips")

# Draw a nested violinplot and split the violins for easier comparison
plot = sns.violinplot(x="day", y="total_bill", hue="sex", data=tips, split=True,
               inner="quart", palette={"Male": "b", "Female": "y"})
sns.despine(left=True)

fig = plot.get_figure()
fig.savefig('gender_salary.png')
'''
from numpy import *
import seaborn as sns


file_data='donnes_random.data'
 
 
# ouverture du fichier texte 
f=open(file_data,'r')
lignes =f.readlines()



tableau = []

for ligne in lignes:
	ligne = str(ligne).split(" ")
	tableau2 = []
	tableau2.append(int(ligne[0]) )
	tableau2.append(int(ligne[1].split("\\")[0]) )
	tableau.append(tableau2 )




sns.set(style="whitegrid", palette="pastel", color_codes=True)

# Load the example tips dataset
tips = sns.load_dataset("tips")

# Draw a nested violinplot and split the violins for easier comparison
sns.violinplot(x="day", y="total_bill", hue="sex", data=tableau)
sns.despine(left=True)

'''