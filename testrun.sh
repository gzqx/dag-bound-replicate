cmake . > /dev/null 2>&1
make > /dev/null 2>&1
for (( i = 0; i < 10; i++ )); do
	j=`expr $i + 1`
	echo "Term $j"
	./simulate
	sleep 1
done
