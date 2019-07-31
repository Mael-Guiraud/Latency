for file in ../view/graph*
 do
   dot -Tpdf $file -o ../view/$file.pdf

done