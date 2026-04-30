#! /bin/bash
g++ -pedantic-errors -Wall -Weffc++ -Wextra -Wconversion -Wsign-conversion -O0 -o testp.exe tester.cpp


# The runtime variables
radixSortnAvgTime1=0
radixSortnAvgTime2=0
msdrSortAvgTime1=0
msdrSortAvgTime1=0
quickSortAvgTime1=0
quickSortAvgTime2=0
parseString=(radixSortnAvgTime1 radixSortnAvgTime2 msdrSortAvgTime1 msdrSortAvgTime2 quickSortAvgTime1 quickSortAvgTime2)

# extrema of our 2 axes
maxn=0
maxt=0

function getAverageTime(){
	for avgTime in ${parseString[@]}; do ((${avgTime}=0)); done
	local niter=$1
	until [ $niter -eq 0 ]; do
		local rawOutput=$(./testp.exe $2)
		local i=0
		for runTime in $rawOutput; do
			((${parseString[$i]}+=$runTime))
			((i++))
		done
		((niter--))
	done
	for avgTime in ${parseString[@]}; do
		((${avgTime}/=$1))
		if [ ${!avgTime} -gt $maxt ]; then
			maxt=${!avgTime}
		fi
	done
	if [ $2 -gt $maxn ]; then
		maxn=$2
	fi
	echo "<circle cx=\"$2\" cy=\"$radixSortnAvgTime1\" fill=\"orange\"/>" >> $3
	echo "<circle cx=\"$2\" cy=\"$radixSortnAvgTime2\" fill=\"red\"/>" >> $3
	echo "<circle cx=\"$2\" cy=\"$msdrSortAvgTime1\" fill=\"cyan\"/>" >> $3
	echo "<circle cx=\"$2\" cy=\"$msdrSortAvgTime2\" fill=\"blue\"/>" >> $3
	echo "<circle cx=\"$2\" cy=\"$quickSortAvgTime1\" fill=\"magenta\"/>" >> $3
	echo "<circle cx=\"$2\" cy=\"$quickSortAvgTime2\" fill=\"purple\"/>" >> $3
}
echo "" > output.txt

#yellow/orange radixnSort
# cyan/blue msdrSort
# magental/purple quicksort

# beginning, end, iterator integer
bUnmodified=4
b=$bUnmodified
e=$bUnmodified
i=$bUnmodified

# calculate until either radix sort outperforms quicksort on its worst case or a minute passes during one of the tests
while :; do
	getAverageTime 5 $i output.txt
	for avgTime in ${parseString[@]}; do 
		if [ ${!avgTime} -gt $1 ]; then # break if test takes over 1 second
			break 2; 
		fi 
	done
	if [ $radixSortnAvgTime1 -lt $quickSortAvgTime1 ] || [ $msdrSortAvgTime1 -lt $quickSortAvgTime1 ]; then
		break
	else
		((i*=2))
		e=$i
	fi
done
echo "maximum elements: $maxn, maximum time: $maxt (microseconds)"

# calculate intersections
eUnmodified=$e
((i=($b+$e)/2))

# ...for radixSortn 
for((pri=0;$i!=$pri;i=($b+$e)/2)); do
	pri=$i
	getAverageTime 5 $i output.txt
	if [ $msdrSortAvgTime2 -lt $quickSortAvgTime2 ]; then
		e=$i
	elif [ $msdrSortAvgTime2 -gt $quickSortAvgTime2 ]; then
		b=$i
	else
		break
	fi
done
echo "Estimated intersection of msdrSort and quickSort: n=$i at $msdrSortAvgTime2 microseconds"
e=$eUnmodified
b=$bUnmodified
((i=($b+$e)/2))

# ...for msdrSort
for((pri=0;$i!=$pri;i=($b+$e)/2)); do
	pri=$i
	getAverageTime 5 $i output.txt
	if [ $radixSortnAvgTime2 -lt $quickSortAvgTime2 ]; then
		e=$i
	elif [ $radixSortnAvgTime2 -gt $quickSortAvgTime2 ]; then
		b=$i
	else
		break
	fi
done
echo "Estimated intersection of radixSortn and quickSort: n=$i at $radixSortnAvgTime2 microseconds"

# Print the graph

echo "<?xml version=\"1.0\" encoding=\"utf-8\"?><svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 $maxn $maxt\"><style>circle { r: 0.75%;} </style>" > output.svg
echo "<g transform=\"scale(1, -1) translate(0, -$maxt) \">" >> output.svg
for((j=0;$j<=10;j++)); do
	echo "<line x1=\"$(($j*$maxn/10))\" y1=\"0\" x2=\"$(($j*$maxn/10))\" y2=\"$maxt\" stroke=\"gray\" stroke-width=\"0.05%\"/>" >> output.svg
	echo "<line y1=\"$(($j*$maxt/10))\" x1=\"0\" y2=\"$(($j*$maxt/10))\" x2=\"$maxn\" stroke=\"gray\" stroke-width=\"0.05%\"/>" >> output.svg
done
cat output.txt >> output.svg
echo "</g><line stroke=\"black\" stroke-width=\"1%\" x1=\"0\" y1=\"0\" x2=\"0\" y2=\"100%\"/><line stroke=\"black\" stroke-width=\"1%\" x1=\"0\" y1=\"100%\" x2=\"100%\" y2=\"100%\"/>" >> output.svg
for((j=0;$j<=10;j++)); do
	((i=$j*$maxn/10))
	echo "<text x=\"0\" y=\"0\" fill=\"black\" font-size=\"$(($maxt/50))\" transform=\"translate($i, $((99*$maxt/100))) rotate(-30)\">$i</text>" >> output.svg
	((i=$j*$maxt/10))
	echo "<text x=\"0\" y=\"0\" fill=\"black\" font-size=\"$(($maxt/50))\" transform=\"translate($((2*$maxn/100)), $(($maxt-$i))) rotate(-30)\">$i</text>" >> output.svg
done
echo "</svg>" >> output.svg

