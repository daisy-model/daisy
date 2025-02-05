grep dai_system_test tests -r | sort -fV | cut -d "(" -f 2 | cut -d ")" -f 1 | tr " " "," > test-list.csv
