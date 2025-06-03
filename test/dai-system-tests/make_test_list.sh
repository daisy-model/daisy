grep dai_system_test tests -r | sort -fV | cut -d "(" -f 2 | cut -d ")" -f 1 | tr " " "," > test-list.csv
echo "Test list generated"
echo "You need to manually add"
echo "',1' to all lines where daisy.log files should be deleted and"
echo "',0' to all lines where daisy.log files should be kept"
