for i in `ls ../datas/graphes/`;
do
	
	neato -Tpdf -o ../datas/graphes/$i.pdf ../datas/graphes/$i

done