let sum=$(echo '0.0' | bc)
for i in {1..2}
do
	for j in {1..11}
	do
	num=`. test.sh  $i`
	echo $num
	sum=$(echo $sum + $num| bc)
	done
	echo "n=$i priemer="$(echo $sum/10 | bc)
	sum=0
done
