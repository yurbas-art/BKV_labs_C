#!/bin/bash
gcc $1 -o prog
if [ $? -ne 0 ]
 then
 echo "Ошибка компиляции"
 exit
fi
for i in input*
do 
 no=`echo $i|cut -b 6-7`
 ./prog <$i >OUT
diff output$no.txt OUT &>/dev/null
 if [ $? -ne 0 ]
   then
    echo Тест $no не пройден
    exit
 fi
   echo $no пройден
done
