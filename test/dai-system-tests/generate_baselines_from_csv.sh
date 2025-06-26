if [ $# -ne 2 ]
then
    echo "Usage:
bash generate_baseline_from_csv.sh <path-to-daisy-binary> <csv-file>"
    exit 1
fi
while read row;
do
    readarray -d "," -t args <<< $row
    bash generate_baseline.sh ${1} ${args[0]} ${args[1]} ${args[2]}
done < "${2}"
