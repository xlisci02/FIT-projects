let sum=$(echo '0.0' | bc)
for i in {1..23}
do
	for j in {1..10}
	do
	input_string=`python3 -c "import random;\
		print(\",\".join([str(random.randint(1,1024)) for _ in range(0,$i)]))"`
	num=`. test.sh  $input_string`
	# echo $input_string
	echo $num
	sum=$(echo $sum + $num| bc)
	done
	echo "n=$i priemer="$(echo $sum/10 | bc)
	sum=0
done
